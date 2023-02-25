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

# create an ExprTk option for building the library with a sanitizer
option(ExprTk_USE_SANITiZER "Use a sanitizer. The sanitizer is specified by ExprTk_SANITIZER" OFF)
# define the requested sanitizer
set(ExprTk_SANITIZER "" CACHE STRING "The sanitizer options are: ASAN, MSAN, LSAN, USAN")
if (ExprTk_USE_SANITiZER)
  if (ExprTk_SANITIZER STREQUAL "ASAN")
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
  elseif (ExprTk_SANITIZER STREQUAL "MSAN")
    add_compile_options(-fsanitize=memory -fno-omit-frame-pointer)
  elseif (ExprTk_SANITIZER STREQUAL "LSAN")
    add_compile_options(-fsanitize=leak -fno-omit-frame-pointer)
  elseif (ExprTk_SANITIZER STREQUAL "USAN")
    add_compile_options(-fsanitize=undefined -fno-omit-frame-pointer)
  else ()
    message(FATAL_ERROR "The sanitizer options are: ASAN, MSAN, LSAN, USAN")
  endif ()
endif ()

# create an interface library for the ExprTk header
add_library(ExprTk INTERFACE)
# set the include directory for the interface library
target_include_directories(ExprTk INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

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

# create an ExprTK option for building benchmarks
option(ExprTk_BUILD_BENCHMARKS "Build ExprTk benchmarks" ON)
# if ExprTk_BUILD_BENCHMARKS is set to ON, then build the benchmarks
if (ExprTk_BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif ()

#########################
# Installation commands #
#########################

# Includes
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if (NOT ExprTk_EXPORT_GROUP)
  set(ExprTk_EXPORT_GROUP ExprTkExport)
endif ()
if (NOT ExprTk_INSTALL_DOC_DIR)
  set(ExprTK_INSTALL_DOC_DIR ${CMAKE_INSTALL_DOCDIR})
endif ()
if (NOT ExprTk_INSTALL_INCLUDE_DIR)
  set(ExprTk_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR})
endif ()
if (NOT ExprTk_INSTALL_INCLUDE_SUBDIR)
  set(ExprTk_INSTALL_INCLUDE_SUBDIR ExprTk)
endif ()
if (NOT ExprTk_INSTALL_BIN_DIR)
  set(ExprTk_INSTALL_BIN_DIR ${CMAKE_INSTALL_BINDIR})
endif ()
if (NOT ExprTk_INSTALL_LIB_DIR)
  set(ExprTk_INSTALL_LIB_DIR ${CMAKE_INSTALL_LIBDIR})
endif ()
if (NOT ExprTk_INSTALL_CMAKE_DIR)
  set(ExprTk_INSTALL_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/ExprTk)
endif ()

# Install documentation
install(FILES
  readme.txt
  DESTINATION ${ExprTk_INSTALL_DOC_DIR}/ExprTk/${ExprTk_VERSION}/ COMPONENT ExprTkDevelopment)

# Install required header files
install(FILES
  include/exprtk.hpp
  DESTINATION ${ExprTk_INSTALL_INCLUDE_DIR} COMPONENT ExprTkDevelopment)
target_include_directories(ExprTk INTERFACE
  $<INSTALL_INTERFACE:${ExprTk_INSTALL_INCLUDE_DIR}>)

# Install library
install(TARGETS ExprTk EXPORT ${ExprTk_EXPORT_GROUP}
  RUNTIME DESTINATION ${ExprTk_INSTALL_BIN_DIR} COMPONENT Runtime # .exe, .dll
  LIBRARY DESTINATION ${ExprTk_INSTALL_LIB_DIR} COMPONENT Runtime # .so, .dll
  ARCHIVE DESTINATION ${ExprTk_INSTALL_LIB_DIR} COMPONENT ExprTkDevelopment #[[.a, .lib]])

# Export Targets file
export(EXPORT ${ExprTk_EXPORT_GROUP}
  FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/ExprTkTargets.cmake"
  NAMESPACE ExprTk::)

# Install Targets file
install(EXPORT ${ExprTk_EXPORT_GROUP}
  FILE "ExprTkTargets.cmake"
  NAMESPACE ExprTk::
  DESTINATION ${ExprTk_INSTALL_CMAKE_DIR} COMPONENT ExprTkDevelopment)

# Create Config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ExprTkConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/ExprTkConfig.cmake"
  INSTALL_DESTINATION ${ExprTk_INSTALL_CMAKE_DIR})

# Generate the version file for the Config file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/ExprTkConfigVersion.cmake"
  VERSION "${ExprTk_VERSION}"
  COMPATIBILITY AnyNewerVersion)

# Install Config and ConfigVersion files
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/ExprTkConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/ExprTkConfigVersion.cmake"
  DESTINATION ${ExprTk_INSTALL_CMAKE_DIR} COMPONENT ExprTkDevelopment)
