/**
 * \file StaticModelFilter.h
 */

#ifdef ENABLE_CLANG_SUPPORT
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Tooling/Tooling.h>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/TargetSelect.h>

#include <ATK/Core/BaseFilter.h>

#include "StaticModelFilter.h"

namespace ATK
{
  template<typename DataType>
  StaticModelFilterGenerator<DataType>::~StaticModelFilterGenerator()
  {
  }
  
  template<typename DataType>
  void StaticModelFilterGenerator<DataType>::parseString(const std::string& fullfile)
  {
    llvm::LLVMContext context;
    
    std::unique_ptr<clang::CodeGenAction> action = std::make_unique<clang::EmitLLVMOnlyAction>(&context);
    clang::tooling::runToolOnCode/*WithArgs*/(action.get(), "int foo(int x){ return ++x;}");
    
    std::unique_ptr<llvm::Module> module = action->takeModule();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    llvm::EngineBuilder builder(std::move(module));
    builder.setMCJITMemoryManager(std::make_unique<llvm::SectionMemoryManager>());
    std::unique_ptr<llvm::ExecutionEngine> EE(builder.create());
    
    auto fn = reinterpret_cast<int (*)(int)>(EE->getFunctionAddress("foo"));
  }

  template<typename DataType>
  std::unique_ptr<BaseFilter> StaticModelFilterGenerator<DataType>::generateDynamicFilter() const
  {
    return std::unique_ptr<BaseFilter>();
  }
}

#endif

