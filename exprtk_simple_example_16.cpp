/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 16                                          *
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
#include <cstdlib>
#include <string>

#include "exprtk.hpp"


template <typename T>
void linear_least_squares()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string linear_least_squares_program =
                  " if (x[] == y[])                                       "
                  " {                                                     "
                  "   beta  := (sum(x * y) - sum(x) * sum(y) / x[]) /     "
                  "            (sum(x^2) -  sum(x)^2 / x[]);              "
                  "                                                       "
                  "   alpha := avg(y) - beta * avg(x);                    "
                  "                                                       "
                  "   rmse  := sqrt(sum((beta * x + alpha - y)^2) / y[]); "
                  " }                                                     "
                  " else                                                  "
                  " {                                                     "
                  "   alpha := null;                                      "
                  "   beta  := null;                                      "
                  "   rmse  := null;                                      "
                  " }                                                     ";

   T x[] = {T(  1), T(  2), T(3), T(  4), T(  5), T(6), T(  7), T(  8), T(  9), T(10)};
   T y[] = {T(8.7), T(6.8), T(6), T(5.6), T(3.8), T(3), T(2.4), T(1.7), T(0.4), T(-1)};

   T alpha = T(0);
   T beta  = T(0);
   T rmse  = T(0);

   symbol_table_t symbol_table;
   symbol_table.add_variable("alpha", alpha);
   symbol_table.add_variable("beta" , beta );
   symbol_table.add_variable("rmse" , rmse );
   symbol_table.add_vector  ("x"    , x    );
   symbol_table.add_vector  ("y"    , y    );

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(linear_least_squares_program,expression);

   expression.value();

   printf("alpha: %15.12f\n", alpha);
   printf("beta:  %15.12f\n", beta );
   printf("rmse:  %15.12f\n", rmse );
   printf("y = %15.12fx + %15.12f\n", beta, alpha);
}

int main()
{
   linear_least_squares<double>();
   return 0;
}
