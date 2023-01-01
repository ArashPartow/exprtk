/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 17                                          *
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
struct rnd_01 : public exprtk::ifunction<T>
{
   using exprtk::ifunction<T>::operator();

   rnd_01() : exprtk::ifunction<T>(0)
   { ::srand(static_cast<unsigned int>(time(NULL))); }

   inline T operator()()
   {
      // Note: Do not use this in production
      // Result is in the interval [0,1)
      return T(::rand() / T(RAND_MAX + 1.0));
   }
};

template <typename T>
void monte_carlo_pi()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string monte_carlo_pi_program =
                  " var experiments[5 * 10^7] := [(rnd_01^2 + rnd_01^2) <= 1]; "
                  " 4 * sum(experiments) / experiments[];                      ";

   rnd_01<T> rnd01;

   symbol_table_t symbol_table;
   symbol_table.add_function("rnd_01",rnd01);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(monte_carlo_pi_program,expression);

   const T approximate_pi = expression.value();

   const T real_pi = T(3.141592653589793238462643383279502); // or close enough...

   printf("pi ~ %20.17f\terror: %20.17f\n",
          approximate_pi,
          std::abs(real_pi - approximate_pi));
}

int main()
{
   monte_carlo_pi<double>();
   return 0;
}
