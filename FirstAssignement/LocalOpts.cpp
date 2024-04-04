//===-- LocalOpts.cpp - Example Transformations --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Utils/LocalOpts.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

bool getConstantFromInstruction(Instruction &inst, ConstantInt *&C, Value *&Param){
    if (auto *constant = dyn_cast<ConstantInt>(inst.getOperand(0))) {
        C = constant;
        Param = inst.getOperand(1);
    }
    else if (auto *constant = dyn_cast<ConstantInt>(inst.getOperand(1))) {
        C = constant;
        Param = inst.getOperand(0);
    }
    else {
        return false;
    }
    return true;
}

bool algebraicIdentity_stregthReduction(BasicBlock &B) {
    
	unsigned int instructionOpcode;
	ConstantInt *C;
	Value *Param;

	
	// Algebraic Identity -- Strenght Reeduction //
	for (auto inst = B.begin(); inst != B.end(); inst++){
		
		Instruction &Inst = *inst;
		instructionOpcode = Inst.getOpcode();

		switch(instructionOpcode) {	
			case Instruction::Add:
				if ( !(getConstantFromInstruction(Inst, C, Param)) )
					break;

				if (C->getValue() == 0){
					Inst.replaceAllUsesWith(Param);
				}
						
				break;
			case Instruction::Mul:
				if ( !(getConstantFromInstruction(Inst, C, Param)) )
					break;

				if (C->getValue() == 1){
						Inst.replaceAllUsesWith(Param);
				}
				else {
					// --- Strenght Reduction -- //

					if (C->getValue().isPowerOf2()) {
						Constant *shiftCount = ConstantInt::get( C->getType(), C->getValue().exactLogBase2() );
						Instruction *shift_left = BinaryOperator::Create( BinaryOperator::Shl, Param, shiftCount );
						shift_left->insertAfter(&Inst);
						Inst.replaceAllUsesWith(shift_left);
					}
			
					else if( (C->getValue()-1).isPowerOf2() ) {
						Constant *shiftCount = ConstantInt::get( C->getType(), (C->getValue()-1).exactLogBase2() );
						Instruction *shift_left = BinaryOperator::Create( BinaryOperator::Shl, Param, shiftCount );
						shift_left->insertAfter(&Inst);

						Instruction *new_add = BinaryOperator::Create( BinaryOperator::Add, shift_left, Param );
						new_add->insertAfter(shift_left);
						Inst.replaceAllUsesWith(new_add);

					} 
				
					else if( (C->getValue()+1).isPowerOf2() ) {
						Constant *shiftCount = ConstantInt::get( C->getType(), (C->getValue()+1).exactLogBase2() );
						Instruction *shift_left = BinaryOperator::Create( BinaryOperator::Shl, Param, shiftCount );
						shift_left->insertAfter(&Inst);

						Instruction *new_sub = BinaryOperator::Create( BinaryOperator::Sub, shift_left, Param );
						new_sub->insertAfter(shift_left);
						Inst.replaceAllUsesWith(new_sub);
					}
				}
			
				break;
			case Instruction::SDiv:
				// -- Si assume che la costante sia sempre il secondo temrine -- //
				if (dyn_cast<ConstantInt>(Inst.getOperand(1))) {
						C = dyn_cast<ConstantInt>(Inst.getOperand(1));
						Param = Inst.getOperand(0);
				}
				else { break;}

				// -- Algebric identity -- //
				if (C->getValue() == 1){
						Inst.replaceAllUsesWith(Param);
				}
				else {
					if (C->getValue().isPowerOf2()) {
						Constant *shiftCount = ConstantInt::get( C->getType(), C->getValue().exactLogBase2() );
						Instruction *shift_right = BinaryOperator::Create( BinaryOperator::LShr, Param, shiftCount );
						shift_right->insertAfter(&Inst);
						Inst.replaceAllUsesWith(shift_right);
					}
				}

				break;
			default:
				break;
		}
	}
	
	
    return true;
}

bool multiInstructionOptimization(BasicBlock &B){

	unsigned int instructionOpcode;
	ConstantInt *C;
	Value *Param;

	for (auto inst = B.begin(); inst != B.end(); inst++){

		Instruction &Inst = *inst;
		instructionOpcode = Inst.getOpcode();

		if(!(instructionOpcode == Instruction::Add || instructionOpcode == Instruction::Sub))
			continue;

		if ( !(getConstantFromInstruction(Inst, C, Param)) )
			continue;

		Instruction::BinaryOps oppositeOpCode = (instructionOpcode==Instruction::Add) ? Instruction::Sub : Instruction::Add;

		// -- Scorriamo tutti gli user della funzione --
		for (auto userInst = Inst.user_begin(); userInst != Inst.user_end(); ++userInst) { 

			// -- cast necessario per avere un oggetto di tipo Instruction
			if (auto *casted = dyn_cast<Instruction>(*userInst)){
				Instruction &user_instruction = *casted;

				if (user_instruction.getOpcode() == oppositeOpCode){
					ConstantInt *userC;
					Value *userParam;
					if ( !(getConstantFromInstruction(user_instruction, userC, userParam)) )
						continue;

					if (C->getValue() == userC->getValue())
						user_instruction.replaceAllUsesWith(Param);
				}
			}	
		}
	}
	return true;
}

bool deadCodeElimination(BasicBlock &B){

	auto inst = B.begin();

	while(inst != B.end()){
		Instruction &Inst = *inst;

		if ( (Inst.hasNUses(0)) && (Inst.isBinaryOp()) ){
			inst = Inst.eraseFromParent();
		}
		else{
			inst++;
		}
	}

	return true;
}


bool runOnFunction(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (algebraicIdentity_stregthReduction(*Iter)) {
      Transformed = true;
    }
	if (multiInstructionOptimization(*Iter)) {
		Transformed = true;
	}

	if (deadCodeElimination(*Iter)){
		Transformed = true;
	}
  }

  return Transformed;
}

PreservedAnalyses LocalOpts::run(Module &M, ModuleAnalysisManager &AM) {
  for (auto Fiter = M.begin(); Fiter != M.end(); ++Fiter)
    if (runOnFunction(*Fiter))
      return PreservedAnalyses::none();
  
  return PreservedAnalyses::all();
}
