/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 6                                           *
 * Author: Arash Partow (1999-2013)                           *
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
void vector_function()
{
   typedef exprtk::expression<T> expression_t;
   std::string expression_string = "z := (3sin(x) + 2log(y))";

   const std::size_t vec_size = 5;

   T x[vec_size] = { T(1.1), T(2.2), T(3.3), T(4.4), T(5.5) };
   T y[vec_size] = { T(1.1), T(2.2), T(3.3), T(4.4), T(5.5) };
   T z[vec_size] = { T(0.0), T(0.0), T(0.0), T(0.0), T(0.0) };

   T x_ = x[0];
   T y_ = y[0];
   T z_ = z[0];

   exprtk::symbol_table<T> symbol_table;
   symbol_table.add_variable("x",x_);
   symbol_table.add_variable("y",y_);
   symbol_table.add_variable("z",z_);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   exprtk::parser<T> parser;
   parser.compile(expression_string,expression);

   for (std::size_t i = 0; i < vec_size; ++i)
   {
      x_ = x[i]; y_ = y[i]; z_ = z[i];
      expression.value();
      x[i] = x_; y[i] = y_; z[i] = z_;
   }
}

int main()
{
   vector_function<double>();
   return 0;
}
