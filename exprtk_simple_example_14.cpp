/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 14                                          *
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
void stddev_example()
{
   typedef exprtk::expression<T> expression_t;
   typedef exprtk::parser<T>     parser_t;

   const std::string stddev_program =
                  " var x[25] := {                     "
                  "                 1,  2,  3,  4,  5, "
                  "                 6,  7,  8,  9, 10, "
                  "                11, 12, 13, 14, 15, "
                  "                16, 17, 18, 19, 20, "
                  "                21, 22, 23, 24, 25  "
                  "              };                    "
                  "                                    "
                  " sqrt(sum([x - avg(x)]^2) / x[])    ";

   expression_t expression;

   parser_t parser;
   parser.compile(stddev_program,expression);

   const T stddev = expression.value();

   printf("stddev(1..25) = %10.6f\n",stddev);
}

int main()
{
   stddev_example<double>();
   return 0;
}
