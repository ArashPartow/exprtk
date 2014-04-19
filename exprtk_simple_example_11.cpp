/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 11                                          *
 * Author: Arash Partow (1999-2014)                           *
 * URL: http://www.partow.net/programming/exprtk/index.html   *
 *                                                            *
 * Copyright notice:                                          *
 * Free use of the Mathematical Expression Toolkit Library is *
 * permitted under the guidelines and in accordance with the  *
 * most current version of the Common Public License.         *
 * http://www.opensource.org/licenses/cpl1.0.php              *
 *                                                            *
 **************************************************************
*/


#include <cstdio>
#include <string>
#include "exprtk.hpp"


template<typename T>
void square_wave2()
{
   std::string expr_string = " r := 0;                               "
                             " for(i := 0; i < 1000; i += 1)         "
                             " {                                     "
                             "   r += (1/(2i+1))*sin((4i+2)*pi*f*t); "
                             " };                                    "
                             " r *= a*(4 / pi);                      ";

   static const T pi = T(3.14159265358979323846);

   T f = pi / T(10);
   T t = T(0);
   T a = T(10);

   exprtk::symbol_table<T> symbol_table;
   symbol_table.add_variable("f",f);
   symbol_table.add_variable("t",t);
   symbol_table.add_variable("a",a);
   symbol_table.add_constants();

   exprtk::expression<T> expression;
   expression.register_symbol_table(symbol_table);

   exprtk::parser<T> parser;
   parser.enable_unknown_symbol_resolver();

   parser.compile(expr_string,expression);

   const T delta = (T(4) * pi) / T(1000);

   for (t = (T(-2) * pi); t <= (T(+2) * pi); t += delta)
   {
      T result = expression.value();
      printf("%19.15f\t%19.15f\n",t,result);
   }
}

int main()
{
   square_wave2<double>();
   return 0;
}
