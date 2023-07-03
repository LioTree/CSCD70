#include <DFA/Domain/Variable.h>

using namespace llvm;
using dfa::Variable;

raw_ostream &operator<<(raw_ostream &Outs, const Variable &Var) {
  CHECK(Var.Var != nullptr);
  Var.Var->printAsOperand(Outs);
  return Outs;
}

void Variable::Initializer::visitInstruction(Instruction &I) {

  /// @done(CSCD70) Please complete this method.
  Variable var{I};
  if(DomainIdMap.find(var) == DomainIdMap.end()) {
    DomainVector.push_back(var);
    DomainIdMap.insert(std::make_pair(var, DomainVector.size() - 1));
  }
}
