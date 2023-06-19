#include "LocalOpts.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"

using namespace llvm;

PreservedAnalyses MultiInstOptPass::run([[maybe_unused]] Function &F,
                                        FunctionAnalysisManager &) {

  /// @done(CSCD70) Please complete this method.
  std::vector<Instruction *> instructionsToDelete;
  for (auto &B : F) {
    for (auto &I : B) {
      // 判断是否是加法指令
      if (I.isBinaryOp() && I.getOpcode() == Instruction::Add) {
        Value *op1 = I.getOperand(0);
        Value *op2 = I.getOperand(1);

        for (Use &U : I.uses()) {
          User *UserInst = U.getUser();
          Instruction *I2;
          if ((I2 = dyn_cast<Instruction>(UserInst)) &&
              I2->getOpcode() == Instruction::Sub) {
            if (I2->getOperand(0) == &I && I2->getOperand(1) == op2) {
              I2->replaceAllUsesWith(op1);
              instructionsToDelete.push_back(I2);
            } else if (I2->getOperand(0) == &I && I2->getOperand(1) == op1) {
              I2->replaceAllUsesWith(op2);
              instructionsToDelete.push_back(I2);
            }
          }
        }
      }
    }
  }
  for (Instruction *I : instructionsToDelete) {
    I->eraseFromParent();
  }

  return PreservedAnalyses::none();
}