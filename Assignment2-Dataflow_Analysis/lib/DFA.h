#pragma once // NOLINT(llvm-header-guard)

#include "4-LCM/LCM.h"

#include <DFA/Domain/Expression.h>
#include <DFA/Domain/Variable.h>
#include <DFA/Flow/BackwardAnalysis.h>
#include <DFA/Flow/ForwardAnalysis.h>
#include <DFA/MeetOp.h>

#include <llvm/IR/PassManager.h>
#include <map>
#include <utility>
#include <vector>

class AvailExprs final : public dfa::ForwardAnalysis<dfa::Expression, dfa::Bool,
                                                     dfa::Intersect<dfa::Bool>>,
                         public llvm::AnalysisInfoMixin<AvailExprs> {
private:
  using ForwardAnalysis_t = dfa::ForwardAnalysis<dfa::Expression, dfa::Bool,
                                                 dfa::Intersect<dfa::Bool>>;

  friend llvm::AnalysisInfoMixin<AvailExprs>;
  static llvm::AnalysisKey Key;

  virtual std::string getName() const override { return "AvailExprs"; }
  virtual bool transferFunc(const llvm::Instruction &, const DomainVal_t &,
                            DomainVal_t &) override;

public:
  using Result = typename ForwardAnalysis_t::AnalysisResult_t;
  using ForwardAnalysis_t::run;
};

class AvailExprsWrapperPass
    : public llvm::PassInfoMixin<AvailExprsWrapperPass> {
public:
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &FAM) {
    FAM.getResult<AvailExprs>(F);
    return llvm::PreservedAnalyses::all();
  }
};

class Liveness final : public dfa::BackwardAnalysis<dfa::Variable, dfa::Bool,
                                                    dfa::Union<dfa::Bool>>,
                       public llvm::AnalysisInfoMixin<Liveness> {
private:
  using BackwardAnalysis_t =
      dfa::BackwardAnalysis<dfa::Variable, dfa::Bool, dfa::Union<dfa::Bool>>;

  friend llvm::AnalysisInfoMixin<Liveness>;
  static llvm::AnalysisKey Key;

  virtual std::string getName() const override { return "Liveness"; }
  virtual bool transferFunc(const llvm::Instruction &, const DomainVal_t &,
                            DomainVal_t &) override;

public:
  using Result = typename BackwardAnalysis_t::AnalysisResult_t;
  using BackwardAnalysis_t::run;
};

/// @done(CSCD70) Please complete the main body of the following passes, similar
///               to the Available Expressions pass above.

class LivenessWrapperPass : public llvm::PassInfoMixin<LivenessWrapperPass> {
public:
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &FAM) {

    FAM.getResult<Liveness>(F);
    return llvm::PreservedAnalyses::all();
  }
};

class SCCP final : public dfa::ForwardAnalysis<dfa::Variable, dfa::Constant,
                                               dfa::Intersect<dfa::Constant>>,
                   public llvm::AnalysisInfoMixin<SCCP> {
private:
  using ForwardAnalysis_t = dfa::ForwardAnalysis<dfa::Variable, dfa::Constant,
                                                 dfa::Intersect<dfa::Constant>>;
  using ForwardAnalysis_t::printInstDomainValMap;

  friend llvm::AnalysisInfoMixin<SCCP>;
  static llvm::AnalysisKey Key;
  std::map<std::pair<llvm::BasicBlock *, llvm::BasicBlock *>, bool>
  ExecFlags;
  std::map<llvm::BasicBlock *, bool> BBExecFlags;
  std::vector<std::pair<llvm::BasicBlock *,llvm::BasicBlock *>> FlowWL;
  std::vector<llvm::Instruction *> SSAWL;

  virtual std::string getName() const override { return "SCCP"; }
  virtual bool transferFunc(const llvm::Instruction &, const DomainVal_t &,
                            DomainVal_t &) override;
  virtual void
  printInstDomainValMap(const llvm::Instruction &Inst) const override;

public:
  using Result = typename ForwardAnalysis_t::AnalysisResult_t;
  virtual Result run(llvm::Function &F,
                     llvm::FunctionAnalysisManager &FAM) override;
};

class SCCPWrapperPass : public llvm::PassInfoMixin<SCCPWrapperPass> {
public:
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &FAM) {

    /// @done(CSCD70) Get the result from the main body.
    FAM.getResult<SCCP>(F);
    return llvm::PreservedAnalyses::all();
  }
};
