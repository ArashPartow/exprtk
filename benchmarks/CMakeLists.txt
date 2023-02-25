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

# create executables for benchmark
add_executable(exprtk_benchmark exprtk_benchmark.cpp)
target_link_libraries(exprtk_benchmark ExprTk)
