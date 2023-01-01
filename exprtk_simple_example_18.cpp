/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 18                                          *
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
#include <string>

#include "exprtk.hpp"


template <typename T>
void file_io()
{
   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>   expression_t;
   typedef exprtk::parser<T>       parser_t;

   const std::string fileio_program =
                " var file_name := 'file.txt';                         "
                " var stream    := null;                               "
                "                                                      "
                " if (stream := open(file_name,'w'))                   "
                "   println('Successfully opened file: ' + file_name); "
                " else                                                 "
                " {                                                    "
                "   println('Failed to open file: ' + file_name);      "
                "   return [false];                                    "
                " }                                                    "
                "                                                      "
                " var s := 'Hello world...\n';                         "
                "                                                      "
                " for (var i := 0; i < 10; i += 1)                     "
                " {                                                    "
                "   write(stream,s);                                   "
                " }                                                    "
                "                                                      "
                " if (close(stream))                                   "
                "   println('Sucessfully closed file: ' + file_name);  "
                " else                                                 "
                " {                                                    "
                "   println('Failed to close file: ' + file_name);     "
                "   return [false];                                    "
                " }                                                    ";

   exprtk::rtl::io::file::package<T> fileio_package;
   exprtk::rtl::io::println<T>       println;

   symbol_table_t symbol_table;
   symbol_table.add_function("println",println);
   symbol_table.add_package (fileio_package   );

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(fileio_program,expression);

   printf("Result %10.3f\n",expression.value());
}

int main()
{
   file_io<double>();
   return 0;
}
