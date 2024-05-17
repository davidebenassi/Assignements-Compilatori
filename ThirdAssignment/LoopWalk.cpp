//===-- LoopWalk.cpp - Loop Incariant Code Motion Implementation --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Utils/LoopWalk.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ADT/SetVector.h"


using namespace llvm;

bool isLoopInvariant(Instruction &inst, std::set <Instruction*> set, Loop &loop){

  // * Instruction Type Check * //
  if ( inst.getOpcode() == Instruction::Br || inst.getOpcode() == Instruction::ICmp || isa<PHINode>(inst) )
    return false;
        

  // * Operand Check * //
  for(auto op = inst.op_begin(); op != inst.op_end(); ++op){

    if( Instruction *op_inst = dyn_cast<Instruction>(*op) ){

      if ( loop.contains(op_inst->getParent()) ) {
        if(!set.empty()){
          if ( !set.count(op_inst) )
            return false;
        }
        else
          return false;
      }

      if ( isa<PHINode>(op_inst) )
        return false;

      if(!set.empty()){
        if ( !set.count(op_inst) )
          return false;
      }
      else
        return false;
    }
    else if ( auto *constant = dyn_cast<ConstantInt>(*op) ){
      continue;
    }
  }
  return true;
}

// Returns true if there is no instruction's uses outside the Loop
bool isDeadOutsideLoop(Loop &L, Instruction* Inst){

	for(auto userInst = Inst->user_begin(); userInst != Inst->user_end(); ++userInst){
		if (auto *casted = dyn_cast<Instruction>(*userInst)){ 
			Instruction &user_instruction = *casted;
			if ( !L.contains( user_instruction.getParent() ) )
        return false;
		}
	}
  
	return true;
}

bool dominatesAllLoopExits(DominatorTree &DT, Loop &L, Instruction* Inst){
  SmallVector<BasicBlock*, 4> ExitBlocks;
  L.getExitBlocks(ExitBlocks);

  BasicBlock *instructionParentBB = Inst->getParent();

  for (auto *ExitBlock : ExitBlocks) {
    if(!DT.dominates(instructionParentBB, ExitBlock))
      return false;
  }
  return true;
}

// Returns true if all instruction's operands are not in "loopInvariantInstructionsSet"
bool operandsAlreadyMoved(Instruction* Inst, std::set <Instruction*> set){ 
	
  for(auto op = Inst->op_begin(); op != Inst->op_end(); ++op){
    if( Instruction *op_inst = dyn_cast<Instruction>(*op) ){
      if (set.count(op_inst))
        return false;
    }
  }
  return true;
}

PreservedAnalyses LoopWalk::run(Loop &L, LoopAnalysisManager &LAM, LoopStandardAnalysisResults &LAR, LPMUpdater &LU) {

  std::set <Instruction*> loopInvariantInstructionsSet;

  if(!L.isLoopSimplifyForm()) {
  	outs() << "Loop not in Normal Form \n";
    return PreservedAnalyses::all();
  }

  // * Loop on Loop Basic Blocks - Find Loop Invariant Instructions * //

  for (Loop::block_iterator BI = L.block_begin(); BI != L.block_end(); ++BI){       // * LOOP BB Iterator * //
    BasicBlock *BB = *BI;
   	
    for (auto inst = BB->begin(); inst != BB->end(); ++inst ){                      // * Instruction Iterator * //
      Instruction &Inst = *inst;
        
      if ( isLoopInvariant(Inst, loopInvariantInstructionsSet, L) ){
        loopInvariantInstructionsSet.insert(&Inst);
      }
    }
  }


  // * Code Motion Candidates * //

  DominatorTree &DT = LAR.DT;

  for (Instruction* Inst : loopInvariantInstructionsSet){
    if( !dominatesAllLoopExits(DT, L, Inst) && !isDeadOutsideLoop(L, Inst) ){
      loopInvariantInstructionsSet.erase(Inst);
    }
  }

  // * Loop on Code Motion Candidates and move them to Loop Preheader * //

  BasicBlock* LoopPreheader = L.getLoopPreheader();
  Instruction *insertPoint = LoopPreheader->getTerminator();

  while( !loopInvariantInstructionsSet.empty() ) {
    for (Instruction* Inst : loopInvariantInstructionsSet) {                          // * Instructions in set * //
      
      if(operandsAlreadyMoved(Inst, loopInvariantInstructionsSet)){
        outs() << "Moving --> " << *Inst << "\n";
        Inst->removeFromParent();
        Inst->insertBefore(insertPoint);
        loopInvariantInstructionsSet.erase(Inst);
      }  
    }
  }

  return PreservedAnalyses::all();
}
