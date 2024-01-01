#
# ******************************************************************
# *           C++ Mathematical Expression Toolkit Library          *
# *                                                                *
# * Author: Arash Partow (1999-2024)                               *
# * URL: https://www.partow.net/programming/exprtk/index.html      *
# *                                                                *
# * Copyright notice:                                              *
# * Free use of the C++ Mathematical Expression Toolkit Library is *
# * permitted under the guidelines and in accordance with the most *
# * current version of the MIT License.                            *
# * https://www.opensource.org/licenses/MIT                        *
# * SPDX-License-Identifier: MIT                                   *
# *                                                                *
# ******************************************************************
#

cmake_minimum_required(VERSION 3.15)

project(
   ExprTk
   LANGUAGES CXX
   VERSION 0.0.3
   DESCRIPTION  "C++ Mathematical Expression Toolkit Library (ExprTk)"
   HOMEPAGE_URL "https://www.partow.net/programming/exprtk/index.html"
)

# define ExprTk header-only library
add_library(
    exprtk
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/exprtk.hpp
)

add_library(exprtk::exprtk ALIAS exprtk)

target_include_directories(
    exprtk
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(exprtk INTERFACE cxx_std_11)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "ExprTkConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
