#include "LocalOpts.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/Casting.h"
#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

using namespace llvm;

int getPowerOfTwo(int num) {
  int count = 0;

  if (num <= 0)
    return -1;

  while (num > 1) {
    if (num % 2 != 0)
      return -1;

    num /= 2;
    count++;
  }

  return count;
}

PreservedAnalyses StrengthReductionPass::run([[maybe_unused]] Function &F,
                                             FunctionAnalysisManager &) {

  /// @done(CSCD70) Please complete this method.
  std::vector<Instruction *> instructionsToDelete;
  for (auto &B : F) {
    for (auto &I : B) {
      // 判断是否是乘法指令
      if (I.isBinaryOp() && I.getOpcode() == Instruction::Mul) {
        Value *op1 = I.getOperand(0);
        Value *op2 = I.getOperand(1);

        if (ConstantInt *constantInt = dyn_cast<ConstantInt>(op1)) {
          int64_t value = constantInt->getSExtValue();
          if (getPowerOfTwo(value) != -1) {
            Value *new_value =
                ConstantInt::getSigned(I.getType(), getPowerOfTwo(value));
            I.replaceAllUsesWith(
                BinaryOperator::Create(Instruction::Shl, op2, new_value,
                                       I.getOperand(3)->getName(), &I));
            instructionsToDelete.push_back(&I);
          }
        } else if (ConstantInt *constantInt = dyn_cast<ConstantInt>(op2)) {
          int64_t value = constantInt->getSExtValue();
          if (getPowerOfTwo(value) != -1) {
            Value *new_value =
                ConstantInt::getSigned(I.getType(), getPowerOfTwo(value));
            I.replaceAllUsesWith(
                BinaryOperator::Create(Instruction::Shl, op1, new_value,
                                       I.getOperand(3)->getName(), &I));
            instructionsToDelete.push_back(&I);
          }
        }
      }
    }
  }
  for (llvm::Instruction *I : instructionsToDelete) {
    I->eraseFromParent();
  }

  return PreservedAnalyses::none();
}
