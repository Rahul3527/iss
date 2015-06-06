// main.cpp
// build with:
//g++ Simulator.cpp -std=c++11
// clang++ -g Simulator.cpp `llvm-config --cppflags --ldflags --libs` -std=c++11 -ldl -lpthread -o sim 
#include <random>
#include <fstream>
#include <iostream>
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
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/RTDyldMemoryManager.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/PassManager.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar.h>
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
  HostFunction func_ptr;
};

class Simulator
{
  public:
  Simulator();
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

Simulator::Simulator():
  context(llvm::getGlobalContext()),
  IRB(context)
{
  using namespace llvm;
  InitOperations();
  //context = llvm::getGlobalContext();
  SMDiagnostic error;
  std::unique_ptr<Module> M = llvm::parseIRFile("Behavior.bc", error, context);
  MyModule = M.get();
  string err_str;
/*
    OwningPtr<MemoryBuffer> result;
    MemoryBuffer *mb;
    llvm::error_code ec = MemoryBuffer::getFile("Behavior.bc", result);
    mb = result.take();
    err_str = ec.message();
    if (!mb) {
      error() <<"Cannot open \"" <<bitcode_file() <<"\": " <<err_str <<endl;
      exit(1);
    }
    MyModule = llvm::ParseBitcodeFile(mb,context,&err_str);
    if (!MyModule) {
      error() <<"Failed to load module from bitcode file: " <<err_str <<endl;
      exit(1);
    }
    delete mb;
*/
/*
    for (llvm::Module::iterator f = MyModule->begin(), ef = MyModule->end(); f!=ef; ++f) 
    {
      f->addFnAttr(Attributes::AlwaysInline);
    }
*/
    for (int32_t i = 0; i < (int32_t)OPCODE::INVALID; i++) {
      OPCODE op = (OPCODE) i;
      llvm::Function *func = MyModule->getFunction(GetFuncName(op));
      func->addFnAttr(Attribute::AlwaysInline);
      OpFunctionMap[op] = func; 
    }
    test_type = MyModule->getFunction("test")->getFunctionType();


    passManager = new llvm::PassManager();
    passManager->add(createAlwaysInlinerPass());
    fPassManager = new llvm::FunctionPassManager(MyModule);
    //fPassManager->add(new DataLayout(MyModule));
    //fPassManager->add(new DataLayout(*EE->getDataLayout()));
    fPassManager->add(createGVNPass());
    fPassManager->add(createInstructionCombiningPass());
    fPassManager->add(createCFGSimplificationPass());
    fPassManager->add(createDeadStoreEliminationPass());


    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    //atexit(llvm_shutdown);  // Call llvm_shutdown() on exit.
    //llvm::EngineBuilder builder(MyModule);
    llvm::EngineBuilder builder(std::move(M));
    //llvm::EngineBuilder builder(MyModule);
    builder.setErrorStr(&err_str);
    builder.setEngineKind(EngineKind::JIT);
    builder.setOptLevel(CodeGenOpt::Default); // None/Less/Default/Aggressive
    //TargetOptions options;
    //options.JITExceptionHandling = 1;
    //builder.setTargetOptions(options);
    EE = builder.create();
    if (!EE) {
      std::cout <<"failed to create execution engine: " <<err_str <<"\n";
      exit(1);
    }

/*
    //MyModule->dump();
    EE = llvm::EngineBuilder(std::move(M)).create();
*/
    //string ErrStr;
    //EE = llvm::EngineBuilder(std::move(M)).setErrorStr(&ErrStr).setMCPU("i386").create();
    EE->DisableLazyCompilation(true);
}

void Simulator::Simulate()
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
  /*
    llvm::Function *func =
      llvm::cast<llvm::Function>(MyModule->getOrInsertFunction(f_name.str(), 
            llvm::Type::getVoidTy(context), (llvm::Type *)0));
  */
    llvm::Function *func = llvm::Function::Create(test_type, 
                                                  llvm::Function::ExternalLinkage,
                                                  f_name.str(),
                                                  MyModule);
    // Add a basic block to the function. As before, it automatically inserts
    // because of the last argument.
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "EntryBlock", func);

    // Create a basic block builder with default parameters.  The builder will
    // automatically append instructions to the basic block `BB'.
    IRB.SetInsertPoint(BB);
    llvm::Function *sim_func; 
    CInst *inst; 
    int b_size = 0;
    do {
        inst = &instructions[index];
        llvm::Value *dst = IRB.getInt32(inst->dst_reg);
        llvm::Value *src0 = IRB.getInt32(inst->src0_reg);
        llvm::Value *src1 = IRB.getInt32(inst->src1_reg);
        sim_func = OpFunctionMap[inst->opcode];
        llvm::Value *oprnds[] = {dst, src0, src1};
        llvm::ArrayRef <llvm::Value *> ref(oprnds, 3);
        IRB.CreateCall(sim_func, ref);
        index++;
        b_size++;
        //cout << "Index " << index << endl;
    //} while(b_size < 10 || inst->opcode != OPCODE::JMP);
    } while(b_size < 10  && index < total_inst);
    IRB.CreateRetVoid();  
    passManager->run(*MyModule); 
    fPassManager->run(*func);
    EE->finalizeObject();

    Blocks[block_index].func_ptr = reinterpret_cast<HostFunction>(EE->getPointerToFunction(func));
    std::cout << "calling " << f_name.str() << endl;
    (Blocks[block_index].func_ptr)(); 
    block_index++;
    //cout << "BlockIndex " << block_index << endl;
  }
  total_blocks = block_index;
  //MyModule->dump();
  struct timeval tp;
  gettimeofday(&tp, NULL);
  long int start = tp.tv_sec * 1000 + tp.tv_usec / 1000;
  for (int32_t i = 0; i < 100; i++) {
    for (int32_t j = 0 ; j < total_blocks; j++) {
      (Blocks[j].func_ptr)();
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
  Simulator sim;
  sim.LoadBinary("t.bin");
  sim.Simulate();
}


