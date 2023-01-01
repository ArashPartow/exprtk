/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 6                                           *
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
void vector_function()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string expression_string =
                  " for (var i := 0; i < min(x[],y[],z[]); i += 1) "
                  " {                                              "
                  "   z[i] := 3sin(x[i]) + 2log(y[i]);             "
                  " }                                              ";

   T x[] = { T(1.1), T(2.2), T(3.3), T(4.4), T(5.5) };
   T y[] = { T(1.1), T(2.2), T(3.3), T(4.4), T(5.5) };
   T z[] = { T(0.0), T(0.0), T(0.0), T(0.0), T(0.0) };

   symbol_table_t symbol_table;
   symbol_table.add_vector("x",x);
   symbol_table.add_vector("y",y);
   symbol_table.add_vector("z",z);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(expression_string,expression);

   expression.value();
}

int main()
{
   vector_function<double>();
   return 0;
}
