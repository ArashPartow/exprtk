C++ Mathematical Expression Toolkit Library

[INTRODUCTION]
The C++ Mathematical Expression Library  (ExprTk) is a simple to  use,
easy  to  integrate and  extremely  efficient mathematical  expression
parsing and  evaluation engine.  The parsing  engine supports  various
kinds of  functional, logic  processing semantics  and is  very easily
extendible.


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


[MACROS]
ExprTk utilizes certain macros  to modify the underlying  behaviour of
the parser and the evaluation engine. The following macros are used to
switch off certain capabilities  within the ExprTk evaluation  engine.
The capabilities are predominantly related to expression optimisations
and the ability to evaluate strings within expressions.

(1) exprtk_disable_string_capabilities
(2) exprtk_disable_cardinal_pow_optimisation
(3) exprtk_disable_extended_optimisations

(1) "exprtk_disable_string_capabilities"
If defined, the macro will disable all string processing capabilities.
When defined, if an expression  containing a string or string  related
action  is encountered,  a compilation  error will  be raised  by the
parser.

(2) "exprtk_disable_cardinal_pow_optimisation"
If  defined,  the  macro  will  disable  the  special  case  regarding
exponentiation  of  a  variable  to  an  integer  constant  (where the
constant is  <= 25).  Defining this  variable may  be desirable if the
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


[FILES]
(00) Makefile
(01) readme.txt
(02) exprtk.hpp
(03) exprtk_test.cpp
(04) exprtk_benchmark.cpp
