
FILE(GLOB ATK_MODELLING_SRC *.cpp)
FILE(GLOB ATK_MODELLING_HEADERS *.h)

SET(ATK_MODELLING_LIBRARIES ${ATK_CORE_LIBRARY})

if(ENABLE_CLANG_SUPPORT)
  LIST(APPEND ATK_MODELLING_DEFINITIONS -DENABLE_CLANG_SUPPORT)
  LIST(APPEND ATK_MODELLING_LIBRARIES
    LLVMAnalysis
    LLVMAsmParser
    LLVMAsmPrinter
    LLVMBinaryFormat
    LLVMBitReader
    LLVMBitWriter
    LLVMCodeGen
    LLVMCore
    LLVMCoroutines
    LLVMCoverage
    LLVMDebugInfoCodeView
    LLVMDemangle
    LLVMExecutionEngine
    LLVMGlobalISel
    LLVMInstCombine
    LLVMInstrumentation
    LLVMIRReader
    LLVMLinker
    LLVMLTO
    LLVMMC
    LLVMMCDisassembler
    LLVMMCJIT
    LLVMMCParser
    LLVMObjCARCOpts
    LLVMObject
    LLVMOption
    LLVMPasses
    LLVMProfileData
    LLVMRuntimeDyld
    LLVMScalarOpts
    LLVMSelectionDAG
    LLVMSupport
    LLVMTarget
    LLVMTransformUtils
    LLVMVectorize
    LLVMX86AsmParser
    LLVMX86AsmPrinter
    LLVMX86CodeGen
    LLVMX86Desc
    LLVMX86Info
    LLVMX86Utils
    LLVMipo
    clangAnalysis
    clangAST
    clangBasic
    clangCodeGen
    clangDriver
    clangEdit
    clangFrontend
    clangLex
    clangParse
    clangSema
    clangSerialization
    clangTooling
    ncurses
    z
  )
endif(ENABLE_CLANG_SUPPORT)
SET(ATK_MODELLING_INCLUDE ${ATK_INCLUDE_DIRS} ${Boost_INCLUDE_DIR})

if(ENABLE_LOG)
  LIST(APPEND ATK_MODELLING_DEFINITIONS -DENABLE_LOG)
  LIST(APPEND ATK_MODELLING_LIBRARIES ${Boost_LOG_LIBRARY})
endif(ENABLE_LOG)

SET(ATK_MODELLING_NAME ATKModelling)
SET(ATK_MODELLING_FOLDER Modelling)

ATK_add_library(ATK_MODELLING)
