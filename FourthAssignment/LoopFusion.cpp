#include "llvm/Transforms/Utils/LoopFusion.h"


using namespace llvm;

bool areAdjacent(Loop* l1, Loop* l2) {

    BasicBlock *l2EntryBlock = (l2->isGuarded()) ? l2->getLoopPreheader()->getUniquePredecessor() : l2->getLoopPreheader();
    if (l2EntryBlock == nullptr)
        return false;

    if (l1->isGuarded()){
        BasicBlock* loopGuard = l1->getLoopPreheader()->getUniquePredecessor();
        if(loopGuard == nullptr)
            return false;
        
        for (BasicBlock *successorBlock : successors(loopGuard)){
            if (successorBlock == l2EntryBlock)
                return true;
        }
        
        return false;

    }

    else {
        BasicBlock *l1Exit = l1->getExitBlock();
        if (l1Exit == nullptr)
            return false;
        
        if (l1Exit == l2EntryBlock)
            return true;
    }

    return false;
}

bool areCFEquivalent(Loop* l1, Loop* l2, DominatorTree &DT, PostDominatorTree &PDT) {
    BasicBlock* L1Block = (l1->isGuarded()) ? l1->getLoopPreheader()->getUniquePredecessor() : l1->getLoopPreheader();
    BasicBlock* L2PreHeader = l2->getLoopPreheader();

    if (DT.dominates(L1Block, L2PreHeader) && PDT.dominates(L2PreHeader, L1Block))
        return true;
    else
        return false;
}

bool sameTripCount(Loop* l1, Loop* l2, ScalarEvolution &SE) {
    const SCEV *l1TripCount = SE.getBackedgeTakenCount(l1);
    const SCEV *l2TripCount = SE.getBackedgeTakenCount(l2);

    if (l1TripCount == l2TripCount)
        return true;
    return false;
}

void fuseLoop(Loop* l1, Loop* l2) {

}

PreservedAnalyses LoopFusionPass::run(Function &F, FunctionAnalysisManager &AM) {

    std::deque <Loop*> loops;
    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);

    DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
    PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);

    ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);

    for (auto *L : LI){
        loops.push_front(L);
    }


    /* Get just 2 loops */

    for (int i=0; i<loops.size()-1; i++){
        Loop* L1 = loops[i];
        Loop* L2 = loops[i+1];

        if (!L1 || !L2) {
            errs() << "One of the loops is null.\n";
            return PreservedAnalyses::all();
        }


        // Controlli per capire se i due loop sono candidati alla fusion
        if(areCFEquivalent(L1, L2, DT, PDT) && areAdjacent(L1, L2) && sameTripCount(L1, L2, SE)){
            outs() << "Loop " << i << " and " << i+1 << " are Loop Fusion Candidates\n";


            PHINode *L1InductionVariable = L1->getCanonicalInductionVariable();
            PHINode *L2InductionVariable = L2->getCanonicalInductionVariable();

            L2InductionVariable->replaceAllUsesWith(L1InductionVariable);

            
            BasicBlock* l1Latch = L1->getLoopLatch();
            BasicBlock* l2Latch = L2->getLoopLatch();

            Instruction* L1BodyTerminator = l1Latch->getUniquePredecessor()->getTerminator();
            Instruction* L2BodyTerminator = l2Latch->getUniquePredecessor()->getTerminator();
            Instruction* L1HeaderTerminator = L1->getHeader()->getTerminator();
            Instruction* L2HeaderTerminator = L2->getHeader()->getTerminator();
            
            BasicBlock* lastL2BodyBlock = l2Latch->getUniquePredecessor();

            BasicBlock* l2PreHeader = L2->getLoopPreheader();
            BasicBlock* l2Exit = L2->getExitBlock();
            
            L1BodyTerminator->replaceSuccessorWith(l1Latch, lastL2BodyBlock);
            L2BodyTerminator->replaceSuccessorWith(l2Latch, l1Latch);
            L1HeaderTerminator->replaceSuccessorWith(l2PreHeader, l2Exit);
            L2HeaderTerminator->replaceSuccessorWith(lastL2BodyBlock, l2Latch);
        }
        else {
            outs() << "Loop " << i << " and " << i+1 << " are NOT Loop Fusion Candidates\n";
        }
        

        /*
        se trovo due candidati li fondo
        interrompo l'analisi in quel punto
        e la faccio ripartire dall'inizio.
        */
    }
    
    
    return PreservedAnalyses::all();
}

