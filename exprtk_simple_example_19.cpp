/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 19                                          *
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
class randu : public exprtk::igeneric_function<T>
{
public:

   typedef typename exprtk::igeneric_function<T> igfun_t;
   typedef typename igfun_t::parameter_list_t    parameter_list_t;
   typedef typename igfun_t::generic_type        generic_type;
   typedef typename generic_type::vector_view    vector_t;

   using exprtk::igeneric_function<T>::operator();

   randu()
   : exprtk::igeneric_function<T>("V|VTT")
      /*
         Overloads:
         0. V   - vector
         1. VTT - vector, r0, r1
      */
   { ::srand(static_cast<unsigned int>(time(NULL))); }

   inline T operator()(const std::size_t& ps_index, parameter_list_t parameters)
   {
      vector_t v(parameters[0]);

      std::size_t r0 = 0;
      std::size_t r1 = v.size() - 1;

      if (
           (1 == ps_index) &&
           !exprtk::rtl::vecops::helper::
              load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0)
         )
         return T(0);

      for (std::size_t i = r0; i <= r1; ++i)
      {
         v[i] = rnd();
      }

      return T(1);
   }

private:

   inline T rnd()
   {
      // Note: Do not use this in production
      // Result is in the interval [0,1)
      return T(::rand() / T(RAND_MAX + 1.0));
   }
};

template <typename T>
void vector_randu()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string vecrandu_program =
                " var noise[6] := [0];                     "
                "                                          "
                " if (randu(noise,0,5) == false)           "
                " {                                        "
                "   println('Failed to generate noise');   "
                "   return [false];                        "
                " }                                        "
                "                                          "
                " var noisy[6] := signal + (noise - 1/2);  "
                "                                          "
                " for (var i := 0; i < noisy[]; i += 1)    "
                " {                                        "
                "   println('noisy[',i,'] = ', noisy[i]);  "
                " }                                        "
                "                                          "
                " println('avg: ', avg(noisy));            "
                "                                          ";

   T signal[] = { T(1.1), T(2.2), T(3.3), T(4.4), T(5.5), T(6.6), T(7.7) };

   exprtk::rtl::io::println<T> println;
   randu<T>                    randu;

   symbol_table_t symbol_table;
   symbol_table.add_vector  ("signal" , signal );
   symbol_table.add_function("println", println);
   symbol_table.add_function("randu"  , randu  );

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(vecrandu_program,expression);

   expression.value();
}

int main()
{
   vector_randu<double>();
   return 0;
}
