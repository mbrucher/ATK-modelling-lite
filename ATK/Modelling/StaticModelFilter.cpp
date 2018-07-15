/**
 * \file StaticModelFilter.h
 */

#ifdef ENABLE_CLANG_SUPPORT

#include <clang/AST/ASTContext.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/Basic/DiagnosticOptions.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/FileManager.h>
#include <clang/Basic/FileSystemOptions.h>
#include <clang/Basic/LangOptions.h>
#include <clang/Basic/MemoryBufferCache.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Lex/HeaderSearch.h>
#include <clang/Lex/HeaderSearchOptions.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/PreprocessorOptions.h>
#include <clang/Parse/ParseAST.h>
#include <clang/Sema/Sema.h>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

#include <ATK/Core/BaseFilter.h>

#include "StaticModelFilter.h"

namespace
{
  std::unique_ptr<llvm::ExecutionEngine> EE;
}

namespace ATK
{
  template<typename DataType>
  StaticModelFilterGenerator<DataType>::~StaticModelFilterGenerator()
  {
  }
  
  template<typename DataType>
  std::unique_ptr<BaseFilter> StaticModelFilterGenerator<DataType>::generateDynamicFilter() const
  {
    return std::unique_ptr<BaseFilter>();
  }

  template<typename Function>
  Function parseString(const std::string& fullfile, const std::string& function)
  {
    clang::DiagnosticOptions diagnosticOptions;
    std::unique_ptr<clang::TextDiagnosticPrinter> textDiagnosticPrinter =
      std::make_unique<clang::TextDiagnosticPrinter>(llvm::outs(),
                                                     &diagnosticOptions);
    llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> diagIDs;
    
    std::unique_ptr<clang::DiagnosticsEngine> diagnosticsEngine =
      std::make_unique<clang::DiagnosticsEngine>(diagIDs, &diagnosticOptions, textDiagnosticPrinter.get());
    
    clang::LangOptions languageOptions;
    clang::FileSystemOptions fileSystemOptions;
    clang::FileManager fileManager(fileSystemOptions);
    clang::SourceManager sourceManager(*diagnosticsEngine,
                                       fileManager);
    std::shared_ptr<clang::HeaderSearchOptions> headerSearchOptions(new clang::HeaderSearchOptions());
    
    const std::shared_ptr<clang::TargetOptions> targetOptions = std::make_shared<clang::TargetOptions>();
    targetOptions->Triple = llvm::sys::getDefaultTargetTriple();

    std::unique_ptr<clang::TargetInfo> targetInfo(
      clang::TargetInfo::CreateTargetInfo(*diagnosticsEngine, targetOptions));

    clang::HeaderSearch headerSearch(headerSearchOptions,
                                     sourceManager,
                                     *diagnosticsEngine,
                                     languageOptions,
                                     targetInfo.get());
    clang::MemoryBufferCache PCMCache;
    clang::CompilerInstance compInst;

    std::shared_ptr<clang::PreprocessorOptions> opts(std::make_shared<clang::PreprocessorOptions>());
    clang::Preprocessor preprocessor(opts,
                                     *diagnosticsEngine,
                                     languageOptions,
                                     sourceManager,
                                     PCMCache,
                                     headerSearch,
                                     compInst);
    preprocessor.Initialize(*targetInfo);

    auto filter = llvm::MemoryBuffer::getMemBufferCopy(fullfile);

    sourceManager.setMainFileID(sourceManager.createFileID(std::move(filter)));

    clang::IdentifierTable identifierTable(languageOptions);
    clang::SelectorTable selectorTable;

    clang::Builtin::Context builtinContext;
    builtinContext.InitializeTarget(*targetInfo, nullptr);
    clang::ASTContext astContext(languageOptions,
                                 sourceManager,
                                 identifierTable,
                                 selectorTable,
                                 builtinContext);
    astContext.InitBuiltinTypes(*targetInfo);
    compInst.setTarget(targetInfo.get());

    llvm::LLVMContext context;
    std::unique_ptr<clang::CodeGenAction> action = std::make_unique<clang::EmitLLVMAction>(&context);
    
    textDiagnosticPrinter->BeginSourceFile(languageOptions, &preprocessor);

    compInst.ExecuteAction(*action);

    std::unique_ptr<llvm::Module> module = action->takeModule();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    
    llvm::EngineBuilder builder(std::move(module));
    builder.setMCJITMemoryManager(std::make_unique<llvm::SectionMemoryManager>());
    builder.setOptLevel(llvm::CodeGenOpt::Level::Aggressive);
    EE.reset(builder.create());
    
    return reinterpret_cast<Function>(EE->getFunctionAddress(function));
  }
  
  typedef int (*IntInt)(int);
  template IntInt parseString<IntInt>(const std::string& fullfile, const std::string& function);
}

#endif

