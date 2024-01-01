/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 23                                          *
 * Author: Arash Partow (1999-2024)                           *
 * URL: https://www.partow.net/programming/exprtk/index.html  *
 *                                                            *
 * Copyright notice:                                          *
 * Free use of the Mathematical Expression Toolkit Library is *
 * permitted under the guidelines and in accordance with the  *
 * most current version of the MIT License.                   *
 * https://www.opensource.org/licenses/MIT                    *
 * SPDX-License-Identifier: MIT                               *
 *                                                            *
 **************************************************************
*/


#include <cstdio>
#include <string>

#include "exprtk.hpp"


template <typename T>
void real_1d_discrete_fourier_transform()
{
   typedef exprtk::symbol_table<T>         symbol_table_t;
   typedef exprtk::expression<T>           expression_t;
   typedef exprtk::parser<T>               parser_t;
   typedef exprtk::function_compositor<T>  compositor_t;
   typedef typename compositor_t::function function_t;

   const T sampling_rate = 1024.0;            // ~1KHz
   const T N             = 8 * sampling_rate; // 8 seconds worth of samples

   std::vector<T> input (static_cast<std::size_t>(N),0.0);
   std::vector<T> output(static_cast<std::size_t>(N),0.0);

   exprtk::rtl::io::println<T> println;

   symbol_table_t symbol_table;
   symbol_table.add_vector   ("input"        , input        );
   symbol_table.add_vector   ("output"       , output       );
   symbol_table.add_function ("println"      , println      );
   symbol_table.add_constant ("N"            , N            );
   symbol_table.add_constant ("sampling_rate", sampling_rate);
   symbol_table.add_pi();

   compositor_t compositor(symbol_table);
   compositor.load_vectors(true);

   compositor.add(
      function_t("dft_1d_real")
      .var("N")
      .expression
      (
         " for (var k := 0; k < N; k += 1)        "
         " {                                      "
         "    var k_real := 0.0;                  "
         "    var k_imag := 0.0;                  "
         "                                        "
         "    for (var i := 0; i < N; i += 1)     "
         "    {                                   "
         "       var theta := 2pi * k * i / N;    "
         "       k_real += input[i] * cos(theta); "
         "       k_imag -= input[i] * sin(theta); "
         "    };                                  "
         "                                        "
         "    output[k] := hypot(k_real,k_imag);  "
         " }                                      "
      ));

   const std::string dft_program =
      "                                                                          "
      " /*                                                                       "
      "    Generate an aggregate waveform comprised of three                     "
      "    sine waves of varying frequencies and amplitudes.                     "
      " */                                                                       "
      " const var num_waves := 3;                                                "
      "                                                                          "
      " var frequencies[num_waves] := { 111.125, 222.250, 333.375 }; /* Hz    */ "
      " var amplitudes [num_waves] := {  11.111,  22.222,  33.333 }; /* Power */ "
      "                                                                          "
      " for (var i := 0; i < N; i += 1)                                          "
      " {                                                                        "
      "    var time := i / sampling_rate;                                        "
      "                                                                          "
      "    for (var j := 0; j < frequencies[]; j += 1)                           "
      "    {                                                                     "
      "       var frequency := frequencies[j];                                   "
      "       var amplitude := amplitudes [j];                                   "
      "       var theta     := 2 * pi * frequency * time;                        "
      "                                                                          "
      "       input[i] += amplitude * sin(theta);                                "
      "    }                                                                     "
      " };                                                                       "
      "                                                                          "
      " dft_1d_real(input[]);                                                    "
      "                                                                          "
      " var freq_bin_size   := sampling_rate / N;                                "
      " var max_bin         := ceil(N / 2);                                      "
      " var max_noise_level := 1e-6;                                             "
      "                                                                          "
      " /* Normalise amplitudes */                                               "
      " output /= max_bin;                                                       "
      "                                                                          "
      " println('1D Real DFT Frequencies');                                      "
      "                                                                          "
      " for (var k := 0; k < max_bin; k += 1)                                    "
      " {                                                                        "
      "    if (output[k] > max_noise_level)                                      "
      "    {                                                                     "
      "       var freq_begin := k * freq_bin_size;                               "
      "       var freq_end   := freq_begin + freq_bin_size;                      "
      "                                                                          "
      "       println('Index: ', k,' ',                                          "
      "               'Freq. range: [', freq_begin, 'Hz, ', freq_end, 'Hz) ',    "
      "               'Amplitude: ', output[k]);                                 "
      "    }                                                                     "
      " }                                                                        "
      "                                                                          ";

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(dft_program,expression);

   expression.value();
}

int main()
{
   real_1d_discrete_fourier_transform<double>();
   return 0;
}
