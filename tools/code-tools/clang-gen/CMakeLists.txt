execute_process (
    COMMAND llvm-config-3.8 --libs
    OUTPUT_VARIABLE LLVM_LIBRARIES
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

coffee_application(
    TARGET LCodeGenerator
    TITLE "LLVM Code Generator"
    SOURCES main.cpp
    LIBRARIES
    clangTooling
    clangFrontendTool
    clangFrontend
    clangDriver
    clangSerialization
    clangCodeGen
    clangParse
    clangSema
    clangStaticAnalyzerFrontend
    clangStaticAnalyzerCheckers
    clangStaticAnalyzerCore
    clangAnalysis
    clangARCMigrate
    clangRewrite
    clangRewriteFrontend
    clangEdit
    clangAST
    clangLex
    clangBasic

    ${LLVM_LIBRARIES}
    curses
    )

target_include_directories ( LCodeGenerator
    PRIVATE
    /usr/lib/llvm-3.8/include
    /usr/include/llvm-3.8
    )

target_compile_definitions ( LCodeGenerator
    PRIVATE
    -D__STDC_CONSTANT_MACROS
    -D__STDC_LIMIT_MACROS
    )

target_compile_features ( LCodeGenerator
    PRIVATE
    cxx_return_type_deduction
    )
