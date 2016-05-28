C++ Mathematical Expression Toolkit Library

[00 - INTRODUCTION]
The C++ Mathematical Expression  Toolkit Library (ExprTk) is  a simple
to  use,   easy  to   integrate  and   extremely  efficient   run-time
mathematical  expression parsing  and evaluation  engine. The  parsing
engine  supports numerous  forms  of  functional and  logic processing
semantics and is easily extendible.

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[01 - CAPABILITIES]
The  ExprTk expression  evaluator supports  the following  fundamental
arithmetic operations, functions and processes:

 (00) Types:           Scalar, Vector, String

 (01) Basic operators: +, -, *, /, %, ^

 (02) Assignment:      :=, +=, -=, *=, /=, %=

 (03) Equalities &
      Inequalities:    =, ==, <>, !=, <, <=, >, >=

 (04) Boolean logic:   and, mand, mor, nand, nor, not, or, shl, shr,
                       xnor, xor, true, false

 (05) Functions:       abs, avg, ceil, clamp, equal, erf, erfc,  exp,
                       expm1, floor, frac,  log, log10, log1p,  log2,
                       logn,  max,  min,  mul,  ncdf,  nequal,  root,
                       round, roundn, sgn, sqrt, sum, swap, trunc

 (06) Trigonometry:    acos, acosh, asin, asinh, atan, atanh,  atan2,
                       cos,  cosh, cot,  csc, sec,  sin, sinc,  sinh,
                       tan, tanh, hypot, rad2deg, deg2grad,  deg2rad,
                       grad2deg

 (07) Control
      structures:      if-then-else, ternary conditional, switch-case,
                       return-statement

 (08) Loop statements: while, for, repeat-until, break, continue

 (09) String
      processing:      in, like, ilike, concatenation

 (10) Optimisations:   constant-folding, simple strength reduction and
                       dead code elimination

 (11) Calculus:        numerical integration and differentiation

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[02 - EXAMPLE EXPRESSIONS]
The  following  is  a  short listing  of  the  types  of  mathematical
expressions that can be parsed and evaluated using the ExprTk library.

  (01) sqrt(1 - (3 / x^2))
  (02) clamp(-1, sin(2 * pi * x) + cos(y / 2 * pi), +1)
  (03) sin(2.34e-3 * x)
  (04) if(((x[2] + 2) == 3) and ((y + 5) <= 9),1 + w, 2 / z)
  (05) inrange(-2,m,+2) == if(({-2 <= m} and [m <= +2]),1,0)
  (06) ({1/1}*[1/2]+(1/3))-{1/4}^[1/5]+(1/6)-({1/7}+[1/8]*(1/9))
  (07) a * exp(2.2 / 3.3 * t) + c
  (08) z := x + sin(2.567 * pi / y)
  (09) u := 2.123 * {pi * z} / (w := x + cos(y / pi))
  (10) 2x + 3y + 4z + 5w == 2 * x + 3 * y + 4 * z + 5 * w
  (11) 3(x + y) / 2.9 + 1.234e+12 == 3 * (x + y) / 2.9 + 1.234e+12
  (12) (x + y)3.3 + 1 / 4.5 == [x + y] * 3.3 + 1 / 4.5
  (13) (x + y[i])z + 1.1 / 2.7 == (x + y[i]) * z + 1.1 / 2.7
  (14) (sin(x / pi) cos(2y) + 1) == (sin(x / pi) * cos(2 * y) + 1)
  (15) 75x^17 + 25.1x^5 - 35x^4 - 15.2x^3 + 40x^2 - 15.3x + 1
  (16) (avg(x,y) <= x + y ? x - y : x * y) + 2.345 * pi / x
  (17) while (x <= 100) { x -= 1; }
  (18) x <= 'abc123' and (y in 'AString') or ('1x2y3z' != z)
  (19) ((x + 'abc') like '*123*') or ('a123b' ilike y)
  (20) sgn(+1.2^3.4z / -5.6y) <= {-7.8^9 / -10.11x }

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[03 - COPYRIGHT NOTICE]
Free  use  of  the  C++  Mathematical  Expression  Toolkit  Library is
permitted under the guidelines and in accordance with the most current
version of the Common Public License.

http://www.opensource.org/licenses/cpl1.0.php

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[04 - DOWNLOADS & UPDATES]
The most  recent version  of the C++ Mathematical  Expression  Toolkit
Library including all updates and tests can be found at the  following
locations:

  (a) Download:   http://www.partow.net/programming/exprtk/index.html
  (b) Repository: https://github.com/ArashPartow/exprtk

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[05 - INSTALLATION]
The header  file exprtk.hpp  should be  placed in a project or  system
include path (e.g: /usr/include/).

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[06 - COMPILATION]
  (a) For a complete build: make clean all
  (b) For a PGO build: make clean pgo
  (c) To strip executables: make strip_bin
  (d) Execute valgrind check: make valgrind_check

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[07 - COMPILER COMPATIBILITY]
ExprTk has been built error and warning free using the following set
of C++ compilers:

  (*) GNU Compiler Collection (3.3+)
  (*) Intel C++ Compiler (8.x+)
  (*) Clang/LLVM (1.1+)
  (*) PGI C++ (10.x+)
  (*) Microsoft Visual Studio C++ Compiler (8.1+)
  (*) IBM XL C/C++ (9.x+)
  (*) C++ Builder (XE4+)

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[08 - BUILT-IN OPERATIONS & FUNCTIONS]

(0) Arithmetic & Assignment Operators
+----------+---------------------------------------------------------+
| OPERATOR | DEFINITION                                              |
+----------+---------------------------------------------------------+
|  +       | Addition between x and y.  (eg: x + y)                  |
+----------+---------------------------------------------------------+
|  -       | Subtraction between x and y.  (eg: x - y)               |
+----------+---------------------------------------------------------+
|  *       | Multiplication between x and y.  (eg: x * y)            |
+----------+---------------------------------------------------------+
|  /       | Division between x and y.  (eg: x / y)                  |
+----------+---------------------------------------------------------+
|  %       | Modulus of x with respect to y.  (eg: x % y)            |
+----------+---------------------------------------------------------+
|  ^       | x to the power of y.  (eg: x ^ y)                       |
+----------+---------------------------------------------------------+
|  :=      | Assign the value of x to y. Where y is either a variable|
|          | or vector type.  (eg: y := x)                           |
+----------+---------------------------------------------------------+
|  +=      | Increment x by the value of the expression on the right |
|          | hand side. Where x is either a variable or vector type. |
|          | (eg: x += abs(y - z))                                   |
+----------+---------------------------------------------------------+
|  -=      | Decrement x by the value of the expression on the right |
|          | hand side. Where x is either a variable or vector type. |
|          | (eg: x[i] -= abs(y + z))                                |
+----------+---------------------------------------------------------+
|  *=      | Assign the multiplication of x by the value of the      |
|          | expression on the righthand side to x. Where x is either|
|          | a variable or vector type.                              |
|          | (eg: x *= abs(y / z))                                   |
+----------+---------------------------------------------------------+
|  /=      | Assign the division of x by the value of the expression |
|          | on the right-hand side to x. Where x is either a        |
|          | variable or vector type.  (eg: x[i + j] /= abs(y * z))  |
+----------+---------------------------------------------------------+
|  %=      | Assign x modulo the value of the expression on the right|
|          | hand side to x. Where x is either a variable or vector  |
|          | type.  (eg: x[2] %= y ^ 2)                              |
+----------+---------------------------------------------------------+

(1) Equalities & Inequalities
+----------+---------------------------------------------------------+
| OPERATOR | DEFINITION                                              |
+----------+---------------------------------------------------------+
| == or =  | True only if x is strictly equal to y. (eg: x == y)     |
+----------+---------------------------------------------------------+
| <> or != | True only if x does not equal y. (eg: x <> y or x != y) |
+----------+---------------------------------------------------------+
|  <       | True only if x is less than y. (eg: x < y)              |
+----------+---------------------------------------------------------+
|  <=      | True only if x is less than or equal to y. (eg: x <= y) |
+----------+---------------------------------------------------------+
|  >       | True only if x is greater than y. (eg: x > y)           |
+----------+---------------------------------------------------------+
|  >=      | True only if x greater than or equal to y. (eg: x >= y) |
+----------+---------------------------------------------------------+

(2) Boolean Operations
+----------+---------------------------------------------------------+
| OPERATOR | DEFINITION                                              |
+----------+---------------------------------------------------------+
| true     | True state or any value other than zero (typically 1).  |
+----------+---------------------------------------------------------+
| false    | False state, value of zero.                             |
+----------+---------------------------------------------------------+
| and      | Logical AND, True only if x and y are both true.        |
|          | (eg: x and y)                                           |
+----------+---------------------------------------------------------+
| mand     | Multi-input logical AND, True only if all inputs are    |
|          | true. Left to right short-circuiting of expressions.    |
|          | (eg: mand(x > y, z < w, u or v, w and x))               |
+----------+---------------------------------------------------------+
| mor      | Multi-input logical OR, True if at least one of the     |
|          | inputs are true. Left to right short-circuiting of      |
|          | expressions.  (eg: mor(x > y, z < w, u or v, w and x))  |
+----------+---------------------------------------------------------+
| nand     | Logical NAND, True only if either x or y is false.      |
|          | (eg: x nand y)                                          |
+----------+---------------------------------------------------------+
| nor      | Logical NOR, True only if the result of x or y is false |
|          | (eg: x nor y)                                           |
+----------+---------------------------------------------------------+
| not      | Logical NOT, Negate the logical sense of the input.     |
|          | (eg: not(x and y) == x nand y)                          |
+----------+---------------------------------------------------------+
| or       | Logical OR, True if either x or y is true. (eg: x or y) |
+----------+---------------------------------------------------------+
| xor      | Logical XOR, True only if the logical states of x and y |
|          | differ.  (eg: x xor y)                                  |
+----------+---------------------------------------------------------+
| xnor     | Logical XNOR, True iff the biconditional of x and y is  |
|          | satisfied.  (eg: x xnor y)                              |
+----------+---------------------------------------------------------+
| &        | Similar to AND but with left to right expression short  |
|          | circuiting optimisation.  (eg: (x & y) == (y and x))    |
+----------+---------------------------------------------------------+
| |        | Similar to OR but with left to right expression short   |
|          | circuiting optimisation.  (eg: (x | y) == (y or x))     |
+----------+---------------------------------------------------------+

(3) General Purpose Functions
+----------+---------------------------------------------------------+
| FUNCTION | DEFINITION                                              |
+----------+---------------------------------------------------------+
| abs      | Absolute value of x.  (eg: abs(x))                      |
+----------+---------------------------------------------------------+
| avg      | Average of all the inputs.                              |
|          | (eg: avg(x,y,z,w,u,v) == (x + y + z + w + u + v) / 6)   |
+----------+---------------------------------------------------------+
| ceil     | Smallest integer that is greater than or equal to x.    |
+----------+---------------------------------------------------------+
| clamp    | Clamp x in range between r0 and r1, where r0 < r1.      |
|          | (eg: clamp(r0,x,r1)                                     |
+----------+---------------------------------------------------------+
| equal    | Equality test between x and y using normalized epsilon  |
+----------+---------------------------------------------------------+
| erf      | Error function of x.  (eg: erf(x))                      |
+----------+---------------------------------------------------------+
| erfc     | Complimentary error function of x.  (eg: erfc(x))       |
+----------+---------------------------------------------------------+
| exp      | e to the power of x.  (eg: exp(x))                      |
+----------+---------------------------------------------------------+
| expm1    | e to the power of x minus 1, where x is very small.     |
|          | (eg: expm1(x))                                          |
+----------+---------------------------------------------------------+
| floor    | Largest integer that is less than or equal to x.        |
|          | (eg: floor(x))                                          |
+----------+---------------------------------------------------------+
| frac     | Fractional portion of x.  (eg: frac(x))                 |
+----------+---------------------------------------------------------+
| hypot    | Hypotenuse of x and y (eg: hypot(x,y) = sqrt(x*x + y*y))|
+----------+---------------------------------------------------------+
| iclamp   | Inverse-clamp x outside of the range r0 and r1. Where   |
|          | r0 < r1. If x is within the range it will snap to the   |
|          | closest bound. (eg: iclamp(r0,x,r1)                     |
+----------+---------------------------------------------------------+
| inrange  | In-range returns 'true' when x is within the range r0   |
|          | and r1. Where r0 < r1.  (eg: inrange(r0,x,r1)           |
+----------+---------------------------------------------------------+
| log      | Natural logarithm of x.  (eg: log(x))                   |
+----------+---------------------------------------------------------+
| log10    | Base 10 logarithm of x.  (eg: log10(x))                 |
+----------+---------------------------------------------------------+
| log1p    | Natural logarithm of 1 + x, where x is very small.      |
|          | (eg: log1p(x))                                          |
+----------+---------------------------------------------------------+
| log2     | Base 2 logarithm of x.  (eg: log2(x))                   |
+----------+---------------------------------------------------------+
| logn     | Base N logarithm of x. where n is a positive integer.   |
|          | (eg: logn(x,8))                                         |
+----------+---------------------------------------------------------+
| max      | Largest value of all the inputs. (eg: max(x,y,z,w,u,v)) |
+----------+---------------------------------------------------------+
| min      | Smallest value of all the inputs. (eg: min(x,y,z,w,u))  |
+----------+---------------------------------------------------------+
| mul      | Product of all the inputs.                              |
|          | (eg: mul(x,y,z,w,u,v,t) == (x * y * z * w * u * v * t)) |
+----------+---------------------------------------------------------+
| ncdf     | Normal cumulative distribution function.  (eg: ncdf(x)) |
+----------+---------------------------------------------------------+
| nequal   | Not-equal test between x and y using normalized epsilon |
+----------+---------------------------------------------------------+
| root     | Nth-Root of x. where n is a positive integer.           |
|          | (eg: root(x,3) == x^(1/3))                              |
+----------+---------------------------------------------------------+
| round    | Round x to the nearest integer.  (eg: round(x))         |
+----------+---------------------------------------------------------+
| roundn   | Round x to n decimal places  (eg: roundn(x,3))          |
|          | where n > 0 and is an integer.                          |
|          | (eg: roundn(1.2345678,4) == 1.2346)                     |
+----------+---------------------------------------------------------+
| sgn      | Sign of x, -1 where x < 0, +1 where x > 0, else zero.   |
|          | (eg: sgn(x))                                            |
+----------+---------------------------------------------------------+
| sqrt     | Square root of x, where x >= 0.  (eg: sqrt(x))          |
+----------+---------------------------------------------------------+
| sum      | Sum of all the inputs.                                  |
|          | (eg: sum(x,y,z,w,u,v,t) == (x + y + z + w + u + v + t)) |
+----------+---------------------------------------------------------+
| swap     | Swap the values of the variables x and y and return the |
| <=>      | current value of y.  (eg: swap(x,y) or x <=> y)         |
+----------+---------------------------------------------------------+
| trunc    | Integer portion of x.  (eg: trunc(x))                   |
+----------+---------------------------------------------------------+

(4) Trigonometry Functions
+----------+---------------------------------------------------------+
| FUNCTION | DEFINITION                                              |
+----------+---------------------------------------------------------+
| acos     | Arc cosine of x expressed in radians. Interval [-1,+1]  |
|          | (eg: acos(x))                                           |
+----------+---------------------------------------------------------+
| acosh    | Inverse hyperbolic cosine of x expressed in radians.    |
|          | (eg: acosh(x))                                          |
+----------+---------------------------------------------------------+
| asin     | Arc sine of x expressed in radians. Interval [-1,+1]    |
|          | (eg: asin(x))                                           |
+----------+---------------------------------------------------------+
| asinh    | Inverse hyperbolic sine of x expressed in radians.      |
|          | (eg: asinh(x))                                          |
+----------+---------------------------------------------------------+
| atan     | Arc tangent of x expressed in radians. Interval [-1,+1] |
|          | (eg: atan(x))                                           |
+----------+---------------------------------------------------------+
| atan2    | Arc tangent of (x / y) expressed in radians. [-pi,+pi]  |
|          | eg: atan2(x,y)                                          |
+----------+---------------------------------------------------------+
| atanh    | Inverse hyperbolic tangent of x expressed in radians.   |
|          | (eg: atanh(x))                                          |
+----------+---------------------------------------------------------+
| cos      | Cosine of x.  (eg: cos(x))                              |
+----------+---------------------------------------------------------+
| cosh     | Hyperbolic cosine of x.  (eg: cosh(x))                  |
+----------+---------------------------------------------------------+
| cot      | Cotangent of x.  (eg: cot(x))                           |
+----------+---------------------------------------------------------+
| csc      | Cosecant of x.  (eg: csc(x))                            |
+----------+---------------------------------------------------------+
| sec      | Secant of x.  (eg: sec(x))                              |
+----------+---------------------------------------------------------+
| sin      | Sine of x.  (eg: sin(x))                                |
+----------+---------------------------------------------------------+
| sinc     | Sine cardinal of x.  (eg: sinc(x))                      |
+----------+---------------------------------------------------------+
| sinh     | Hyperbolic sine of x.  (eg: sinh(x))                    |
+----------+---------------------------------------------------------+
| tan      | Tangent of x.  (eg: tan(x))                             |
+----------+---------------------------------------------------------+
| tanh     | Hyperbolic tangent of x.  (eg: tanh(x))                 |
+----------+---------------------------------------------------------+
| deg2rad  | Convert x from degrees to radians.  (eg: deg2rad(x))    |
+----------+---------------------------------------------------------+
| deg2grad | Convert x from degrees to gradians.  (eg: deg2grad(x))  |
+----------+---------------------------------------------------------+
| rad2deg  | Convert x from radians to degrees.  (eg: rad2deg(x))    |
+----------+---------------------------------------------------------+
| grad2deg | Convert x from gradians to degrees.  (eg: grad2deg(x))  |
+----------+---------------------------------------------------------+

(5) String Processing
+----------+---------------------------------------------------------+
| FUNCTION | DEFINITION                                              |
+----------+---------------------------------------------------------+
|  = , ==  | All common equality/inequality operators are applicable |
|  !=, <>  | to strings and are applied in a case sensitive manner.  |
|  <=, >=  | In the following example x, y and z are of type string. |
|  < , >   | (eg: not((x <= 'AbC') and ('1x2y3z' <> y)) or (z == x)  |
+----------+---------------------------------------------------------+
| in       | True only if x is a substring of y.                     |
|          | (eg: x in y or 'abc' in 'abcdefgh')                     |
+----------+---------------------------------------------------------+
| like     | True only if the string x matches the pattern y.        |
|          | Available wildcard characters are '*' and '?' denoting  |
|          | zero or more and zero or one matches respectively.      |
|          | (eg: x like y or 'abcdefgh' like 'a?d*h')               |
+----------+---------------------------------------------------------+
| ilike    | True only if the string x matches the pattern y in a    |
|          | case insensitive manner. Available wildcard characters  |
|          | are '*' and '?' denoting zero or more and zero or one   |
|          | matches respectively.                                   |
|          | (eg: x ilike y or 'a1B2c3D4e5F6g7H' ilike 'a?d*h')      |
+----------+---------------------------------------------------------+
| [r0:r1]  | The closed interval [r0,r1] of the specified string.    |
|          | eg: Given a string x with a value of 'abcdefgh' then:   |
|          | 1. x[1:4] == 'bcde'                                     |
|          | 2. x[ :5] == x[:5] == 'abcdef'                          |
|          | 3. x[3: ] == x[3:] =='cdefgh'                           |
|          | 4. x[ : ] == x[:] == 'abcdefgh'                         |
|          | 5. x[4/2:3+2] == x[2:5] == 'cdef'                       |
|          |                                                         |
|          | Note: Both r0 and r1 are assumed to be integers, where  |
|          | r0 <= r1. They may also be the result of an expression, |
|          | in the event they have fractional components truncation |
|          | will be performed. (eg: 1.67 --> 1)                     |
+----------+---------------------------------------------------------+
|  :=      | Assign the value of x to y. Where y is a mutable string |
|          | or string range and x is either a string or a string    |
|          | range. eg:                                              |
|          | 1. y := x                                               |
|          | 2. y := 'abc'                                           |
|          | 3. y := x[:i + j]                                       |
|          | 4. y := '0123456789'[2:7]                               |
|          | 5. y := '0123456789'[2i + 1:7]                          |
|          | 6. y := (x := '0123456789'[2:7])                        |
|          | 7. y[i:j] := x                                          |
|          | 8. y[i:j] := (x + 'abcdefg'[8 / 4:5])[m:n]              |
|          |                                                         |
|          | Note: For options 7 and 8 the shorter of the two ranges |
|          | will denote the number characters that are to be copied.|
+----------+---------------------------------------------------------+
|  +       | Concatenation of x and y. Where x and y are strings or  |
|          | string ranges. eg                                       |
|          | 1. x + y                                                |
|          | 2. x + 'abc'                                            |
|          | 3. x + y[:i + j]                                        |
|          | 4. x[i:j] + y[2:3] + '0123456789'[2:7]                  |
|          | 5. 'abc' + x + y                                        |
|          | 6. 'abc' + '1234567'                                    |
|          | 7. (x + 'a1B2c3D4' + y)[i:2j]                           |
+----------+---------------------------------------------------------+
|  +=      | Append to x the value of y. Where x is a mutable string |
|          | and y is either a string or a string range. eg:         |
|          | 1. x += y                                               |
|          | 2. x += 'abc'                                           |
|          | 3. x += y[:i + j] + 'abc'                               |
|          | 4. x += '0123456789'[2:7]                               |
+----------+---------------------------------------------------------+
| <=>      | Swap the values of x and y. Where x and y are mutable   |
|          | strings.  (eg: x <=> y)                                 |
+----------+---------------------------------------------------------+
| []       | The string size operator returns the size of the string |
|          | being actioned.                                         |
|          | eg:                                                     |
|          | 1. 'abc'[] == 3                                         |
|          | 2. var max_str_length := max(s0[],s1[],s2[],s3[])       |
|          | 3. ('abc' + 'xyz')[] == 6                               |
|          | 4. (('abc' + 'xyz')[1:4])[] == 4                        |
+----------+---------------------------------------------------------+

(6) Control Structures
+----------+---------------------------------------------------------+
|STRUCTURE | DEFINITION                                              |
+----------+---------------------------------------------------------+
| if       | If x is true then return y else return z.               |
|          | eg:                                                     |
|          | 1. if(x, y, z)                                          |
|          | 2. if((x + 1) > 2y, z + 1, w / v)                       |
|          | 3. if(x > y) z;                                         |
|          | 4. if(x <= 2*y) { z + w };                              |
+----------+---------------------------------------------------------+
| if-else  | The if-else/else-if statement. Subject to the condition |
|          | branch the statement will return either the value of the|
|          | consequent or the alternative branch.                   |
|          | eg:                                                     |
|          | 1. if (x > y) z; else w;                                |
|          | 2. if (x > y) z; else if (w != u) v;                    |
|          | 3. if (x < y) {z; w + 1;} else u;                       |
|          | 4. if ((x != y) and (z > w))                            |
|          |    {                                                    |
|          |      y := sin(x) / u;                                   |
|          |      z := w + 1;                                        |
|          |    }                                                    |
|          |    else if (x > (z + 1))                                |
|          |    {                                                    |
|          |      w := abs (x - y) + z;                              |
|          |      u := (x + 1) > 2y ? 2u : 3u;                       |
|          |    }                                                    |
+----------+---------------------------------------------------------+
| switch   | The first true case condition that is encountered will  |
|          | determine the result of the switch. If none of the case |
|          | conditions hold true, the default action is assumed as  |
|          | the final return value. This is sometimes also known as |
|          | a multi-way branch mechanism.                           |
|          | eg:                                                     |
|          | switch                                                  |
|          | {                                                       |
|          |   case x > (y + z) : 2 * x / abs(y - z);                |
|          |   case x < 3       : sin(x + y);                        |
|          |   default          : 1 + x;                             |
|          | }                                                       |
+----------+---------------------------------------------------------+
| while    | The structure will repeatedly evaluate the internal     |
|          | statement(s) 'while' the condition is true. The final   |
|          | statement in the final iteration will be used as the    |
|          | return value of the loop.                               |
|          | eg:                                                     |
|          | while ((x -= 1) > 0)                                    |
|          | {                                                       |
|          |   y := x + z;                                           |
|          |   w := u + y;                                           |
|          | }                                                       |
+----------+---------------------------------------------------------+
| repeat/  | The structure will repeatedly evaluate the internal     |
| until    | statement(s) 'until' the condition is true. The final   |
|          | statement in the final iteration will be used as the    |
|          | return value of the loop.                               |
|          | eg:                                                     |
|          | repeat                                                  |
|          |   y := x + z;                                           |
|          |   w := u + y;                                           |
|          | until ((x += 1) > 100)                                  |
+----------+---------------------------------------------------------+
| for      | The structure will repeatedly evaluate the internal     |
|          | statement(s) while the condition is true. On each loop  |
|          | iteration, an 'incrementing' expression is evaluated.   |
|          | The conditional is mandatory whereas the initialiser    |
|          | and incrementing expressions are optional.              |
|          | eg:                                                     |
|          | for (var x := 0; (x < n) and (x != y); x += 1)          |
|          | {                                                       |
|          |   y := y + x / 2 - z;                                   |
|          |   w := u + y;                                           |
|          | }                                                       |
+----------+---------------------------------------------------------+
| break    | Break terminates the execution of the nearest enclosed  |
| break[]  | loop, allowing for the execution to continue on external|
|          | to the loop. The default break statement will set the   |
|          | return value of the loop to NaN, where as the return    |
|          | based form will set the value to that of the break      |
|          | expression.                                             |
|          | eg:                                                     |
|          | while ((i += 1) < 10)                                   |
|          | {                                                       |
|          |   if (i < 5)                                            |
|          |     j -= i + 2;                                         |
|          |   else if (i % 2 == 0)                                  |
|          |     break;                                              |
|          |   else                                                  |
|          |     break[2i + 3];                                      |
|          | }                                                       |
+----------+---------------------------------------------------------+
| continue | Continue results in the remaining portion of the nearest|
|          | enclosing loop body to be skipped.                      |
|          | eg:                                                     |
|          | for (var i := 0; i < 10; i += 1)                        |
|          | {                                                       |
|          |   if (i < 5)                                            |
|          |     continue;                                           |
|          |   j -= i + 2;                                           |
|          | }                                                       |
+----------+---------------------------------------------------------+
| return   | Return immediately from within the current expression.  |
|          | With the option of passing back a variable number of    |
|          | values (scalar, vector or string). eg:                  |
|          | 1. return [1];                                          |
|          | 2. return [x, 'abx'];                                   |
|          | 3. return [x, x + y,'abx'];                             |
|          | 4. return [];                                           |
|          | 5. if (x < y)                                           |
|          |     return [x, x - y, 'result-set1', 123.456];          |
|          |    else                                                 |
|          |     return [y, x + y, 'result-set2'];                   |
+----------+---------------------------------------------------------+
| ?:       | Ternary conditional statement, similar to that of the   |
|          | above denoted if-statement.                             |
|          | eg:                                                     |
|          | 1. x ? y : z                                            |
|          | 2. x + 1 > 2y ? z + 1 : (w / v)                         |
|          | 3. min(x,y) > z ? (x < y + 1) ? x : y : (w * v)         |
+----------+---------------------------------------------------------+
| ~        | Evaluate each sub-expression, then return as the result |
|          | the value of the last sub-expression. This is sometimes |
|          | known as multiple sequence point evaluation.            |
|          | eg:                                                     |
|          | ~(i := x + 1, j := y / z, k := sin(w/u)) == (sin(w/u))) |
|          | ~{i := x + 1; j := y / z; k := sin(w/u)} == (sin(w/u))) |
+----------+---------------------------------------------------------+
| [*]      | Evaluate any consequent for which its case statement is |
|          | true. The return value will be either zero or the result|
|          | of the last consequent to have been evaluated.          |
|          | eg:                                                     |
|          | [*]                                                     |
|          | {                                                       |
|          |   case (x + 1) > (y - 2)    : x := z / 2 + sin(y / pi); |
|          |   case (x + 2) < abs(y + 3) : w / 4 + min(5y,9);        |
|          |   case (x + 3) == (y * 4)   : y := abs(z / 6) + 7y;     |
|          | }                                                       |
+----------+---------------------------------------------------------+
| []       | The vector size operator returns the size of the vector |
|          | being actioned.                                         |
|          | eg:                                                     |
|          | 1. v[]                                                  |
|          | 2. max_size := max(v0[],v1[],v2[],v3[])                 |
+----------+---------------------------------------------------------+

Note: In  the  tables  above, the  symbols x, y, z, w, u  and v  where
appropriate may represent any of one the following:

   1. Literal numeric/string value
   2. A variable
   3. A vector element
   4. A vector
   5. A string
   6. An expression comprised of [1], [2] or [3] (eg: 2 + x / vec[3])

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[09 - Fundamental Types]
ExprTk supports three fundamental types which can be used freely in
expressions. The types are as follows:

   (1) Scalar
   (2) Vector
   (3) String


(1) Scalar Type
The scalar type  is a singular  numeric value. The  underlying type is
that used  to specialize  the ExprTk  components (float,  double, long
double, MPFR et al).


(2) Vector Type
The vector type is  a fixed size sequence  of scalar values. A  vector
can  be indexed  resulting  in  a scalar  value. Operations  between a
vector and scalar will result in a vector with a size equal to that of
the original vector, whereas operations between vectors will result in
a vector of size equal to that of the smaller of the two.


(3) String Type
The string type is a variable length sequence of 8-bit chars.  Strings
can be  assigned and  concatenated to  one another,  they can  also be
manipulated via sub-ranges using the range definition syntax.  Strings
however can not interact with scalar or vector types.

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[10 - COMPONENTS]
There are three primary components, that are specialized upon a  given
numeric type, which make up the core of ExprTk. The components are  as
follows:

   (1) Symbol Table  exprtk::symbol_table<NumericType>
   (2) Expression    exprtk::expression<NumericType>
   (3) Parser        exprtk::parser<NumericType>


(1) Symbol Table
A structure that is used  to store references to variables,  constants
and functions that are to  be used within expressions. Furthermore  in
the context  of composited  recursive functions  the symbol  table can
also be thought of as a simple representation of a stack specific  for
the expression(s) that reference it. The following is a list of the
types a symbol table can handle:

   (a) Numeric variables
   (b) Numeric constants
   (c) Numeric vector elements
   (d) String variables
   (e) String constants
   (f) Functions
   (g) Vararg functions

During the compilation  process if an  expression is found  to require
any  of  the  elements   noted  above,  the  expression's   associated
symbol_table  will  be  queried  for  the  element  and  if  present a
reference to the element will be embedded within the expression's AST.
This allows for the original  element to be modified independently  of
the  expression  instance  and  to also  allow  the  expression  to be
evaluated using the current value of the element.

The  example  below demonstrates  the  relationship between variables,
symbol_table and expression. Note  the variables are modified  as they
normally would in a program, and when the expression is  evaluated the
current values assigned to the variables will be used.

   typedef exprtk::symbol_table<double> symbol_table_t;
   typedef exprtk::expression<double>     expression_t;
   typedef exprtk::parser<double>             parser_t;

   symbol_table_t symbol_table;
   expression_t   expression;
   parser_t       parser;

   double x = 0;
   double y = 0;

   std::string expression_string = "x * y + 3";
   symbol_table.add_variable("x",x);
   symbol_table.add_variable("y",y);

   expression.register_symbol_table(symbol_table);

   parser.compile(expression_string,expression);

   x = 1.0;
   y = 2.0;
   expression.value(); // 1 * 2 + 3

   x = 3.7;
   expression.value(); // 3.7 * 2 + 3

   y = -9.0;
   expression.value(); // 3.7 * -9 + 3

   // 'x * -9 + 3' for x in range of [0,100) in steps of 0.0001
   for (x = 0.0; x < 100.0; x += 0.0001)
   {
      expression.value(); // x * -9 + 3
   }


Note: It is possible to register multiple symbol_tables with a  single
expression  object. In  the event  an expression  has multiple  symbol
tables,  and  where  there  exists  conflicts  between  symbols,   the
compilation stage  will resolve  the conflicts  based on  the order of
registration of the symbol_tables to the expression.

   typedef exprtk::symbol_table<double> symbol_table_t;
   typedef exprtk::expression<double>     expression_t;
   typedef exprtk::parser<double>             parser_t;

   symbol_table_t symbol_table0;
   symbol_table_t symbol_table1;

   expression_t   expression;
   parser_t       parser;

   double x0 = 123.0;
   double x1 = 678.0;

   std::string expression_string = "x + 1";

   symbol_table0.add_variable("x",x0);
   symbol_table1.add_variable("x",x1);

   expression.register_symbol_table(symbol_table0);
   expression.register_symbol_table(symbol_table1);

   parser.compile(expression_string,expression);

   expression.value(); // 123 + 1


(2) Expression
A structure that holds an abstract syntax tree or AST for a  specified
expression and is used to evaluate said expression. Evaluation of  the
expression is accomplished by performing a post-order traversal of the
AST.  If  a  compiled  Expression  uses  variables  or  user   defined
functions, it will have an associated Symbol Table, which will contain
references  to said  variables, functions  or strings. An example  AST
structure for the denoted expression is as follows:

Expression:  z := (x + y^-2.345) * sin(pi / min(w - 7.3,v))

                  [Root]
                    |
               [Assignment]
        ________/        \_____
       /                       \
 Variable(z)            [Multiplication]
                ____________/      \___________
               /                               \
              /                          [Unary-Func(sin)]
         [Addition]                            |
      ____/      \____                    [Division]
     /                \                 ___/      \___
 Variable(x)   [Exponentiation]        /              \
              ______/   \______  Constant(pi)  [Binary-Func(min)]
             /                 \                ____/    \____
        Variable(y)        [Negation]          /              \
                               |              /           Variable(v)
                        Constant(2.345)      /
                                            /
                                     [Subtraction]
                                   ____/      \____
                                  /                \
                             Variable(w)      Constant(7.3)


The above denoted AST will be evaluated in the following order:

   (01) Load Variable  (z)        (10) Load Constant  (7.3)
   (02) Load Variable  (x)        (11) Subtraction    (09 & 10)
   (03) Load Variable  (y)        (12) Load Variable  (v)
   (04) Load Constant  (2.345)    (13) Min            (11 & 12)
   (05) Negation       (04)       (14) Division       (08 & 13)
   (06) Exponentiation (03 & 05)  (15) Sin            (14)
   (07) Addition       (02 & 06)  (16) Multiplication (07 & 15)
   (08) Load Constant  (pi)       (17) Assignment     (01 & 16)
   (09) Load Variable  (w)


(3) Parser
A  component  which  takes  as input  a  string  representation  of an
expression and attempts to compile said input with the result being an
instance  of  Expression.  If  an  error  is  encountered  during  the
compilation  process, the  parser will  stop compiling  and return  an
error status code,  with a more  detailed description of  the error(s)
and  its  location  within  the  input  provided  by  the  'get_error'
interface.

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[11 - COMPILATION OPTIONS]
The exprtk::parser  when being  instantiated takes  as input  a set of
options  to be  used during  the compilation  process of  expressions.
An  example instantiation  of exprtk::parser  where only  the  joiner,
commutative and strength reduction options are enabled is as  follows:

   typedef exprtk::parser<NumericType>::settings_t settings_t;

   std::size_t compile_options = settings_t::e_joiner            +
                                 settings_t::e_commutative_check +
                                 settings_t::e_strength_reduction;

   parser_t parser(compile_options);


Currently  seven  types of  compile  time options  are  supported, and
enabled by default. The options and their explanations are as follows:

   (1) Replacer
   (2) Joiner
   (3) Numeric Check
   (4) Bracket Check
   (5) Sequence Check
   (6) Commutative Check
   (7) Strength Reduction Check


(1) Replacer (e_replacer)
Enable replacement of specific  tokens with other tokens.  For example
the token  "true" of  type symbol  will be  replaced with  the numeric
token of value one.

   (a) (x < y) == true   --->  (x < y) == 1
   (b) false == (x > y)  --->  0 == (x > y)


(2) Joiner (e_joiner)
Enable  joining  of  multi-character  operators  that  may  have  been
incorrectly  disjoint in the  string  representation  of the specified
expression. For example the consecutive tokens of ">" "=" will  become
">=" representing  the "greater  than or  equal to"  operator. If  not
properly resolved the  original form will  cause a compilation  error.
The  following  is  a listing  of the  scenarios that  the joiner  can
handle:

   (a) '>' '='  --->  '>='  (gte)
   (b) '<' '='  --->  '<='  (lte)
   (c) '=' '='  --->  '=='  (equal)
   (d) '!' '='  --->  '!='  (not-equal)
   (e) '<' '>'  --->  '<>'  (not-equal)
   (f) ':' '='  --->  ':='  (assignment)
   (g) '+' '='  --->  '+='  (addition assignment)
   (h) '-' '='  --->  '-='  (subtraction assignment)
   (i) '*' '='  --->  '*='  (multiplication assignment)
   (j) '/' '='  --->  '/='  (division assignment)
   (k) '%' '='  --->  '%='  (modulo assignment)
   (l) '+' '-'  --->  '-'   (subtraction)
   (m) '-' '+'  --->  '-'   (subtraction)
   (n) '-' '-'  --->  '+'   (addition)
   (o) '<=' '>' --->  '<=>' (swap)


An example of the transformation that takes place is as follows:

   (a) (x > = y) and (z ! = w)  --->  (x >= y) and (z != w)


(3) Numeric Check (e_numeric_check)
Enable validation of tokens representing numeric types so as to  catch
any errors prior  to the costly  process of the  main compilation step
commencing.


(4) Bracket Check (e_bracket_check)
Enable  the  check for  validating  the ordering  of  brackets in  the
specified expression.


(5) Sequence Check (e_sequence_check)
Enable the  check for  validating that  sequences of  either pairs  or
triplets of tokens make sense.  For example the following sequence  of
tokens when encountered will raise an error:

   (a) (x + * 3)  --->  sequence error


(6) Commutative Check (e_commutative_check)
Enable the check that will transform sequences of pairs of tokens that
imply a multiplication operation.  The following are some  examples of
such transformations:

   (a) 2x             --->  2 * x
   (b) 25x^3          --->  25 * x^3
   (c) 3(x + 1)       --->  3 * (x + 1)
   (d) (x + 1)4       --->  (x + 1) * 4
   (e) 5foo(x,y)      --->  5 * foo(x,y)
   (f) foo(x,y)6 + 1  --->  foo(x,y) * 6 + 1
   (g) (4((2x)3))     --->  4 * ((2 * x) * 3)
   (h) w(x) + (y)z    --->  w * x + y * z


(7) Strength Reduction Check (e_strength_reduction)
Enable  the  use  of  strength  reduction  optimisations  during   the
compilation  process.  In  ExprTk  strength  reduction   optimisations
predominantly involve  transforming sub-expressions  into other  forms
that  are algebraically  equivalent yet  less costly  to compute.  The
following are examples of the various transformations that can occur:

   (a) (x / y) / z        --->  x / (y * z)
   (b) (x / y) / (z / w)  --->  (x * w) / (y * z)
   (c) (2 * x) - (2 * y)  --->  2 * (x - y)
   (d) (2 / x) / (3 / y)  --->  (2 / 3) / (x * y)
   (e) (2 * x) * (3 * y)  --->  (2 * 3) * (x * y)


Note:
When using  strength reduction  in conjunction  with expressions whose
inputs or sub-expressions may result  in values nearing either of  the
bounds of the underlying numeric  type (eg: double), there may  be the
possibility of a decrease in the precision of results.

In  the following  example the  given expression  which represents  an
attempt at computing the average  between x and y will  be transformed
as follows:

   (0.5 * x) + (y * 0.5) ---> 0.5 * (x + y)

There  may be  situations where  the above  transformation will  cause
numerical overflows and  that the original  form of the  expression is
desired over the strength reduced form. In these situations it is best
to turn off strength reduction optimisations  or to use a type with  a
larger numerical bound.

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[12 - SPECIAL FUNCTIONS]
The purpose  of special  functions in  ExprTk is  to provide  compiler
generated equivalents of common mathematical expressions which can  be
invoked by  using the  'special function'  syntax (eg:  $f12(x,y,z) or
$f82(x,y,z,w)).

Special functions dramatically decrease  the total evaluation time  of
expressions which would otherwise  have been written using  the common
form by reducing the total number  of nodes in the evaluation tree  of
an  expression  and  by  also  leveraging  the  compiler's  ability to
correctly optimize such expressions for a given architecture.

          3-Parameter                       4-Parameter
 +-------------+-------------+    +--------------+------------------+
 |  Prototype  |  Operation  |    |  Prototype   |    Operation     |
 +-------------+-------------+    +--------------+------------------+
   $f00(x,y,z) | (x + y) / z       $f48(x,y,z,w) | x + ((y + z) / w)
   $f01(x,y,z) | (x + y) * z       $f49(x,y,z,w) | x + ((y + z) * w)
   $f02(x,y,z) | (x + y) - z       $f50(x,y,z,w) | x + ((y - z) / w)
   $f03(x,y,z) | (x + y) + z       $f51(x,y,z,w) | x + ((y - z) * w)
   $f04(x,y,z) | (x - y) + z       $f52(x,y,z,w) | x + ((y * z) / w)
   $f05(x,y,z) | (x - y) / z       $f53(x,y,z,w) | x + ((y * z) * w)
   $f06(x,y,z) | (x - y) * z       $f54(x,y,z,w) | x + ((y / z) + w)
   $f07(x,y,z) | (x * y) + z       $f55(x,y,z,w) | x + ((y / z) / w)
   $f08(x,y,z) | (x * y) - z       $f56(x,y,z,w) | x + ((y / z) * w)
   $f09(x,y,z) | (x * y) / z       $f57(x,y,z,w) | x - ((y + z) / w)
   $f10(x,y,z) | (x * y) * z       $f58(x,y,z,w) | x - ((y + z) * w)
   $f11(x,y,z) | (x / y) + z       $f59(x,y,z,w) | x - ((y - z) / w)
   $f12(x,y,z) | (x / y) - z       $f60(x,y,z,w) | x - ((y - z) * w)
   $f13(x,y,z) | (x / y) / z       $f61(x,y,z,w) | x - ((y * z) / w)
   $f14(x,y,z) | (x / y) * z       $f62(x,y,z,w) | x - ((y * z) * w)
   $f15(x,y,z) | x / (y + z)       $f63(x,y,z,w) | x - ((y / z) / w)
   $f16(x,y,z) | x / (y - z)       $f64(x,y,z,w) | x - ((y / z) * w)
   $f17(x,y,z) | x / (y * z)       $f65(x,y,z,w) | ((x + y) * z) - w
   $f18(x,y,z) | x / (y / z)       $f66(x,y,z,w) | ((x - y) * z) - w
   $f19(x,y,z) | x * (y + z)       $f67(x,y,z,w) | ((x * y) * z) - w
   $f20(x,y,z) | x * (y - z)       $f68(x,y,z,w) | ((x / y) * z) - w
   $f21(x,y,z) | x * (y * z)       $f69(x,y,z,w) | ((x + y) / z) - w
   $f22(x,y,z) | x * (y / z)       $f70(x,y,z,w) | ((x - y) / z) - w
   $f23(x,y,z) | x - (y + z)       $f71(x,y,z,w) | ((x * y) / z) - w
   $f24(x,y,z) | x - (y - z)       $f72(x,y,z,w) | ((x / y) / z) - w
   $f25(x,y,z) | x - (y / z)       $f73(x,y,z,w) | (x * y) + (z * w)
   $f26(x,y,z) | x - (y * z)       $f74(x,y,z,w) | (x * y) - (z * w)
   $f27(x,y,z) | x + (y * z)       $f75(x,y,z,w) | (x * y) + (z / w)
   $f28(x,y,z) | x + (y / z)       $f76(x,y,z,w) | (x * y) - (z / w)
   $f29(x,y,z) | x + (y + z)       $f77(x,y,z,w) | (x / y) + (z / w)
   $f30(x,y,z) | x + (y - z)       $f78(x,y,z,w) | (x / y) - (z / w)
   $f31(x,y,z) | x * y^2 + z       $f79(x,y,z,w) | (x / y) - (z * w)
   $f32(x,y,z) | x * y^3 + z       $f80(x,y,z,w) | x / (y + (z * w))
   $f33(x,y,z) | x * y^4 + z       $f81(x,y,z,w) | x / (y - (z * w))
   $f34(x,y,z) | x * y^5 + z       $f82(x,y,z,w) | x * (y + (z * w))
   $f35(x,y,z) | x * y^6 + z       $f83(x,y,z,w) | x * (y - (z * w))
   $f36(x,y,z) | x * y^7 + z       $f84(x,y,z,w) | x*y^2 + z*w^2
   $f37(x,y,z) | x * y^8 + z       $f85(x,y,z,w) | x*y^3 + z*w^3
   $f38(x,y,z) | x * y^9 + z       $f86(x,y,z,w) | x*y^4 + z*w^4
   $f39(x,y,z) | x * log(y)+z      $f87(x,y,z,w) | x*y^5 + z*w^5
   $f40(x,y,z) | x * log(y)-z      $f88(x,y,z,w) | x*y^6 + z*w^6
   $f41(x,y,z) | x * log10(y)+z    $f89(x,y,z,w) | x*y^7 + z*w^7
   $f42(x,y,z) | x * log10(y)-z    $f90(x,y,z,w) | x*y^8 + z*w^8
   $f43(x,y,z) | x * sin(y)+z      $f91(x,y,z,w) | x*y^9 + z*w^9
   $f44(x,y,z) | x * sin(y)-z      $f92(x,y,z,w) | (x and y) ? z : w
   $f45(x,y,z) | x * cos(y)+z      $f93(x,y,z,w) | (x or  y) ? z : w
   $f46(x,y,z) | x * cos(y)-z      $f94(x,y,z,w) | (x <   y) ? z : w
   $f47(x,y,z) | x ? y : z         $f95(x,y,z,w) | (x <=  y) ? z : w
                                   $f96(x,y,z,w) | (x >   y) ? z : w
                                   $f97(x,y,z,w) | (x >=  y) ? z : w
                                   $f98(x,y,z,w) | (x ==  y) ? z : w
                                   $f99(x,y,z,w) | x*sin(y)+z*cos(w)

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[13 - VARIABLE , VECTOR & STRING DEFINITION]
ExprTk supports the definition of expression local variables,  vectors
and  strings.  The definitions  must  be unique  as  shadowing is  not
allowed and object life-times are based on scope. Definitions use  the
following general form:

   var <name> := <initialiser>;

(1) Variable Definition
Variables are  of numeric  type denoting  a single  value. They can be
explicitly initialised to a value, otherwise they will be defaulted to
zero. The following are examples of variable definitions:

   (a) Initialise x to zero
       var x;

   (b) Initialise y to three
       var y := 3;

   (c) Initialise z to the expression
       var z := if(max(1,x + y) > 2,w,v);


(2) Vector Definition
Vectors are arrays of a common numeric type. The elements in a  vector
can be explicitly initialised, otherwise they will all be defaulted to
zero. The following are examples of vector definitions:

   (a) Initialise all values to zero
       var x[3];

   (b) Initialise all values to zero
       var x[3] := {};

   (c) Initialise all values to given expression
       var x[3] := [123 + 3y + sin(w/z)];

   (d) Initialise the first two values, all other elements to zero
       var x[3] := { 1 + x[2], sin(y[0] / x[]) + 3 };

   (e) Initialise the first three (all) values
       var x[3] := { 1, 2, 3 };

   (f) Error as there are too many initialisers
       var x[3] := { 1, 2, 3, 4 };

   (g) Error as a vector of size zero is not allowed.
       var x[0];


(3) String Definition
Strings are a sequence of  8-bit characters. They can only  be defined
with an explicit initialisation  value. The following are  examples of
string variable definitions:

   (a) Initialise to a string
       var x := 'abc';

   (b) Initialise to a string expression
       var x := 'abc' + '123';

   (c) Initialise to a string range
       var x := 'abc123'[2:4];

   (d) Initialise to another string variable
       var x := 'abc';
       var y := x;

   (e) Initialise to another string variable range
       var x := 'abc123';
       var y := x[2:4];

   (f) Initialise to a string expression
       var x := 'abc';
       var y := x + '123';

   (g) Initialise to a string expression range
       var x := 'abc';
       var y := (x + '123')[1:3];


(4) Return Value
Variable and vector  definitions have a  return value. In  the case of
variable definitions, the value  to which the variable  is initialised
will be returned. Where as for vectors, the value of the first element
(eg: v[0]) will be returned.


(5) Variable/Vector Assignment
The value of a variable can be assigned to a vector and a vector or  a
vector expression can be assigned to a variable.

  (a) Variable To Vector:
      Every element of the vector is assigned the value of the variable
      or expression.
      var x    := 3;
      var y[3] := { 1, 2, 3 };
      y := x + 1;

  (b) Vector To Variable:
      The variable is assigned the value of the first element of the
      vector (aka vec[0])
      var x    := 3;
      var y[3] := { 1, 2, 3 };
      x := y + 1;


Note: During the expression  compilation phase, tokens are  classified
based on the following priorities:

   (a) Reserved keywords or operators (+, -, and, or, etc)
   (b) Base functions (abs, sin, cos, min, max etc)
   (c) Symbol table variables
   (d) Expression local defined variables
   (e) Symbol table functions
   (f) Unknown symbol resolver based variables

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[14 - VECTOR PROCESSING]
ExprTk  provides  support  for   various  forms  of  vector   oriented
arithmetic, inequalities and  processing. The various  supported pairs
are as follows:

   (a) vector and vector (eg: v0 + v1)
   (b) vector and scalar (eg: v  + 33)
   (c) scalar and vector (eg: 22 *  v)

The following is a list of operations that can be used in  conjunction
with vectors:

   (a) Arithmetic:       +, -, *, /, %
   (b) Exponentiation:   vector ^ scalar
   (c) Assignment:       :=, +=, -=, *=, /=, %=, <=>
   (d) Inequalities:     <, <=, >, >=, ==, =
   (e) Unary operations:
       abs, acos, acosh, asin, asinh, atan, atanh, ceil, cos,  cosh,
       cot, csc,  deg2grad, deg2rad,  erf, erfc,  exp, expm1, floor,
       frac, grad2deg, log, log10, log1p, log2, rad2deg, round, sec,
       sgn, sin, sinc, sinh, sqrt, swap, tan, tanh, trunc
   (f) Aggregate and Reduce operations:
       avg, max, min, mul, sum

Note: When one of  the above  described operations  is being performed
between two  vectors, the  operation will  only span  the size  of the
smallest vector.  The elements  of the  larger vector  outside of  the
range will not be included.

The  following  simple  example  demonstrates  the  vector  processing
capabilities by computing the dot-product of the vectors v0 and v1 and
then assigning it to the variable v0dotv1:

   var v0[3] := { 1, 2, 3 };
   var v1[3] := { 4, 5, 6 };
   var v0dotv1 := sum(v0 * v1);


The following is a for-loop based implementation that is equivalent to
the previously mentioned dot-product computation expression:

   var v0[3] := { 1, 2, 3 };
   var v1[3] := { 4, 5, 6 };
   var v0dotv1;

   for (var i := 0; i < min(v0[],v1[]); i += 1)
   {
     v0dotv1 += (v0[i] * v1[i]);
   }


Note: In the scenario of inequalities between two vectors, the  result
is not  a vector  but rather  a singular  variable denoting  a boolean
state  of either  'true' or  'false' depending  on the  nature of  the
inequality.

   var x[3] := { 1, 1, 1 };
   var y[3] := { 3, 2, 1 };

   y > x == false


Note:  When  the  aggregate  operations  denoted  above  are  used  in
conjunction with a  vector or vector  expression, the return  value is
not a vector but rather a single value.

   var x[3] := { 1, 2, 3 };

   sum(x)      ==  6
   sum(1 + 2x) == 15
   avg(3x + 1) ==  7
   min(1 / x)  == (1 / 3)
   max(x / 2)  == (3 / 2)

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[15 - USER DEFINED FUNCTIONS]
ExprTk provides a means  whereby custom functions can  be defined  and
utilized within  expressions.  The   concept  requires  the  user   to
provide a reference  to the function  coupled with an  associated name
that  will be invoked within  expressions. Functions may take numerous
inputs but will always return a single value of the underlying numeric
type.

During  expression  compilation  when required  the  reference  to the
function  will be  obtained from  the associated  symbol_table and  be
embedded into the expression.

There are five types of function interface:

   +---+----------------------+-------------+
   | # |         Name         | Return Type |
   +---+----------------------+-------------+
   | 1 | ifunction            | Scalar      |
   | 2 | ivararg_function     | Scalar      |
   | 3 | igeneric_function    | Scalar      |
   | 4 | igeneric_function II | String      |
   | 5 | function_compositor  | Scalar      |
   +---+----------------------+-------------+


(1) ifunction
This  interface  supports  zero  to  20  input  parameters.  The usage
requires a custom function be  derived from ifunction and to  override
one  of the  21 function  operators. As  part of  the constructor  the
custom function will define how many parameters it expects to  handle.
The following example defines a 3 parameter function called 'foo':

   template <typename T>
   struct foo : public exprtk::ifunction<T>
   {
      foo() : exprtk::ifunction<T>(3)
      {}

      T operator()(const T& v1, const T& v2, const T& v3)
      {
         return T(1) + (v1 * v2) / T(v3);
      }
   };


(2) ivararg_function
This interface supports a variable  number of arguments as input  into
the  function.  The  function operator  interface  uses  a std::vector
specialized upon type T to facilitate parameter passing. The following
example defines a vararg function called 'boo':

   template <typename T>
   struct boo : public exprtk::ivararg_function<T>
   {
      inline T operator()(const std::vector<T>& arglist)
      {
         T result = T(0);

         for (std::size_t i = 0; i < arglist.size(); ++i)
         {
            result += arglist[i] / arglist[i > 0 ? (i - 1) : 0];
         }

         return result;
      }
   };


(3) igeneric_function
This interface supports  a variable number  of arguments and  types as
input  into  the  function. The  function  operator  interface uses  a
std::vector  specialized  upon  the  type_store  type  to   facilitate
parameter passing.

    Scalar <-- function(i_0, i_1, i_2....., i_N)


The  fundamental  types  that  can  be  passed  into  the  function as
parameters and their views are as follows:

   (1) Scalar - scalar_view
   (2) Vector - vector_view
   (3) String - string_view


The above denoted type  views provide non-const reference-like  access
to each parameter, as such modifications made to the input  parameters
will  persist after  the function  call has  completed. The  following
example defines a generic function called 'too':

   template <typename T>
   struct too : public exprtk::igeneric_function<T>
   {
      typedef typename exprtk::igeneric_function<T>::parameter_list_t
                                                     parameter_list_t;

      too()
      {}

      inline T operator()(parameter_list_t parameters)
      {
         for (std::size_t i = 0; i < parameters.size(); ++i)
         {
            ...
         }

         return T(0);
      }
   };


In the example above, the input 'parameters' to the function operator,
parameter_list_t,  is  a  type  of  std::vector  of  type_store.  Each
type_store  instance  has  a  member  called  'type'  which  holds the
enumeration pertaining to the underlying type of the type_store. There
are three type enumerations:

   (1) e_scalar - literals, variables, vector elements, expressions
       eg: 123.456, x, vec[3x + 1], 2x + 3

   (2) e_vector - vectors, vector expressions
       eg: vec1, 2 * vec1 + vec2 / 3

   (3) e_string - strings, string literals and range variants of both
       eg: 'AString', s0, 'AString'[x:y], s1[1 + x:] + 'AString'


Each of the  parameters can be  accessed using its  designated view. A
typical loop for processing the parameters is as follows:

   inline T operator()(parameter_list_t parameters)
   {
      typedef typename exprtk::igeneric_function<T>::generic_type
                                                     generic_type;

      typedef typename generic_type::scalar_view scalar_t;
      typedef typename generic_type::vector_view vector_t;
      typedef typename generic_type::string_view string_t;

      for (std::size_t i = 0; i < parameters.size(); ++i)
      {
         generic_type& gt = parameters[i];

         if (generic_type::e_scalar == gt.type)
         {
            scalar_t x(gt);
            ...
         }
         else if (generic_type::e_vector == gt.type)
         {
            vector_t vector(gt);
            ...
         }
         else if (generic_type::e_string == gt.type)
         {
            string_t string(gt);
            ...
         }
      }

      return T(0);
   }


Most often than not a custom generic function will require a  specific
sequence of parameters, rather than some arbitrary sequence of  types.
In those situations, ExprTk can perform compile-time type checking  to
validate that function invocations  are carried out using  the correct
sequence of parameters. Furthermore  performing the checks at  compile
-time rather than at run-time (aka every time the function is invoked)
will result in expression evaluation performance gains.

Compile-time type  checking of  input parameters  can be  requested by
passing  a string  to the  constructor of  the igeneric_function  that
represents the required sequence of parameter types. When no parameter
sequence is provided, it is implied the function can accept a variable
number of parameters comprised of any of the fundamental types.

Each fundamental type has an  associated character. The following is a
listing of said characters and their meanings:

   (1) T - Scalar
   (2) V - Vector
   (3) S - String
   (4) ? - Any type (Scalar, Vector or String)
   (5) * - Wildcard operator
   (6) | - Parameter sequence delimiter


No other characters other than  the six denoted above may  be included
in the parameter sequence  definition. If any such  invalid characters
do exist, registration of the associated generic function to a  symbol
table ('add_function' method) will fail. If the parameter sequence  is
modified resulting in it becoming  invalid after having been added  to
the symbol table but before the compilation step, a compilation  error
will be incurred.

The  following   example  demonstrates   a  simple   generic  function
implementation with a user specified parameter sequence:

   template <typename T>
   struct moo : public exprtk::igeneric_function<T>
   {
      typedef typename exprtk::igeneric_function<T>::parameter_list_t
                                                     parameter_list_t;

      moo()
      : exprtk::igeneric_function<T>("SVTT")
      {}

      inline T operator()(parameter_list_t parameters)
      {
         ...
      }
   };


In the example above the  generic function 'moo' expects exactly  four
parameters in the following sequence:

   (1) String
   (2) Vector
   (3) Scalar
   (4) Scalar


(4) igeneric_function II
This interface is identical to  the igeneric_function, in that in  can
consume an  arbitrary number  of parameters  of varying  type, but the
difference being  that the  function returns  a string  and as such is
treated as a string when  invoked within expressions. As a  result the
function call can  alias a string  and interact with  other strings in
situations such as concatenation and equality operations.

    String <-- function(i_0, i_1, i_2....., i_N)


The following example defines a generic function  named 'toupper' with
the string return type function operator being explicitly overridden:

   template <typename T>
   struct toupper : public exprtk::igeneric_function<T>
   {
      typedef exprtk::igeneric_function<T> igenfunct_t
      typedef typename igenfunct_t::generic_type generic_t;
      typedef typename igenfunct_t::parameter_list_t parameter_list_t;
      typedef typename generic_t::string_view string_t;

      toupper()
      : exprtk::igeneric_function<T>("S",igenfunct_t::e_rtrn_string)
      {}

      inline T operator()(std::string& result,
                          parameter_list_t parameters)
      {
         result.clear();

         string_t string(params[0]);

         for (std::size_t i = 0; i < string.size(); ++i)
         {
            result += std::toupper(string[i]);
         }

         return T(0);
      }
   };


In the example above the  generic function 'toupper' expects only  one
input parameter  of type  string, as  noted by  the parameter sequence
string passed during the  constructor. Furthermore a second  parameter
is passed to the constructor indicating that it should be treated as a
string returning function -  by default it is  assumed to be a  scalar
returning function.

When executed,  the function  will return  as a  result a  copy of the
input string converted to uppercase form. An example expression  using
the toupper function registered as the symbol 'toupper' is as follows:

   "'ABCDEF' == toupper('aBc') + toupper('DeF')"


Note: When adding a string type returning generic function to a symbol
table the  'add_function' is invoked.  The example  below demonstrates
how this can be done:

   toupper<T> tu;

   exprtk::symbol_table<T> symbol_table;

   symbol_table.add_function("toupper",tu);


Note: Two further  refinements to the  type checking facility  are the
possibilities  of  a variable  number  of common  types  which can  be
accomplished by using a wildcard '*' and a special 'any type' which is
done using  the '?'  character. It  should be  noted that the wildcard
operator is  associated with  the previous  type in  the sequence  and
implies one or more of that type.

   template <typename T>
   struct zoo : public exprtk::igeneric_function<T>
   {
      typedef typename exprtk::igeneric_function<T>::parameter_list_t
                                                     parameter_list_t;

      zoo()
      : exprtk::igeneric_function<T>("SVT*V?")
      {}

      inline T operator()(parameter_list_t parameters)
      {
         ...
      }
   };


In the example above the generic function 'zoo' expects at least  five
parameters in the following sequence:

   (1) String
   (2) Vector
   (3) One or more Scalars
   (4) Vector
   (5) Any type (one type of either a scalar, vector or string)


A final  piece of  type checking  functionality is  available for  the
scenarios where  a single  function name  is intended  to be  used for
multiple distinct parameter sequences,  another name for this  feature
is function  overloading. The  parameter sequences  are passed  to the
constructor as a  single string delimited  by the pipe  '|' character.
Two specific overrides of the  function operator are provided one  for
standard generic functions and one for string returning functions. The
overrides are as follows:

      // Scalar <-- function(psi,i_0,i_1,....,i_N)
      inline T operator()(const std::size_t& ps_index,
                          parameter_list_t parameters)
      {
         ...
      }

      // String <-- function(psi,i_0,i_1,....,i_N)
      inline T operator()(const std::size_t& ps_index,
                          std::string& result,
                          parameter_list_t parameters)
      {
         ...
      }


When the function  operator is invoked  the 'ps_index' parameter  will
have as its value the index of the parameter sequence that matches the
specific invocation. This way complex and time consuming type checking
conditions need not  be executed in  the function itself  but rather a
simple and efficient  dispatch to a  specific implementation for  that
particular parameter sequence can be performed.

   template <typename T>
   struct roo : public exprtk::igeneric_function<T>
   {
      typedef typename exprtk::igeneric_function<T>::parameter_list_t
                                                     parameter_list_t;

      moo()
      : exprtk::igeneric_function<T>("SVTT|SS|TTV|S?V*S")
      {}

      inline T operator()(const std::size_t& ps_index,
                          parameter_list_t parameters)
      {
         ...
      }
   };


In the example above there are four distinct parameter sequences  that
can be processed  by the generic  function 'roo'. Any  other parameter
sequences will cause a compilation error. The four valid sequences are
as follows:

    Sequence-0    Sequence-1    Sequence-2    Sequence-3
      'SVTT'         'SS'          'TTV'       'S?V*S'
   (1) String    (1) String    (1) Scalar    (1) String
   (2) Vector    (2) String    (2) Scalar    (2) Any Type
   (3) Scalar                  (3) Vector    (3) One or more Vectors
   (4) Scalar                                (4) String


(5) function_compositor
The function  compositor is  a factory  that allows  one to define and
construct a function using ExprTk syntax. The functions are limited to
returning a single scalar value and consuming up to six parameters  as
input.

All composited functions are registered with a symbol table,  allowing
them to call other functions that have been registered with the symbol
table instance. Furthermore the  functions can be recursive  in nature
due to the inherent  function prototype forwarding that  occurs during
construction.  The following  example  defines, by using two different
methods, composited functions and implicitly registering the functions
with the denoted symbol table.

   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::function_compositor<T>    compositor_t;
   typedef typename compositor_t::function     function_t;

   symbol_table_t symbol_table;

   compositor_t compositor(symbol_table);

   // define function koo0(v1,v2) { ... }
   compositor
      .add(
        function_t(
         "koo0",
         " 1 + cos(v1 * v2) / 3;",
         "v1","v2"));

   // define function koo1(x,y,z) { ... }
   compositor
      .add(function_t()
         .name("koo1")
         .var("x").var("y").var("z")
         .expression("1 + cos(x * y) / z;"));


(6) Using Functions In Expressions
For the above denoted custom and composited functions to be used in an
expression, an instance of each function needs to be registered with a
symbol_table that  has been  associated with  the expression instance.
The following demonstrates how all the pieces are put together:

   typedef exprtk::symbol_table<double>      symbol_table_t;
   typedef exprtk::expression<double>          expression_t;
   typedef exprtk::parser<double>                  parser_t;
   typedef exprtk::function_compositor<double> compositor_t;
   typedef typename compositor_t::function       function_t;

   foo<double> f;
   boo<double> b;
   too<double> t;
   toupper<double> tu;

   symbol_table_t symbol_table;
   compositor_t   compositor(symbol_table);

   symbol_table.add_function("foo",f);
   symbol_table.add_function("boo",b);
   symbol_table.add_function("too",t);

   symbol_table.add_function("toupper",
                             tu,
                             symbol_table_t::e_ft_strfunc);

   compositor
      .add(function_t()
         .name("koo")
         .var("v1")
         .var("v2")
         .expression("1 + cos(v1 * v2) / 3;"));

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   std::string expression_str =
                " if (foo(1,2,3) + boo(1) > boo(1/2,2/3,3/4,4/5)) "
                "   koo(3,4);                                     "
                " else                                            "
                "   too(2 * v1 + v2 / 3, 'abcdef'[2:4], 3.3);     "
                "                                                 ";

   parser_t parser;
   parser.compile(expression_str,expression);

   expression.value();


(7) Function Side-Effects
All function calls are assumed  to have side-effects by default.  This
assumption implicitly disables constant folding optimisations when all
parameters being passed to the function are deduced as being constants
at compile time.

If it is certain that the function being registered does not have  any
side effects and can  be correctly constant folded  where appropriate,
then during the construction of the function the side-effect trait  of
the function can be disabled.

   template <typename T>
   struct foo : public exprtk::ifunction<T>
   {
      foo() : exprtk::ifunction<T>(3)
      {
          exprtk::disable_has_side_effects(*this);
      }

      T operator()(const T& v1, const T& v2, const T& v3)
      { ... }
   };


(8) Zero Parameter Functions
When  either  an  ifunction,  ivararg_function  or   igeneric_function
derived type is defined with zero number of parameters, there are  two
calling  conventions  within  expressions  that  are  allowed.  For  a
function named 'foo' with zero input parameters the calling styles are
as follows:

   (1)  x + sin(foo()- 2) / y
   (2)  x + sin(foo  - 2) / y


By default the  zero parameter trait  is disabled. In  order to enable
it, a process similar to that of  enabling of the side effect trait is
carried out:

   template <typename T>
   struct foo : public exprtk::ivararg_function<T>
   {
      foo()
      {
         exprtk::enable_zero_parameters(*this);
      }

      inline T operator()(const std::vector<T>& arglist)
      { ... }
   };

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[16 - EXPRESSION DEPENDENTS]
Any  expression  that  is  not  a  literal  (aka  constant)  will have
dependencies. The types of  'dependencies' an expression can  have are
as follows:

   (a) Variables
   (b) Vectors
   (c) Strings
   (d) Functions
   (e) Assignments


In  the  following  example the  denoted  expression  has its  various
dependencies listed:

   z := abs(x + sin(2 * pi / y))

   (a) Variables:   x, y, z and pi
   (b) Functions:   abs, sin
   (c) Assignments: z


ExprTk allows for  the derivation of  expression dependencies via  the
'dependent_entity_collector'  (DEC).  When  activated  either  through
'compile_options' at the construction  of the parser or  through calls
to enabler methods just prior to compilation, the DEC will proceed  to
collect any  of the  relevant types  that are  encountered during  the
parsing  phase.   Once  the   compilation  process   has  successfully
completed, the  caller can  then obtain  a list  of symbols  and their
associated types from the DEC.

The kinds of  questions one can  ask regarding the  dependent entities
within an expression are as follows:

  * What user defined or local variables, vectors or strings are used?
  * What functions or custom user functions are used?
  * Which variables, vectors or strings have values assigned to them?


The following example demonstrates usage of the DEC in determining the
dependents of the given expression:

   typedef typename parser_t::
      dependent_entity_collector::symbol_t symbol_t;

   std::string expression_string =
                  "z := abs(x + sin(2 * pi / y))";

   T x,y,z;

   parser_t parser;
   symbol_table_t symbol_table;

   symbol_table.add_variable("x",x);
   symbol_table.add_variable("y",y);
   symbol_table.add_variable("z",z);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   //Collect only variable and function symbols
   parser.dec().collect_variables() = true;
   parser.dec().collect_functions() = true;

   if (!parser.compile(expression_string,expression))
   {
      // error....
   }

   std::deque<symbol_t> symbol_list;

   parser.dec().symbols(symbol_list);

   for (std::size_t i = 0; i < symbol_list.size(); ++i)
   {
      symbol_t& symbol = symbol_list[i];

      switch (symbol.second)
      {
         case parser_t::e_st_variable : ... break;
         case parser_t::e_st_vector   : ... break;
         case parser_t::e_st_string   : ... break;
         case parser_t::e_st_function : ... break;
      }
   }


Note: The 'symbol_t' type is a std::pair comprising of the symbol name
(std::string) and the associated type of the symbol as denoted by  the
cases in the switch statement.

Having  particular  symbols  (variable  or  function)  present  in  an
expression is one form of dependency. Another and just as  interesting
and important type of  dependency is that of  assignments. Assignments
are the set of dependent symbols that 'may' have their values modified
within an expression. The following are example expressions and  their
associated assignments:

       Assignments   Expression
   (1) x             x := y + z
   (2) x, y          x += y += z
   (3) x, y, z       x := y += sin(z := w + 2)
   (4) z, w          if (x > y, z := x + 2, w := 'A String')
   (5) None          x + y + z


Note: In expression 4, both variables 'z' and 'w' are denoted as being
assignments even though only one of  them can be modified at the  time
of  evaluation.  Furthermore the  determination  of which  of  the two
variables the  modification will  occur upon  can only  be known  with
certainty at evaluation time and not beforehand, hence both are listed
as being candidates for assignment.

The following builds upon the previous example demonstrating the usage
of the DEC in determining the 'assignments' of the given expression:

   //Collect assignments
   parser.dec().collect_assignments() = true;

   if (!parser.compile(expression_string,expression))
   {
      // error....
   }

   std::deque<symbol_t> symbol_list;

   parser.dec().assignment_symbols(symbol_list);

   for (std::size_t i = 0; i < symbol_list.size(); ++i)
   {
      symbol_t& symbol = symbol_list[i];

      switch (symbol.second)
      {
         case parser_t::e_st_variable : ... break;
         case parser_t::e_st_vector   : ... break;
         case parser_t::e_st_string   : ... break;
      }
   }


Note: The assignments will only consist of variable types and as  such
will not contain symbols denoting functions.

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[17 - ENABLING AND DISABLING FEATURES]
The parser can be configured via its settings instance to either allow
or  disallow certain  features that  are available  within the  ExprTk
grammar. The features fall  into one  of the following six categories:

   (1) Base Functions
   (2) Control Flow Structures
   (3) Logical Operators
   (4) Arithmetic Operators
   (5) Inequality Operators
   (6) Assignment Operators


(1) Base Functions
The list of available base functions is as follows:

   abs, acos, acosh, asin,  asinh, atan, atanh, atan2,  avg, ceil,
   clamp,  cos,  cosh, cot,  csc,  equal, erf,  erfc,  exp, expm1,
   floor,  frac,  hypot,  iclamp, like,  log,  log10,  log2, logn,
   log1p, mand, max, min, mod,  mor, mul, ncdf, pow, root,  round,
   roundn, sec, sgn, sin, sinc, sinh, sqrt, sum, swap, tan,  tanh,
   trunc, not_equal, inrange, deg2grad, deg2rad, rad2deg, grad2deg


The above mentioned base functions  can be either enabled or  disabled
'all' at once, as is demonstrated below:

    parser_t parser;
    expression_t expression;

    parser.settings().disable_all_base_functions();

    parser
      .compile("2 * abs(2 - 3)",expression); // compilation failure

    parser.settings().enable_all_base_functions();

    parser
      .compile("2 * abs(2 - 3)",expression); // compilation success


One can also enable or disable specific base functions. The  following
example  demonstrates  the disabling  of  the trigonometric  functions
'sin' and 'cos':

    parser_t parser;
    expression_t expression;

    parser.settings()
      .disable_base_function(settings_t::e_bf_sin)
      .disable_base_function(settings_t::e_bf_cos);

    parser
      .compile("(sin(x) / cos(x)) == tan(x)",expression); // failure

    parser.settings()
      .enable_base_function(settings_t::e_bf_sin)
      .enable_base_function(settings_t::e_bf_cos);

    parser
      .compile("(sin(x) / cos(x)) == tan(x)",expression); // success


(2) Control Flow Structures
The list of available control flow structures is as follows:

   (a) If or If-Else
   (b) Switch statement
   (c) For Loop
   (d) While Loop
   (e) Repeat Loop


The  above  mentioned  control flow structures  can be  either enabled
or disabled 'all' at once, as is demonstrated below:

    parser_t parser;
    expression_t expression;

    std::string program =
       " var x := 0;                      "
       " for (var i := 0; i < 10; i += 1) "
       " {                                "
       "   x += i;                        "
       " }                                ";

    parser.settings().disable_all_control_structures();

    parser
      .compile(program,expression); // compilation failure

    parser.settings().enable_all_control_structures();

    parser
      .compile(program,expression); // compilation success


One can also enable or  disable specific control flow structures.  The
following example demonstrates the  disabling of the for-loop  control
flow structure:

    parser_t parser;
    expression_t expression;

    std::string program =
       " var x := 0;                      "
       " for (var i := 0; i < 10; i += 1) "
       " {                                "
       "   x += i;                        "
       " }                                ";

    parser.settings()
      .disable_control_structure(settings_t::e_ctrl_for_loop);

    parser
      .compile(program,expression); // failure

    parser.settings()
      .enable_control_structure(settings_t::e_ctrl_for_loop);

    parser
      .compile(program,expression); // success


(3) Logical Operators
The list of available logical operators is as follows:

   and, nand, nor, not, or, xnor, xor, &, |


The  above  mentioned  logical  operators  can  be  either  enabled or
disabled 'all' at once, as is demonstrated below:

    parser_t parser;
    expression_t expression;

    parser.settings().disable_all_logic_ops();

    parser
      .compile("1 or not(0 and 1)",expression); // compilation failure

    parser.settings().enable_all_logic_ops();

    parser
      .compile("1 or not(0 and 1)",expression); // compilation success


One can also enable or disable specific logical operators. The  following
example  demonstrates  the disabling  of  the 'and' logical operator:

    parser_t parser;
    expression_t expression;

    parser.settings()
      .disable_logic_operation(settings_t::e_logic_and);

    parser
      .compile("1 or not(0 and 1)",expression); // failure

    parser.settings()
      .enable_logic_operation(settings_t::e_logic_and);

    parser
      .compile("1 or not(0 and 1)",expression); // success


(4) Arithmetic Operators
The list of available arithmetic operators is as follows:

   +, -, *, /, %, ^


The  above  mentioned  arithmetic  operators  can  be  either  enabled or
disabled 'all' at once, as is demonstrated below:

    parser_t parser;
    expression_t expression;

    parser.settings().disable_all_arithmetic_ops();

    parser
      .compile("1 + 2 / 3",expression); // compilation failure

    parser.settings().enable_all_arithmetic_ops();

    parser
      .compile("1 + 2 / 3",expression); // compilation success


One can also enable or disable specific arithmetic operators. The  following
example demonstrates the disabling of  the addition '+' arithmetic operator:

    parser_t parser;
    expression_t expression;

    parser.settings()
      .disable_arithmetic_operation(settings_t::e_arith_add);

    parser
      .compile("1 + 2 / 3",expression); // failure

    parser.settings()
      .enable_arithmetic_operation(settings_t::e_arith_add);

    parser
      .compile("1 + 2 / 3",expression); // success


(5) Inequality Operators
The list of available inequality operators is as follows:

   <, <=, >, >=, ==, =, != <>


The  above  mentioned  inequality  operators  can  be  either  enabled or
disabled 'all' at once, as is demonstrated below:

    parser_t parser;
    expression_t expression;

    parser.settings().disable_all_inequality_ops();

    parser
      .compile("1 < 3",expression); // compilation failure

    parser.settings().enable_all_inequality_ops();

    parser
      .compile("1 < 3",expression); // compilation success


One can also enable or disable specific inequality operators. The  following
example demonstrates the disabling of the less-than '<' inequality operator:

    parser_t parser;
    expression_t expression;

    parser.settings()
      .disable_inequality_operation(settings_t::e_ineq_lt);

    parser
      .compile("1 < 3",expression); // failure

    parser.settings()
      .enable_inequality_operation(settings_t::e_ineq_lt);

    parser
      .compile("1 < 3",expression); // success


(6) Assignment Operators
The list of available assignment operators is as follows:

   :=, +=, -=, *=, /=, %=


The  above  mentioned  assignment  operators  can  be  either  enabled or
disabled 'all' at once, as is demonstrated below:

    parser_t parser;
    expression_t expression;
    symbol_table_t symbol_table;

    T x = T(0);

    symbol_table.add_variable("x",x);

    expression.register_symbol_table(symbol_table);

    parser.settings().disable_all_assignment_ops();

    parser
      .compile("x := 3",expression); // compilation failure

    parser.settings().enable_all_assignment_ops();

    parser
      .compile("x := 3",expression); // compilation success


One can also enable or disable specific assignment operators. The  following
example demonstrates the disabling of the '+=' addition assignment operator:

    parser_t parser;
    expression_t expression;
    symbol_table_t symbol_table;

    T x = T(0);

    symbol_table.add_variable("x",x);

    expression.register_symbol_table(symbol_table);

    parser.settings()
      .disable_assignment_operation(settings_t::e_assign_addass);

    parser
      .compile("x += 3",expression); // failure

    parser.settings()
      .enable_assignment_operation(settings_t::e_assign_addass);

    parser
      .compile("x += 3",expression); // success


Note: In the event of a base function being disabled, one can redefine
the  base  function  using  the  standard  custom  function definition
process. In the  following example the 'sin' function is disabled then
redefined as a function taking degree input.

    template <typename T>
    struct sine_deg : public exprtk::ifunction<T>
    {
       sine_deg() : exprtk::ifunction<T>(1) {}

       inline T operator()(const T& v)
       {
          const T pi = exprtk::details::numeric::constant::pi;
          return std::sin((v * T(pi)) / T(180));
       }
    };

    .
    .
    .

    typedef exprtk::symbol_table<T> symbol_table_t;
    typedef exprtk::expression<T>     expression_t;
    typedef exprtk::parser<T>             parser_t;

    typedef typename parser_t::settings_store settings_t;

    sine_deg<T> sine;

    symbol_table.add_reserved_function("sin",sine);

    expression_t expression;

    expression.register_symbol_table(symbol_table);

    parser_t parser;

    parser.settings()
       .disable_base_function(settings_t::e_bf_sin);

    parser.compile("1 + sin(30)",expression);


In the example above, the custom 'sin' function is registered with the
symbol_table using the method 'add_reserved_function'. This is done so
as to bypass the checks for reserved words that are carried out on the
provided symbol names when calling the standard 'add_function' method.
Normally if  a user  specified symbol  name conflicts  with any of the
ExprTk reserved words, the add_function call will fail.

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[18 - EXPRESSION RETURN VALUES]
ExprTk expressions can return immediately from any point by  utilizing
the return call. Furthermore the  return call can be used  to transfer
out multiple return values from within the expression.

If an expression evaluation exits using a return point, the result  of
the call to the 'value' method will be NaN, and it's expected that the
return values will be available from the results_context.

In  the  following example  there  are  three  return  points  in  the
expression.  If  neither  of  the  return  points  are  hit,  then the
expression will return normally.

   std::string expression_string =
                  " if (x < y)                                   "
                  "   return [x + 1,'return-call 1'];            "
                  " else if (x > y)                              "
                  "   return [y / 2, y + 1, 'return-call 2'];    "
                  " else if (equal(x,y))                         "
                  "   x + y;                                     "
                  " return [x, y, x + y, x - y, 'return-call 3'] ";

   typedef exprtk::symbol_table<double> symbol_table_t;
   typedef exprtk::expression<double>     expression_t;
   typedef exprtk::parser<double>             parser_t;

   symbol_table_t symbol_table;
   expression_t   expression;
   parser_t       parser;

   double x = 0;
   double y = 0;

   symbol_table.add_variable("x",x);
   symbol_table.add_variable("y",y);

   expression.register_symbol_table(symbol_table);

   parser.compile(expression_string,expression);

   T result = expression.value();

   if (expression.results().count())
   {
      typedef exprtk::results_context<T> results_context_t;
      typedef typename results_context_t::type_store_t type_t;
      typedef typename type_t::scalar_view scalar_t;
      typedef typename type_t::vector_view vector_t;
      typedef typename type_t::string_view string_t;

      const results_context_t& results = expression.results();

      for (std::size_t i = 0; i < results.count(); ++i)
      {
         type_t t = results[i];

         switch (t.type)
         {
            case type_t::e_scalar : ...
                                    break;

            case type_t::e_vector : ...
                                    break;

            case type_t::e_string : ...
                                    break;

            default               : continue;
         }
   }


Note: Processing of the return results is similar to that of the
generic function call parameters.

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[19 - COMPILATION ERRORS]
When attempting to compile  a malformed or otherwise  erroneous ExprTk
expression, the  compilation process  will result  in an  error, as is
indicated  by  the  'compile'  method  returning  a  false  value.   A
diagnostic indicating the first error encountered and its cause can be
obtained by  invoking the  'error' method,  as is  demonstrated in the
following example:

   if (!parser.compile(expression_string,expression))
   {
      printf("Error: %s\n", parser.error().c_str());
      return false;
   }


Any error(s) resulting from a failed compilation will be stored in the
parser instance until the next time a compilation is performed. Before
then errors can be enumerated  in the order they occurred  by invoking
the 'get_error' method which itself will return a 'parser_error' type.
A parser_error object will contain an error diagnostic, an error  mode
(or class), and the character position of the error in the  expression
string. The following example demonstrates the enumeration of error(s)
in the event of a failed compilation.

   if (!parser.compile(expression_string,expression))
   {
      for (std::size_t i = 0; i < parser.error_count(); ++i)
      {
         typedef exprtk::parser_error::type error_t;

         error_t error = parser.get_error(i);

         printf("Error[%02d] Position: %02d Type: [%14s] Msg: %s\n",
                i,
                error.token.position,
                exprtk::parser_error::to_str(error.mode).c_str(),
                error.diagnostic.c_str());
      }

      return false;
   }


For  expressions  comprised  of  multiple  lines,  the  error position
provided in the  parser_error object can  be converted into  a pair of
line and column numbers by invoking the 'update_error' function as  is
demonstrated by the following example:

   if (!parser.compile(program_str,expression))
   {
      for (std::size_t i = 0; i < parser.error_count(); ++i)
      {
         typedef exprtk::parser_error::type error_t;

         error_t error = parser.get_error(i);

         exprtk::parser_error::update_error(error,program_str);

         printf("Error[%02d] at line: %d column: %d\n",
                i,
                error.line_no,
                error.column_no);
      }

      return false;
   }


Note: There are five  distinct error modes in  ExprTk which denote the
class of an error. These classes are as follows:

   (a) Syntax
   (b) Token
   (c) Numeric
   (d) Symbol Table
   (e) Lexer


(a) Syntax Errors
These are errors  related to invalid  syntax found within  the denoted
expression. Examples are invalid sequences of operators and variables,
incorrect number  of parameters  to functions,  invalid conditional or
loop structures and invalid use of keywords.

   eg:  'for := sin(x,y,z) + 2 * equal > until[2 - x,3]'


(b) Token Errors
Errors in this class relate to  token level errors detected by one  or
more of the following checkers:

   (1) Bracket Checker
   (2) Numeric Checker
   (3) Sequence Checker


(c) Numeric Errors
This class of  error is related  to conversion of  numeric values from
their  string form  to the  underlying numerical  type (float,  double
etc).

(d) Symbol Table Errors
This is the class of errors related to failures when interacting  with
the registered symbol_table instance. Errors such as not being able to
find,  within  the  symbol_table,  symbols  representing  variables or
functions, to being unable to create new variables in the symbol_table
via the 'unknown symbol resolver' mechanism.

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[20 - EXPRTK NOTES]
The following is a list of facts and suggestions one may want to take
into account when using ExprTk:

 (00) Precision  and performance  of expression  evaluations are  the
      dominant principles of the ExprTk library.

 (01) ExprTk  uses a  rudimentary imperative  programming model  with
      syntax based  on languages  such as  Pascal and  C. Furthermore
      ExprTk  is  an LL(2)  type  grammar and  is  processed using  a
      recursive descent parsing algorithm.

 (02) Supported types are float, double, long double and MPFR/GMP.

 (03) Standard mathematical operator precedence is applied (BEDMAS).

 (04) Results of expressions that are deemed as being 'valid' are  to
      exist within the set of Real numbers. All other results will be
      of the value: Not-A-Number (NaN).

 (05) Supported   user  defined   types   are   numeric  and   string
      variables, numeric vectors and functions.

 (06) All  reserved words,  keywords, variable,  vector, string   and
      function names are case-insensitive.

 (07) Variable, vector, string variable and function names must begin
      with  a  letter  (A-Z or  a-z), then  can be  comprised of  any
      combination of letters, digits and underscores. (eg: x, var1 or
      power_func99)

 (08) Expression lengths and sub-expression lists are limited only by
      storage capacity.

 (09) The  life-time of  objects registered  with or  created from  a
      specific symbol-table must span  at least the life-time  of the
      compiled expressions which utilize objects, such as  variables,
      of that  symbol-table, otherwise  the result  will be undefined
      behavior.

 (10) Equal  and  Nequal  are  normalised-epsilon  equality routines,
      which use epsilons of 0.0000000001 and 0.000001 for double  and
      float types respectively.

 (11) All trigonometric functions  assume radian input  unless stated
      otherwise.

 (12) Expressions may contain  white-space characters such  as space,
      tabs, new-lines, control-feed et al.
      ('\n', '\r', '\t', '\b', '\v', '\f')

 (13) Strings may be comprised of any combination of letters, digits
      special characters including (~!@#$%^&*()[]|=+ ,./?<>;:"`~_) or
      hexadecimal escaped sequences (eg: \0x30) and must be enclosed
      with single-quotes.
      eg: 'Frankly my dear, \0x49 do n0t give a damn!'

 (14) User defined  normal functions  can have  up to  20 parameters,
      where as  user defined  generic-functions and  vararg-functions
      can have an unlimited number of parameters.

 (15) The inbuilt polynomial functions can be at most of degree 12.

 (16) Where appropriate constant folding optimisations may be applied.
      (eg: The expression '2 + (3 - (x / y))' becomes '5 - (x / y)')

 (17) If the strength reduction compilation option has been enabled,
      then where applicable strength reduction optimisations may be
      applied.

 (18) String  processing capabilities  are available  by default.  To
      turn them  off, the  following needs  to be  defined at compile
      time: exprtk_disable_string_capabilities

 (19) Composited functions can call themselves or any other functions
      that have been defined prior to their own definition.

 (20) Recursive calls made from within composited functions will have
      a stack size bound by the stack of the executing architecture.

 (21) User  defined functions  by default  are assumed  to have  side
      effects. As such an "all constant parameter" invocation of such
      functions wont result in constant folding. If the function  has
      no side effects then that  can be noted during the  constructor
      of  the  ifunction  allowing it  to  be  constant folded  where
      appropriate.

 (22) The entity relationship between symbol_table and an  expression
      is many-to-many. However the intended 'typical' use-case  where
      possible, is to have a single symbol table manage the  variable
      and function requirements of multiple expressions.

 (23) The common use-case  for an expression  is to have  it compiled
      only  ONCE  and  then subsequently  have it  evaluated multiple
      times. An extremely  inefficient and suboptimal  approach would
      be to recompile an expression  from its string form every  time
      it requires evaluating.

 (24) It is  strongly recommended  that  the  return value  of method
      invocations from  the parser  and symbol_table  types be  taken
      into account. Specifically the  'compile' method of the  parser
      and the 'add_xxx'  set of methods  of the symbol_table  as they
      denote either the success or failure state of the invoked call.
      Continued processing from  a failed  state without having first
      rectified the underlying issue  will in turn result  in further
      failures and undefined behaviours.

 (25) The following are examples of compliant floating point value
      representations:

      (1) 12345         (5) -123.456
      (2) +123.456e+12  (6) 123.456E-12
      (3) +012.045e+07  (7) .1234
      (4) 123.456f      (8) -321.654E+3L

 (26) Expressions may contain any of the following comment styles:

      (1) // .... \n
      (2) #  .... \n
      (3) /* .... */

 (27) The  'null'  value  type  is  a  special  non-zero  type   that
      incorporates specific semantics when undergoing operations with
      the standard numeric type. The following is a list of type  and
      boolean results associated with the use of 'null':

      (1) null  +,-,*,/,%  x    --> x
      (2) x     +,-,*,/,%  null --> x
      (3) null  +,-,*,/,%  null --> null
      (4) null     ==      null --> true
      (5) null     ==      x    --> true
      (6) x        ==      null --> true
      (7) x        !=      null --> false
      (8) null     !=      null --> false
      (9) null     !=      x    --> false

 (28) The following is a list  of reserved words and symbols  used by
      ExprTk. Attempting to  add a variable  or custom function  to a
      symbol table using any of  the reserved words will result  in a
      failure.

      abs, acos, acosh, and, asin, asinh, atan, atan2, atanh, avg,
      break, case,  ceil, clamp,  continue, cosh,  cos, cot,  csc,
      default,  deg2grad, deg2rad,  else, equal,  erfc, erf,  exp,
      expm1, false, floor, for, frac, grad2deg, hypot, iclamp, if,
      ilike, in, inrange, in, like, log, log10, log1p, log2, logn,
      mand,  max,  min,  mod,  mor,  mul,  nand,  ncdf,  nor, not,
      not_equal,  not,  null, or,  pow,  rad2deg, repeat,  return,
      root, roundn, round,  sec, sgn, shl,  shr, sinc, sinh,  sin,
      sqrt, sum, swap, switch, tanh, tan, true, trunc, until, var,
      while, xnor, xor, xor

 (29) Every valid ExprTk statement is a "value returning" expression.
      Unlike some languages that limit the types of expressions  that
      can be  performed in  certain situations,  in ExprTk  any valid
      expression can be used in any "value consuming" context. eg:

      var y := 3;
      for (var x := switch
                    {
                      case 1  :  7;
                      case 2  : -1 + ~{var x{};};
                      default :  y > 2 ? 3 : 4;
                    };
           x != while (y > 0) { y -= 1; };
           x -= {
                  if(min(x,y) < 2 * max(x,y))
                    x + 2;
                  else
                    x + y - 3;
                }
          )
      {
        (x + y) / (x - y);
      }

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[21 - SIMPLE EXPRTK EXAMPLE]
--- snip ---
#include <cstdio>
#include <string>

#include "exprtk.hpp"

template <typename T>
struct myfunc : public exprtk::ifunction<T>
{
   myfunc() : exprtk::ifunction<T>(2) {}

   T operator()(const T& v1, const T& v2)
   {
      return T(1) + (v1 * v2) / T(3);
   }
};

int main()
{
   typedef exprtk::symbol_table<double> symbol_table_t;
   typedef exprtk::expression<double>     expression_t;
   typedef exprtk::parser<double>             parser_t;
   typedef exprtk::parser_error::type          error_t;

   std::string expression_str =
                  "z := 2 myfunc([4 + sin(x / pi)^3],y ^ 2)";

   double x = 1.1;
   double y = 2.2;
   double z = 3.3;

   myfunc<double> mf;

   symbol_table_t symbol_table;
   symbol_table.add_constants();
   symbol_table.add_variable("x",x);
   symbol_table.add_variable("y",y);
   symbol_table.add_variable("z",z);
   symbol_table.add_function("myfunc",mf);

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;

   if (!parser.compile(expression_str,expression))
   {
      // A compilation error has occurred. Attempt to
      // print all errors to stdout.

      printf("Error: %s\tExpression: %s\n",
             parser.error().c_str(),
             expression_str.c_str());

      for (std::size_t i = 0; i < parser.error_count(); ++i)
      {
         // Include the specific nature of each error
         // and its position in the expression string.

         error_t error = parser.get_error(i);

         printf("Error: %02d Position: %02d "
                "Type: [%s] "
                "Message: %s "
                "Expression: %s\n",
                static_cast<int>(i),
                static_cast<int>(error.token.position),
                exprtk::parser_error::to_str(error.mode).c_str(),
                error.diagnostic.c_str(),
                expression_str.c_str());
      }

      return 1;
   }

   // Evaluate the expression and obtain its result.

   double result = expression.value();

   printf("Result: %10.5f\n",result);

   return 0;
}
--- snip ---

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[22 - BUILD OPTIONS]
When building ExprTk there are a number of defines that will enable or
disable certain features and  capabilities. The defines can  either be
part of a compiler command line switch or scoped around the include to
the ExprTk header. The defines are as follows:

   (1) exprtk_enable_debugging
   (2) exprtk_disable_comments
   (3) exprtk_disable_break_continue
   (4) exprtk_disable_sc_andor
   (5) exprtk_disable_enhanced_features
   (6) exprtk_disable_string_capabilities


(1) exprtk_enable_debugging
This define will enable printing of debug information to stdout during
the compilation process.

(2) exprtk_disable_comments
This define will disable the ability for expressions to have comments.
Expressions that have comments when parsed with a build that has  this
option, will result in a compilation failure.

(3) exprtk_disable_break_continue
This  define  will  disable  the  loop-wise  'break'  and   'continue'
capabilities. Any expression that contains those keywords will  result
in a compilation failure.

(4) exprtk_disable_sc_andor
This define  will disable  the short-circuit  '&' (and)  and '|'  (or)
operators

(5) exprtk_disable_enhanced_features
This  define  will  disable all  enhanced  features  such as  strength
reduction and special  function optimisations and  expression specific
type instantiations.  This feature  will reduce  compilation times and
binary sizes but will  also result in massive  performance degradation
of expression evaluations.

(6) exprtk_disable_string_capabilities
This  define  will  disable all  string  processing  capabilities. Any
expression that contains a string or string related syntax will result
in a compilation failure.

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[23 - FILES]
The source distribution of ExprTk is comprised of the following set of
files:

   (00) Makefile
   (01) readme.txt
   (02) exprtk.hpp
   (03) exprtk_test.cpp
   (04) exprtk_benchmark.cpp
   (05) exprtk_simple_example_01.cpp
   (06) exprtk_simple_example_02.cpp
   (07) exprtk_simple_example_03.cpp
   (08) exprtk_simple_example_04.cpp
   (09) exprtk_simple_example_05.cpp
   (10) exprtk_simple_example_06.cpp
   (11) exprtk_simple_example_07.cpp
   (12) exprtk_simple_example_08.cpp
   (13) exprtk_simple_example_09.cpp
   (14) exprtk_simple_example_10.cpp
   (15) exprtk_simple_example_11.cpp
   (16) exprtk_simple_example_12.cpp
   (17) exprtk_simple_example_13.cpp
   (18) exprtk_simple_example_14.cpp
   (19) exprtk_simple_example_15.cpp
   (20) exprtk_simple_example_16.cpp

     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[24 - LANGUAGE STRUCTURE]
+-------------------------------------------------------------+
|00 - If Statement                                            |
|                                                             |
|   [if] ---> [(] ---> [condition] -+-> [,] -+                |
|                                   |        |                |
|   +---------------<---------------+        |                |
|   |                                        |                |
|   |  +------------------<------------------+                |
|   |  |                                                      |
|   |  +--> [consequent] ---> [,] ---> [alternative] ---> [)] |
|   |                                                         |
|   +--> [)] --+-> [{] ---> [expression*] ---> [}] --+        |
|              |                                     |        |
|              |                +---------<----------+        |
|   +----<-----+                |                             |
|   |                           v                             |
|   +--> [consequent] --> [;] -{*}-> [else-statement]         |
|                                                             |
+-------------------------------------------------------------+
|01 - Else Statement                                          |
|                                                             |
|   [else] -+-> [alternative] ---> [;]                        |
|           |                                                 |
|           +--> [{] ---> [expression*] ---> [}]              |
|           |                                                 |
|           +--> [if-statement]                               |
|                                                             |
+-------------------------------------------------------------+
|02 - Ternary Statement                                       |
|                                                             |
|   [condition] ---> [?] ---> [consequent] ---> [:] --+       |
|                                                     |       |
|   +------------------------<------------------------+       |
|   |                                                         |
|   +--> [alternative] --> [;]                                |
|                                                             |
+-------------------------------------------------------------+
|03 - While Loop                                              |
|                                                             |
|   [while] ---> [(] ---> [condition] ---> [)] ---+           |
|                                                 |           |
|   +----------------------<----------------------+           |
|   |                                                         |
|   +--> [{] ---> [expression*] ---> [}]                      |
|                                                             |
+-------------------------------------------------------------+
|04 - Repeat Until Loop                                       |
|                                                             |
|   [repeat] ---> [expression*] ---+                          |
|                                  |                          |
|   +--------------<---------------+                          |
|   |                                                         |
|   +--> [until] ---> [(] ---> [condition] --->[)]            |
|                                                             |
+-------------------------------------------------------------+
|05 - For Loop                                                |
|                                                             |
|   [for] ---> [(] -+-> [initialise expression] --+--+        |
|                   |                             |  |        |
|                   +------------->---------------+  v        |
|                                                    |        |
|   +-----------------------<------------------------+        |
|   |                                                         |
|   +--> [;] -+-> [condition] -+-> [;] ---+                   |
|             |                |          |                   |
|             +------->--------+          v                   |
|                                         |                   |
|   +------------------<---------+--------+                   |
|   |                            |                            |
|   +--> [increment expression] -+-> [)] --+                  |
|                                          |                  |
|   +------------------<-------------------+                  |
|   |                                                         |
|   +--> [{] ---> [expression*] ---> [}]                      |
|                                                             |
+-------------------------------------------------------------+
|06 - Switch Statement                                        |
|                                                             |
|   [switch] ---> [{] ---+                                    |
|                        |                                    |
|   +---------<----------+-----------<-----------+            |
|   |                                            |            |
|   +--> [case] ---> [condition] ---> [:] ---+   |            |
|                                            |   |            |
|   +-------------------<--------------------+   |            |
|   |                                            |            |
|   +--> [consequent] ---> [;] --------->--------+            |
|   |                                            |            |
|   |                                            |            |
|   +--> [default] ---> [consequent] ---> [;] ---+            |
|   |                                            |            |
|   +---------------------<----------------------+            |
|   |                                                         |
|   +--> [}]                                                  |
|                                                             |
+-------------------------------------------------------------+
|07 - Multi Subexpression Statement                           |
|                                                             |
|                   +--------------<---------------+          |
|                   |                              |          |
|   [~] ---> [{\(] -+-> [expression] -+-> [;\,] ---+          |
|                                     |                       |
|   +----------------<----------------+                       |
|   |                                                         |
|   +--> [}\)]                                                |
|                                                             |
+-------------------------------------------------------------+
|08 - Multi Case-Consequent Statement                         |
|                                                             |
|   [[*]] ---> [{] ---+                                       |
|                     |                                       |
|   +--------<--------+--------------<----------+             |
|   |                                           |             |
|   +--> [case] ---> [condition] ---> [:] ---+  |             |
|                                            |  |             |
|   +-------------------<--------------------+  |             |
|   |                                           |             |
|   +--> [consequent] ---> [;] ---+------>------+             |
|                                 |                           |
|                                 +--> [}]                    |
|                                                             |
+-------------------------------------------------------------+
|09 - Variable Definition Statement                           |
|                                                             |
|   [var] ---> [symbol] -+-> [:=] -+-> [expression] -+-> [;]  |
|                        |         |                 |        |
|                        |         +-----> [{}] -->--+        |
|                        |                           |        |
|                        +------------->-------------+        |
|                                                             |
+-------------------------------------------------------------+
|10 - Vector Definition Statement                             |
|                                                             |
|   [var] ---> [symbol] ---> [[] ---> [constant] ---> []] --+ |
|                                                           | |
|   +---------------------------<---------------------------+ |
|   |                                                         |
|   |                   +--------->---------+                 |
|   |                   |                   |                 |
|   +--> [:=] ---> [{] -+-+-> [expression] -+-> [}] ---> [;]  |
|                         |                 |                 |
|                         +--<--- [,] <-----+                 |
|                                                             |
+-------------------------------------------------------------+
|11 - String Definition Statement                             |
|                                                             |
|   [var] --> [symbol] --> [:=] --> [str-expression] ---> [;] |
|                                                             |
+-------------------------------------------------------------+
|12 - Range Statement                                         |
|                                                             |
|      +-------->--------+                                    |
|      |                 |                                    |
| [[] -+-> [expression] -+-> [:] -+-> [expression] -+--> []]  |
|                                 |                 |         |
|                                 +-------->--------+         |
|                                                             |
+-------------------------------------------------------------+
|13 - Return Statement                                        |
|                                                             |
|   [return] ---> [[] -+-> [expression] -+-> []] ---> [;]     |
|                      |                 |                    |
|                      +--<--- [,] <-----+                    |
|                                                             |
+-------------------------------------------------------------+
