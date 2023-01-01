/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 11                                          *
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
void square_wave2()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string wave_program =
                  " var r := 0;                                         "
                  " for (var i := 0; i < 1000; i += 1)                  "
                  " {                                                   "
                  "   r += (1 / (2i + 1)) * sin((4i + 2) * pi * f * t); "
                  " };                                                  "
                  " r *= a * (4 / pi);                                  ";

   static const T pi = T(3.141592653589793238462643383279502);

   T f = pi / T(10);
   T t = T(0);
   T a = T(10);

   symbol_table_t symbol_table;
   symbol_table.add_variable("f",f);
   symbol_table.add_variable("t",t);
   symbol_table.add_variable("a",a);
   symbol_table.add_constants();

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(wave_program,expression);

   const T delta = (T(4) * pi) / T(1000);

   for (t = (T(-2) * pi); t <= (T(+2) * pi); t += delta)
   {
      const T result = expression.value();
      printf("%19.15f\t%19.15f\n", t, result);
   }
}

int main()
{
   square_wave2<double>();
   return 0;
}
