# **************************************************************
# *         C++ Mathematical Expression Toolkit Library        *
# *                                                            *
# * Author: Arash Partow (1999-2023)                           *
# * URL: https://www.partow.net/programming/exprtk/index.html  *
# *                                                            *
# * Copyright notice:                                          *
# * Free use of the Mathematical Expression Toolkit Library is *
# * permitted under the guidelines and in accordance with the  *
# * most current version of the MIT License.                   *
# * http://www.opensource.org/licenses/MIT                     *
# *                                                            *
# **************************************************************

# set the minimum version of cmake required
cmake_minimum_required(VERSION 3.14)

# set the project name
project(ExprTk)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set additional compiler flags
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Appends the cmake path to MAKE_MODULE_PATH variable.
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Determine ExprTk Source Version
find_package(Git QUIET)
include(GitDetermineVersion)
# The upcoming command sets the following variables: ExprTk_VERSION, ExprTk_VERSION_MAJOR, ExprTk_VERSION_MINOR,
# ExprTk_VERSION_PATCH, ExprTk_VERSION_PATCH_EXTRA, ExprTk_VERSION_FULL
determine_version("${CMAKE_CURRENT_SOURCE_DIR}" "${GIT_EXECUTABLE}" "ExprTk")

# create an interface library for the ExprTk header
add_library(ExprTk INTERFACE)
# set the include directory for the interface library
target_include_directories(ExprTk INTERFACE include)

# create an ExprTK option for building testing
option(ExprTk_BUILD_TESTING "Build ExprTk tests" ON)
# if ExprTk_BUILD_TESTING is set to ON, then build the tests
if (ExprTk_BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif ()

# create an ExprTK option for building examples
option(ExprTk_BUILD_EXAMPLES "Build ExprTk examples" ON)
# if ExprTk_BUILD_EXAMPLES is set to ON, then build the examples
if (ExprTk_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif ()
