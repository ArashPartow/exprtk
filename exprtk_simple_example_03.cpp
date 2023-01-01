/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 3                                           *
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
void polynomial()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string expression_string =
                  "25x^5 - 35x^4 - 15x^3 + 40x^2 - 15x + 1";

   const T r0 = T(0);
   const T r1 = T(1);
         T  x = T(0);

   symbol_table_t symbol_table;
   symbol_table.add_variable("x",x);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(expression_string,expression);

   const T delta = T(1.0 / 100.0);

   for (x = r0; x <= r1; x += delta)
   {
      printf("%19.15f\t%19.15f\n", x, expression.value());
   }
}

int main()
{
   polynomial<double>();
   return 0;
}
