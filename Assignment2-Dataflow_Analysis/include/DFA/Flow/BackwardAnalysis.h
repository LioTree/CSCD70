#pragma once // NOLINT(llvm-header-guard)

#include "Framework.h"
#include "llvm/IR/SymbolTableListTraits.h"

/// @done(CSCD70) Please instantiate for the backward pass, similar to the
///               forward one.
/// @sa @c ForwardAnalysis

namespace dfa {

typedef llvm::iterator_range<llvm::const_succ_iterator>
    BackwardMeetBBConstRange_t;
typedef llvm::iterator_range<
    std::reverse_iterator<llvm::Function::const_iterator>>
    BackwardBBConstRange_t;
typedef llvm::iterator_range<llvm::BasicBlock::const_reverse_iterator>
    BackwardInstConstRange_t;

template <typename TDomainElem, typename TValue, typename TMeetOp>
class BackwardAnalysis
    : public Framework<TDomainElem, TValue, TMeetOp, BackwardMeetBBConstRange_t,
                       BackwardBBConstRange_t, BackwardInstConstRange_t> {
protected:
  using Framework_t =
      Framework<TDomainElem, TValue, TMeetOp, BackwardMeetBBConstRange_t,
                BackwardBBConstRange_t, BackwardInstConstRange_t>;
  using typename Framework_t::AnalysisResult_t;
  using typename Framework_t::BBConstRange_t;
  using typename Framework_t::DomainVal_t;
  using typename Framework_t::InstConstRange_t;
  using typename Framework_t::MeetBBConstRange_t;
  using typename Framework_t::MeetOperands_t;

  using Framework_t::BVs;
  using Framework_t::DomainIdMap;
  using Framework_t::DomainVector;
  using Framework_t::InstDomainValMap;

  using Framework_t::getBoundaryVal;
  using Framework_t::getName;
  using Framework_t::run;
  using Framework_t::stringifyDomainWithMask;

  virtual void printInstDomainValMap(const llvm::Instruction &Inst) const final override {
    using llvm::errs;
    using llvm::outs;
    const llvm::BasicBlock *const ParentBB = Inst.getParent();

    outs() << Inst << "\n";
    LOG_ANALYSIS_INFO << "\t"
                      << stringifyDomainWithMask(InstDomainValMap.at(&Inst));
    if (&Inst == &(ParentBB->back())) {
      errs() << "\n";
      // LOG_ANALYSIS_INFO << "\t" << stringifyDomainWithMask(BVs.at(ParentBB));
      LOG_ANALYSIS_INFO << "\t"
                        << stringifyDomainWithMask(getBoundaryVal(*ParentBB));
    } // if (&Inst == &(*ParentBB->back()))
  }

  virtual MeetBBConstRange_t
  getMeetBBConstRange(const llvm::BasicBlock &BB) const final override {
    return llvm::successors(&BB);
  }
  virtual InstConstRange_t getInstConstRange(const llvm::BasicBlock &BB) const final override {
    return make_range(BB.rbegin(), BB.rend());
  }
  virtual BBConstRange_t getBBConstRange(const llvm::Function &F) const final override {
    return llvm::reverse(F);
  }
  // this method should be in Liveness, but it's more convenient to put it here
  virtual MeetOperands_t getMeetOperands(const llvm::BasicBlock &BB) const override {
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
        which means if %0 is alive in INPUT[BB3], this information should only be propagated to BB2 instead of BB7.
        similarly, if %6 is alive in INPUT[BB3], this information should only be propagated to BB7 instead of BB2.
      */
      for (auto &Phi : MeetBB->phis()) {
        for (auto &PhiIncomingBlock : Phi.blocks()) {

          if (&BB != PhiIncomingBlock) {
            auto It = std::find(DomainVector.begin(), DomainVector.end(),
                                Phi.getIncomingValueForBlock(PhiIncomingBlock));
            int Index = std::distance(DomainVector.begin(), It);
            DV[Index] = DV[Index] & TValue();
          }
        }
      }
      Operands.push_back(DV);
    }
    return Operands;
  }
};

} // namespace dfa