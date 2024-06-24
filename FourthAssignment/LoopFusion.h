#ifndef LLVM_TRANSFORMS_UTILS_LOOP_FUSION_PASS_H
#define LLVM_TRANSFORMS_UTILS_LOOP_FUSION_PASS_H

#include "llvm/IR/PassManager.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/PostDominators.h"

namespace llvm {

	class LoopFusionPass : public PassInfoMixin<LoopFusionPass> {
		public:
		PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
	};
}
#endif
