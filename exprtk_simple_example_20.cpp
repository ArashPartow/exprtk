/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 20                                          *
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
#include <stdexcept>
#include <map>
#include <string>

#include "exprtk.hpp"


struct vector_access_rtc : public exprtk::vector_access_runtime_check
{
   typedef std::map<void*, std::string> map_t;
   map_t vector_map;

   bool handle_runtime_violation(violation_context& context)
   {
      const map_t::iterator itr     = vector_map.find(static_cast<void*>(context.base_ptr));
      const std::string vector_name = (itr != vector_map.end()) ?
                                      itr->second : "Unknown"   ;

      printf("Runtime vector access violation\n"
             "Vector: %s base: %p end: %p access: %p typesize: %d\n",
             vector_name.c_str(),
             context.base_ptr   ,
             context.end_ptr    ,
             context.access_ptr ,
             static_cast<unsigned int>(context.type_size));

      throw std::runtime_error
         ("Runtime vector access violation. Vector: " + vector_name);

      return false;
   }
};

template <typename T>
void vector_overflow_example()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string expression_str =
      " for (var i := 0; i < max(v0[],v1[]); i += 1) "
      " {                                            "
      "    v0[i] := (2 * v0[i]) + (v1[i] / 3);       "
      " }                                            ";

   T v0[5 ] = { 0, 1, 2, 3, 4 };
   T v1[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

   vector_access_rtc vec_rtc;

   vec_rtc.vector_map[v0] = "v0";
   vec_rtc.vector_map[v1] = "v1";

   symbol_table_t symbol_table;
   expression_t   expression;
   parser_t       parser;

   symbol_table.add_vector("v0", v0);
   symbol_table.add_vector("v1", v1);

   expression.register_symbol_table(symbol_table);

   parser.register_vector_access_runtime_check(vec_rtc);

   if (!parser.compile(expression_str, expression))
   {
      printf("Error: %s\tExpression: %s\n",
             parser.error().c_str(),
             expression_str.c_str());

      return;
   }

   try
   {
      expression.value();
   }
   catch(std::runtime_error& exception)
   {
      printf("Exception: %s\n",exception.what());
   }
}

int main()
{
   vector_overflow_example<double>();
   return 0;
}
