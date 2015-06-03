// main.cpp
// build with:
// clang++ BitcodeLoader.cpp `llvm-config --cppflags --ldflags --libs` -std=c++11 -ldl -lpthread
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>
#include "llvm/Support/TargetSelect.h"
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/RTDyldMemoryManager.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>

#include <fstream>
#include <iostream>

using namespace std;
//#define USE_MCJIT
#define USE_JIT

#include "MCJITLoader.h"
#include "JITLoader.h"

int32_t main(int32_t args, char* argv[])
{
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::LLVMContext &context = llvm::getGlobalContext();
  llvm::Module *m = load_module(string(argv[1]), context);
  if(m)
  {
    cout << "Gotcha..!!!!!!" << endl;
  }
  return 0;
}

/*
int32_t JitFunction(std::unique_ptr<llvm::Module> &M)
{
  //std::unique_ptr<llvm::RTDyldMemoryManager> MemMgr(new llvm::SectionMemoryManager());
  llvm::RTDyldMemoryManager *MemMgr = new llvm::SectionMemoryManager();
  if (!MemMgr) {
    cout << "Unable to create memory manager.";
    return -1;
  }
  llvm::Module *Mod = M.get();


  cout << "In JIT 1" << endl;
    // Build engine with JIT
  std::string err;
  llvm::EngineBuilder factory(std::move(M));
  factory.setErrorStr(&err);
  factory.setEngineKind(llvm::EngineKind::JIT);
  //factory.setUseMCJIT(true);
  factory.setMCJITMemoryManager(std::unique_ptr<llvm::RTDyldMemoryManager>(MemMgr));
  llvm::ExecutionEngine *m_EE = factory.create();
  cout << "In JIT 2" << endl;

  //llvm::sys::DynamicLibrary::AddSymbol("_Z4testv", reinterpret_cast<void*>(test));

  llvm::Function* f = Mod->getFunction("add32");
   if (f) {
      cout << "JIT Dumping add32 " << endl;
      f->dump();  
      cout << "JIT Done dumping add32" << endl; 
    }
 
  m_EE->finalizeObject();

  cout << "In JIT 3" << endl;
  void* poi = m_EE->getPointerToFunction(f);
  cout << "In JIT 4" << endl;
  void (*add32)(int32_t, int32_t, int32_t) = reinterpret_cast<void (*)(int32_t, int32_t, int32_t)>(poi); 

  try {
      add32(0, 2, 3);
  } catch (int e) {
      std::cout << "catched " << e << std::endl;
  }
  return 0;
}

*/
