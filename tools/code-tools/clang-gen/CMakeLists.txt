project(LCodeGenerator C CXX)
cmake_minimum_required(VERSION 3.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

execute_process(
  COMMAND llvm-config-3.8 --libs
  OUTPUT_VARIABLE LLVM_LIBRARIES
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_executable(LCodeGenerator main.cpp)

add_library(CodeWrapper STATIC wrapping.cpp)

target_link_libraries(
  LCodeGenerator
  PRIVATE clangTooling
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
          dl
          z
          rt
          pthread
          m
)

target_include_directories(
  LCodeGenerator PRIVATE /usr/lib/llvm-3.8/include /usr/include/llvm-3.8
)

target_compile_definitions(
  LCodeGenerator PRIVATE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS
)

target_compile_features(LCodeGenerator PRIVATE cxx_return_type_deduction)

target_compile_features(CodeWrapper PRIVATE cxx_return_type_deduction)

target_include_directories(
  LCodeGenerator PRIVATE ${CMAKE_SOURCE_DIR}/../../../include
)

target_include_directories(
  CodeWrapper PRIVATE ${CMAKE_SOURCE_DIR}/../../../include
)
