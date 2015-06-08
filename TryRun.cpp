// main.cpp
// build with:
//g++ Simulator.cpp -std=c++11
// clang++ -g Simulator.cpp `llvm-config --cppflags --ldflags --libs` -std=c++11 -ldl -lpthread -o sim 
#include <random>
#include <fstream>
#include <iostream>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Bitcode/ReaderWriter.h>                                              
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/DataLayout.h>
//#include <llvm/Support/system_error.h>                                              
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>
#include "llvm/Support/TargetSelect.h"
//#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/JIT.h>
//#include <llvm/ExecutionEngine/RTDyldMemoryManager.h>
//#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/ExecutionEngine/JITMemoryManager.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/PassManager.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Support/ManagedStatic.h>
//#include <llvm/DataLayout.h>     
//#include <llvm/Target/TargetOptions.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/time.h>
#include "Inst.h"

using namespace std;
typedef void (*HostFunction)();

struct BasicBlock
{
  
  int32_t pc; //
  //HostFunction func_ptr;
  void *func_ptr;
};

class Simulator
{
  public:
  Simulator(llvm::LLVMContext &Context);
  ~Simulator() {delete EE; llvm::llvm_shutdown();}
  void LoadBinary(string filename);
  void Simulate();

  private:
    CInst *instructions;
    int32_t total_inst; 
    int32_t total_blocks;

    llvm::LLVMContext &context;
    llvm::IRBuilder<> IRB;
    llvm::Module *MyModule;
    llvm::ExecutionEngine* EE;
    llvm::PassManager *passManager;
    llvm::FunctionPassManager *fPassManager;
    llvm::FunctionType *test_type; 
    std::map<OPCODE, llvm::Function*> OpFunctionMap;
    BasicBlock *Blocks;
    //llvm::LLVMContext &context;
};

Simulator::Simulator(llvm::LLVMContext &Context):
  context(Context),
  IRB(Context)
{
  using namespace llvm;
  InitOperations();
}

void Simulator::Simulate()
{
  llvm::SMDiagnostic error;
  //std::unique_ptr<llvm::Module> M = llvm::ParseIRFile("Behavior.bc", error, context);
  //MyModule = M.get();
  MyModule = llvm::ParseIRFile("t1.ll", error, context);
  EE = llvm::EngineBuilder(MyModule).create(); 
  if (!EE) {
    fprintf(stderr, "Could not create ExecutionEngine\n");
    exit(1);
  }
  //EE->DisableLazyCompilation(true);

  EE->finalizeObject();
  using namespace llvm;
  //void (*add32)(int32_t, int32_t, int32_t) = 
  //  reinterpret_cast<void (*)(int32_t, int32_t, int32_t)>(EE->getFunctionAddress("add32")); 
  //add32(0, 1, 2);
  //cout << "Called add" << endl;
  for (int32_t i = 1; i < 200; i++) {
    ostringstream f_name;
    f_name << "func";
    f_name << i;
    HostFunction func_ptr = 
      reinterpret_cast<decltype(HostFunction())>(EE->getPointerToNamedFunction(f_name.str()));
    func_ptr();
  }
}

int32_t main(int32_t args, char* argv[])
{

  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
  llvm::LLVMContext &Context = llvm::getGlobalContext();
  Simulator sim(Context);
  sim.Simulate();
}


