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
    for (const auto *It = Inst.op_begin(); It != Inst.op_end(); It++) {
      if (DomainVector[I].contain(*It)) {
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
  ODV = TDV;
  return true;
}

Liveness::MeetOperands_t Liveness::getMeetOperands(const llvm::BasicBlock &BB) const {
  MeetOperands_t Operands;

  MeetBBConstRange_t MeetBBConstRange = getMeetBBConstRange(BB);
  for (const llvm::BasicBlock *MeetBB : MeetBBConstRange) {
    DomainVal_t DV = InstDomainValMap.at(&(MeetBB->front()));

    /*
    phi instructions requires special consideration.
    For example, in 2-Liveness.ll:
      BB3 has two predecessors(BB2 and BB7), and it has two phi instructions:
        %.01 = phi i32 [ 1, %2 ], [ %6, %7 ]
        %.0 = phi i32 [ %0, %2 ], [ %8, %7 ]
      which means if %0 is alive in INPUT[BB3], this information should only be
    propagated to BB2 instead of BB7. similarly, if %6 is alive in INPUT[BB3],
    this information should only be propagated to BB7 instead of BB2.
    */
    for (auto &Phi : MeetBB->phis()) {
      for (auto &PhiIncomingBlock : Phi.blocks()) {

        if (&BB != PhiIncomingBlock) {
          auto It = std::find(DomainVector.begin(), DomainVector.end(),
                              Phi.getIncomingValueForBlock(PhiIncomingBlock));
          int Index = std::distance(DomainVector.begin(), It);
          DV[Index] = DV[Index] & dfa::Bool();
        }
      }
    }
    Operands.push_back(DV);
  }
  return Operands;
}