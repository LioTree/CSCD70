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
  for(const auto &Op : I.operands()) {
    if (isa<Instruction>(Op) || isa<Argument>(Op)) {
      Variable Var{Op};
      if(DomainIdMap.find(Var) == DomainIdMap.end()) {
        DomainVector.push_back(Var);
        DomainIdMap.insert(std::make_pair(Var, DomainVector.size() - 1));
      }
    }
  }
}
