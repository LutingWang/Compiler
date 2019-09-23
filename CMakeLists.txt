cmake_minimum_required(VERSION 3.7.1)
 
set (CMAKE_C_COMPILER "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang")
set (CMAKE_CXX_COMPILER "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++")
project(compiler)

set (Compiler_VERSION_MAJOR 1)
set (Compiler_VERSION_MINOR 0)

set (CMAKE_CXX_STANDARD 11)
set (PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

option (OUTPUT_LEX "Output lexer result" ON)

configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/compilerConfig.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/compilerConfig.h"
)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")
add_subdirectory ("${PROJECT_SOURCE_DIR}")

