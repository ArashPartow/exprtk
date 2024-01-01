/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 09                                          *
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
void primes()
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

   //Mode 1 - if statement based
   compositor.add(
      function_t("is_prime_impl1")
      .vars("x", "y")
      .expression
      (
         " if (y == 1,true,                "
         "    if (0 == (x % y),false,      "
         "       is_prime_impl1(x,y - 1))) "
      ));

   compositor.add(
      function_t("is_prime1")
      .var("x")
      .expression
      (
         " if (frac(x) != 0)                                   "
         "    return [false];                                  "
         " else if (x <= 0)                                    "
         "    return [false];                                  "
         " else                                                "
         "    is_prime_impl1(x,min(x - 1,trunc(sqrt(x)) + 1)); "
      ));

   //Mode 2 - switch statement based
   compositor.add(
      function_t("is_prime_impl2")
      .vars("x", "y")
      .expression
      (
         " switch                                          "
         " {                                               "
         "    case y == 1       : true;                    "
         "    case (x % y) == 0 : false;                   "
         "    default           : is_prime_impl2(x,y - 1); "
         " }                                               "
      ));

   compositor.add(
      function_t("is_prime2")
      .var("x")
      .expression
      (
         " switch                                                                  "
         " {                                                                       "
         "    case x <= 0       : false;                                           "
         "    case frac(x) != 0 : false;                                           "
         "    default           : is_prime_impl2(x,min(x - 1,trunc(sqrt(x)) + 1)); "
         " }                                                                       "
      ));

   //Mode 3 - switch statement and for-loop based
   compositor.add(
      function_t("is_prime3")
      .var("x")
      .expression
      (
         " switch                                              "
         " {                                                   "
         "    case x <= 1       : return [false];              "
         "    case frac(x) != 0 : return [false];              "
         "    case x == 2       : return [true ];              "
         " };                                                  "
         "                                                     "
         " var prime_lut[27] :=                                "
         " {                                                   "
         "      2,   3,   5,   7,  11,  13,  17,  19,  23,     "
         "     29,  31,  37,  41,  43,  47,  53,  59,  61,     "
         "     67,  71,  73,  79,  83,  89,  97, 101, 103      "
         " };                                                  "
         "                                                     "
         " var upper_bound := min(x - 1, trunc(sqrt(x)) + 1);  "
         "                                                     "
         " for (var i := 0; i < prime_lut[]; i += 1)           "
         " {                                                   "
         "    if (prime_lut[i] >= upper_bound)                 "
         "       return [true];                                "
         "    else if ((x % prime_lut[i]) == 0)                "
         "       return [false];                               "
         " };                                                  "
         "                                                     "
         " var lower_bound := prime_lut[prime_lut[] - 1] + 2;  "
         "                                                     "
         " for (var i := lower_bound; i < upper_bound; i += 2) "
         " {                                                   "
         "    if ((x % i) == 0)                                "
         "    {                                                "
         "       return [false];                               "
         "    }                                                "
         " };                                                  "
         "                                                     "
         " return [true];                                      "
      ));

   std::string expression_str1 = "is_prime1(x)";
   std::string expression_str2 = "is_prime2(x)";
   std::string expression_str3 = "is_prime3(x)";

   expression_t expression1;
   expression_t expression2;
   expression_t expression3;
   expression1.register_symbol_table(symbol_table);
   expression2.register_symbol_table(symbol_table);
   expression3.register_symbol_table(symbol_table);

   parser_t parser;

   parser.compile(expression_str1, expression1);
   parser.compile(expression_str2, expression2);
   parser.compile(expression_str3, expression3);

   for (std::size_t i = 0; i < 15000; ++i)
   {
      x = static_cast<T>(i);

      const T result1 = expression1.value();
      const T result2 = expression2.value();
      const T result3 = expression3.value();

      const bool results_concur = (result1 == result2) &&
                                  (result1 == result3) ;

      printf("%03d  Result1: %c  Result2: %c  Result3: %c  "
             "Results Concur: %c\n",
             static_cast<unsigned int>(i),
             (result1 == T(1)) ? 'T' : 'F',
             (result2 == T(1)) ? 'T' : 'F',
             (result3 == T(1)) ? 'T' : 'F',
             (results_concur)  ? 'T' : 'F');
   }
}

int main()
{
   primes<double>();
   return 0;
}
