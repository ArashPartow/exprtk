/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 04                                          *
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


#include <cstdio>
#include <string>

#include "exprtk.hpp"


template <typename T>
void fibonacci()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   T x = T(0);

   compositor_t compositor;

   compositor.add(
      function_t("fibonacci")
      .var("x")
      .expression
      (
         " switch                      "
         " {                           "
         "    case x == 0 : 0;         "
         "    case x == 1 : 1;         "
         "    default     :            "
         "    {                        "
         "       var prev := 0;        "
         "       var curr := 1;        "
         "       while ((x -= 1) > 0)  "
         "       {                     "
         "          var temp := prev;  "
         "          prev := curr;      "
         "          curr += temp;      "
         "       };                    "
         "    };                       "
         " }                           "
      ));

   symbol_table_t& symbol_table = compositor.symbol_table();
   symbol_table.add_constants();
   symbol_table.add_variable("x",x);

   std::string expression_str = "fibonacci(x)";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(expression_str,expression);

   for (std::size_t i = 0; i < 40; ++i)
   {
      x = static_cast<T>(i);

      const T result = expression.value();

      printf("fibonacci(%3d) = %10.0f\n",
             static_cast<int>(i),
             result);
   }
}

int main()
{
   fibonacci<double>();
   return 0;
}
