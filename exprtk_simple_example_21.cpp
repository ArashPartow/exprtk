/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 21                                          *
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
void binomial_option_pricing_model()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string european_option_binomial_model_program =
      " var dt     := t / n;                                              "
      " var z      := exp(r * dt);                                        "
      " var z_inv  := 1 / z;                                              "
      " var u      := exp(v * sqrt(dt));                                  "
      " var p_up   := (z * u - 1) / (u^2 - 1);                            "
      " var p_down := 1 - p_up;                                           "
      "                                                                   "
      " var option_price[n + 1] := [0];                                   "
      "                                                                   "
      " for (var i := 0; i <= n; i += 1)                                  "
      " {                                                                 "
      "    var base_price := s * u^(n - 2i);                              "
      "                                                                   "
      "    option_price[i] :=                                             "
      "       switch                                                      "
      "       {                                                           "
      "          case callput_flag == 'call' : max(base_price - k, 0);    "
      "          case callput_flag == 'put'  : max(k - base_price, 0);    "
      "       };                                                          "
      " };                                                                "
      "                                                                   "
      " for (var j := n - 1; j >= 0; j -= 1)                              "
      " {                                                                 "
      "    for (var i := 0; i <= j; i += 1)                               "
      "    {                                                              "
      "       option_price[i] := z_inv *                                  "
      "          (p_up * option_price[i] + p_down * option_price[i + 1]); "
      "    }                                                              "
      " };                                                                "
      "                                                                   "
      " option_price[0];                                                  ";

   T s = T( 100.00); // Spot / Stock / Underlying / Base price
   T k = T( 110.00); // Strike price
   T v = T(   0.30); // Volatility
   T t = T(   2.22); // Years to maturity
   T r = T(   0.05); // Risk free rate
   T n = T(1000.00); // Number of time steps

   std::string callput_flag;

   symbol_table_t symbol_table;
   symbol_table.add_variable("s",s);
   symbol_table.add_variable("k",k);
   symbol_table.add_variable("t",t);
   symbol_table.add_variable("r",r);
   symbol_table.add_variable("v",v);
   symbol_table.add_constant("n",n);
   symbol_table.add_stringvar("callput_flag",callput_flag);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(european_option_binomial_model_program,expression);

   callput_flag = "call";

   const T binomial_call_option_price = expression.value();

   callput_flag = "put";

   const T binomial_put_option_price = expression.value();

   printf("BinomialPrice(call, %5.3f, %5.3f, %5.3f, %5.3f, %5.3f) = %22.18f\n",
          s, k, t, r, v,
          binomial_call_option_price);

   printf("BinomialPrice(put , %5.3f, %5.3f, %5.3f, %5.3f, %5.3f) = %22.18f\n",
          s, k, t, r, v,
          binomial_put_option_price);

   const T put_call_parity_diff =
      (binomial_call_option_price - binomial_put_option_price) -
      (s - k * std::exp(-r * t));

   printf("Put-Call parity difference: %20.17f\n", put_call_parity_diff);
}

int main()
{
   binomial_option_pricing_model<double>();
   return 0;
}
