#include "llvm/IR/Constants.h"
#include "llvm/Support/Casting.h"
#include <vector>
#include "LocalOpts.h"

using namespace llvm;

PreservedAnalyses AlgebraicIdentityPass::run([[maybe_unused]] Function &F,
                                             FunctionAnalysisManager &) {

  /// @done(CSCD70) Please complete this method.
  std::vector<Instruction *> instructionsToDelete;
  for (auto &B : F) {
    for (auto &I : B) {
      // 判断是否是加法指令
      if (I.isBinaryOp() && I.getOpcode() == Instruction::Add) {
        Value *op1 = I.getOperand(0);
        Value *op2 = I.getOperand(1);

        // 判断是否满足优化条件：x + 0 或者 0 + x
        if (ConstantInt *constantInt = dyn_cast<ConstantInt>(op1)) {
          if (constantInt->isZero()) {
            I.replaceAllUsesWith(op2); // 将指令替换为op2
            instructionsToDelete.push_back(&I);
          }
        } else if (ConstantInt *constantInt = dyn_cast<ConstantInt>(op2)) {
          if (constantInt->isZero()) {
            I.replaceAllUsesWith(op1); // 将指令替换为op1
            instructionsToDelete.push_back(&I);
          }
        }
      }
      // 判断是否是乘法指令
      else if (I.isBinaryOp() && I.getOpcode() == Instruction::Mul) {
        Value *op1 = I.getOperand(0);
        Value *op2 = I.getOperand(1);

        // 判断是否满足优化条件：x * 1 或者 1 * x
        if (ConstantInt *constantInt = dyn_cast<ConstantInt>(op1)) {
          if (constantInt->isOne()) {
            I.replaceAllUsesWith(op2); // 将指令替换为op2
            instructionsToDelete.push_back(&I);
          }
        } else if (ConstantInt *constantInt = dyn_cast<ConstantInt>(op2)) {
          if (constantInt->isOne()) {
            I.replaceAllUsesWith(op1); // 将指令替换为op1
            instructionsToDelete.push_back(&I);
          }
        }
      }
    }
  }
  for (llvm::Instruction *I : instructionsToDelete) {
    I->eraseFromParent(); // 从父节点中删除指令
  }

  return PreservedAnalyses::none();
}
