/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Benchmarks                                                 *
 * Author: Arash Partow (1999-2012)                           *
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
#include <string>
#include <deque>

#include "exprtk.hpp"

const std::string expression_list[] = {
                                         "(y + x)",
                                         "2 * (y + x)",
                                         "(2 * y + 2 * x)",
                                         "(y + x / y) * (x - y / x)",
                                         "x / ((x + y) * (x - y)) / y",
                                         "1 - ((x * y) + (y / x)) - 3",
                                         "1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55",
                                         "sin(2 * x) + cos(pi / y)",
                                         "1 - sin(2 * x) + cos(pi / y)",
                                         "sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)",
                                         "(x^2 / sin(2 * pi / y)) -x / 2",
                                         "x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y",
                                         "clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)",
                                         "max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))",
                                         "if(avg(x,y) <= x + y, x - y, x * y) + 2 * pi / x"
                                      };
const std::size_t expression_list_size = sizeof(expression_list) / sizeof(std::string);

template <typename T,
          typename Allocator,
          template <typename,typename> class Sequence>
bool load_expression(exprtk::symbol_table<T>& symbol_table,
                     Sequence<exprtk::expression<T>,Allocator>& expr_seq,
                     const typename exprtk::parser<T>::optimization_level& opt_level)
{
   exprtk::parser<double> parser;
   for (std::size_t i = 0; i < expression_list_size; ++i)
   {
      exprtk::expression<double> expression;
      expression.register_symbol_table(symbol_table);
      if (!parser.compile(expression_list[i],expression,opt_level))
      {
         std::cout << "[load_expression] - Parser Error: " << parser.error() << "\tExpression: " << expression_list[i] << std::endl;
         return false;
      }
      expr_seq.push_back(expression);
   }
   return true;
}

template <typename T>
void run_benchmark(T& x, T& y,
                   exprtk::expression<T>& expression,
                   const std::string& expr_string)
{
   static const double lower_bound = -85.0;
   static const double upper_bound = +85.0;
   double delta = 0.01;
   double total = 0.0;
   unsigned int count = 0;
   exprtk::timer timer;
   timer.start();
   for (x = lower_bound; x <= upper_bound; x += delta)
   {
      for (y = lower_bound; y <= upper_bound; y += delta)
      {
         total += expression.value();
         ++count;
      }
   }
   timer.stop();
   if (T(0.0) != total)
      printf("[exprtk] Total Time:%12.8f  Rate:%14.3fevals/sec Expression: %s\n",
             timer.time(),
             count / timer.time(),
             expr_string.c_str());
   else
      std::cerr << "Error - expression: " << expr_string << "\n";
}

const double pi = 3.14159265358979323846;

template <typename T>
inline T avg(const T v1, const T v2)
{
   return (v1 + v2) / T(2.0);
}

template <typename T>
inline T clamp(const T l, const T v, const T u)
{
   return ((v < l) ? l : ((v > u) ? u : v));
}

template <typename T> inline T func00(const T x, const T y) { return (y + x); }
template <typename T> inline T func01(const T x, const T y) { return T(2.0) * (y + x); }
template <typename T> inline T func02(const T x, const T y) { return (T(2.0) * y + T(2.0) * x); }
template <typename T> inline T func03(const T x, const T y) { return (y + x / y) * (x - y / x); }
template <typename T> inline T func04(const T x, const T y) { return x / ((x + y) * (x - y)) / y; }
template <typename T> inline T func05(const T x, const T y) { return T(1.0) - ((x * y) + (y / x)) - T(3.0); }
template <typename T> inline T func06(const T x, const T y) { return (1.1*pow(x,T(1.0))+2.2*pow(y,T(2.0))-3.3*pow(x,T(3.0))+4.4*pow(y,T(15.0))-5.5*pow(x,T(23.0))+6.6*pow(y,T(55.0))); }
template <typename T> inline T func07(const T x, const T y) { return std::sin(T(2.0) * x) + std::cos(pi / y); }
template <typename T> inline T func08(const T x, const T y) { return T(1.0) - std::sin(2.0 * x) + std::cos(pi / y); }
template <typename T> inline T func09(const T x, const T y) { return std::sqrt(T(111.111) - std::sin(T(2.0) * x) + std::cos(pi / y) / T(333.333)); }
template <typename T> inline T func10(const T x, const T y) { return (std::pow(x,T(2.0)) / std::sin(T(2.0) * pi / y)) -x / T(2.0); }
template <typename T> inline T func11(const T x, const T y) { return (x + (std::cos(y - std::sin(2 / x * pi)) - std::sin(x - std::cos(2 * y / pi))) - y); }
template <typename T> inline T func12(const T x, const T y) { return clamp(T(-1.0), std::sin(T(2.0) * pi * x) + std::cos(y / T(2.0) * pi), + T(1.0)); }
template <typename T> inline T func13(const T x, const T y) { return std::max(T(3.33), std::min(sqrt(T(1.0) - std::sin(T(2.0) * x) + std::cos(pi / y) / T(3.0)), T(1.11))); }
template <typename T> inline T func14(const T x, const T y) { return ((avg(x,y) <= x + y) ? x - y : x * y) + T(2.0) * pi / x; }

template <typename T, typename NativeFunction>
void run_native_benchmark(T& x, T& y, NativeFunction f, const std::string& expr_string)
{
   static const double lower_bound = -85.0;
   static const double upper_bound = +85.0;
   double delta = 0.01;
   double total = 0.0;
   unsigned int count = 0;
   exprtk::timer timer;
   timer.start();
   for (x = lower_bound; x <= upper_bound; x += delta)
   {
      for (y = lower_bound; y <= upper_bound; y += delta)
      {
         total += f(x,y);
         ++count;
      }
   }
   timer.stop();
   if (T(0.0) != total)
      printf("[native] Total Time:%12.8f  Rate:%14.3fevals/sec Expression: %s\n",
             timer.time(),
             count / timer.time(),
             expr_string.c_str());
   else
      std::cerr << "Error - expression: " << expr_string << "\n";
}

template <typename T>
bool run_parse_benchmark(exprtk::symbol_table<T>& symbol_table)
{
   static const std::size_t rounds = 1000000;
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
            std::cout << "[run_parse_benchmark] - Parser Error: " << parser.error() << "\tExpression: " << expression_list[i] << std::endl;
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

int main()
{
   double x = 0;
   double y = 0;

   exprtk::symbol_table<double> symbol_table;
   symbol_table.add_constants();
   symbol_table.add_variable("x",x);
   symbol_table.add_variable("y",y);

   std::deque<exprtk::expression<double> > optimized_expr_list;
   std::deque<exprtk::expression<double> > expr_list;

   if (!load_expression(symbol_table,optimized_expr_list,exprtk::parser<double>::e_all))
   {
      return 1;
   }

   if (!load_expression(symbol_table,expr_list,exprtk::parser<double>::e_none))
   {
      return 1;
   }

   {
      std::cout << "--- EXPRTK [Opt All]---" << std::endl;
      for (std::size_t i = 0; i < optimized_expr_list.size(); ++i)
      {
         run_benchmark(x,y,optimized_expr_list[i],expression_list[i]);
      }

      std::cout << "--- EXPRTK [No Opt]---" << std::endl;
      for (std::size_t i = 0; i < expr_list.size(); ++i)
      {
         run_benchmark(x,y,expr_list[i],expression_list[i]);
      }
   }

   {
      std::cout << "--- NATIVE ---" << std::endl;
      run_native_benchmark(x,y,func00<double>,expression_list[ 0]);
      run_native_benchmark(x,y,func01<double>,expression_list[ 1]);
      run_native_benchmark(x,y,func02<double>,expression_list[ 2]);
      run_native_benchmark(x,y,func03<double>,expression_list[ 3]);
      run_native_benchmark(x,y,func04<double>,expression_list[ 4]);
      run_native_benchmark(x,y,func05<double>,expression_list[ 5]);
      run_native_benchmark(x,y,func06<double>,expression_list[ 6]);
      run_native_benchmark(x,y,func07<double>,expression_list[ 7]);
      run_native_benchmark(x,y,func08<double>,expression_list[ 8]);
      run_native_benchmark(x,y,func09<double>,expression_list[ 9]);
      run_native_benchmark(x,y,func10<double>,expression_list[10]);
      run_native_benchmark(x,y,func11<double>,expression_list[11]);
      run_native_benchmark(x,y,func12<double>,expression_list[12]);
      run_native_benchmark(x,y,func13<double>,expression_list[13]);
      run_native_benchmark(x,y,func14<double>,expression_list[14]);
   }

   {
      std::cout << "--- PARSE ----" << std::endl;
      run_parse_benchmark(symbol_table);
   }

   return 0;
}
