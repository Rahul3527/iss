//===-- examples/HowToUseJIT/HowToUseJIT.cpp - An example use of the JIT --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This small program provides an example of how to quickly build a small
//  module with two functions and execute it with the JIT.
//
// Goal:
//  The goal of this snippet is to create in the memory
//  the LLVM module consisting of two functions as follow: 
//
// int add1(int x) {
//   return x+1;
// }
//
// int foo() {
//   return add1(10);
// }
//
// then compile the module via JIT, then execute the `foo'
// function and return result to a driver, i.e. to a "host program".
//
// Some remarks and questions:
//
// - could we invoke some code using noname functions too?
//   e.g. evaluate "foo()+foo()" without fears to introduce
//   conflict of temporary function name with some real
//   existing function name?
//
//===----------------------------------------------------------------------===//

#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include <llvm/PassManager.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Target/TargetMachine.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/time.h>
#include "Inst.h"

using namespace llvm;
using namespace std;

typedef void (*HostFunction)(int32_t *);
int32_t srf[32];
llvm::Value *srf_arg;

struct HostBlock
{
  
  int32_t pc; //
  //HostFunction func_ptr;
  void *func_ptr;
};


llvm::Module *MyModule;
llvm::ExecutionEngine* EE;
llvm::PassManager *passManager;
llvm::FunctionPassManager *fPassManager;
    llvm::FunctionType *test_type; 

CInst *instructions;
int32_t total_inst; 
int32_t total_blocks;
std::map<OPCODE, llvm::Function*> OpFunctionMap;
HostBlock *Blocks;

void LoadBinary(string filename)
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
}

void inline AddCalls(int32_t index, llvm::IRBuilder<> &IRB, llvm::LLVMContext &Context)
{
    CInst *inst; 
    inst = &instructions[index];
    //llvm::Value *srf_ptr = IRB.getInt32PtrTy(Context); 
    llvm::Value *dst = IRB.getInt32(inst->dst_reg);
    llvm::Value *src0 = IRB.getInt32(inst->src0_reg);
    llvm::Value *src1 = IRB.getInt32(inst->src1_reg);
    llvm::Function *sim_func = OpFunctionMap[inst->opcode];
    //llvm::Value *oprnds[] = {proc, dst, src0, src1};
    //llvm::Value *oprnds[] = {srf_arg, dst, src0, src1};
    //llvm::ArrayRef <llvm::Value *> ref(oprnds, 4);
    //IRB.CreateCall(sim_func, ref);
    IRB.CreateCall4(sim_func, srf_arg, dst, src0, src1);
}

void GenerateCode(llvm::LLVMContext &Context)
{
  int32_t index = 0;
  Blocks = new HostBlock[1000];
  int32_t block_index = 0;

  llvm::IRBuilder<> IRB(Context);

  while(index < 210) {

    Blocks[block_index].pc = index;  
    ostringstream f_name;
    f_name << "func";
    f_name << block_index;

/*
  Function *func =
    cast<Function>(MyModule->getOrInsertFunction(f_name.str().c_str(), 
                                          Type::getVoidTy(Context),
                                          Type::getInt32PtrTy(Context),
                                          (Type *)0));
*/
    Function *func = llvm::Function::Create(test_type, 
                                      Function::ExternalLinkage,
                                      f_name.str().c_str(), 
                                      MyModule);

    llvm::Function::arg_iterator arg0 = func->arg_begin();
    arg0->setName("s");
    srf_arg = arg0;
//Function *F = Function::Create(FT, Function::ExternalLinkage, FnName, M);
    // Add a basic block to the function. As before, it automatically inserts
    // because of the last argument.
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(Context, "EntryBlock", func);

    // Create a basic block builder with default parameters.  The builder will
    // automatically append instructions to the basic block `BB'.
    IRB.SetInsertPoint(BB);
    CInst *inst; 
    int b_size = 0;
    do {
        AddCalls(index, IRB, Context);
        llvm::Function *Add1F = MyModule->getFunction("add1");
        Value *Ten = IRB.getInt32(10);
        IRB.CreateCall(Add1F, Ten);
        index++;
        b_size++;
    } while(b_size < 10  && index < total_inst);
    IRB.CreateRetVoid();  
    passManager->run(*MyModule); 
    fPassManager->run(*func);
    Blocks[block_index].func_ptr = (void *)(EE->getPointerToFunction(func));
    block_index++;
  }
  EE->finalizeObject();
  total_blocks = block_index;
  MyModule->dump();

}

void AddSample(LLVMContext &Context)
{
  // Create some module to put our function into it.
  //Module *M = new Module("test", Context);

  // Create the add1 function entry and insert this entry into module M.  The
  // function will have a return type of "int" and take an argument of "int".
  // The '0' terminates the list of argument types.
  Function *Add1F =
    cast<Function>(MyModule->getOrInsertFunction("add1", Type::getInt32Ty(Context),
                                          Type::getInt32Ty(Context),
                                          (Type *)0));

  // Add a basic block to the function. As before, it automatically inserts
  // because of the last argument.
  BasicBlock *BB = BasicBlock::Create(Context, "EntryBlock", Add1F);

  // Create a basic block builder with default parameters.  The builder will
  // automatically append instructions to the basic block `BB'.
  IRBuilder<> builder(BB);

  // Get pointers to the constant `1'.
  Value *One = builder.getInt32(1);

  // Get pointers to the integer argument of the add1 function...
  assert(Add1F->arg_begin() != Add1F->arg_end()); // Make sure there's an arg
  Argument *ArgX = Add1F->arg_begin();  // Get the arg
  ArgX->setName("AnArg");            // Give it a nice symbolic name for fun.

  // Create the add instruction, inserting it into the end of BB.
  Value *Add = builder.CreateAdd(One, ArgX);
  // Create the return instruction and add it to the basic block
  builder.CreateRet(Add);

  Add1F->addFnAttr(llvm::Attribute::AlwaysInline);
  // Now, function add1 is ready.


  // Now we're going to create function `foo', which returns an int and takes no
  // arguments.
  Function *FooF =
    cast<Function>(MyModule->getOrInsertFunction("foo", Type::getInt32Ty(Context),
                                          (Type *)0));

  // Add a basic block to the FooF function.
  BB = BasicBlock::Create(Context, "EntryBlock", FooF);

  // Tell the basic block builder to attach itself to the new basic block
  builder.SetInsertPoint(BB);

  // Get pointer to the constant `10'.
  Value *Ten = builder.getInt32(10);

  // Pass Ten to the call to Add1F
  CallInst *Add1CallRes = builder.CreateCall(Add1F, Ten);
  Add1CallRes->setTailCall(true);

  // Create the return instruction and add it to the basic block.
  builder.CreateRet(Add1CallRes);
}

int main() {
  
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
  llvm::LLVMContext &Context = llvm::getGlobalContext();

  llvm::SMDiagnostic error;
  

  MyModule = llvm::ParseIRFile("Behavior.bc", error, Context);

  std::string ErrStr;

  //EE = llvm::EngineBuilder(MyModule).create(); 
  llvm::EngineBuilder builder(MyModule);
  builder.setErrorStr(&ErrStr);
  builder.setEngineKind(llvm::EngineKind::JIT);
  builder.setOptLevel(llvm::CodeGenOpt::Default); // None/Less/Default/Aggressive       
  llvm::TargetOptions options;                                                          
  //options.JITExceptionHandling = 1;                                               
  builder.setTargetOptions(options);                                              
  EE = builder.create();

  if (!EE) {
    fprintf(stderr, "Could not create ExecutionEngine: %s\n", ErrStr.c_str());
    exit(1);
  }
  //M = new Module("test", Context);
    passManager = new llvm::PassManager();
    passManager->add(llvm::createAlwaysInlinerPass());
    fPassManager = new llvm::FunctionPassManager(MyModule);
    fPassManager->add(new DataLayout(*EE->getDataLayout()));
    //fPassManager->add(new DataLayout(*EE->getDataLayout()));
    fPassManager->add(llvm::createGVNPass());
    fPassManager->add(llvm::createInstructionCombiningPass());
    fPassManager->add(llvm::createCFGSimplificationPass());
    fPassManager->add(llvm::createDeadStoreEliminationPass());


  EE->DisableLazyCompilation(true);

  InitOperations();
  EE->finalizeObject();
    for (int32_t i = 0; i < (int32_t)OPCODE::INVALID; i++) {
      OPCODE op = (OPCODE) i;
      llvm::Function *func = MyModule->getFunction(GetFuncName(op));
      func->addFnAttr(llvm::Attribute::AlwaysInline);
      //void *fn_addr = EE->getPointerToNamedFunction(GetFuncName(op));
      //void *fn_addr = (void*)EE->getFunctionAddress(GetFuncName(op));
      //EE->addGlobalMapping(func, fn_addr);
      OpFunctionMap[op] = func; 
    }
  test_type = MyModule->getFunction("test")->getFunctionType();
  AddSample(Context);
  llvm::Function *FooF = MyModule->getFunction("foo");
  // Call the `foo' function with no arguments:
  std::vector<GenericValue> noargs;
  GenericValue gv = EE->runFunction(FooF, noargs);

  // Import result of execution:
  outs() << "Result: " << gv.IntVal << "\n";

  //outs() << "We just constructed this LLVM module:\n\n" << *M;
  outs() << "\n\nRunning foo: ";
  outs().flush();

  LoadBinary("t.bin");
  GenerateCode(Context);

  //Function *func0 = M->getFunction("func0");
  std::vector<GenericValue> noargsfunc;
  //GenericValue gvfunc = EE->runFunction(func0, noargsfunc);
  // Now we create the JIT.
  //ExecutionEngine* EE = EngineBuilder(M).create();
  struct timeval tp;
  gettimeofday(&tp, NULL);
  long int start = tp.tv_sec * 1000 + tp.tv_usec / 1000;
  for (int32_t j = 0; j < 10000; j++) {
 
  for (int i = 0; i < 20; i++) {
    ostringstream f_name;
    f_name << "func";
    f_name << i;
    //Function *f0 = M->getFunction(f_name.str().c_str());
    //cout << "calling " << f_name.str() << endl;
    ((HostFunction)Blocks[i].func_ptr)(srf);
    //GenericValue gvfunc = EE->runFunction(f0, noargsfunc);
  }
  }
  gettimeofday(&tp, NULL);
  long int end = tp.tv_sec * 1000 + tp.tv_usec / 1000;
  cout << "Time = " << end - start << endl; 

  EE->freeMachineCodeForFunction(FooF);
  delete EE;
  llvm_shutdown();
  return 0;
}
