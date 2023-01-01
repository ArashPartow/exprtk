/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 12                                          *
 * Author: Arash Partow (1999-2023)                           *
 * URL: https://www.partow.net/programming/exprtk/index.html  *
 *                                                            *
 * Copyright notice:                                          *
 * Free use of the Mathematical Expression Toolkit Library is *
 * permitted under the guidelines and in accordance with the  *
 * most current version of the MIT License.                   *
 * https://www.opensource.org/licenses/MIT                    *
 *                                                            *
 **************************************************************
*/


#include <cstdio>
#include <string>

#include "exprtk.hpp"


template <typename T>
void bubble_sort()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string bubblesort_program =
                  " var upper_bound := v[];                           "
                  " var swapped := false;                             "
                  " repeat                                            "
                  "   swapped := false;                               "
                  "   for (var i := 0; i < upper_bound; i += 1)       "
                  "   {                                               "
                  "     for (var j := i + 1; j < upper_bound; j += 1) "
                  "     {                                             "
                  "       if (v[i] > v[j])                            "
                  "       {                                           "
                  "         v[i] <=> v[j];                            "
                  "         swapped := true;                          "
                  "       };                                          "
                  "     };                                            "
                  "   };                                              "
                  "   upper_bound -= 1;                               "
                  " until (not(swapped) or (upper_bound == 0));       ";

   T v[] = { T(9.9), T(2.2), T(1.1), T(5.5), T(7.7), T(4.4), T(3.3) };

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
