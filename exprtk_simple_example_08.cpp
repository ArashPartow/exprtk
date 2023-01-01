/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 8                                           *
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
void composite()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::parser_error::type      error_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   compositor_t compositor;

   T x = T(1);
   T y = T(2);

   symbol_table_t& symbol_table = compositor.symbol_table();
   symbol_table.add_constants();
   symbol_table.add_variable("x",x);
   symbol_table.add_variable("y",y);

   compositor
      .add(
      function_t("f","sin(x / pi)","x"));          // f(x) = sin(x / pi)

   compositor
      .add(
      function_t("g","3*[f(x) + f(y)]","x","y"));  // g(x,y) = 3[f(x) + f(y)]

   std::string expression_string = "g(1 + f(x), f(y) / 2)";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;

   if (!parser.compile(expression_string,expression))
   {
      printf("Error: %s\tExpression: %s\n",
             parser.error().c_str(),
             expression_string.c_str());

      for (std::size_t i = 0; i < parser.error_count(); ++i)
      {
         const error_t error = parser.get_error(i);

         printf("Error: %02d  Position: %02d Type: [%14s] Msg: %s\tExpression: %s\n",
                static_cast<unsigned int>(i),
                static_cast<unsigned int>(error.token.position),
                exprtk::parser_error::to_str(error.mode).c_str(),
                error.diagnostic.c_str(),
                expression_string.c_str());
      }

      return;
   }

   const T result = expression.value();

   printf("%s = %e\n", expression_string.c_str(), result);
}

int main()
{
   composite<double>();
   return 0;
}
