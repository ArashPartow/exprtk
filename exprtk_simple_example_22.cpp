/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 22                                          *
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
void compute_european_option_implied_volatility()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   const std::string option_implied_volatility_program =
      " const var epsilon   := 0.0000001;                               "
      " const var max_iters := 1000;                                    "
      "                                                                 "
      " var v   := 0.5; /* Initial volatility guess */                  "
      " var itr := 0;                                                   "
      "                                                                 "
      " while ((itr += 1) <= max_iters)                                 "
      " {                                                               "
      "    var price :=                                                 "
      "       switch                                                    "
      "       {                                                         "
      "          case callput_flag == 'call' : bsm_call(s, k, r, t, v); "
      "          case callput_flag == 'put'  : bsm_put (s, k, r, t, v); "
      "       };                                                        "
      "                                                                 "
      "    var price_diff := price - target_price;                      "
      "                                                                 "
      "    if (abs(price_diff) <= epsilon)                              "
      "    {                                                            "
      "       break;                                                    "
      "    };                                                           "
      "                                                                 "
      "    var vega := bsm_vega(s, k, r, t, v);                         "
      "                                                                 "
      "    if (vega < epsilon)                                          "
      "    {                                                            "
      "       itr := max_iters + 1;                                     "
      "       break;                                                    "
      "    };                                                           "
      "                                                                 "
      "    v -= price_diff / vega;                                      "
      " };                                                              "
      "                                                                 "
      " itr <= max_iters ? v : null;                                    ";

   T s            = T(100.00); // Spot / Stock / Underlying / Base price
   T k            = T(110.00); // Strike price
   T t            = T(  2.22); // Years to maturity
   T r            = T(  0.05); // Risk free rate
   T target_price = T(  0.00);

   std::string callput_flag;

   symbol_table_t symbol_table(symbol_table_t::e_immutable);
   symbol_table.add_variable("s",s);
   symbol_table.add_variable("k",k);
   symbol_table.add_variable("t",t);
   symbol_table.add_variable("r",r);
   symbol_table.add_stringvar("callput_flag",callput_flag);
   symbol_table.add_variable ("target_price",target_price);
   symbol_table.add_pi();

   compositor_t compositor(symbol_table);

   compositor.add(
      function_t("bsm_call")
      .vars("s", "k", "r", "t", "v")
      .expression
      (
         " var d1 := (log(s / k) + (r + v^2 / 2) * t) / (v * sqrt(t)); "
         " var d2 := d1 - v * sqrt(t);                                 "
         " s * ncdf(d1) - k * exp(-r * t) * ncdf(d2);                  "
      ));

   compositor.add(
      function_t("bsm_put")
      .vars("s", "k", "r", "t", "v")
      .expression
      (
         " var d1 := (log(s / k) + (r + v^2 / 2) * t) / (v * sqrt(t)); "
         " var d2 := d1 - v * sqrt(t);                                 "
         " k * exp(-r * t) * ncdf(-d2) - s * ncdf(-d1);                "
      ));

   compositor.add(
      function_t("bsm_vega")
      .vars("s", "k", "r", "t", "v")
      .expression
      (
         " var d1 := (log(s / k) + (r + v^2 / 2) * t) / (v * sqrt(t)); "
         " s * exp(-d1^2 / 2) / sqrt(2pi) * sqrt(t);                   "
      ));

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(option_implied_volatility_program, expression);

   {
      callput_flag = "call";
      target_price = T(18.339502);

      const T call_option_implied_vol = expression.value();

      printf("Call Option(s: %5.3f, k: %5.3f, t: %5.3f, r: %5.3f) "
             "@ $%8.6f Implied volatility = %10.8f\n",
             s, k, t, r, target_price, call_option_implied_vol);
   }

   {
      callput_flag = "put";
      target_price = T(16.782764);

      const T put_option_implied_vol = expression.value();

      printf("Put  Option(s: %5.3f, k: %5.3f, t: %5.3f, r: %5.3f) "
             "@ $%8.6f Implied volatility = %10.8f\n",
             s, k, t, r, target_price, put_option_implied_vol);
   }
}

int main()
{
   compute_european_option_implied_volatility<double>();
   return 0;
}
