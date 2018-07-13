/**
 * \file StaticModelFilter.h
 */

#ifdef ENABLE_CLANG_SUPPORT
//#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>

//#include <clang/AST/ASTContext.h>
//#include <clang/AST/ASTConsumer.h>
//#include <clang/AST/Decl.h>
//#include <clang/AST/Expr.h>
//#include <clang/AST/Stmt.h>
#include <clang/Basic/DiagnosticOptions.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/FileManager.h>
#include <clang/Basic/FileSystemOptions.h>
#include <clang/Basic/LangOptions.h>
#include <clang/Basic/MemoryBufferCache.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Lex/HeaderSearch.h>
#include <clang/Lex/HeaderSearchOptions.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/PreprocessorOptions.h>

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
    clang::DiagnosticOptions diagnosticOptions;
    std::unique_ptr<clang::TextDiagnosticPrinter> pTextDiagnosticPrinter =
      std::make_unique<clang::TextDiagnosticPrinter>(
                                     llvm::outs(),
                                     diagnosticOptions);
    llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> pDiagIDs;
    
    std::unique_ptr<clang::DiagnosticsEngine> pDiagnosticsEngine =
      std::make_unique<clang::DiagnosticsEngine>(pDiagIDs, pTextDiagnosticPrinter);
    
    clang::LangOptions languageOptions;
    clang::FileSystemOptions fileSystemOptions;
    clang::FileManager fileManager(fileSystemOptions);
    clang::SourceManager sourceManager(
                                       *pDiagnosticsEngine,
                                       fileManager);
    std::shared_ptr<clang::HeaderSearchOptions> headerSearchOptions(new clang::HeaderSearchOptions());
    // Add ATK here and other stuff
    
    const std::shared_ptr<clang::TargetOptions> targetOptions = std::make_shared<clang::TargetOptions>();
    targetOptions->Triple = llvm::sys::getDefaultTargetTriple();
    
    clang::TargetInfo* pTargetInfo =
      clang::TargetInfo::CreateTargetInfo(
                                        *pDiagnosticsEngine,
                                        targetOptions);
    
    clang::HeaderSearch headerSearch(headerSearchOptions,
                                     sourceManager,
                                     *pDiagnosticsEngine,
                                     languageOptions,
                                     pTargetInfo);
    clang::MemoryBufferCache PCMCache;
    clang::CompilerInstance compInst;
    
    std::shared_ptr<clang::PreprocessorOptions> pOpts(std::make_shared<clang::PreprocessorOptions>());
    clang::Preprocessor preprocessor(
                                     pOpts,
                                     *pDiagnosticsEngine,
                                     languageOptions,
                                     sourceManager,
                                     PCMCache,
                                     headerSearch,
                                     compInst);
    preprocessor.Initialize(*pTargetInfo);
  }

  template<typename DataType>
  std::unique_ptr<BaseFilter> StaticModelFilterGenerator<DataType>::generateDynamicFilter() const
  {
    return std::unique_ptr<BaseFilter>();
  }
}

#endif

