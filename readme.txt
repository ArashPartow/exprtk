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
                     clamp, inrange, sgn, erf, erfc

(3) Trigonometry:    sin, cos, tan, acos, asin, atan, atan2, cosh,
                     cot, csc, sec, sinh, tanh, rad2deg, deg2rad,
                     deg2grad, grad2deg, hyp

(4) Equalities &
    Inequalities:    =, ==, <>, !=, <, <=, >, >=,

(5) Boolean logic:   and, or, xor, not, nand, nor, shr, shl, true,
                     false

(6) Conditional &
    Loop statement:  if-then-else, while

(7) Assigment:       :=



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
(16) if(avg(x,y) <= x + y, x - y, x * y) + 2 * pi / x
(17) fib_i := fib_i + (x := y + 0 * (fib_i := x + (y := fib_i)))
(18) while(x <= 100) { x := x + 1 }
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

         3-Parameter                     4-Parameter
 +------------+-------------+   +--------------+------------------+
 | Prototype  |  Operation  |   | Prototype    |    Operation     |
 +------------+-------------+   +--------------+------------------+
  sf00(x,y,z) | (x + y) / z      sf35(x,y,z,w) | x + ((y + z) / w)
  sf01(x,y,z) | (x + y) * z      sf36(x,y,z,w) | x + ((y + z) * w)
  sf02(x,y,z) | (x - y) / z      sf37(x,y,z,w) | x + ((y - z) / w)
  sf03(x,y,z) | (x - y) * z      sf38(x,y,z,w) | x + ((y - z) * w)
  sf04(x,y,z) | (x * y) + z      sf39(x,y,z,w) | x + ((y * z) / w)
  sf05(x,y,z) | (x * y) - z      sf40(x,y,z,w) | x + ((y * z) * w)
  sf06(x,y,z) | (x * y) / z      sf41(x,y,z,w) | x + ((y / z) + w)
  sf07(x,y,z) | (x * y) * z      sf42(x,y,z,w) | x + ((y / z) / w)
  sf08(x,y,z) | (x / y) + z      sf43(x,y,z,w) | x + ((y / z) * w)
  sf09(x,y,z) | (x / y) - z      sf44(x,y,z,w) | x - ((y + z) / w)
  sf10(x,y,z) | (x / y) / z      sf45(x,y,z,w) | x - ((y + z) * w)
  sf11(x,y,z) | (x / y) * z      sf46(x,y,z,w) | x - ((y - z) / w)
  sf12(x,y,z) | x / (y + z)      sf47(x,y,z,w) | x - ((y - z) * w)
  sf13(x,y,z) | x / (y - z)      sf48(x,y,z,w) | x - ((y * z) / w)
  sf14(x,y,z) | x / (y * z)      sf49(x,y,z,w) | x - ((y * z) * w)
  sf15(x,y,z) | x / (y / z)      sf50(x,y,z,w) | x - ((y / z) / w)
  sf16(x,y,z) | x - (y / z)      sf51(x,y,z,w) | x - ((y / z) * w)
  sf17(x,y,z) | x - (y / z)      sf52(x,y,z,w) | ((x + y) * z) - w
  sf18(x,y,z) | x * y^2 + z      sf53(x,y,z,w) | ((x - y) * z) - w
  sf19(x,y,z) | x * y^3 + z      sf54(x,y,z,w) | ((x * y) * z) - w
  sf20(x,y,z) | x * y^4 + z      sf55(x,y,z,w) | ((x / y) * z) - w
  sf21(x,y,z) | x * y^5 + z      sf56(x,y,z,w) | ((x + y) / z) - w
  sf22(x,y,z) | x * y^6 + z      sf57(x,y,z,w) | ((x - y) / z) - w
  sf23(x,y,z) | x * y^7 + z      sf58(x,y,z,w) | ((x * y) / z) - w
  sf24(x,y,z) | x * y^8 + z      sf59(x,y,z,w) | ((x / y) / z) - w
  sf25(x,y,z) | x * y^9 + z      sf60(x,y,z,w) | (x * y) + (z * w)
  sf26(x,y,z) | x * log(y)+z     sf61(x,y,z,w) | (x * y) - (z * w)
  sf27(x,y,z) | x * log(y)-z     sf62(x,y,z,w) | (x / y) + (z / w)
  sf28(x,y,z) | x * log10(y)+z   sf63(x,y,z,w) | (x / y) - (z / w)
  sf29(x,y,z) | x * log10(y)-z   sf64(x,y,z,w) | x * y^2 + z * w^2
  sf30(x,y,z) | x * sin(y)+z     sf65(x,y,z,w) | x * y^3 + z * w^3
  sf31(x,y,z) | x * sin(y)-z     sf66(x,y,z,w) | x * y^4 + z * w^4
  sf32(x,y,z) | x * cos(y)+z     sf67(x,y,z,w) | x * y^5 + z * w^5
  sf33(x,y,z) | x * cos(y)-z     sf68(x,y,z,w) | x * y^6 + z * w^6
  sf34(x,y,z) | x ? y : z        sf69(x,y,z,w) | x * y^7 + z * w^7
                                 sf70(x,y,z,w) | x * y^8 + z * w^8
                                 sf71(x,y,z,w) | x * y^9 + z * w^9
                                 sf72(x,y,z,w) | (x and y) ? z : w
                                 sf73(x,y,z,w) | (x  or y) ? z : w
                                 sf74(x,y,z,w) | (x <   y) ? z : w
                                 sf75(x,y,z,w) | (x <=  y) ? z : w
                                 sf76(x,y,z,w) | (x >   y) ? z : w
                                 sf77(x,y,z,w) | (x >=  y) ? z : w
                                 sf78(x,y,z,w) | (x ==  y) ? z : w
                                 sf79(x,y,z,w) | x*sin(y) + z*cos(w)



[MACROS]
ExprTk utilizes certain macros  to modify the underlying  behaviour of
the parser and the evaluation engine. The following macros are used to
switch off certain capabilities  within the ExprTk evaluation  engine.
The capabilities are predominantly related to expression optimisations
and the ability to evaluate strings within expressions.

(1) exprtk_disable_string_capabilities
(2) exprtk_disable_cardinal_pow_optimisation
(3) exprtk_disable_extended_optimisations
(4) exprtk_disable_extended_operator_optimizations
(5) exprtk_lean_and_mean
(6) exprtk_lean_and_mean_numeric_only

(1) "exprtk_disable_string_capabilities"
If defined, the macro will disable all string processing capabilities.
When defined, if an expression  containing a string or string  related
action  is  encountered,  a compilation  error will  be raised  by the
parser.

(2) "exprtk_disable_cardinal_pow_optimisation"
If  defined,  the  macro  will  disable  the  special  case  regarding
exponentiation  of  a  variable  to  an  integer  constant  (where the
constant is  <= 60).  Defining this  variable may  be desirable if the
error magnitude of the results using this special case are intolerable
with regards to the precision required. When defined, the pow function
used for all other powers will be invoked.

(3) "exprtk_disable_extended_optimisations"
If defined, the macro will disable the third tier optimisations.  This
group of  optimisations creates  roughly 4K  type instantiations. This
large number of type and branch instantiations in one translation unit
may cause some  older compilers to  crash or not  be able to  properly
compile  ExprTk.  If  such compiler  problems  are  encountered it  is
recommended to test  having this particular  macro defined. It  should
also  be noted  that some  of the  third tier  optimisations are  also
available through  the predefined  'special functions',  however these
require that expressions utilize them explicitly.

(4) "exprtk_disable_extended_operator_optimizations"
By default most of the mathematical operators are included as part  of
the optimisation process. However if this macro is defined, then  only
the basic mathematical operators (+,-,*,/,^) will be included.

(5) "exprtk_lean_and_mean"
The default mode  of ExprTk is  lean and mean.  This macro encompasses
both modes [3] and [4].

(6) "exprtk_lean_and_mean_numeric_only"
The mode when this macro is  defined, is 'lean and mean' coupled  with
all string capabilities disabled [1].

Note:  Foregoing  a  few  extra  clock  cycles  during  compilation in
exchange  for a  dramatic increase in performance  during run-time  is
always a worthy undertaking.



[FILES]
(00) Makefile
(01) readme.txt
(02) exprtk.hpp
(03) exprtk_test.cpp
(04) exprtk_benchmark.cpp
