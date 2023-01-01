/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 13                                          *
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
#include <cstdlib>
#include <ctime>
#include <string>

#include "exprtk.hpp"


template <typename T>
void savitzky_golay_filter()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string sgfilter_program =
                  " var weight[9] :=                                          "
                  "       {                                                   "
                  "         -21, 14,  39,                                     "
                  "          54, 59,  54,                                     "
                  "          39, 14, -21                                      "
                  "       };                                                  "
                  "                                                           "
                  " if (v_in[] >= weight[])                                   "
                  " {                                                         "
                  "   var lower_bound := trunc(weight[] / 2);                 "
                  "   var upper_bound := v_in[] - lower_bound;                "
                  "                                                           "
                  "   v_out := 0;                                             "
                  "                                                           "
                  "   for (var i := lower_bound; i < upper_bound; i += 1)     "
                  "   {                                                       "
                  "     for (var j := -lower_bound; j <= lower_bound; j += 1) "
                  "     {                                                     "
                  "       v_out[i] += weight[j + lower_bound] * v_in[i + j];  "
                  "     };                                                    "
                  "   };                                                      "
                  "                                                           "
                  "   v_out /= sum(weight);                                   "
                  " }                                                         ";

   const std::size_t n = 1024;

   std::vector<T> v_in;
   std::vector<T> v_out;

   const T pi = T(3.141592653589793238462643383279502);

   srand(static_cast<unsigned int>(time(0)));

   // Generate a signal with noise.
   for (T t = T(-5); t <= T(+5); t += T(10.0 / n))
   {
      const T noise = T(0.5 * (rand() / (RAND_MAX + 1.0) - 0.5));
      v_in.push_back(sin(2.0 * pi * t) + noise);
   }

   v_out.resize(v_in.size());

   symbol_table_t symbol_table;
   symbol_table.add_vector("v_in" , v_in );
   symbol_table.add_vector("v_out", v_out);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(sgfilter_program,expression);

   expression.value();

   for (std::size_t i = 0; i < v_out.size(); ++i)
   {
      printf("%10.6f\t%10.6f\n", v_in[i], v_out[i]);
   }
}

int main()
{
   savitzky_golay_filter<double>();
   return 0;
}
