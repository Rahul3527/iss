// main.cpp
// build with:
//g++ Simulator.cpp -std=c++11
// clang++ -g Simulator.cpp `llvm-config --cppflags --ldflags --libs` -std=c++11 -ldl -lpthread -o sim 
#include <random>
#include <fstream>
#include <iostream>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Attributes.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>
#include "llvm/Support/TargetSelect.h"
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/RTDyldMemoryManager.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/PassManager.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/time.h>
#include "Inst.h"

using namespace std;

struct BasicBlock
{
  int32_t pc; //
  void *func_ptr;
};

class Simulator
{
  public:
  Simulator(llvm::LLVMContext &context);
  void LoadBinary(string filename);
  void Simulate(llvm::LLVMContext &context);

  private:
    CInst *instructions;
    int32_t total_inst; 
    int32_t total_blocks;

    llvm::Module *MyModule;
    llvm::ExecutionEngine* EE;
    llvm::PassManager *passManager;
    llvm::FunctionPassManager *fPassManager;
  
    std::map<OPCODE, llvm::Function*> OpFunctionMap;
    BasicBlock *Blocks;
    //llvm::LLVMContext &context;
};

Simulator::Simulator(llvm::LLVMContext &context)
{
  using namespace llvm;
  InitOperations();
  //context = llvm::getGlobalContext();
  SMDiagnostic error;
  std::unique_ptr<Module> M = llvm::parseIRFile("Behavior.bc", error, context);
  MyModule = M.get();
  if(!MyModule)
  {
    std::string what;
    llvm::raw_string_ostream os(what);
    error.print("error after parseIR()", os);
    std::cerr << what;
  } // end if
  else {
    //MyModule->dump();
    EE = llvm::EngineBuilder(std::move(M)).create();
    //string ErrStr;
    //EE = llvm::EngineBuilder(std::move(M)).setErrorStr(&ErrStr).setMCPU("i386").create();
    EE->DisableLazyCompilation(true);

    passManager = new llvm::PassManager();
    passManager->add(createAlwaysInlinerPass());
    fPassManager = new llvm::FunctionPassManager(MyModule);
    //fPassManager->add(new DataLayout(MyModule));
    //fPassManager->add(new DataLayout(*EE->getDataLayout()));
    fPassManager->add(createGVNPass());
    fPassManager->add(createInstructionCombiningPass());
    fPassManager->add(createCFGSimplificationPass());
    fPassManager->add(createDeadStoreEliminationPass());
    for (int32_t i = 0; i < (int32_t)OPCODE::INVALID; i++) {
      OPCODE op = (OPCODE) i;
      llvm::Function *func = MyModule->getFunction(GetFuncName(op));
      OpFunctionMap[op] = func; 
    }
  }
}

void Simulator::Simulate(llvm::LLVMContext &context)
{
  int32_t index = 0;
  Blocks = new BasicBlock[1000];
  int32_t block_index = 0;
  while(index < total_inst) {

    Blocks[block_index].pc = index;  
    ostringstream f_name;
    f_name << "func";
    f_name << block_index;
    //string f_name = string("func");// + string(index);
    llvm::Function *func =
      llvm::cast<llvm::Function>(MyModule->getOrInsertFunction(f_name.str(), llvm::Type::getVoidTy(context), (llvm::Type *)0));

    // Add a basic block to the function. As before, it automatically inserts
    // because of the last argument.
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "EntryBlock", func);

    // Create a basic block builder with default parameters.  The builder will
    // automatically append instructions to the basic block `BB'.
    llvm::IRBuilder<> builder(BB);

    builder.SetInsertPoint(BB);
    llvm::Function *sim_func; 
    CInst *inst; 
    int b_size = 0;
    do {
        inst = &instructions[index];
        llvm::Value *dst = builder.getInt32(inst->dst_reg);
        llvm::Value *src0 = builder.getInt32(inst->src0_reg);
        llvm::Value *src1 = builder.getInt32(inst->src1_reg);
        sim_func = OpFunctionMap[inst->opcode];
        llvm::Value *oprnds[] = {dst, src0, src1};
        llvm::ArrayRef <llvm::Value *> ref(oprnds, 3);
        builder.CreateCall(sim_func, ref);
        index++;
        b_size++;
        //cout << "Index " << index << endl;
    //} while(b_size < 10 || inst->opcode != OPCODE::JMP);
    } while(b_size < 10  && index < total_inst);
    builder.CreateRetVoid();  
    fPassManager->run(*func);
    EE->finalizeObject();
    passManager->run(*MyModule); 

    Blocks[block_index].func_ptr = EE->getPointerToFunction(func);
    block_index++;
    //cout << "BlockIndex " << block_index << endl;
  }
  total_blocks = block_index;
  MyModule->dump();
  struct timeval tp;
  gettimeofday(&tp, NULL);
  long int start = tp.tv_sec * 1000 + tp.tv_usec / 1000;
  for (int32_t i = 0; i < 100; i++) {
    for (int32_t j = 0 ; j < total_blocks; j++) {
      void (*func)() = reinterpret_cast<void (*)()>(Blocks[j].func_ptr);
      func(); 
    }
  }
  gettimeofday(&tp, NULL);
  long int end = tp.tv_sec * 1000 + tp.tv_usec / 1000;
  cout << "Time = " << end - start << endl; 
}
void Simulator::LoadBinary(string filename)
{
  ifstream infile;
  infile.open(filename.c_str(), ios::in|ios::binary); 
  instructions = new CInst[100000];
  int32_t index = 0;
  while (!infile.eof()) {
    infile.read ( (char *)&instructions[index], sizeof(CInst) );
    index++;
  }
  infile.close();   
  total_inst = index; 
  /*
  for (int32_t i = 0; i < total_inst; i++) {
    instructions[i].Print();
  }
  */
}

int32_t main(int32_t args, char* argv[])
{
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::LLVMContext &context = llvm::getGlobalContext();
  Simulator sim(context);
  sim.LoadBinary("t.bin");
  sim.Simulate(context);
}


