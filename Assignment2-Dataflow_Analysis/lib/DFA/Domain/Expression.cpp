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

  /// @done(CSCD70) Please complete this method.
  Expression expr{BO}; 
  if(DomainIdMap.find(&expr) == DomainIdMap.end()) {
    DomainVector.push_back(expr);
    DomainIdMap.insert(std::make_pair(expr, DomainVector.size() - 1));
  }
}
