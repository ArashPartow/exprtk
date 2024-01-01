/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 10                                          *
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


#include <cmath>
#include <cstdio>
#include <string>

#include "exprtk.hpp"


template <typename T>
void newton_sqrt()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   T x = T(0);

   symbol_table_t symbol_table;

   symbol_table.add_constants();
   symbol_table.add_variable("x",x);

   compositor_t compositor(symbol_table);

   compositor.add(
      function_t("newton_sqrt")
      .var("x")
      .expression
      (
         " switch                                                   "
         " {                                                        "
         "    case x < 0  : null;                                   "
         "    case x == 0 : 0;                                      "
         "    case x == 1 : 1;                                      "
         "    default:                                              "
         "    {                                                     "
         "       var remaining_itrs := 100;                         "
         "       var sqrt_x := x / 2;                               "
         "       repeat                                             "
         "          if (equal(sqrt_x * sqrt_x, x))                  "
         "             break[sqrt_x];                               "
         "          else                                            "
         "             sqrt_x := (1 / 2) * (sqrt_x + (x / sqrt_x)); "
         "       until ((remaining_itrs -= 1) <= 0);                "
         "    };                                                    "
         " }                                                        "
      ));

   const std::string expression_str = "newton_sqrt(x)";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(expression_str,expression);

   for (x = T(0); x < T(500); x += T(0.5))
   {
      const T result = expression.value();
      const T real   = std::sqrt(x);
      const T error  = std::abs(result - real);

      const bool err_in_bound = error <= exprtk::details::numeric::constant::pi;

      printf("sqrt(%6.2f) - Result: %15.13f\tReal: %15.13f\tError: %18.16f EIB: %c\n",
             x,
             result,
             real,
             error,
             err_in_bound ? 'T' : 'F');
   }
}

int main()
{
   newton_sqrt<double>();
   return 0;
}
