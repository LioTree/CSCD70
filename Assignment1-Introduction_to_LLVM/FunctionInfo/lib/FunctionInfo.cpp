#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace {

class FunctionInfoPass final : public PassInfoMixin<FunctionInfoPass> {
public:
  PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {
    outs() << "CSCD70 Function Information Pass"
           << "\n";

    /// @done(CSCD70) Please complete this method.
    for (Function &F : M) {
      // 1. Name
      StringRef functionName = F.getName();
      outs() << "Function Name: " << functionName << "\n";

      // 2. Number of Arguments
      unsigned int numArgs = F.arg_size();
      if (F.isVarArg())
        outs() << "Number of Arguments: " << numArgs << "+*\n";
      else
        outs() << "Number of Arguments: " << numArgs << "\n";

      // 3. Number of Direct Call Sites
      unsigned int numDirectCalls = 0;
      for (User *U : F.users()) {
        if (Instruction *I = dyn_cast<Instruction>(U)) {
            numDirectCalls++;
        }
      }
      outs() << "Number of Calls: " << numDirectCalls << "\n";

      // 4. Number of Basic Blocks
      unsigned int numBasicBlocks = 0;
      for (BasicBlock &BB : F) {
        numBasicBlocks++;
      }
      outs() << "Number OF BBs: " << numBasicBlocks << "\n";

      // 5. Number of Instructions
      unsigned int numInstructions = 0;
      for (BasicBlock &BB : F) {
        numInstructions += BB.size();
      }
      outs() << "Number of Instructions: " << numInstructions << "\n";
    }

    return PreservedAnalyses::all();
  }
}; // class FunctionInfoPass

} // anonymous namespace

extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
      .APIVersion = LLVM_PLUGIN_API_VERSION,
      .PluginName = "FunctionInfo",
      .PluginVersion = LLVM_VERSION_STRING,
      .RegisterPassBuilderCallbacks =
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) -> bool {
                  if (Name == "function-info") {
                    MPM.addPass(FunctionInfoPass());
                    return true;
                  }
                  return false;
                });
          } // RegisterPassBuilderCallbacks
  };        // struct PassPluginLibraryInfo
}
