/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 24                                          *
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


template <typename T, T Process(const unsigned char)>
struct char_process : public exprtk::igeneric_function<T>
{
   typedef typename exprtk::igeneric_function<T> igfun_t;
   typedef typename igfun_t::parameter_list_t    parameter_list_t;
   typedef typename igfun_t::generic_type        generic_type;
   typedef typename generic_type::string_view    string_t;

   using exprtk::igeneric_function<T>::operator();

   char_process()
   : exprtk::igeneric_function<T>("S")
   {}

   inline T operator()(parameter_list_t parameters)
   {
      const unsigned char c = string_t(parameters[0])[0];
      return Process(c);
   }
};

template <typename T>
T is_digit_func(const unsigned char c)
{
   return (('0' <= c) && (c <= '9')) ? T(1) : T(0);
}

template <typename T>
T to_num_func(const unsigned char c)
{
   return static_cast<T>(c - '0');
}

template <typename T>
void rpn_example()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string rpn_program =
      " var stack[1000] := [0];                                                  "
      " var top_index   := 0;                                                    "
      "                                                                          "
      " for (var i := 0; i < rpn_expression[]; i += 1)                           "
      " {                                                                        "
      "    var c := rpn_expression[i : i + 1];                                   "
      "                                                                          "
      "    if (c == ' ')                                                         "
      "    {                                                                     "
      "       continue;                                                          "
      "    }                                                                     "
      "    else if (is_digit(c))                                                 "
      "    {                                                                     "
      "       stack[top_index] := to_num(c);                                     "
      "       top_index        += 1;                                             "
      "    }                                                                     "
      "    else                                                                  "
      "    {                                                                     "
      "       var operator := c;                                                 "
      "       var operand1 := stack[top_index - 2];                              "
      "       var operand2 := stack[top_index - 1];                              "
      "       top_index    -= 2;                                                 "
      "                                                                          "
      "       switch                                                             "
      "       {                                                                  "
      "          case operator == '+' : stack[top_index] := operand1 + operand2; "
      "          case operator == '-' : stack[top_index] := operand1 - operand2; "
      "          case operator == '*' : stack[top_index] := operand1 * operand2; "
      "          case operator == '/' : stack[top_index] := operand1 / operand2; "
      "          case operator == '^' : stack[top_index] := operand1 ^ operand2; "
      "       };                                                                 "
      "                                                                          "
      "       top_index += 1;                                                    "
      "    }                                                                     "
      " };                                                                       "
      "                                                                          "
      " println(stack[0], ' = ', rpn_expression);                                "
      "                                                                          ";

   std::string rpn_expression;

   char_process<T,is_digit_func<T> > isdigit;
   char_process<T,to_num_func<T> >   tonum;
   exprtk::rtl::io::println<T>       println;

   symbol_table_t symbol_table;
   symbol_table.add_stringvar("rpn_expression", rpn_expression);
   symbol_table.add_function ("println"       , println       );
   symbol_table.add_function ("is_digit"      , isdigit       );
   symbol_table.add_function ("to_num"        , tonum         );

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(rpn_program, expression);

   const std::string rpn_expressions[] =
   {
      "2 3 8 / ^ 4 6 * + 3 9 / -", // 2 ^ (3 / 8) + 4 * 6 - 3 / 9
      "1 2 / 6 5 2 - / * 7 +"    , // (1 / 2) * (6 / (5 - 2)) + 7
      "1 2 * 3 / 4 * 5 / 6 *"    , // ((((1 * 2) / 3) * 4) / 5) * 6
      "8 6 4 + * 2 /"              // (8 * (6 + 4)) / 2
   };

   for (std::size_t i = 0; i < sizeof(rpn_expressions) / sizeof(std::string); ++i)
   {
      rpn_expression = rpn_expressions[i];
      expression.value();
   }
}

int main()
{
   rpn_example<double>();
   return 0;
}
