#ifdef USE_JIT
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

using namespace std;

llvm::Module *load_module(string fileName, llvm::LLVMContext &Context)
{
  llvm::LLVMContext context;// = llvm::getGlobalContext();
  using namespace llvm;
  // parse it
  SMDiagnostic error;
  //std::unique_ptr<Module> M = parseIR(fileName, error, context);
  std::unique_ptr<Module> M = parseIRFile(fileName, error, context);
  Module *MyModule = M.get();
  if(!MyModule)
  {
    std::string what;
    llvm::raw_string_ostream os(what);
    error.print("error after parseIR()", os);
    std::cerr << what;
  } // end if
  else {
    MyModule->dump();
    ExecutionEngine* EE = EngineBuilder(std::move(M)).create();
    EE->DisableLazyCompilation(true);

    llvm::PassManager *pm = new PassManager();
    pm->add(createAlwaysInlinerPass());
    llvm::FunctionPassManager *fpm = new FunctionPassManager(MyModule);
    //fpm->add(new DataLayout(MyModule));
    //fpm->add(new DataLayout(*EE->getDataLayout()));
    fpm->add(createGVNPass());
    fpm->add(createInstructionCombiningPass());
    fpm->add(createCFGSimplificationPass());
    fpm->add(createDeadStoreEliminationPass());


/*
    // Create some module to put our function into it.
    std::unique_ptr<Module> Owner = make_unique<Module>("test", context);
    Module *NewModule = Owner.get();
*/
    // Create the Try1 function entry and insert this entry into module M.  The
    // function will have a return type of "void" and take an argument of "void".
    // The '0' terminates the list of argument types.
    Function *Try1 =
      cast<Function>(MyModule->getOrInsertFunction("Try1", Type::getVoidTy(context),
                                            (Type *)0));

    // Add a basic block to the function. As before, it automatically inserts
    // because of the last argument.
    BasicBlock *BB = BasicBlock::Create(context, "EntryBlock", Try1);

    // Create a basic block builder with default parameters.  The builder will
    // automatically append instructions to the basic block `BB'.
    IRBuilder<> builder(BB);

    builder.SetInsertPoint(BB);

    // Get pointers to the constant `1'.
    Value *One = builder.getInt32(1);
    Value *Zero = builder.getInt32(0);
    Value *Two = builder.getInt32(2);
    Value *Three = builder.getInt32(3);

    Function *add32 = MyModule->getFunction("add32");
    add32->addFnAttr(Attribute::AlwaysInline);
    Function *sub32 = MyModule->getFunction("sub32");
    Function *mul32 = MyModule->getFunction("mul32");
/*
    Function::Create(add32->getFunctionType(), Function::ExternalLinkage, add32->getName(), NewModule); 
    Function::Create(sub32->getFunctionType(), Function::ExternalLinkage, sub32->getName(), NewModule); 
    Function::Create(mul32->getFunctionType(), Function::ExternalLinkage, mul32->getName(), NewModule); 

    Value *add2ops[] = {Two, Two, Zero};
    Value *sub1ops[] = {Two, Two, One};
    Value *mul1ops[] = {Two, Three, One};
    CallInst *Add1CallRes = builder.CreateCall(add32, add1ops);
    CallInst *Add2CallRes = builder.CreateCall(add32, add2ops);
*/
    Value *add1ops[] = {One, Two, Three};
    ArrayRef <Value *> ref(add1ops, 3);
    builder.CreateCall(add32, ref);
    builder.CreateRetVoid();  
    pm->run(*MyModule);
    fpm->run(*Try1);
    EE->finalizeObject();
    // Now we create the JIT.
    //EE->addGlobalMapping(add32, (void *)&::add32);
    //EE->addGlobalMapping(add32, (void *)&add32);
    outs() << "We just constructed this LLVM module:\n\n" << *MyModule;
    outs() << "\n\nRunning Try: \n\n";
    outs().flush();
    // Call the `Try' function with no arguments:
    //std::vector<GenericValue> noargs;
    //GenericValue gv =  EE->runFunction(Try1, noargs);
    
    void (*try1)() = 
  using namespace llvm;
      reinterpret_cast<void (*)()>(EE->getPointerToFunction(Try1));
    try1(); 
    delete EE;
  }
  return NULL;
}
#endif // end of USE_JIT
