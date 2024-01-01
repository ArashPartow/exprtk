/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 15                                          *
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
void black_scholes_merton_model()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string bsm_model_program =
      " var d1 := (log(s / k) + (r + v^2 / 2) * t) / (v * sqrt(t)); "
      " var d2 := d1 - v * sqrt(t);                                 "
      "                                                             "
      " if (callput_flag == 'call')                                 "
      "    s * ncdf(d1) - k * e^(-r * t) * ncdf(d2);                "
      " else if (callput_flag == 'put')                             "
      "    k * e^(-r * t) * ncdf(-d2) - s * ncdf(-d1);              "
      "                                                             ";

   T s = T(60.00); // Spot / Stock / Underlying / Base price
   T k = T(65.00); // Strike price
   T v = T( 0.30); // Volatility
   T t = T( 0.25); // Years to maturity
   T r = T( 0.08); // Risk free rate

   std::string callput_flag;

   static const T e = exprtk::details::numeric::constant::e;

   symbol_table_t symbol_table;
   symbol_table.add_variable("s",s);
   symbol_table.add_variable("k",k);
   symbol_table.add_variable("t",t);
   symbol_table.add_variable("r",r);
   symbol_table.add_variable("v",v);
   symbol_table.add_constant("e",e);
   symbol_table.add_stringvar("callput_flag",callput_flag);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(bsm_model_program,expression);

   callput_flag = "call";

   const T bsm_call_option_price = expression.value();

   callput_flag = "put";

   const T bsm_put_option_price = expression.value();

   printf("BSM(call, %5.3f, %5.3f, %5.3f, %5.3f, %5.3f) = %10.6f\n",
          s, k, t, r, v,
          bsm_call_option_price);

   printf("BSM(put , %5.3f, %5.3f, %5.3f, %5.3f, %5.3f) = %10.6f\n",
          s, k, t, r, v,
          bsm_put_option_price);

   const T put_call_parity_diff =
      (bsm_call_option_price - bsm_put_option_price) -
      (s - k * std::exp(-r * t));

   printf("Put-Call parity difference: %20.17f\n", put_call_parity_diff);
}

int main()
{
   black_scholes_merton_model<double>();
   return 0;
}
