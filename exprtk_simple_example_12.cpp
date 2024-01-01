/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 12                                          *
 * Author: Arash Partow (1999-2024)                           *
 * URL: https://www.partow.net/programming/exprtk/index.html  *
 *                                                            *
 * Copyright notice:                                          *
 * Free use of the Mathematical Expression Toolkit Library is *
 * permitted under the guidelines and in accordance with the  *
 * most current version of the MIT License.                   *
 * https://www.opensource.org/licenses/MIT                    *
 * SPDX-License-Identifier: MIT                               *
 *                                                            *
 **************************************************************
*/


#include <string>

#include "exprtk.hpp"


template <typename T>
void bubble_sort()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string bubblesort_program =
      " var upper_bound := v[];                      "
      "                                              "
      " repeat                                       "
      "    var new_upper_bound := 0;                 "
      "                                              "
      "    for (var i := 1; i < upper_bound; i += 1) "
      "    {                                         "
      "       if (v[i - 1] > v[i])                   "
      "       {                                      "
      "          v[i - 1] <=> v[i];                  "
      "          new_upper_bound := i;               "
      "       };                                     "
      "    };                                        "
      "                                              "
      "    upper_bound := new_upper_bound;           "
      "                                              "
      " until (upper_bound <= 1);                    ";

   T v[] = { T(9.1), T(2.2), T(1.3), T(5.4), T(7.5), T(4.6), T(3.7) };

   symbol_table_t symbol_table;
   symbol_table.add_vector("v",v);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(bubblesort_program,expression);

   expression.value();
}

int main()
{
   bubble_sort<double>();
   return 0;
}
