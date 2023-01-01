/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 2                                           *
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
void square_wave()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string expr_string =
                  "a*(4/pi)*"
                  "((1 /1)*sin( 2*pi*f*t)+(1 /3)*sin( 6*pi*f*t)+"
                  " (1 /5)*sin(10*pi*f*t)+(1 /7)*sin(14*pi*f*t)+"
                  " (1 /9)*sin(18*pi*f*t)+(1/11)*sin(22*pi*f*t)+"
                  " (1/13)*sin(26*pi*f*t)+(1/15)*sin(30*pi*f*t)+"
                  " (1/17)*sin(34*pi*f*t)+(1/19)*sin(38*pi*f*t)+"
                  " (1/21)*sin(42*pi*f*t)+(1/23)*sin(46*pi*f*t)+"
                  " (1/25)*sin(50*pi*f*t)+(1/27)*sin(54*pi*f*t))";

   static const T pi = T(3.141592653589793238462643383279502);

   const T f = pi / T(10);
   const T a = T(10);
         T t = T(0);

   symbol_table_t symbol_table;
   symbol_table.add_variable("t",t);
   symbol_table.add_constant("f",f);
   symbol_table.add_constant("a",a);
   symbol_table.add_constants();

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(expr_string,expression);

   const T delta = (T(4) * pi) / T(1000);

   for (t = (T(-2) * pi); t <= (T(+2) * pi); t += delta)
   {
      const T result = expression.value();
      printf("%19.15f\t%19.15f\n", t, result);
   }
}

int main()
{
   square_wave<double>();
   return 0;
}
