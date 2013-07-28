/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * ExprTk vs Native Benchmarks                                *
 * Author: Arash Partow (1999-2013)                           *
 * URL: http://www.partow.net/programming/exprtk/index.html   *
 *                                                            *
 * Copyright notice:                                          *
 * Free use of the Mathematical Expression Toolkit Library is *
 * permitted under the guidelines and in accordance with the  *
 * most current version of the Common Public License.         *
 * http://www.opensource.org/licenses/cpl1.0.php              *
 *                                                            *
 **************************************************************
*/


#include <cstdio>
#include <cmath>
#include <iostream>
#include <string>
#include <deque>

#include "exprtk.hpp"

const std::string expression_list[] = {
                                         "(y + x)",
                                         "2 * (y + x)",
                                         "(2 * y + 2 * x)",
                                         "((1.23 * x^2) / y) - 123.123",
                                         "(y + x / y) * (x - y / x)",
                                         "x / ((x + y) + (x - y)) / y",
                                         "1 - ((x * y) + (y / x)) - 3",
                                         "(5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)",
                                         "1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55",
                                         "sin(2 * x) + cos(pi / y)",
                                         "1 - sin(2 * x) + cos(pi / y)",
                                         "sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)",
                                         "(x^2 / sin(2 * pi / y)) - x / 2",
                                         "x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y",
                                         "clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)",
                                         "max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))",
                                         "if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x * y) + 2 * pi / x"
                                      };

const std::size_t expression_list_size = sizeof(expression_list) / sizeof(std::string);


static const double lower_bound_x = -100.0;
static const double lower_bound_y = -100.0;
static const double upper_bound_x = +100.0;
static const double upper_bound_y = +100.0;
static const double delta = 0.0111;


template <typename T,
          typename Allocator,
          template <typename,typename> class Sequence>
bool load_expression(exprtk::symbol_table<T>& symbol_table,
                     Sequence<exprtk::expression<T>,Allocator>& expr_seq)
{
   exprtk::parser<double> parser;
   for (std::size_t i = 0; i < expression_list_size; ++i)
   {
      exprtk::expression<double> expression;
      expression.register_symbol_table(symbol_table);
      if (!parser.compile(expression_list[i],expression))
      {
         printf("[load_expression] - Parser Error: %s\tExpression: %s\n",
                parser.error().c_str(),
                expression_list[i].c_str());
         return false;
      }
      expr_seq.push_back(expression);
   }
   return true;
}

template <typename T>
void run_exprtk_benchmark(T& x, T& y,
                          exprtk::expression<T>& expression,
                          const std::string& expr_string)
{
   T total = T(0);
   unsigned int count = 0;
   exprtk::timer timer;
   timer.start();
   for (x = lower_bound_x; x <= upper_bound_x; x += delta)
   {
      for (y = lower_bound_y; y <= upper_bound_y; y += delta)
      {
         total += expression.value();
         ++count;
      }
   }
   timer.stop();
   if (T(0) != total)
      printf("[exprtk] Total Time:%12.8f  Rate:%14.3fevals/sec Expression: %s\n",
             timer.time(),
             count / timer.time(),
             expr_string.c_str());
   else
      printf("run_exprtk_benchmark() - Error running benchmark for expression: %s\n",expr_string.c_str());
}

template <typename T> struct native;

template <typename T, typename NativeFunction>
void run_native_benchmark(T& x, T& y, NativeFunction f, const std::string& expr_string)
{
   T total = T(0);
   unsigned int count = 0;
   exprtk::timer timer;
   timer.start();
   for (x = lower_bound_x; x <= upper_bound_x; x += delta)
   {
      for (y = lower_bound_y; y <= upper_bound_y; y += delta)
      {
         total += f(x,y);
         ++count;
      }
   }
   timer.stop();
   if (T(0) != total)
      printf("[native] Total Time:%12.8f  Rate:%14.3fevals/sec Expression: %s\n",
             timer.time(),
             count / timer.time(),
             expr_string.c_str());
   else
      printf("run_native_benchmark() - Error running benchmark for expression: %s\n",expr_string.c_str());
}

template <typename T>
bool run_parse_benchmark(exprtk::symbol_table<T>& symbol_table)
{
   static const std::size_t rounds = 100000;
   exprtk::parser<double> parser;
   exprtk::expression<double> expression;

   expression.register_symbol_table(symbol_table);

   for (std::size_t i = 0; i < expression_list_size; ++i)
   {
      exprtk::timer timer;
      timer.start();
      for (std::size_t r = 0; r < rounds; ++r)
      {
         if (!parser.compile(expression_list[i],expression))
         {
            printf("[run_parse_benchmark] - Parser Error: %s\tExpression: %s\n",
                   parser.error().c_str(),
                   expression_list[i].c_str());
            return false;
         }
      }
      timer.stop();
      printf("[parse] Total Time:%12.8f  Rate:%14.3fparse/sec Expression: %s\n",
             timer.time(),
             rounds / timer.time(),
             expression_list[i].c_str());
   }
   return true;
}

const double pi = 3.14159265358979323846;

template <typename T>
struct native
{
   typedef typename exprtk::details::functor_t<T> functor_t;
   typedef typename functor_t::Type Type;

   static inline T avg(Type x, Type y)
   {
      return (x + y) / T(2);
   }

   static inline T clamp(const Type l, const Type v, const Type u)
   {
      return ((v < l) ? l : ((v > u) ? u : v));
   }

   static inline T func00(Type x, Type y)
   {
      return (y + x);
   }

   static inline T func01(Type x, Type y)
   {
      return T(2) * (y + x);
   }

   static inline T func02(Type x, Type y)
   {
      return (T(2) * y + T(2) * x);
   }

   static inline T func03(Type x, Type y)
   {
      return ((T(1.23) * (x * x)) / y) - T(123.123);
   }

   static inline T func04(Type x, Type y)
   {
      return (y + x / y) * (x - y / x);
   }

   static inline T func05(Type x, Type y)
   {
      return x / ((x + y) + (x - y)) / y;
   }

   static inline T func06(Type x, Type y)
   {
      return T(1) - ((x * y) + (y / x)) - T(3);
   }

   static inline T func07(Type x, Type y)
   {
      return (T(5.5) + x) + (T(2) * x - T(2) / T(3) * y) * (x / T(3) + y / T(4)) + (y + T(7.7));
   }

   static inline T func08(Type x, Type y)
   {
      using namespace std;
      return (T(1.1)*pow(x,T(1))+T(2.2)*pow(y,T(2))-T(3.3)*pow(x,T(3))+T(4.4)*pow(y,T(15))-T(5.5)*pow(x,T(23))+T(6.6)*pow(y,T(55)));
   }

   static inline T func09(Type x, Type y)
   {
      return std::sin(T(2) * x) + std::cos(pi / y);
   }

   static inline T func10(Type x, Type y)
   {
      return T(1) - std::sin(T(2) * x) + std::cos(pi / y);
   }

   static inline T func11(Type x, Type y)
   {
      return std::sqrt(T(111.111) - std::sin(T(2) * x) + std::cos(pi / y) / T(333.333));
   }

   static inline T func12(Type x, Type y)
   {
      return ((x * x) / std::sin(T(2) * pi / y)) - x / T(2);
   }

   static inline T func13(Type x, Type y)
   {
      return (x + (std::cos(y - std::sin(2 / x * pi)) - std::sin(x - std::cos(2 * y / pi))) - y);
   }

   static inline T func14(Type x, Type y)
   {
      return clamp(T(-1), std::sin(T(2) * pi * x) + std::cos(y / T(2) * pi), + T(1));
   }

   static inline T func15(Type x, Type y)
   {
      return std::max(T(3.33), std::min(sqrt(T(1) - std::sin(T(2) * x) + std::cos(pi / y) / T(3)), T(1.11)));
   }

   static inline T func16(Type x, Type y)
   {
      return (((y + (x * T(2.2))) <= (x + y + T(1.1))) ? x - y : x * y) + T(2) * pi / x;
   }
};

void pgo_primer();

int main()
{
   pgo_primer();

   double x = 0;
   double y = 0;

   exprtk::symbol_table<double> symbol_table;
   symbol_table.add_constants();
   symbol_table.add_variable("x",x);
   symbol_table.add_variable("y",y);

   std::deque<exprtk::expression<double> > expr_list;

   if (!load_expression(symbol_table,expr_list))
   {
      return 1;
   }

   {
      std::cout << "--- EXPRTK ---" << std::endl;
      for (std::size_t i = 0; i < expr_list.size(); ++i)
      {
         run_exprtk_benchmark(x,y,expr_list[i],expression_list[i]);
      }
   }

   {
      std::cout << "--- NATIVE ---" << std::endl;
      run_native_benchmark(x,y,native<double>::func00,expression_list[ 0]);
      run_native_benchmark(x,y,native<double>::func01,expression_list[ 1]);
      run_native_benchmark(x,y,native<double>::func02,expression_list[ 2]);
      run_native_benchmark(x,y,native<double>::func03,expression_list[ 3]);
      run_native_benchmark(x,y,native<double>::func04,expression_list[ 4]);
      run_native_benchmark(x,y,native<double>::func05,expression_list[ 5]);
      run_native_benchmark(x,y,native<double>::func06,expression_list[ 6]);
      run_native_benchmark(x,y,native<double>::func07,expression_list[ 7]);
      run_native_benchmark(x,y,native<double>::func08,expression_list[ 8]);
      run_native_benchmark(x,y,native<double>::func09,expression_list[ 9]);
      run_native_benchmark(x,y,native<double>::func10,expression_list[10]);
      run_native_benchmark(x,y,native<double>::func11,expression_list[11]);
      run_native_benchmark(x,y,native<double>::func12,expression_list[12]);
      run_native_benchmark(x,y,native<double>::func13,expression_list[13]);
      run_native_benchmark(x,y,native<double>::func14,expression_list[14]);
      run_native_benchmark(x,y,native<double>::func15,expression_list[15]);
      run_native_benchmark(x,y,native<double>::func16,expression_list[16]);
   }

   {
      std::cout << "--- PARSE ----" << std::endl;
      run_parse_benchmark(symbol_table);
   }

   return 0;
}

void pgo_primer()
{
   exprtk::pgo_primer<double>();

   static const double lower_bound_x = -50.0;
   static const double lower_bound_y = -50.0;
   static const double upper_bound_x = +50.0;
   static const double upper_bound_y = +50.0;
   static const double delta = 0.07;

   double total = 0.0;

   for (double x = lower_bound_x; x <= upper_bound_x; x += delta)
   {
      for (double y = lower_bound_y; y <= upper_bound_y; y += delta)
      {
         total += native<double>::func00(x,y);
         total += native<double>::func01(x,y);
         total += native<double>::func02(x,y);
         total += native<double>::func03(x,y);
         total += native<double>::func04(x,y);
         total += native<double>::func05(x,y);
         total += native<double>::func06(x,y);
         total += native<double>::func07(x,y);
         total += native<double>::func08(x,y);
         total += native<double>::func09(x,y);
         total += native<double>::func10(x,y);
         total += native<double>::func11(x,y);
         total += native<double>::func12(x,y);
         total += native<double>::func13(x,y);
         total += native<double>::func14(x,y);
         total += native<double>::func15(x,y);
         total += native<double>::func16(x,y);
      }
   }
}
