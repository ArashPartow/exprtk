/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 5                                           *
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
struct myfunc : public exprtk::ifunction<T>
{
   using exprtk::ifunction<T>::operator();

   myfunc()
   : exprtk::ifunction<T>(2)
   { exprtk::disable_has_side_effects(*this); }

   inline T operator()(const T& v1, const T& v2)
   {
      return T(1) + (v1 * v2) / T(3);
   }
};

template <typename T>
inline T myotherfunc(T v0, T v1, T v2)
{
   return std::abs(v0 - v1) * v2;
}

template <typename T>
void custom_function()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string expression_string =
                  "myfunc(sin(x / pi), otherfunc(3 * y, x / 2, x * y))";

   T x = T(1);
   T y = T(2);
   myfunc<T> mf;

   symbol_table_t symbol_table;
   symbol_table.add_variable("x",x);
   symbol_table.add_variable("y",y);
   symbol_table.add_function("myfunc",mf);
   symbol_table.add_function("otherfunc",myotherfunc);
   symbol_table.add_constants();

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(expression_string,expression);

   const T result = expression.value();
   printf("Result: %10.5f\n",result);
}

int main()
{
   custom_function<double>();
   return 0;
}
