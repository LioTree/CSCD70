#include "DFA.h"

/// @done(CSCD70) Please complete this file.
using namespace llvm;

AnalysisKey Liveness::Key;

bool Liveness::transferFunc(const Instruction &Inst, const DomainVal_t &IDV,
                            DomainVal_t &ODV) {

  /// @done(CSCD70) Please complete this method.
  DomainVal_t TDV = IDV;
  const Value *InstValue = dyn_cast<Value>(&Inst);
  for (std::size_t i = 0; i < TDV.size(); i++) {
    // OUT[B] - defB
    if (InstValue && DomainVector[i].contain(InstValue)) {
      TDV[i] = TDV[i] & dfa::Bool();
    }
    // useB
    if (DomainVector[i].contain(Inst.getOperand(0)) ||
        DomainVector[i].contain(Inst.getOperand(1))) {
      TDV[i] = TDV[i] | dfa::Bool().top();
    }
  }
  if (TDV == ODV)
    return false;
  InstDomainValMap[&Inst] = TDV;
  return true;
}