#include "llvm/Transforms/Utils/LoopFusion.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"


using namespace llvm;

/* Utility Functions */
BasicBlock* getLoopEntryBlock(Loop* l) {
    /* Entry block = guard if loop is guarded, else entry block = preheader*/
    
    return (l->isGuarded()) ? l->getLoopPreheader()->getUniquePredecessor() : l->getLoopPreheader();
}

BasicBlock* getLoopBody(Loop *L) {
    /* Loop body = successor of loop header that is contained in the loop */

    BranchInst *HeaderBranchInst = dyn_cast<BranchInst>(L->getHeader()->getTerminator());
    if (!HeaderBranchInst)
        return nullptr;
    
    if (L->contains(HeaderBranchInst->getSuccessor(0)))
        return HeaderBranchInst->getSuccessor(0);

    if (L->contains(HeaderBranchInst->getSuccessor(1)))
        return HeaderBranchInst->getSuccessor(1);

    return nullptr;
}

std::deque <Loop*> getLoops(LoopInfo &LI) {
    std::deque <Loop*> loops;

    for (auto *L : LI){
        loops.push_front(L);
    }

    return loops;
}
/* ***************** */

bool areAdjacent(Loop* l1, Loop* l2) {

    BasicBlock *l2EntryBlock = getLoopEntryBlock(l2);
    if (l2EntryBlock == nullptr){
        errs() << "l2entry nullptr\n";
        return false;
    }

    if (l1->isGuarded()){
        BasicBlock* loopGuard = l1->getLoopPreheader()->getUniquePredecessor();
        if(loopGuard == nullptr){
            errs() << "loopguard nullptr\n";
            return false;
        }
        
        for (BasicBlock *successorBlock : successors(loopGuard)){
            if (successorBlock == l2EntryBlock)
                return true;
        }
        
        errs() << "l1 guarded fail\n";
        return false;

    }

    else {
        BasicBlock *l1Exit = l1->getExitBlock();
        if (l1Exit == nullptr){
            errs() << "l1exit nullptr \n";
            return false;

        }
        
        if (l1Exit == l2EntryBlock)
            return true;
    }

    errs() << "not adj\n";
    return false;
}

bool areCFEquivalent(Loop* l1, Loop* l2, DominatorTree &DT, PostDominatorTree &PDT) {
    BasicBlock* L1Block = getLoopEntryBlock(l1);
    BasicBlock* L2PreHeader = l2->getLoopPreheader();

    if (DT.dominates(L1Block, L2PreHeader) && PDT.dominates(L2PreHeader, L1Block))
        return true;
    else{
        errs() << "not cf equivalent\n";
        return false;
    }
        
}

bool sameTripCount(Loop* l1, Loop* l2, ScalarEvolution &SE) {
    const SCEV *l1TripCount = SE.getBackedgeTakenCount(l1);
    const SCEV *l2TripCount = SE.getBackedgeTakenCount(l2);

    if (l1TripCount == l2TripCount)
        return true;

    errs() << "not same trip count\n";
    return false;
}

bool fuseLoop(Loop* l1, Loop* l2, LoopInfo &LI /*, DominatorTree &DT*/) {
    /* Replace Uses */
    PHINode *l1InductionVariable = l1->getCanonicalInductionVariable();
    PHINode *l2InductionVariable = l2->getCanonicalInductionVariable();

    l2InductionVariable->replaceAllUsesWith(l1InductionVariable);

    /* Change Control Flow */
    BasicBlock *l2PreHeader = getLoopEntryBlock(l2);

    BasicBlock *l1Header = l1->getHeader();
    BasicBlock *l2Header = l2->getHeader();

    /* l2 loop body entry point */
    BasicBlock *l2Body = getLoopBody(l2);

    BasicBlock *l1Latch = l1->getLoopLatch();
    BasicBlock *l2Latch = l2->getLoopLatch();

    BasicBlock *l2Exit = l2->getExitBlock();

    l1Header->getTerminator()->replaceSuccessorWith(l2PreHeader, l2Exit);


    /* 
    In caso in cui il body contenga , ad esempio, un if-else, questo potrebbe 
    creare 2 blocchi che puntano entrambi al latch.
    Questi 2 sarebbero nella lista dei predecessors e perciò vengono fatti puntare al nuovo body.

    Stesso discorso per i blocchi del body di l2 che puntavano al latch di l2 che ora
    punteranno al latch di l1
    */

    for ( auto *l1BodyBlock : predecessors(l1Latch) ) {
        l1BodyBlock->getTerminator()->replaceSuccessorWith(l1Latch, l2Body);
    }

    for ( auto *l2BodyBlock : predecessors(l2Latch) ) {
        l2BodyBlock->getTerminator()->replaceSuccessorWith(l2Latch, l1Latch);
    }

    l2Header->getTerminator()->replaceSuccessorWith(l2Body, l2Latch);

    /* Update fused loop blocks structure */
    std::vector<BasicBlock*> blocksToTransfer;

    for ( auto block : l2->blocks() ) {
        if (block != l2Header && block != l2Latch) {
            blocksToTransfer.push_back(block);
        }     
    }

    for (auto block : blocksToTransfer) {
        l2->removeBlockFromLoop(block);
        l1->addBasicBlockToLoop(block, LI);
    }

    return true;
}

/* 
bool areNotNegativeDistanceDependent(Loop* l1, Loop* l2, DependenceInfo &DI) {
    for (auto *BBLoop1 : l1->blocks()) {
        for (auto &InstLoop1 : *BBLoop1) {
            for (auto *BBLoop2 : l2->blocks()) {
                for (auto &InstLoop2 : *BBLoop2) {
                    if (auto Dep = DI.depends(&InstLoop1, &InstLoop2, true)) {
                        if (Dep) {
                            errs() << "Dependence found between:\n";
                            errs() << "  Instruction in L1: " << InstLoop1 << "\n";
                            errs() << "  Instruction in L2: " << InstLoop2 << "\n";
                            return false;
                        }
                    }
                }
            }
        }
    }

    outs() << "No dep found\n";
    return true;
}
*/

PreservedAnalyses LoopFusionPass::run(Function &F, FunctionAnalysisManager &AM) {

    std::deque <Loop*> loops;
    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);

    DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
    PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);

    ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
    //DependenceInfo &DI = AM.getResult<DependenceAnalysis>(F);

    bool fusionOccurred;

    do {
        outs() << "\n-----------------------\n";
        fusionOccurred = false;
        loops = getLoops(LI);
        
        if (loops.size() == 1){
            outs() << "Only one Loop\n";
            return PreservedAnalyses::all();
        }

        for (int i=0; i<loops.size()-1; i++){
            Loop* L1 = loops[i];
            Loop* L2 = loops[i+1];

            if (!L1 || !L2) {
                errs() << "One of the loops is null.\n";
                return PreservedAnalyses::all();
            }

            outs() << "ITERATION NUMBER : " << i << "\n";
            outs() << "*** LOOP " << i << " ***\n\n";
            for (auto *BB : L1->blocks()) {
                BB->print(outs());
                outs() << "\n";
            }

            outs() << "\n\n*** LOOP " << i+1 << " ***\n\n";
            for (auto *BB : L2->blocks()) {
                BB->print(outs());
                outs() << "\n";
            }

            if(areCFEquivalent(L1, L2, DT, PDT) && areAdjacent(L1, L2) && sameTripCount(L1, L2, SE)){
                outs() << "Loop " << i << " and " << i+1 << " are Loop Fusion Candidates\n";
                
                
                if(fuseLoop(L1, L2, LI)){
                    LI.erase(L2);
                    outs() << "Fused Loop " << i << " and " << i+1 << "\n"; 
                    EliminateUnreachableBlocks(F);

                    fusionOccurred = true;
                    break;
                }
                else{
                    outs() << "Unable to fuse Loop " << i << " and " << i+1 << "\n"; 
                }              
            }
            else {
                outs() << "Loop " << i << " and " << i+1 << " are NOT Loop Fusion Candidates\n";
            }  
        }
    }
    while(fusionOccurred);
    
    
    return PreservedAnalyses::all();
}
