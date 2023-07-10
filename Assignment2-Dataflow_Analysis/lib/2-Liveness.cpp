#include "DFA.h"

/// @done(CSCD70) Please complete this file.
using namespace llvm;

AnalysisKey Liveness::Key;

bool Liveness::transferFunc(const Instruction &Inst, const DomainVal_t &IDV,
                            DomainVal_t &ODV) {

  /// @done(CSCD70) Please complete this method.
  DomainVal_t TDV = IDV;
  const Value *InstValue = dyn_cast<Value>(&Inst);
  for (std::size_t I = 0; I < TDV.size(); I++) {
    // OUT[B] - defB
    if (InstValue && DomainVector[I].contain(InstValue)) {
      TDV[I] = TDV[I] & dfa::Bool();
    }
    // useB
    for(const auto *It = Inst.op_begin(); It != Inst.op_end(); It++) {
      if(DomainVector[I].contain(*It)) {
        TDV[I] = TDV[I] | dfa::Bool().top();
        break;
      }
    }
    /*
    if (DomainVector[i].contain(Inst.getOperand(0)) ||
        DomainVector[i].contain(Inst.getOperand(1))) {
      TDV[i] = TDV[i] | dfa::Bool().top();
    }
    */
  }
  if (TDV == ODV)
    return false;
  InstDomainValMap[&Inst] = TDV;
  return true;
}