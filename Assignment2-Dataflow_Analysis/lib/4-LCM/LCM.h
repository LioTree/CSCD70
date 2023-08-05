#pragma once // NOLINT(llvm-header-guard)

#include <DFA/Domain/Expression.h>
#include <DFA/Flow/BackwardAnalysis.h>
#include <DFA/Flow/ForwardAnalysis.h>
#include <DFA/MeetOp.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/PassManager.h>
#include <map>
#include <vector>
#include <set>

class AnticipatedExprs final
    : public dfa::BackwardAnalysis<dfa::Expression, dfa::Bool,
                                   dfa::Intersect<dfa::Bool>>,
      public llvm::AnalysisInfoMixin<AnticipatedExprs> {
private:
  using BackwardAnalysis_t = dfa::BackwardAnalysis<dfa::Expression, dfa::Bool,
                                                   dfa::Intersect<dfa::Bool>>;
  using typename BackwardAnalysis_t::MeetBBConstRange_t;
  using typename BackwardAnalysis_t::MeetOperands_t;

  friend llvm::AnalysisInfoMixin<AnticipatedExprs>;
  static llvm::AnalysisKey Key;
  virtual std::string getName() const override { return "AnticipatedExprs"; }
  virtual bool transferFunc(const llvm::Instruction &, const DomainVal_t &,
                            DomainVal_t &) override;
  virtual MeetOperands_t
  getMeetOperands(const llvm::BasicBlock &BB) const override;

public:
  using Result = typename BackwardAnalysis_t::AnalysisResult_t;
  using BackwardAnalysis_t::run;
};

class LCMWrapperPass : public llvm::PassInfoMixin<LCMWrapperPass> {
public:
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &FAM) {

    /// @todo(CSCD70) Get the result from the main body.
    FAM.getResult<AnticipatedExprs>(F);
    return llvm::PreservedAnalyses::all();
  }
};
