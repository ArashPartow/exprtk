C++ Mathematical Expression Toolkit Library

[INTRODUCTION]
The C++ Mathematical Expression Library  (ExprTk) is a simple to  use,
easy  to  integrate and  extremely  efficient mathematical  expression
parsing and  evaluation engine.  The parsing  engine supports  various
kinds of functional and logic  processing semantics and is very easily
extendible.



[CAPABILITIES]
The ExprTk evaluator  supports the following  fundamental mathematical
operations, functions and processes:

(1) Basic operators: +, -, *, /, %, ^

(2) Functions:       min, max, avg, sum, abs, ceil, floor, round,
                     roundn, exp, log, log10, logn, root, sqrt,
                     clamp, inrange, sgn, erf, erfc, frac, trunc

(3) Trigonometry:    sin, cos, tan, acos, asin, atan, atan2, cosh,
                     cot, csc, sec, sinh, tanh, rad2deg, deg2rad,
                     deg2grad, grad2deg, hyp

(4) Equalities &
    Inequalities:    =, ==, <>, !=, <, <=, >, >=

(5) Boolean logic:   and, or, xor, not, nand, nor, shr, shl, true,
                     false

(6) Conditional &
    Loop statement:  if-then-else, while

(7) Assignment:      :=

(8) Calculus:        numerical integration and differentiation

Note 1: Normal mathematical operator precedence is applied (BEDMAS).
Note 2: The trigonometry functions assume radians as input.



[EXAMPLE EXPRESSIONS]
The  following  is  a  short  sample  of  the  types  of  mathematical
expressions that can be parsed and evaluated using the ExprTk library.

(01) sqrt(1 - (x^2))
(02) clamp(-1,sin(2 * pi * x) + cos(y / 2 * pi),+1)
(03) sin(2 * x)
(04) if(((x + 2) == 3) and ((y + 5) <= 9),1 + w, 2 / z)
(05) inrange(-2,m,+2) == if(({-2 <= m} and [m <= +2]),1,0)
(06) ({1/1}*[1/2]+(1/3))-{1/4}^[1/5]+(1/6)-({1/7}+[1/8]*(1/9))
(07) a * exp(2 * t) + c
(08) z := x + sin(2 * pi / y)
(09) u := 2 * (pi * z) / (w := x + cos(y / pi))
(10) 2x + 3y + 4z + 5w == 2 * x + 3 * y + 4 * z + 5 * w
(11) 3(x + y) / 2 + 1 == 3 * (x + y) / 2 + 1
(12) (x + y)3 + 1 / 4 == (x + y) * 3 + 1 / 4
(13) (x + y)z + 1 / 2 == (x + y) * z + 1 / 2
(14) (sin(x/pi)cos(2y) + 1)==(sin(x / pi) * cos(2 * y) + 1)
(15) 25x^5 - 35x^4 - 15x^3 + 40x^2 - 15x + 1
(16) if (avg(x,y) <= x + y, x - y, x * y) + 2 * pi / x
(17) fib_i := fib_i + (x := y + 0 * (fib_i := x + (y := fib_i)))
(18) while (x <= 100) { x := x + 1 }
(19) x <= 'abc123' and (y in 'AString') or ('1x2y3z' != z)
(20) (x like '*123*') or ('a123b' ilike y)



[COPYRIGHT NOTICE]
Free use of the  Mathematical Expression Toolkit Library  is permitted
under the guidelines and in  accordance with the most current  version
of the Common Public License.

http://www.opensource.org/licenses/cpl1.0.php



[DOWNLOADS & UPDATES]
All  updates  and  the most  recent version  of the  C++ Mathematical
Expression Library can be found at:
(1) http://www.partow.net/programming/exprtk/index.html
(2) svn checkout http://exprtk.googlecode.com/svn/ exprtk



[INSTALLATION]
(1) exprtk.hpp should be  placed in  a project or system include path
(e.g: /usr/include/).



[COMPILATION]
(1) For a complete build: make clean all
(2) For a PGO build: make clean pgo
(3) To strip executables: make strip_bin



[COMPILER COMPATIBILITY]
(*) GNU Compiler Collection (4.3+)
(*) Intel® C++ Compiler (9.x+)
(*) Clang/LLVM (1.1+)
(*) PGI C++ (10.x+)
(*) Microsoft Visual Studio C++ Compiler (8.1+)
(*) Comeau C++ Compiler (4.3+)



[SPECIAL FUNCTIONS]
The purpose  of special  functions in  ExprTk is  to provide  compiler
generated equivalents of common mathematical expressions which can  be
invoked by  using the  'special function'  syntax (eg:  $f12(x,y,z) or
$f24(x,y,z,w)).

Special functions dramatically decrease  the total evaluation time  of
expressions which would otherwise  have been written using  the common
form by reducing the total number  of nodes in the evaluation tree  of
an  expression  and  by  also  leveraging  the  compiler's  ability to
correctly optimize such expressions for a given architecture.

          3-Parameter                       4-Parameter
 +-------------+-------------+    +--------------+------------------+
 |  Prototype  |  Operation  |    |  Prototype   |    Operation     |
 +-------------+-------------+    +--------------+------------------+
   $f00(x,y,z) |  (x + y) / z      $f46(x,y,z,w) | x + ((y + z) / w)
   $f01(x,y,z) |  (x + y) * z      $f47(x,y,z,w) | x + ((y + z) * w)
   $f02(x,y,z) |  (x + y) - z      $f48(x,y,z,w) | x + ((y - z) / w)
   $f03(x,y,z) |  (x + y) + z      $f49(x,y,z,w) | x + ((y - z) * w)
   $f04(x,y,z) |  (x - y) / z      $f50(x,y,z,w) | x + ((y * z) / w)
   $f05(x,y,z) |  (x - y) * z      $f51(x,y,z,w) | x + ((y * z) * w)
   $f06(x,y,z) |  (x * y) + z      $f52(x,y,z,w) | x + ((y / z) + w)
   $f07(x,y,z) |  (x * y) - z      $f53(x,y,z,w) | x + ((y / z) / w)
   $f08(x,y,z) |  (x * y) / z      $f54(x,y,z,w) | x + ((y / z) * w)
   $f09(x,y,z) |  (x * y) * z      $f55(x,y,z,w) | x - ((y + z) / w)
   $f10(x,y,z) |  (x / y) + z      $f56(x,y,z,w) | x - ((y + z) * w)
   $f11(x,y,z) |  (x / y) - z      $f57(x,y,z,w) | x - ((y - z) / w)
   $f12(x,y,z) |  (x / y) / z      $f58(x,y,z,w) | x - ((y - z) * w)
   $f13(x,y,z) |  (x / y) * z      $f59(x,y,z,w) | x - ((y * z) / w)
   $f14(x,y,z) |  x / (y + z)      $f60(x,y,z,w) | x - ((y * z) * w)
   $f15(x,y,z) |  x / (y - z)      $f61(x,y,z,w) | x - ((y / z) / w)
   $f16(x,y,z) |  x / (y * z)      $f62(x,y,z,w) | x - ((y / z) * w)
   $f17(x,y,z) |  x / (y / z)      $f63(x,y,z,w) | ((x + y) * z) - w
   $f18(x,y,z) |  x * (y + z)      $f64(x,y,z,w) | ((x - y) * z) - w
   $f19(x,y,z) |  x * (y - z)      $f65(x,y,z,w) | ((x * y) * z) - w
   $f20(x,y,z) |  x * (y * z)      $f66(x,y,z,w) | ((x / y) * z) - w
   $f21(x,y,z) |  x * (y / z)      $f67(x,y,z,w) | ((x + y) / z) - w
   $f22(x,y,z) |  x - (y / z)      $f68(x,y,z,w) | ((x - y) / z) - w
   $f23(x,y,z) |  x - (y / z)      $f69(x,y,z,w) | ((x * y) / z) - w
   $f24(x,y,z) |  x - (y * z)      $f70(x,y,z,w) | ((x / y) / z) - w
   $f25(x,y,z) |  x + (y * z)      $f71(x,y,z,w) | (x * y) + (z * w)
   $f26(x,y,z) |  x + (y / z)      $f72(x,y,z,w) | (x * y) - (z * w)
   $f27(x,y,z) |  x + (y + z)      $f73(x,y,z,w) | (x * y) + (z / w)
   $f28(x,y,z) |  x + (y - z)      $f74(x,y,z,w) | (x * y) - (z / w)
   $f29(x,y,z) |  x * y^2 + z      $f75(x,y,z,w) | (x / y) + (z / w)
   $f30(x,y,z) |  x * y^3 + z      $f76(x,y,z,w) | (x / y) - (z / w)
   $f31(x,y,z) |  x * y^4 + z      $f77(x,y,z,w) | (x / y) - (z * w)
   $f32(x,y,z) |  x * y^5 + z      $f78(x,y,z,w) | x / (y + (z * w))
   $f33(x,y,z) |  x * y^6 + z      $f79(x,y,z,w) | x / (y - (z * w))
   $f34(x,y,z) |  x * y^7 + z      $f80(x,y,z,w) | x * (y + (z * w))
   $f35(x,y,z) |  x * y^8 + z      $f81(x,y,z,w) | x * (y - (z * w))
   $f36(x,y,z) |  x * y^9 + z      $f82(x,y,z,w) | x * y^2 + z * w^2
   $f37(x,y,z) |  x * log(y)+z     $f83(x,y,z,w) | x * y^3 + z * w^3
   $f38(x,y,z) |  x * log(y)-z     $f84(x,y,z,w) | x * y^4 + z * w^4
   $f39(x,y,z) |  x * log10(y)+z   $f85(x,y,z,w) | x * y^5 + z * w^5
   $f40(x,y,z) |  x * log10(y)-z   $f86(x,y,z,w) | x * y^6 + z * w^6
   $f41(x,y,z) |  x * sin(y)+z     $f87(x,y,z,w) | x * y^7 + z * w^7
   $f42(x,y,z) |  x * sin(y)-z     $f88(x,y,z,w) | x * y^8 + z * w^8
   $f43(x,y,z) |  x * cos(y)+z     $f89(x,y,z,w) | x * y^9 + z * w^9
   $f44(x,y,z) |  x * cos(y)-z     $f90(x,y,z,w) | (x and y) ? z : w
   $f45(x,y,z) |  x ? y : z        $f91(x,y,z,w) | (x  or y) ? z : w
                                   $f92(x,y,z,w) | (x <   y) ? z : w
                                   $f93(x,y,z,w) | (x <=  y) ? z : w
                                   $f94(x,y,z,w) | (x >   y) ? z : w
                                   $f95(x,y,z,w) | (x >=  y) ? z : w
                                   $f96(x,y,z,w) | (x ==  y) ? z : w
                                   $f97(x,y,z,w) | x*sin(y) + z*cos(w)



[SIMPLE EXAMPLE]
--- snip ---
#include <cstdio>
#include <string>

#include "exprtk.hpp"

int main()
{
   typedef exprtk::symbol_table<double> symbol_table_t;
   typedef exprtk::expression<double>     expression_t;
   typedef exprtk::parser<double>             parser_t;
   typedef exprtk::parser_error::type          error_t;

   std::string expression_str = "z := 2 [sin(x/pi)^3 + cos(pi/y)^4]";

   double x = 1.1;
   double y = 2.2;
   double z = 3.3;

   symbol_table_t symbol_table;
   symbol_table.add_constants();
   symbol_table.add_variable("x",x);
   symbol_table.add_variable("y",y);
   symbol_table.add_variable("z",z);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;

   if (!parser.compile(expression_str,expression))
   {
      printf("Error: %s\tExpression: %s\n",
             parser.error().c_str(),
             expression_str.c_str());

      for (std::size_t i = 0; i < parser.error_count(); ++i)
      {
         error_t error = parser.get_error(i);
         printf("Err: %02d Pos: %02d Type: [%s] Msg: %s Expr: %s\n",
                static_cast<int>(i),
                static_cast<int>(error.token.position),
                exprtk::parser_error::to_str(error.mode).c_str(),
                error.diagnostic.c_str(),
                expression_str.c_str());
      }

      return 1;
   }

   double result = expression.value();

   printf("Result: %10.5f\n",result);

   return 0;
}
--- snip ---



[FILES]
(00) Makefile
(01) readme.txt
(02) exprtk.hpp
(03) exprtk_test.cpp
(04) exprtk_benchmark.cpp
