#include <DFA/Domain/Expression.h>

using namespace llvm;
using dfa::Expression;

raw_ostream &operator<<(raw_ostream &Outs, const Expression &Expr) {
  Outs << "[" << Instruction::getOpcodeName(Expr.Opcode) << " ";
  Expr.LHS->printAsOperand(Outs, false);
  Outs << ", ";
  Expr.RHS->printAsOperand(Outs, false);
  Outs << "]";
  return Outs;
}

void Expression::Initializer::visitBinaryOperator(BinaryOperator &BO) {

  /// @todo(CSCD70) Please complete this method.
}

// https://stackoverflow.com/questions/5889238/why-is-xor-the-default-way-to-combine-hashes
size_t hash_combine( size_t lhs, size_t rhs ) {
  if constexpr (sizeof(size_t) >= 8) {
    lhs ^= rhs + 0x517cc1b727220a95 + (lhs << 6) + (lhs >> 2);
  } else {
    lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
  }
  return lhs;
}
