/*
 *******************************************************************
 *           C++ Mathematical Expression Toolkit Library           *
 *                                                                 *
 * Author: Arash Partow (1999-2014)                                *
 * URL: http://www.partow.net/programming/exprtk/index.html        *
 *                                                                 *
 * Copyright notice:                                               *
 * Free use of the C++ Mathematical Expression Toolkit Library is  *
 * permitted under the guidelines and in accordance with the most  *
 * current version of the Common Public License.                   *
 * http://www.opensource.org/licenses/cpl1.0.php                   *
 *                                                                 *
 * Example expressions:                                            *
 * (00) (y + x / y) * (x - y / x)                                  *
 * (01) (x^2 / sin(2 * pi / y)) - x / 2                            *
 * (02) sqrt(1 - (x^2))                                            *
 * (03) 1 - sin(2 * x) + cos(pi / y)                               *
 * (04) a * exp(2 * t) + c                                         *
 * (05) if(((x + 2) == 3) and ((y + 5) <= 9),1 + w, 2 / z)         *
 * (06) if(avg(x,y) <= x + y, x - y, x * y) + 2 * pi / x           *
 * (07) z := x + sin(2 * pi / y)                                   *
 * (08) u := 2 * (pi * z) / (w := x + cos(y / pi))                 *
 * (09) clamp(-1,sin(2 * pi * x) + cos(y / 2 * pi),+1)             *
 * (10) inrange(-2,m,+2) == if(({-2 <= m} and [m <= +2]),1,0)      *
 * (11) (1.2sin(x)cos(2y)7 + 1) == (1.2 * sin(x) * cos(2*y) * 7+1) *
 * (12) (x ilike 's*ri?g') and [y < (3 z^7 + w)]                   *
 *                                                                 *
 *******************************************************************
*/


#ifndef INCLUDE_EXPRTK_HPP
#define INCLUDE_EXPRTK_HPP


#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>


namespace exprtk
{
   namespace details
   {
      inline bool is_whitespace(const char c)
      {
         return (' '  == c) || ('\n' == c) ||
                ('\r' == c) || ('\t' == c) ||
                ('\b' == c) || ('\v' == c) ||
                ('\f' == c) ;
      }

      inline bool is_operator_char(const char c)
      {
         return ('+' == c) || ('-' == c) ||
                ('*' == c) || ('/' == c) ||
                ('^' == c) || ('<' == c) ||
                ('>' == c) || ('=' == c) ||
                (',' == c) || ('!' == c) ||
                ('(' == c) || (')' == c) ||
                ('[' == c) || (']' == c) ||
                ('{' == c) || ('}' == c) ||
                ('%' == c) || (':' == c) ||
                ('?' == c) || ('&' == c) ||
                ('|' == c) || (';' == c);
      }

      inline bool is_letter(const char c)
      {
         return (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z'));
      }

      inline bool is_digit(const char c)
      {
         return ('0' <= c) && (c <= '9');
      }

      inline bool is_letter_or_digit(const char c)
      {
         return is_letter(c) || is_digit(c);
      }

      inline bool is_left_bracket(const char c)
      {
         return ('(' == c) || ('[' == c) || ('{' == c);
      }

      inline bool is_right_bracket(const char c)
      {
         return (')' == c) || (']' == c) || ('}' == c);
      }

      inline bool is_bracket(const char c)
      {
         return is_left_bracket(c) || is_right_bracket(c);
      }

      inline bool is_sign(const char c)
      {
         return ('+' == c) || ('-' == c);
      }

      inline bool is_invalid(const char c)
      {
         return !is_whitespace(c)    &&
                !is_operator_char(c) &&
                !is_letter(c)        &&
                !is_digit(c)         &&
                ('.' != c)           &&
                ('_' != c)           &&
                ('$' != c)           &&
                ('~' != c)           &&
                ('\'' != c);
      }

      inline bool imatch(const char c1, const char c2)
      {
         return std::tolower(c1) == std::tolower(c2);
      }

      inline bool imatch(const std::string& s1, const std::string& s2)
      {
         if (s1.size() == s2.size())
         {
            for (std::size_t i = 0; i < s1.size(); ++i)
            {
               if (std::tolower(s1[i]) != std::tolower(s2[i]))
               {
                  return false;
               }
            }
            return true;
         }
         return false;
      }

      inline bool is_valid_sf_symbol(const std::string& symbol)
      {
         //Special function: $f12 or $F34
         return (symbol.size() == 4)  &&
                ('$' == symbol[0])    &&
                imatch('f',symbol[1]) &&
                is_digit(symbol[2])   &&
                is_digit(symbol[3]);
      }

      inline std::string to_str(int i)
      {
         if (0 == i)
            return std::string("0");
         std::string result;
         bool negative = (i < 0);
         if (negative) i *= -1;
         while (i)
         {
            char digit = '0' + char(i % 10);
            result = (digit + result);
            i /= 10;
         }
         if (negative)
            result = "-" + result;
         return result;
      }

      inline void cleanup_escapes(std::string& s)
      {
         std::string::iterator itr1 = s.begin();
         std::string::iterator itr2 = s.begin();
         std::string::iterator end =    s.end();
         std::size_t removal_count = 0;
         while (end != itr1)
         {
            bool bypass = false;
            if ('\\' == (*itr1))
            {
               bypass = true;
               ++removal_count;
               if (end == ++itr1)
                  break;
               else if ('\\' != (*itr1))
               {
                  switch (*itr1)
                  {
                     case 'n' : (*itr1) = '\n'; break;
                     case 'r' : (*itr1) = '\r'; break;
                     case 't' : (*itr1) = '\t'; break;
                  }
                  continue;
               }
               else
                  bypass = false;
            }

            if (!bypass)
            {
               if (itr1 != itr2)
               {
                  (*itr2) = (*itr1);
               }
               ++itr1;
               ++itr2;
            }
         }
         s.resize(s.size() - removal_count);
      }

      class build_string
      {
      public:

         build_string(const std::size_t& initial_size = 64)
         {
            data_.reserve(initial_size);
         }

         inline build_string& operator << (const std::string& s)
         {
            data_ += s;
            return (*this);
         }

         inline build_string& operator << (const char* s)
         {
            data_ += std::string(s);
            return (*this);
         }

         inline operator std::string () const
         {
            return data_;
         }

         inline std::string as_string() const
         {
            return data_;
         }

      private:

         std::string data_;
      };

      struct ilesscompare
      {
         inline bool operator()(const std::string& s1, const std::string& s2) const
         {
            const std::size_t length = std::min(s1.size(),s2.size());
            for (std::size_t i = 0; i < length;  ++i)
            {
               if (std::tolower(s1[i]) > std::tolower(s2[i]))
                  return false;
               else if (std::tolower(s1[i]) < std::tolower(s2[i]))
                  return true;
            }
            return s1.size() < s2.size();
         }
      };

      static const std::string reserved_words[] =
                                  {
                                     "and", "default", "case", "false", "for", "if", "ilike", "in", "like",
                                     "nand", "nor", "not", "null", "or", "repeat", "shl", "shr", "switch",
                                     "true", "until", "while", "xnor", "xor", "&", "|"
                                  };

      static const std::size_t reserved_words_size = sizeof(reserved_words) / sizeof(std::string);

      static const std::string reserved_symbols[] =
                                  {
                                     "abs", "acos", "acosh", "and", "asin", "asinh", "atan", "atanh", "atan2",
                                     "avg", "case", "ceil", "clamp", "cos", "cosh", "cot", "csc", "default",
                                     "deg2grad", "deg2rad", "equal", "erf", "erfc", "exp", "expm1", "false",
                                     "floor", "for", "frac", "grad2deg", "hypot", "iclamp", "if", "ilike", "in",
                                     "inrange", "like", "log", "log10", "log2", "logn", "log1p", "mand", "max",
                                     "min", "mod", "mor", "mul", "nand", "nor", "not", "not_equal", "null", "or",
                                     "pow", "rad2deg", "repeat", "root", "round", "roundn", "sec", "sgn", "shl",
                                     "shr", "sin", "sinh", "sqrt", "sum", "switch", "tan", "tanh", "true", "trunc",
                                     "until", "while", "xnor", "xor", "&", "|"
                                  };

      static const std::size_t reserved_symbols_size = sizeof(reserved_symbols) / sizeof(std::string);

      inline bool is_reserved_word(const std::string& symbol)
      {
         for (std::size_t i = 0; i < reserved_words_size; ++i)
         {
            if (imatch(symbol,reserved_words[i]))
            {
               return true;
            }
         }
         return false;
      }

      inline bool is_reserved_symbol(const std::string& symbol)
      {
         for (std::size_t i = 0; i < reserved_symbols_size; ++i)
         {
            if (imatch(symbol,reserved_symbols[i]))
            {
               return true;
            }
         }
         return false;
      }

      struct cs_match  { static inline bool cmp(const char c0, const char c1) { return c0 == c1; } };
      struct cis_match { static inline bool cmp(const char c0, const char c1) { return std::tolower(c0) == std::tolower(c1); } };

      template <typename Iterator, typename Compare>
      inline bool match_impl(const Iterator pattern_begin,
                             const Iterator pattern_end,
                             const Iterator data_begin,
                             const Iterator data_end,
                             const typename std::iterator_traits<Iterator>::value_type& zero_or_more,
                             const typename std::iterator_traits<Iterator>::value_type& zero_or_one)
      {
         if (0 == std::distance(data_begin,data_end)) return false;
         Iterator d_itr = data_begin;
         Iterator p_itr = pattern_begin;
         Iterator c_itr = data_begin;
         Iterator m_itr = data_begin;
         while ((data_end != d_itr) && (zero_or_more != (*p_itr)))
         {
            if ((!Compare::cmp((*p_itr),(*d_itr))) && (zero_or_one != (*p_itr)))
            {
               return false;
            }
            ++p_itr;
            ++d_itr;
         }

         while (data_end != d_itr)
         {
            if (zero_or_more == (*p_itr))
            {
               if (pattern_end == (++p_itr))
               {
                  return true;
               }
               m_itr = p_itr;
               c_itr = d_itr;
               ++c_itr;
            }
            else if ((Compare::cmp((*p_itr),(*d_itr))) || (zero_or_one == (*p_itr)))
            {
               ++p_itr;
               ++d_itr;
            }
            else
            {
               p_itr = m_itr;
               d_itr = c_itr++;
            }
         }
         while ((p_itr != pattern_end) && (zero_or_more == (*p_itr))) ++p_itr;
         return (p_itr == pattern_end);
      }

      inline bool wc_match(const std::string& wild_card,
                           const std::string& str)
      {
         return match_impl<const char*,cs_match>(wild_card.data(),
                                                 wild_card.data() + wild_card.size(),
                                                 str.data(),
                                                 str.data() + str.size(),
                                                 '*',
                                                 '?');
      }

      inline bool wc_imatch(const std::string& wild_card,
                            const std::string& str)
      {
         return match_impl<const char*,cis_match>(wild_card.data(),
                                                  wild_card.data() + wild_card.size(),
                                                  str.data(),
                                                  str.data() + str.size(),
                                                  '*',
                                                  '?');
      }

      static const double pow10[] = {
                                       1.0,
                                       10.0,
                                       100.0,
                                       1000.0,
                                       10000.0,
                                       100000.0,
                                       1000000.0,
                                       10000000.0,
                                       100000000.0,
                                       1000000000.0,
                                       10000000000.0,
                                       100000000000.0,
                                       1000000000000.0,
                                       10000000000000.0,
                                       100000000000000.0,
                                       1000000000000000.0,
                                       10000000000000000.0,
                                    };

     static const std::size_t pow10_size = sizeof(pow10) / sizeof(double);

      namespace numeric
      {
         namespace constant
         {
            static const double e       =  2.718281828459045235360;
            static const double pi      =  3.141592653589793238462;
            static const double pi_2    =  1.570796326794896619231;
            static const double pi_4    =  0.785398163397448309616;
            static const double pi_180  =  0.017453292519943295769;
            static const double _1_pi   =  0.318309886183790671538;
            static const double _2_pi   =  0.636619772367581343076;
            static const double _180_pi = 57.295779513082320876798;
            static const double log2    =  0.693147180559945309417;
         }

         namespace details
         {
            struct unknown_type_tag {};
            struct real_type_tag    {};
            struct int_type_tag     {};

            template <typename T>
            struct number_type { typedef unknown_type_tag type; };

            #define exprtk_register_real_type_tag(T)                          \
            template<> struct number_type<T> { typedef real_type_tag type; }; \

            #define exprtk_register_int_type_tag(T)                          \
            template<> struct number_type<T> { typedef int_type_tag type; }; \

            exprtk_register_real_type_tag(double)
            exprtk_register_real_type_tag(long double)
            exprtk_register_real_type_tag(float)
            exprtk_register_int_type_tag(short)
            exprtk_register_int_type_tag(int)
            exprtk_register_int_type_tag(long long int)
            exprtk_register_int_type_tag(unsigned short)
            exprtk_register_int_type_tag(unsigned int)
            exprtk_register_int_type_tag(unsigned long long int)

            #undef exprtk_register_real_type_tag
            #undef exprtk_register_int_type_tag

            template <typename T>
            inline bool is_true_impl(const T v)
            {
               return (v != T(0));
            }

            template <typename T>
            inline bool is_false_impl(const T v)
            {
               return (T(0) == v);
            }

            template <typename T>
            inline T equal_impl(const T v0, const T v1, real_type_tag)
            {
               static const T epsilon = T(0.0000000001);
               return (std::abs(v0 - v1) <= (std::max(T(1),std::max(std::abs(v0),std::abs(v1))) * epsilon)) ? T(1) : T(0);
            }

            inline float equal_impl(const float v0, const float v1, real_type_tag)
            {
               static const float epsilon = float(0.000001f);
               return (std::abs(v0 - v1) <= (std::max(1.0f,std::max(std::abs(v0),std::abs(v1))) * epsilon)) ? 1.0f : 0.0f;
            }

            template <typename T>
            inline T equal_impl(const T v0, const T v1, int_type_tag)
            {
               return (v0 == v1) ? 1 : 0;
            }

            template <typename T>
            inline T expm1_impl(const T v, real_type_tag)
            {
               //return std::expm1<T>(v);
               if (std::abs(v) < T(0.00001))
                  return v + (T(0.5) * v * v);
               else
                  return std::exp(v) - T(1);
            }

            template <typename T>
            inline T expm1_impl(const T v, int_type_tag)
            {
               return T(std::exp<double>(v)) - T(1);
            }

            template <typename T>
            inline T nequal_impl(const T v0, const T v1, real_type_tag)
            {
               static const T epsilon = T(0.0000000001);
               return (std::abs(v0 - v1) > (std::max(T(1),std::max(std::abs(v0),std::abs(v1))) * epsilon)) ? T(1) : T(0);
            }

            inline float nequal_impl(const float v0, const float v1, real_type_tag)
            {
               static const float epsilon = float(0.000001f);
               return (std::abs(v0 - v1) > (std::max(1.0f,std::max(std::abs(v0),std::abs(v1))) * epsilon)) ? 1.0f : 0.0f;
            }

            template <typename T>
            inline T nequal_impl(const T v0, const T v1, int_type_tag)
            {
               return (v0 != v1) ? 1 : 0;
            }

            template <typename T>
            inline T modulus_impl(const T v0, const T v1, real_type_tag)
            {
               return std::fmod(v0,v1);
            }

            template <typename T>
            inline T modulus_impl(const T v0, const T v1, int_type_tag)
            {
               return v0 % v1;
            }

            template <typename T>
            inline T pow_impl(const T v0, const T v1, real_type_tag)
            {
               return std::pow(v0,v1);
            }

            template <typename T>
            inline T pow_impl(const T v0, const T v1, int_type_tag)
            {
               return std::pow(static_cast<double>(v0),static_cast<double>(v1));
            }

            template <typename T>
            inline T logn_impl(const T v0, const T v1, real_type_tag)
            {
               return std::log(v0) / std::log(v1);
            }

            template <typename T>
            inline T logn_impl(const T v0, const T v1, int_type_tag)
            {
               return static_cast<T>(logn_impl<double>(static_cast<double>(v0),static_cast<double>(v1),real_type_tag()));
            }

            template <typename T>
            inline T log1p_impl(const T v, real_type_tag)
            {
               if (v > T(-1))
               {
                  if (abs_impl(v,real_type_tag()) > T(0.0001))
                  {
                     return std::log(T(1) + v);
                  }
                  else
                     return (T(-0.5) * v + T(1)) * v;
               }
               else
                  return std::numeric_limits<T>::quiet_NaN();
            }

            template <typename T>
            inline T log1p_impl(const T v, int_type_tag)
            {
               if (v > T(-1))
               {
                  return std::log(T(1) + v);
               }
               else
                  return std::numeric_limits<T>::quiet_NaN();
            }

            template <typename T>
            inline T root_impl(const T v0, const T v1, real_type_tag)
            {
               return std::pow(v0,T(1) / v1);
            }

            template <typename T>
            inline T root_impl(const T v0, const T v1, int_type_tag)
            {
               return root_impl<double>(static_cast<double>(v0),static_cast<double>(v1),real_type_tag());
            }

            template <typename T>
            inline T round_impl(const T v, real_type_tag)
            {
               return ((v < T(0)) ? std::ceil(v - T(0.5)) : std::floor(v + T(0.5)));
            }

            template <typename T>
            inline T roundn_impl(const T v0, const T v1, real_type_tag)
            {
               const int index = std::max<int>(0, std::min<int>(pow10_size - 1, (int)std::floor(v1)));
               const T p10 = T(pow10[index]);
               if (v0 < T(0))
                  return T(std::ceil ((v0 * p10) - T(0.5)) / p10);
               else
                  return T(std::floor((v0 * p10) + T(0.5)) / p10);
            }

            template <typename T>
            inline T roundn_impl(const T v0, const T, int_type_tag)
            {
               return v0;
            }

            template <typename T>
            inline T hypot_impl(const T v0, const T v1, real_type_tag)
            {
               return std::sqrt((v0 * v0) + (v1 * v1));
            }

            template <typename T>
            inline T hypot_impl(const T v0, const T v1, int_type_tag)
            {
               return static_cast<T>(std::sqrt(static_cast<double>((v0 * v0) + (v1 * v1))));
            }

            template <typename T>
            inline T atan2_impl(const T v0, const T v1, real_type_tag)
            {
               return std::atan2(v0,v1);
            }

            template <typename T>
            inline T atan2_impl(const T, const T, int_type_tag)
            {
               return 0;
            }

            template <typename T>
            inline T shr_impl(const T v0, const T v1, real_type_tag)
            {
               return v0 * (T(1) / std::pow(T(2),static_cast<T>(static_cast<int>(v1))));
            }

            template <typename T>
            inline T shr_impl(const T v0, const T v1, int_type_tag)
            {
               return v0 >> v1;
            }

            template <typename T>
            inline T shl_impl(const T v0, const T v1, real_type_tag)
            {
               return v0 * std::pow(T(2),static_cast<T>(static_cast<int>(v1)));
            }

            template <typename T>
            inline T shl_impl(const T v0, const T v1, int_type_tag)
            {
               return v0 << v1;
            }

            template <typename T>
            inline T sgn_impl(const T v, real_type_tag)
            {
                    if (v > T(0)) return T(+1);
               else if (v < T(0)) return T(-1);
               else               return T( 0);
            }

            template <typename T>
            inline T sgn_impl(const T v, int_type_tag)
            {
                    if (v > T(0)) return T(+1);
               else if (v < T(0)) return T(-1);
               else               return T( 0);
            }

            template <typename T>
            inline T and_impl(const T v0, const T& v1, real_type_tag)
            {
               return (is_true_impl(v0) && is_true_impl(v1)) ? T(1) : T(0);
            }

            template <typename T>
            inline T and_impl(const T v0, const T& v1, int_type_tag)
            {
               return v0 && v1;
            }

            template <typename T>
            inline T nand_impl(const T v0, const T& v1, real_type_tag)
            {
               return (is_false_impl(v0) || is_false_impl(v1)) ? T(1) : T(0);
            }

            template <typename T>
            inline T nand_impl(const T v0, const T& v1, int_type_tag)
            {
               return !(v0 && v1);
            }

            template <typename T>
            inline T or_impl(const T v0, const T& v1, real_type_tag)
            {
               return (is_true_impl(v0) || is_true_impl(v1)) ? T(1) : T(0);
            }

            template <typename T>
            inline T or_impl(const T v0, const T& v1, int_type_tag)
            {
               return (v0 || v1);
            }

            template <typename T>
            inline T nor_impl(const T v0, const T& v1, real_type_tag)
            {
               return (is_false_impl(v0) && is_false_impl(v1)) ? T(1) : T(0);
            }

            template <typename T>
            inline T nor_impl(const T v0, const T& v1, int_type_tag)
            {
               return !(v0 || v1);
            }

            template <typename T>
            inline T xor_impl(const T v0, const T& v1, real_type_tag)
            {
               return (is_false_impl(v0) != is_false_impl(v1)) ? T(1) : T(0);
            }

            template <typename T>
            inline T xor_impl(const T v0, const T& v1, int_type_tag)
            {
               return v0 ^ v1;
            }

            template <typename T>
            inline T xnor_impl(const T v0, const T& v1, real_type_tag)
            {
               const bool v0_true = is_true_impl(v0);
               const bool v1_true = is_true_impl(v1);
               if ((v0_true &&  v1_true) || (!v0_true && !v1_true))
                  return T(1);
               else
                  return T(0);
            }

            template <typename T>
            inline T xnor_impl(const T v0, const T& v1, int_type_tag)
            {
               const bool v0_true = is_true_impl(v0);
               const bool v1_true = is_true_impl(v1);
               if ((v0_true &&  v1_true) || (!v0_true && !v1_true))
                  return T(1);
               else
                  return T(0);
            }

            template <typename T>
            inline T erf_impl(T v, real_type_tag)
            {
               #if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
               //Credits: Abramowitz & Stegun Equations 7.1.25-28
               const T t = T(1) / (T(1) + T(0.5) * std::abs(v));
               static const T c[] = {
                                      T( 1.26551223), T(1.00002368),
                                      T( 0.37409196), T(0.09678418),
                                      T(-0.18628806), T(0.27886807),
                                      T(-1.13520398), T(1.48851587),
                                      T(-0.82215223), T(0.17087277)
                                    };
               T result = T(1) - t * std::exp((-v * v) -
                                      c[0] + t * (c[1] + t *
                                     (c[2] + t * (c[3] + t *
                                     (c[4] + t * (c[5] + t *
                                     (c[6] + t * (c[7] + t *
                                     (c[8] + t * (c[9]))))))))));
               return (v >= T(0)) ? result : -result;
               #else
               return ::erf(v);
               #endif
            }

            template <typename T>
            inline T erf_impl(T v, int_type_tag)
            {
               return erf_impl(static_cast<double>(v),real_type_tag());
            }

            template <typename T>
            inline T erfc_impl(T v, real_type_tag)
            {
               #if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
               return T(1) - erf_impl(v,real_type_tag());
               #else
               return ::erfc(v);
               #endif
            }

            template <typename T>
            inline T erfc_impl(T v, int_type_tag)
            {
               return erfc_impl(static_cast<double>(v),real_type_tag());
            }

            template <typename T> inline T   abs_impl(const T v, real_type_tag) { return ((v >= T(0)) ? v : -v); }
            template <typename T> inline T  acos_impl(const T v, real_type_tag) { return std::acos (v); }
            template <typename T> inline T acosh_impl(const T v, real_type_tag) { return std::log(v + std::sqrt((v * v) - T(1))); }
            template <typename T> inline T  asin_impl(const T v, real_type_tag) { return std::asin (v); }
            template <typename T> inline T asinh_impl(const T v, real_type_tag) { return std::log(v + std::sqrt((v * v) + T(1))); }
            template <typename T> inline T  atan_impl(const T v, real_type_tag) { return std::atan (v); }
            template <typename T> inline T atanh_impl(const T v, real_type_tag) { return (std::log(T(1) + v) - log(T(1) - v)) / T(2); }
            template <typename T> inline T  ceil_impl(const T v, real_type_tag) { return std::ceil (v); }
            template <typename T> inline T   cos_impl(const T v, real_type_tag) { return std::cos  (v); }
            template <typename T> inline T  cosh_impl(const T v, real_type_tag) { return std::cosh (v); }
            template <typename T> inline T   exp_impl(const T v, real_type_tag) { return std::exp  (v); }
            template <typename T> inline T floor_impl(const T v, real_type_tag) { return std::floor(v); }
            template <typename T> inline T   log_impl(const T v, real_type_tag) { return std::log  (v); }
            template <typename T> inline T log10_impl(const T v, real_type_tag) { return std::log10(v); }
            template <typename T> inline T  log2_impl(const T v, real_type_tag) { return std::log(v)/T(numeric::constant::log2); }
            template <typename T> inline T   neg_impl(const T v, real_type_tag) { return -v;            }
            template <typename T> inline T   pos_impl(const T v, real_type_tag) { return +v;            }
            template <typename T> inline T   sin_impl(const T v, real_type_tag) { return std::sin  (v); }
            template <typename T> inline T  sinh_impl(const T v, real_type_tag) { return std::sinh (v); }
            template <typename T> inline T  sqrt_impl(const T v, real_type_tag) { return std::sqrt (v); }
            template <typename T> inline T   tan_impl(const T v, real_type_tag) { return std::tan  (v); }
            template <typename T> inline T  tanh_impl(const T v, real_type_tag) { return std::tanh (v); }
            template <typename T> inline T   cot_impl(const T v, real_type_tag) { return T(1) / std::tan(v); }
            template <typename T> inline T   sec_impl(const T v, real_type_tag) { return T(1) / std::cos(v); }
            template <typename T> inline T   csc_impl(const T v, real_type_tag) { return T(1) / std::sin(v); }
            template <typename T> inline T   r2d_impl(const T v, real_type_tag) { return (v * T(numeric::constant::_180_pi)); }
            template <typename T> inline T   d2r_impl(const T v, real_type_tag) { return (v * T(numeric::constant::pi_180));  }
            template <typename T> inline T   d2g_impl(const T v, real_type_tag) { return (v * T(20.0/9.0)); }
            template <typename T> inline T   g2d_impl(const T v, real_type_tag) { return (v * T(9.0/20.0)); }
            template <typename T> inline T  notl_impl(const T v, real_type_tag) { return (v != T(0) ? T(0) : T(1)); }
            template <typename T> inline T  frac_impl(const T v, real_type_tag) { return (v - static_cast<long long>(v)); }
            template <typename T> inline T trunc_impl(const T v, real_type_tag) { return T(static_cast<long long>(v));    }

            template <typename T> inline T   abs_impl(const T v, int_type_tag) { return ((v >= T(0)) ? v : -v); }
            template <typename T> inline T   exp_impl(const T v, int_type_tag) { return std::exp  (v); }
            template <typename T> inline T   log_impl(const T v, int_type_tag) { return std::log  (v); }
            template <typename T> inline T log10_impl(const T v, int_type_tag) { return std::log10(v); }
            template <typename T> inline T  log2_impl(const T v, int_type_tag) { return std::log(v)/T(numeric::constant::log2); }
            template <typename T> inline T   neg_impl(const T v, int_type_tag) { return -v;            }
            template <typename T> inline T   pos_impl(const T v, int_type_tag) { return +v;            }
            template <typename T> inline T  ceil_impl(const T v, int_type_tag) { return v;             }
            template <typename T> inline T floor_impl(const T v, int_type_tag) { return v;             }
            template <typename T> inline T round_impl(const T v, int_type_tag) { return v;             }
            template <typename T> inline T  notl_impl(const T v, int_type_tag) { return !v;            }
            template <typename T> inline T  sqrt_impl(const T v, int_type_tag) { return std::sqrt (v); }
            template <typename T> inline T  frac_impl(const T  , int_type_tag) { return T(0);          }
            template <typename T> inline T trunc_impl(const T v, int_type_tag) { return v;             }
            template <typename T> inline T  acos_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T acosh_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T  asin_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T asinh_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T  atan_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T atanh_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T   cos_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T  cosh_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T   sin_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T  sinh_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T   tan_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T  tanh_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T   cot_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T   sec_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }
            template <typename T> inline T   csc_impl(const T  , int_type_tag) { return std::numeric_limits<T>::quiet_NaN(); }

            template <typename T>
            inline bool is_integer_impl(const T& v, real_type_tag)
            {
               return (T(0) == std::fmod(v,T(1)));
            }

            template <typename T>
            inline bool is_integer_impl(const T&, int_type_tag)
            {
               return true;
            }

         }

         template <typename Type>
         struct numeric_info { enum { length = 0, size = 32, bound_length = 0, min_exp = 0, max_exp = 0 }; };

         template<> struct numeric_info<int>         { enum { length = 10, size = 16, bound_length =  9}; };
         template<> struct numeric_info<float>       { enum { min_exp =  -38, max_exp =  +38}; };
         template<> struct numeric_info<double>      { enum { min_exp = -308, max_exp = +308}; };
         template<> struct numeric_info<long double> { enum { min_exp = -308, max_exp = +308}; };

         template <typename T>
         inline T equal(const T v0, const T v1)
         {
            typename details::number_type<T>::type num_type;
            return details::equal_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T nequal(const T v0, const T v1)
         {
            typename details::number_type<T>::type num_type;
            return details::nequal_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T modulus(const T v0, const T v1)
         {
            typename details::number_type<T>::type num_type;
            return details::modulus_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T pow(const T v0, const T v1)
         {
            typename details::number_type<T>::type num_type;
            return details::pow_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T logn(const T v0, const T v1)
         {
            typename details::number_type<T>::type num_type;
            return details::logn_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T root(const T v0, const T v1)
         {
            typename details::number_type<T>::type num_type;
            return details::root_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T roundn(const T v0, const T v1)
         {
            typename details::number_type<T>::type num_type;
            return details::roundn_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T hypot(const T v0, const T v1)
         {
            typename details::number_type<T>::type num_type;
            return details::hypot_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T atan2(const T v0, const T v1)
         {
            typename details::number_type<T>::type num_type;
            return details::atan2_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T shr(const T v0, const T v1)
         {
            typename details::number_type<T>::type num_type;
            return details::shr_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T shl(const T v0, const T v1)
         {
            typename details::number_type<T>::type num_type;
            return details::shl_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T and_opr(const T v0, const T v1)
         {
            typename details::number_type<T>::type num_type;
            return details::and_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T nand_opr(const T v0, const T v1)
         {
            typename details::number_type<T>::type num_type;
            return details::nand_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T or_opr(const T v0, const T v1)
         {
            typename details::number_type<T>::type num_type;
            return details::or_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T nor_opr(const T v0, const T v1)
         {
            typename details::number_type<T>::type num_type;
            return details::nor_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T xor_opr(const T v0, const T v1)
         {
            typename details::number_type<T>::type num_type;
            return details::xor_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T xnor_opr(const T v0, const T v1)
         {
            typename details::number_type<T>::type num_type;
            return details::xnor_impl(v0,v1,num_type);
         }

         template <typename T>
         inline bool is_integer(const T v)
         {
            typename details::number_type<T>::type num_type;
            return details::is_integer_impl(v,num_type);
         }

         template <typename T, unsigned int N>
         struct fast_exp
         {
            static inline T result(T v)
            {
               unsigned int k = N;
               T l = T(1);
               while (k)
               {
                  if (k & 1)
                  {
                     l *= v;
                     --k;
                  }
                  v *= v;
                  k >>= 1;
               }
               return l;
            }
         };

         template <typename T> struct fast_exp<T,10> { static inline T result(T v) { T v_5 = fast_exp<T,5>::result(v); return v_5 * v_5; } };
         template <typename T> struct fast_exp<T, 9> { static inline T result(T v) { return fast_exp<T,8>::result(v) * v; } };
         template <typename T> struct fast_exp<T, 8> { static inline T result(T v) { T v_4 = fast_exp<T,4>::result(v); return v_4 * v_4; } };
         template <typename T> struct fast_exp<T, 7> { static inline T result(T v) { return fast_exp<T,6>::result(v) * v; } };
         template <typename T> struct fast_exp<T, 6> { static inline T result(T v) { T v_3 = fast_exp<T,3>::result(v); return v_3 * v_3; } };
         template <typename T> struct fast_exp<T, 5> { static inline T result(T v) { return fast_exp<T,4>::result(v) * v; } };
         template <typename T> struct fast_exp<T, 4> { static inline T result(T v) { T v_2 = v * v; return v_2 * v_2; } };
         template <typename T> struct fast_exp<T, 3> { static inline T result(T v) { return v * v * v; } };
         template <typename T> struct fast_exp<T, 2> { static inline T result(T v) { return v * v;     } };
         template <typename T> struct fast_exp<T, 1> { static inline T result(T v) { return v;         } };
         template <typename T> struct fast_exp<T, 0> { static inline T result(T  ) { return T(1);      } };

         #define exprtk_define_unary_function(FunctionName)   \
         template <typename T>                                \
         inline T FunctionName (const T v)                    \
         {                                                    \
            typename details::number_type<T>::type num_type;  \
            return details:: FunctionName##_impl(v,num_type); \
         }                                                    \

         exprtk_define_unary_function(abs  )
         exprtk_define_unary_function(acos )
         exprtk_define_unary_function(acosh)
         exprtk_define_unary_function(asin )
         exprtk_define_unary_function(asinh)
         exprtk_define_unary_function(atan )
         exprtk_define_unary_function(atanh)
         exprtk_define_unary_function(ceil )
         exprtk_define_unary_function(cos  )
         exprtk_define_unary_function(cosh )
         exprtk_define_unary_function(exp  )
         exprtk_define_unary_function(expm1)
         exprtk_define_unary_function(floor)
         exprtk_define_unary_function(log  )
         exprtk_define_unary_function(log10)
         exprtk_define_unary_function(log2 )
         exprtk_define_unary_function(log1p)
         exprtk_define_unary_function(neg  )
         exprtk_define_unary_function(pos  )
         exprtk_define_unary_function(round)
         exprtk_define_unary_function(sin  )
         exprtk_define_unary_function(sinh )
         exprtk_define_unary_function(sqrt )
         exprtk_define_unary_function(tan  )
         exprtk_define_unary_function(tanh )
         exprtk_define_unary_function(cot  )
         exprtk_define_unary_function(sec  )
         exprtk_define_unary_function(csc  )
         exprtk_define_unary_function(r2d  )
         exprtk_define_unary_function(d2r  )
         exprtk_define_unary_function(d2g  )
         exprtk_define_unary_function(g2d  )
         exprtk_define_unary_function(notl )
         exprtk_define_unary_function(sgn  )
         exprtk_define_unary_function(erf  )
         exprtk_define_unary_function(erfc )
         exprtk_define_unary_function(frac )
         exprtk_define_unary_function(trunc)
         #undef exprtk_define_unary_function
      }

      template <typename Iterator, typename Type>
      static inline bool string_to_type_converter_impl_ref(Iterator& itr, const Iterator end, Type& result)
      {
         if (end == itr) return false;

         Type t = 0;
         bool negative = false;

         if ('+' == (*itr))
            ++itr;
         else if ('-' == (*itr))
         {
            ++itr;
            negative = true;
         }

         if (end == itr)
            return false;

         unsigned int digit_count = 0;
         while ((end != itr) && ('0' == (*itr))) ++itr;

         bool return_result = true;

         while (end != itr)
         {
            const unsigned char digit = (*itr - '0');
            if (digit > 9)
            {
               return_result = false;
               break;
            }
            if ((++digit_count) <= numeric::numeric_info<Type>::bound_length)
            {
               t *= 10;
               t += digit;
            }
            else
            {
               typedef unsigned long long int base_type;
               static const base_type max_limit = +std::numeric_limits<Type>::max();
               static const base_type min_limit = -std::numeric_limits<Type>::min();
               base_type tmp = static_cast<base_type>(t) * 10 + digit;
               if (negative && static_cast<base_type>(tmp) > min_limit)
                  return_result = false;
               else if (static_cast<base_type>(tmp) > max_limit)
                  return_result = false;
               t = static_cast<Type>(tmp);
            }
            ++itr;
         }
         result = static_cast<Type>((negative) ? -t : t);
         return return_result;
      }

      template <typename Iterator, typename T>
      static inline bool parse_nan(Iterator& itr, const Iterator end, T& t)
      {
         typedef typename std::iterator_traits<Iterator>::value_type type;
         static const std::size_t nan_length = 3;
         if (std::distance(itr,end) != static_cast<int>(nan_length))
            return false;
         if (static_cast<type>('n') == (*itr))
         {
            if ((static_cast<type>('a') != *(itr + 1)) || (static_cast<type>('n') != *(itr + 2)))
            {
               return false;
            }
         }
         else if ((static_cast<type>('A') != *(itr + 1)) || (static_cast<type>('N') != *(itr + 2)))
         {
            return false;
         }
         t = std::numeric_limits<T>::quiet_NaN();
         return true;
      }

      template <typename Iterator, typename T>
      static inline bool parse_inf(Iterator& itr, const Iterator end, T& t, bool negative)
      {
         static const char inf_uc[] = "INFINITY";
         static const char inf_lc[] = "infinity";
         static const std::size_t inf_length = 8;
         const std::size_t length = std::distance(itr,end);
         if ((3 != length) && (inf_length != length))
            return false;
         const char* inf_itr = ('i' == (*itr)) ? inf_lc : inf_uc;
         while (end != itr)
         {
            if (*inf_itr == static_cast<char>(*itr))
            {
               ++itr;
               ++inf_itr;
               continue;
            }
            else
               return false;
         }
         if (negative)
            t = -std::numeric_limits<T>::infinity();
         else
            t =  std::numeric_limits<T>::infinity();
         return true;
      }

      template <typename Iterator, typename T>
      inline bool string_to_real(Iterator& itr_external, const Iterator end, T& t)
      {
         if (end == itr_external)
            return false;

         Iterator itr = itr_external;
         double d = 0.0;
         bool negative = false;

         if ('+' == (*itr))
            ++itr;
         else if ('-' == (*itr))
         {
            ++itr;
            negative = true;
         }

         if (end == itr)
            return false;

         if (('I' <= (*itr)) && ((*itr) <= 'n'))
         {
            if (('i' == (*itr)) || ('I' == (*itr)))
            {
               return parse_inf(itr,end,t,negative);
            }
            else if (('n' == (*itr)) || ('N' == (*itr)))
            {
               return parse_nan(itr,end,t);
            }
            else
               return false;
         }

         bool instate = false;
         int pre_decimal = 0;

         if ('.' != (*itr))
         {
            const Iterator curr = itr;
            while ((end != itr) && ('0' == (*itr))) ++itr;
            const Iterator post_zero_cull_itr = itr;
            unsigned char digit = 0;

            #define parse_digit_1 \
            if ((digit = static_cast<unsigned char>((*itr) - '0')) < 10) { d *= 10.0; d += digit; } else break; if (end == ++itr) break; \

            #define parse_digit_2 \
            if ((digit = static_cast<unsigned char>((*itr) - '0')) < 10) { d *= 10.0; d += digit; } else break; ++itr;\

            while (end != itr)
            {
               parse_digit_1
               parse_digit_1
               parse_digit_1
               parse_digit_1
               parse_digit_1
               parse_digit_1
               parse_digit_1
               parse_digit_2
            }
            #undef parse_digit_1
            #undef parse_digit_2
            if (curr != itr) instate = true;
            pre_decimal = static_cast<int>(std::distance(post_zero_cull_itr,itr));
         }

         int exponent = 0;

         if (end != itr)
         {
            if ('.' == (*itr))
            {
               ++itr;
               const Iterator curr = itr;
               unsigned char digit = 0;

               #define parse_digit_1 \
               if ((digit = static_cast<unsigned char>((*itr) - '0')) < 10) { d *= 10.0; d += digit; } else break; if (end == ++itr) break; \

               #define parse_digit_2 \
               if ((digit = static_cast<unsigned char>((*itr) - '0')) < 10) { d *= 10.0; d += digit; } else break; ++itr;\

               while (end != itr)
               {
                  parse_digit_1
                  parse_digit_1
                  parse_digit_1
                  parse_digit_1
                  parse_digit_1
                  parse_digit_1
                  parse_digit_1
                  parse_digit_2
               }
               #undef parse_digit_1
               #undef parse_digit_2
               if (curr != itr) instate = true;
               exponent -= static_cast<int>(std::distance(curr,itr));
            }

            if (end != itr)
            {
               typename std::iterator_traits<Iterator>::value_type c = (*itr);

               if (('e' == c) || ('E' == c))
               {
                  ++itr;
                  int exp = 0;
                  if (!string_to_type_converter_impl_ref(itr,end,exp))
                  {
                     if (end == itr)
                        return false;
                     else
                        c = (*itr);
                  }

                  if (
                       (exp < numeric::numeric_info<T>::min_exp) ||
                       (numeric::numeric_info<T>::max_exp < exp)
                     )
                     return false;
                  exponent += exp;
               }

               if (('f' == c) || ('F' == c) || ('l' == c) || ('L' == c))
                  ++itr;
               else if ('#' == c)
               {
                  ++itr;
                  if (end == itr)
                     return false;
                  if ((10.0 != d) || (exponent != -1))
                     return false;
                  if (('I' <= (*itr)) && ((*itr) <= 'n'))
                  {
                     if (('i' == (*itr)) || ('I' == (*itr)))
                     {
                        return parse_inf(itr,end,t,negative);
                     }
                     else if (('n' == (*itr)) || ('N' == (*itr)))
                     {
                        return parse_nan(itr,end,t);
                     }
                     else
                        return false;
                  }
                  return false;
               }
            }
         }

         if ((end != itr) || (!instate))
            return false;
         if (0 != exponent)
         {
            if (
                (std::numeric_limits<T>::max_exponent10 < (exponent + pre_decimal)) ||
                (std::numeric_limits<T>::min_exponent10 > (exponent + pre_decimal))
               )
            {
               return false;
            }

            const int e = std::abs(exponent);

            static const double fract10[] =
                        {
                           0.0,
                           1.0E+001, 1.0E+002, 1.0E+003, 1.0E+004, 1.0E+005, 1.0E+006, 1.0E+007, 1.0E+008, 1.0E+009, 1.0E+010,
                           1.0E+011, 1.0E+012, 1.0E+013, 1.0E+014, 1.0E+015, 1.0E+016, 1.0E+017, 1.0E+018, 1.0E+019, 1.0E+020,
                           1.0E+021, 1.0E+022, 1.0E+023, 1.0E+024, 1.0E+025, 1.0E+026, 1.0E+027, 1.0E+028, 1.0E+029, 1.0E+030,
                           1.0E+031, 1.0E+032, 1.0E+033, 1.0E+034, 1.0E+035, 1.0E+036, 1.0E+037, 1.0E+038, 1.0E+039, 1.0E+040,
                           1.0E+041, 1.0E+042, 1.0E+043, 1.0E+044, 1.0E+045, 1.0E+046, 1.0E+047, 1.0E+048, 1.0E+049, 1.0E+050,
                           1.0E+051, 1.0E+052, 1.0E+053, 1.0E+054, 1.0E+055, 1.0E+056, 1.0E+057, 1.0E+058, 1.0E+059, 1.0E+060,
                           1.0E+061, 1.0E+062, 1.0E+063, 1.0E+064, 1.0E+065, 1.0E+066, 1.0E+067, 1.0E+068, 1.0E+069, 1.0E+070,
                           1.0E+071, 1.0E+072, 1.0E+073, 1.0E+074, 1.0E+075, 1.0E+076, 1.0E+077, 1.0E+078, 1.0E+079, 1.0E+080,
                           1.0E+081, 1.0E+082, 1.0E+083, 1.0E+084, 1.0E+085, 1.0E+086, 1.0E+087, 1.0E+088, 1.0E+089, 1.0E+090,
                           1.0E+091, 1.0E+092, 1.0E+093, 1.0E+094, 1.0E+095, 1.0E+096, 1.0E+097, 1.0E+098, 1.0E+099, 1.0E+100,
                           1.0E+101, 1.0E+102, 1.0E+103, 1.0E+104, 1.0E+105, 1.0E+106, 1.0E+107, 1.0E+108, 1.0E+109, 1.0E+110,
                           1.0E+111, 1.0E+112, 1.0E+113, 1.0E+114, 1.0E+115, 1.0E+116, 1.0E+117, 1.0E+118, 1.0E+119, 1.0E+120,
                           1.0E+121, 1.0E+122, 1.0E+123, 1.0E+124, 1.0E+125, 1.0E+126, 1.0E+127, 1.0E+128, 1.0E+129, 1.0E+130,
                           1.0E+131, 1.0E+132, 1.0E+133, 1.0E+134, 1.0E+135, 1.0E+136, 1.0E+137, 1.0E+138, 1.0E+139, 1.0E+140,
                           1.0E+141, 1.0E+142, 1.0E+143, 1.0E+144, 1.0E+145, 1.0E+146, 1.0E+147, 1.0E+148, 1.0E+149, 1.0E+150,
                           1.0E+151, 1.0E+152, 1.0E+153, 1.0E+154, 1.0E+155, 1.0E+156, 1.0E+157, 1.0E+158, 1.0E+159, 1.0E+160,
                           1.0E+161, 1.0E+162, 1.0E+163, 1.0E+164, 1.0E+165, 1.0E+166, 1.0E+167, 1.0E+168, 1.0E+169, 1.0E+170,
                           1.0E+171, 1.0E+172, 1.0E+173, 1.0E+174, 1.0E+175, 1.0E+176, 1.0E+177, 1.0E+178, 1.0E+179, 1.0E+180,
                           1.0E+181, 1.0E+182, 1.0E+183, 1.0E+184, 1.0E+185, 1.0E+186, 1.0E+187, 1.0E+188, 1.0E+189, 1.0E+190,
                           1.0E+191, 1.0E+192, 1.0E+193, 1.0E+194, 1.0E+195, 1.0E+196, 1.0E+197, 1.0E+198, 1.0E+199, 1.0E+200,
                           1.0E+221, 1.0E+222, 1.0E+223, 1.0E+224, 1.0E+225, 1.0E+226, 1.0E+227, 1.0E+228, 1.0E+229, 1.0E+230,
                           1.0E+231, 1.0E+232, 1.0E+233, 1.0E+234, 1.0E+235, 1.0E+236, 1.0E+237, 1.0E+238, 1.0E+239, 1.0E+240,
                           1.0E+241, 1.0E+242, 1.0E+243, 1.0E+244, 1.0E+245, 1.0E+246, 1.0E+247, 1.0E+248, 1.0E+249, 1.0E+250,
                           1.0E+251, 1.0E+252, 1.0E+253, 1.0E+254, 1.0E+255, 1.0E+256, 1.0E+257, 1.0E+258, 1.0E+259, 1.0E+260,
                           1.0E+261, 1.0E+262, 1.0E+263, 1.0E+264, 1.0E+265, 1.0E+266, 1.0E+267, 1.0E+268, 1.0E+269, 1.0E+270,
                           1.0E+271, 1.0E+272, 1.0E+273, 1.0E+274, 1.0E+275, 1.0E+276, 1.0E+277, 1.0E+278, 1.0E+279, 1.0E+280,
                           1.0E+281, 1.0E+282, 1.0E+283, 1.0E+284, 1.0E+285, 1.0E+286, 1.0E+287, 1.0E+288, 1.0E+289, 1.0E+290,
                           1.0E+291, 1.0E+292, 1.0E+293, 1.0E+294, 1.0E+295, 1.0E+296, 1.0E+297, 1.0E+298, 1.0E+299, 1.0E+300,
                           1.0E+301, 1.0E+302, 1.0E+303, 1.0E+304, 1.0E+305, 1.0E+306, 1.0E+307, 1.0E+308
                        };

            static const std::size_t fract10_size = sizeof(fract10) / sizeof(double);

            if (d != 0.0)
            {
               if (static_cast<std::size_t>(e) < fract10_size)
               {
                  if (exponent > 0)
                     d *= fract10[e];
                  else
                     d /= fract10[e];
               }
               else
                  d *= std::pow(10.0, 1.0 * exponent);
            }
         }
         t = static_cast<T>((negative) ? -d : d);
         return true;
      }

      template <typename T>
      inline bool string_to_real(const std::string& s, T& t)
      {
         const char* begin = s.data();
         const char* end   = s.data() + s.size();
         return string_to_real(begin,end,t);
      }

      template <typename T>
      struct functor_t
      {
         /*
            Note: The following definitions for Type, may require tweaking
                  based on the compiler and target architecture. The benchmark
                  should provide enough information to make the right choice.
         */
         //typedef T Type;
         //typedef const T Type;
         typedef const T& Type;
         typedef T (*qfunc_t)(Type t0, Type t1, Type t2, Type t3);
         typedef T (*tfunc_t)(Type t0, Type t1, Type t2);
         typedef T (*bfunc_t)(Type t0, Type t1);
         typedef T (*ufunc_t)(Type t0);
      };

   } // namespace details

   namespace lexer
   {
      struct token
      {

         enum token_type
         {
            e_none        =   0, e_error       =   1, e_err_symbol  =   2,
            e_err_number  =   3, e_err_string  =   4, e_err_sfunc   =   5,
            e_eof         =   6, e_number      =   7, e_symbol      =   8,
            e_string      =   9, e_assign      =  10, e_shr         =  11,
            e_shl         =  12, e_lte         =  13, e_ne          =  14,
            e_gte         =  15, e_lt          = '<', e_gt          = '>',
            e_eq          = '=', e_rbracket    = ')', e_lbracket    = '(',
            e_rsqrbracket = ']', e_lsqrbracket = '[', e_rcrlbracket = '}',
            e_lcrlbracket = '{', e_comma       = ',', e_add         = '+',
            e_sub         = '-', e_div         = '/', e_mul         = '*',
            e_mod         = '%', e_pow         = '^', e_colon       = ':'
         };

         token()
         : type(e_none),
           value(""),
           position(std::numeric_limits<std::size_t>::max())
         {}

         void clear()
         {
            type     = e_none;
            value    = "";
            position = std::numeric_limits<std::size_t>::max();
         }

         template <typename Iterator>
         inline token& set_operator(const token_type tt, const Iterator begin, const Iterator end, const Iterator base_begin = Iterator(0))
         {
            type = tt;
            value.assign(begin,end);
            if (base_begin)
               position = std::distance(base_begin,begin);
            return *this;
         }

         template <typename Iterator>
         inline token& set_symbol(const Iterator begin, const Iterator end, const Iterator base_begin = Iterator(0))
         {
            type = e_symbol;
            value.assign(begin,end);
            if (base_begin)
               position = std::distance(base_begin,begin);
            return *this;
         }

         template <typename Iterator>
         inline token& set_numeric(const Iterator begin, const Iterator end, const Iterator base_begin = Iterator(0))
         {
            type = e_number;
            value.assign(begin,end);
            if (base_begin)
               position = std::distance(base_begin,begin);
            return *this;
         }

         template <typename Iterator>
         inline token& set_string(const Iterator begin, const Iterator end, const Iterator base_begin = Iterator(0))
         {
            type = e_string;
            value.assign(begin,end);
            if (base_begin)
               position = std::distance(base_begin,begin);
            return *this;
         }

         inline token& set_string(const std::string& s, const std::size_t p)
         {
            type     = e_string;
            value    = s;
            position = p;
            return *this;
         }

         template <typename Iterator>
         inline token& set_error(const token_type et, const Iterator begin, const Iterator end, const Iterator base_begin = Iterator(0))
         {
            if (
                 (e_error      == et) ||
                 (e_err_symbol == et) ||
                 (e_err_number == et) ||
                 (e_err_string == et) ||
                 (e_err_sfunc  == et)
               )
            {
               type = et;
            }
            else
               type = e_error;
            value.assign(begin,end);
            if (base_begin)
               position = std::distance(base_begin,begin);
            return *this;
         }

         static inline std::string to_str(token_type t)
         {
            switch (t)
            {
               case e_none        : return "NONE";
               case e_error       : return "ERROR";
               case e_err_symbol  : return "ERROR_SYMBOL";
               case e_err_number  : return "ERROR_NUMBER";
               case e_err_string  : return "ERROR_STRING";
               case e_eof         : return "EOF";
               case e_number      : return "NUMBER";
               case e_symbol      : return "SYMBOL";
               case e_string      : return "STRING";
               case e_assign      : return ":=";
               case e_shr         : return ">>";
               case e_shl         : return "<<";
               case e_lte         : return "<=";
               case e_ne          : return "!=";
               case e_gte         : return ">=";
               case e_lt          : return "<";
               case e_gt          : return ">";
               case e_eq          : return "=";
               case e_rbracket    : return ")";
               case e_lbracket    : return "(";
               case e_rsqrbracket : return "]";
               case e_lsqrbracket : return "[";
               case e_rcrlbracket : return "}";
               case e_lcrlbracket : return "{";
               case e_comma       : return ",";
               case e_add         : return "+";
               case e_sub         : return "-";
               case e_div         : return "/";
               case e_mul         : return "*";
               case e_mod         : return "%";
               case e_pow         : return "^";
               case e_colon       : return ":";
               default            : return "UNKNOWN";
            }
         }

         inline bool is_error() const
         {
            return (
                     (e_error      == type) ||
                     (e_err_symbol == type) ||
                     (e_err_number == type) ||
                     (e_err_string == type) ||
                     (e_err_sfunc  == type)
                   );
         }

         token_type type;
         std::string value;
         std::size_t position;
      };

      class generator
      {
      public:

         typedef token token_t;
         typedef std::deque<token_t> token_list_t;
         typedef std::deque<token_t>::iterator token_list_itr_t;

         generator()
         : base_itr_(0),
           s_itr_(0),
           s_end_(0)
         {
            clear();
         }

         inline void clear()
         {
            base_itr_ = 0;
            s_itr_    = 0;
            s_end_    = 0;
            token_list_.clear();
            token_itr_ = token_list_.end();
            store_token_itr_ = token_list_.end();
         }

         inline bool process(const std::string& str)
         {
            base_itr_ = str.data();
            s_itr_    = str.data();
            s_end_    = str.data() + str.size();
            eof_token_.set_operator(token_t::e_eof,s_end_,s_end_,base_itr_);
            token_list_.clear();
            while (!is_end(s_itr_))
            {
               scan_token();
               if (token_list_.empty())
                  return true;
               else if (token_list_.back().is_error())
               {
                  return false;
               }
            }
            return true;
         }

         inline bool empty() const
         {
            return token_list_.empty();
         }

         inline std::size_t size() const
         {
            return token_list_.size();
         }

         inline void begin()
         {
            token_itr_ = token_list_.begin();
            store_token_itr_ = token_list_.begin();
         }

         inline void store()
         {
            store_token_itr_ = token_itr_;
         }

         inline void restore()
         {
            token_itr_ = store_token_itr_;
         }

         inline token_t& next_token()
         {
            if (token_list_.end() != token_itr_)
            {
               return *token_itr_++;
            }
            else
               return eof_token_;
         }

         inline token_t& peek_next_token()
         {
            if (token_list_.end() != token_itr_)
            {
               return *token_itr_;
            }
            else
               return eof_token_;
         }

         inline token_t& operator[](const std::size_t& index)
         {
            if (index < token_list_.size())
               return token_list_[index];
            else
               return eof_token_;
         }

         inline token_t operator[](const std::size_t& index) const
         {
            if (index < token_list_.size())
               return token_list_[index];
            else
               return eof_token_;
         }

         inline bool finished() const
         {
            return (token_list_.end() == token_itr_);
         }

      private:

         inline bool is_end(const char* itr)
         {
            return (s_end_ == itr);
         }

         inline void skip_whitespace()
         {
            while (!is_end(s_itr_) && details::is_whitespace(*s_itr_))
            {
               ++s_itr_;
            }
         }

         inline void skip_comments()
         {
            #ifndef exprtk_disable_comments
            //The following comment styles are supported:
            // 1. // .... \n
            // 2. #  .... \n
            // 3. /* .... */
            struct test
            {
               static inline bool comment_start(const char c0, const char c1, int& mode, int& incr)
               {
                  mode = 0;
                       if ('#' == c0)    { mode = 1; incr = 1; }
                  else if ('/' == c0)
                  {
                          if ('/' == c1) { mode = 1; incr = 2; }
                     else if ('*' == c1) { mode = 2; incr = 2; }
                  }
                  return (0 != mode);
               }

               static inline bool comment_end(const char c0, const char c1, const int mode)
               {
                  return ((1 == mode) && ('\n' == c0)) ||
                         ((2 == mode) && ( '*' == c0) && ('/' == c1));
               }
            };

            int mode = 0;
            int increment = 0;
            if (is_end(s_itr_) || is_end((s_itr_ + 1)))
               return;
            else if (!test::comment_start(*s_itr_,*(s_itr_ + 1),mode,increment))
               return;
            s_itr_ += increment;
            while (!is_end(s_itr_) && !test::comment_end(*s_itr_,*(s_itr_ + 1),mode))
            {
               ++s_itr_;
            }
            if (!is_end(s_itr_))
            {
               s_itr_ += mode;
               skip_whitespace();
               skip_comments();
            }
            #endif
         }

         inline void scan_token()
         {
            skip_whitespace();
            skip_comments();
            if (is_end(s_itr_))
            {
               return;
            }
            else if (details::is_operator_char(*s_itr_))
            {
               scan_operator();
               return;
            }
            else if (details::is_letter(*s_itr_))
            {
               scan_symbol();
               return;
            }
            else if (details::is_digit((*s_itr_)) || ('.' == (*s_itr_)))
            {
               scan_number();
               return;
            }
            else if ('$' == (*s_itr_))
            {
               scan_special_function();
               return;
            }
            #ifndef exprtk_disable_string_capabilities
            else if ('\'' == (*s_itr_))
            {
               scan_string();
               return;
            }
            #endif
            else if ('~' == (*s_itr_))
            {
               token_t t;
               t.set_symbol(s_itr_,s_itr_ + 1,base_itr_);
               token_list_.push_back(t);
               ++s_itr_;
               return;
            }
            else
            {
               token_t t;
               t.set_error(token::e_error,s_itr_,s_itr_ + 2,base_itr_);
               token_list_.push_back(t);
               ++s_itr_;
            }
         }

         inline void scan_operator()
         {
            token_t t;

            if (!is_end(s_itr_ + 1))
            {
               token_t::token_type ttype = token_t::e_none;
               char c0 = s_itr_[0];
               char c1 = s_itr_[1];
                    if ((c0 == '<') && (c1 == '=')) ttype = token_t::e_lte;
               else if ((c0 == '>') && (c1 == '=')) ttype = token_t::e_gte;
               else if ((c0 == '<') && (c1 == '>')) ttype = token_t::e_ne;
               else if ((c0 == '!') && (c1 == '=')) ttype = token_t::e_ne;
               else if ((c0 == '=') && (c1 == '=')) ttype = token_t::e_eq;
               else if ((c0 == ':') && (c1 == '=')) ttype = token_t::e_assign;
               else if ((c0 == '<') && (c1 == '<')) ttype = token_t::e_shl;
               else if ((c0 == '>') && (c1 == '>')) ttype = token_t::e_shr;

               if (token_t::e_none != ttype)
               {
                  t.set_operator(ttype,s_itr_,s_itr_ + 2,base_itr_);
                  token_list_.push_back(t);
                  s_itr_ += 2;
                  return;
               }
            }
            if ('<' == *s_itr_)
               t.set_operator(token_t::e_lt ,s_itr_,s_itr_ + 1,base_itr_);
            else if ('>' == *s_itr_)
               t.set_operator(token_t::e_gt ,s_itr_,s_itr_ + 1,base_itr_);
            else if (';' == *s_itr_)
               t.set_operator(token_t::e_eof,s_itr_,s_itr_ + 1,base_itr_);
            else if ('&' == *s_itr_)
               t.set_symbol(s_itr_,s_itr_ + 1,base_itr_);
            else if ('|' == *s_itr_)
               t.set_symbol(s_itr_,s_itr_ + 1,base_itr_);
            else
               t.set_operator(token_t::token_type(*s_itr_),s_itr_,s_itr_ + 1,base_itr_);
            token_list_.push_back(t);
            ++s_itr_;
         }

         inline void scan_symbol()
         {
            const char* begin = s_itr_;
            while (
                   (!is_end(s_itr_)) &&
                   (details::is_letter_or_digit(*s_itr_) || ((*s_itr_) == '_'))
                  )
            {
               ++s_itr_;
            }
            token_t t;
            t.set_symbol(begin,s_itr_,base_itr_);
            token_list_.push_back(t);
         }

         inline void scan_number()
         {
            /*
               Attempt to match a valid numeric value in one of the following formats:
               1. 123456
               2. 123.456
               3. 123.456e3
               4. 123.456E3
               5. 123.456e+3
               6. 123.456E+3
               7. 123.456e-3
               8. 123.456E-3
            */
            const char* begin      = s_itr_;
            bool dot_found         = false;
            bool e_found           = false;
            bool post_e_sign_found = false;
            token_t t;

            while (!is_end(s_itr_))
            {
               if ('.' == (*s_itr_))
               {
                  if (dot_found)
                  {
                     t.set_error(token::e_err_number,begin,s_itr_,base_itr_);
                     token_list_.push_back(t);
                     return;
                  }
                  dot_found = true;
                  ++s_itr_;
                  continue;
               }
               else if (details::imatch('e',(*s_itr_)))
               {
                  const char& c = *(s_itr_ + 1);

                  if (is_end(s_itr_ + 1))
                  {
                     t.set_error(token::e_err_number,begin,s_itr_,base_itr_);
                     token_list_.push_back(t);
                     return;
                  }
                  else if (
                           ('+' != c) &&
                           ('-' != c) &&
                           !details::is_digit(c)
                          )
                  {
                     t.set_error(token::e_err_number,begin,s_itr_,base_itr_);
                     token_list_.push_back(t);
                     return;
                  }

                  e_found = true;
                  ++s_itr_;
                  continue;
               }
               else if (e_found && details::is_sign(*s_itr_))
               {
                  if (post_e_sign_found)
                  {
                     t.set_error(token::e_err_number,begin,s_itr_,base_itr_);
                     token_list_.push_back(t);
                     return;
                  }

                  post_e_sign_found = true;
                  ++s_itr_;
                  continue;
               }
               else if (('.' != (*s_itr_)) && !details::is_digit(*s_itr_))
                  break;
               else
                  ++s_itr_;
            }

            t.set_numeric(begin,s_itr_,base_itr_);
            token_list_.push_back(t);
            return;
         }

         inline void scan_special_function()
         {
            const char* begin = s_itr_;
            token_t t;

            //$fdd(x,x,x) = at least 11 chars
            if (std::distance(s_itr_,s_end_) < 11)
            {
               t.set_error(token::e_err_sfunc,begin,s_itr_,base_itr_);
               token_list_.push_back(t);
               return;
            }

            if (
                !(('$' == *s_itr_)                       &&
                  (details::imatch  ('f',*(s_itr_ + 1))) &&
                  (details::is_digit(*(s_itr_ + 2)))     &&
                  (details::is_digit(*(s_itr_ + 3))))
               )
            {
               t.set_error(token::e_err_sfunc,begin,s_itr_,base_itr_);
               token_list_.push_back(t);
               return;
            }

            s_itr_ += 4; //$fdd = 4chars

            t.set_symbol(begin,s_itr_,base_itr_);
            token_list_.push_back(t);

            return;
         }

         #ifndef exprtk_disable_string_capabilities
         inline void scan_string()
         {
            const char* begin = s_itr_ + 1;
            token_t t;
            if (std::distance(s_itr_,s_end_) < 2)
            {
               t.set_error(token::e_err_string,begin,s_itr_,base_itr_);
               token_list_.push_back(t);
               return;
            }
            ++s_itr_;

            bool escaped_found = false;
            bool escaped = false;

            while (!is_end(s_itr_))
            {
               if ('\\' == *s_itr_)
               {
                  escaped_found = true;
                  escaped = true;
                  ++s_itr_;
                  continue;
               }
               else if (!escaped)
               {
                  if ('\'' == *s_itr_)
                     break;
               }
               else if (escaped)
                  escaped = false;
               ++s_itr_;
            }

            if (is_end(s_itr_))
            {
               t.set_error(token::e_err_string,begin,s_itr_,base_itr_);
               token_list_.push_back(t);
               return;
            }

            if (!escaped_found)
               t.set_string(begin,s_itr_,base_itr_);
            else
            {
               std::string parsed_string(begin,s_itr_);
               details::cleanup_escapes(parsed_string);
               t.set_string(parsed_string, std::distance(base_itr_,begin));
            }

            token_list_.push_back(t);
            ++s_itr_;
            return;
         }
         #endif

      private:

         token_list_t token_list_;
         token_list_itr_t token_itr_;
         token_list_itr_t store_token_itr_;
         token_t eof_token_;
         const char* base_itr_;
         const char* s_itr_;
         const char* s_end_;

         friend class token_scanner;
         friend class token_modifier;
         friend class token_inserter;
         friend class token_joiner;
      };

      class helper_interface
      {
      public:

         virtual void init()                     {              }
         virtual void reset()                    {              }
         virtual bool result()                   { return true; }
         virtual std::size_t process(generator&) { return 0;    }
         virtual ~helper_interface()             {              }
      };

      class token_scanner : public helper_interface
      {
      public:

         virtual ~token_scanner()
         {}

         explicit token_scanner(const std::size_t& stride)
         : stride_(stride)
         {
            if (stride > 4)
            {
               throw std::invalid_argument("token_scanner() - Invalid stride value");
            }
         }

         inline std::size_t process(generator& g)
         {
            if (!g.token_list_.empty())
            {
               for (std::size_t i = 0; i < (g.token_list_.size() - stride_ + 1); ++i)
               {
                  token t;
                  switch (stride_)
                  {
                     case 1 :
                              {
                                 const token& t0 = g.token_list_[i];
                                 if (!operator()(t0)) return i;
                              }
                              break;

                     case 2 :
                              {
                                 const token& t0 = g.token_list_[i    ];
                                 const token& t1 = g.token_list_[i + 1];
                                 if (!operator()(t0,t1)) return i;
                              }
                              break;

                     case 3 :
                              {
                                 const token& t0 = g.token_list_[i    ];
                                 const token& t1 = g.token_list_[i + 1];
                                 const token& t2 = g.token_list_[i + 2];
                                 if (!operator()(t0,t1,t2)) return i;
                              }
                              break;

                     case 4 :
                              {
                                 const token& t0 = g.token_list_[i    ];
                                 const token& t1 = g.token_list_[i + 1];
                                 const token& t2 = g.token_list_[i + 2];
                                 const token& t3 = g.token_list_[i + 3];
                                 if (!operator()(t0,t1,t2,t3)) return i;
                              }
                              break;
                  }
               }
            }

            return (g.token_list_.size() - stride_ + 1);
         }

         virtual bool operator()(const token&)
         {
            return false;
         }

         virtual bool operator()(const token&, const token&)
         {
            return false;
         }

         virtual bool operator()(const token&, const token&, const token&)
         {
            return false;
         }

         virtual bool operator()(const token&, const token&, const token&, const token&)
         {
            return false;
         }

      private:

         std::size_t stride_;
      };

      class token_modifier : public helper_interface
      {
      public:

         inline std::size_t process(generator& g)
         {
            std::size_t changes = 0;
            for (std::size_t i = 0; i < g.token_list_.size(); ++i)
            {
               if (modify(g.token_list_[i])) changes++;
            }

            return changes;
         }

         virtual bool modify(token& t) = 0;
      };

      class token_inserter : public helper_interface
      {
      public:

         explicit token_inserter(const std::size_t& stride)
         : stride_(stride)
         {
            if (stride > 5)
            {
               throw std::invalid_argument("token_inserter() - Invalid stride value");
            }
         }

         inline std::size_t process(generator& g)
         {
            if (g.token_list_.empty())
               return 0;
            std::size_t changes = 0;
            for (std::size_t i = 0; i < (g.token_list_.size() - stride_ + 1); ++i)
            {
               token t;
               int insert_index = -1;
               switch (stride_)
               {
                  case 1 : insert_index = insert(g.token_list_[i],t);
                           break;

                  case 2 : insert_index = insert(g.token_list_[i],g.token_list_[i + 1],t);
                           break;

                  case 3 : insert_index = insert(g.token_list_[i],g.token_list_[i + 1],g.token_list_[i + 2],t);
                           break;

                  case 4 : insert_index = insert(g.token_list_[i],g.token_list_[i + 1],g.token_list_[i + 2],g.token_list_[i + 3],t);
                           break;

                  case 5 : insert_index = insert(g.token_list_[i],g.token_list_[i + 1],g.token_list_[i + 2],g.token_list_[i + 3],g.token_list_[i + 4],t);
                           break;
               }

               if ((insert_index >= 0) && (insert_index <= (static_cast<int>(stride_) + 1)))
               {
                  g.token_list_.insert(g.token_list_.begin() + (i + insert_index),t);
                  changes++;
               }
            }

            return changes;
         }

         virtual inline int insert(const token&, token& )
         {
            return -1;
         }

         virtual inline int insert(const token&, const token&, token&)
         {
            return -1;
         }

         virtual inline int insert(const token&, const token&, const token&, token&)
         {
            return -1;
         }

         virtual inline int insert(const token&, const token&, const token&, const token&, token&)
         {
            return -1;
         }

         virtual inline int insert(const token&, const token&, const token&, const token&, const token&, token&)
         {
            return -1;
         }

      private:

         std::size_t stride_;
      };

      class token_joiner : public helper_interface
      {
      public:

         token_joiner(const std::size_t& stride)
         : stride_(stride)
         {}

         inline std::size_t process(generator& g)
         {
            if (g.token_list_.empty())
               return 0;
            switch (stride_)
            {
               case 2  : return process_stride_2(g);
               case 3  : return process_stride_3(g);
               default : return 0;
            }
         }

         virtual bool join(const token&, const token&, token&) { return false; }
         virtual bool join(const token&, const token&, const token&, token&) { return false; }

      private:

         inline std::size_t process_stride_2(generator& g)
         {
            if (g.token_list_.size() < 2)
               return 0;
            std::size_t changes = 0;
            for (std::size_t i = 0; i < g.token_list_.size() - 1; ++i)
            {
               token t;
               if (join(g.token_list_[i],g.token_list_[i + 1],t))
               {
                  g.token_list_[i] = t;
                  g.token_list_.erase(g.token_list_.begin() + (i + 1));
                  ++changes;
               }
            }

            return changes;
         }

         inline std::size_t process_stride_3(generator& g)
         {
            if (g.token_list_.size() < 3)
               return 0;
            std::size_t changes = 0;
            for (std::size_t i = 0; i < g.token_list_.size() - 2; ++i)
            {
               token t;
               if (join(g.token_list_[i],g.token_list_[i + 1],g.token_list_[i + 2],t))
               {
                  g.token_list_[i] = t;
                  g.token_list_.erase(g.token_list_.begin() + (i + 1),
                                      g.token_list_.begin() + (i + 3));
                  ++changes;
               }
            }

            return changes;
         }

         std::size_t stride_;
      };

      namespace helper
      {

         inline void dump(lexer::generator& generator)
         {
            for (std::size_t i = 0; i < generator.size(); ++i)
            {
               lexer::token t = generator[i];
               printf("Token[%02d] @ %03d  %6s  -->  '%s'\n",
                      static_cast<unsigned int>(i),
                      static_cast<unsigned int>(t.position),
                      t.to_str(t.type).c_str(),
                      t.value.c_str());
            }
         }

         class commutative_inserter : public lexer::token_inserter
         {
         public:

            commutative_inserter()
            : lexer::token_inserter(2)
            {}

            inline void ignore_symbol(const std::string& symbol)
            {
               ignore_set_.insert(symbol);
            }

            inline int insert(const lexer::token& t0, const lexer::token& t1, lexer::token& new_token)
            {
               new_token.type     = lexer::token::e_mul;
               new_token.value    = "*";
               new_token.position = t1.position;
               bool match         = false;

               if (t0.type == lexer::token::e_symbol)
               {
                  if (ignore_set_.end() != ignore_set_.find(t0.value))
                  {
                     return -1;
                  }
                  else if (!t0.value.empty() && ('$' == t0.value[0]))
                  {
                     return -1;
                  }
               }

               if (t1.type == lexer::token::e_symbol)
               {
                  if (ignore_set_.end() != ignore_set_.find(t1.value))
                  {
                     return -1;
                  }
               }
                    if ((t0.type == lexer::token::e_number     ) && (t1.type == lexer::token::e_symbol     )) match = true;
               else if ((t0.type == lexer::token::e_number     ) && (t1.type == lexer::token::e_lbracket   )) match = true;
               else if ((t0.type == lexer::token::e_number     ) && (t1.type == lexer::token::e_lcrlbracket)) match = true;
               else if ((t0.type == lexer::token::e_number     ) && (t1.type == lexer::token::e_lsqrbracket)) match = true;
               else if ((t0.type == lexer::token::e_symbol     ) && (t1.type == lexer::token::e_number     )) match = true;
               else if ((t0.type == lexer::token::e_rbracket   ) && (t1.type == lexer::token::e_number     )) match = true;
               else if ((t0.type == lexer::token::e_rcrlbracket) && (t1.type == lexer::token::e_number     )) match = true;
               else if ((t0.type == lexer::token::e_rsqrbracket) && (t1.type == lexer::token::e_number     )) match = true;
               else if ((t0.type == lexer::token::e_rbracket   ) && (t1.type == lexer::token::e_symbol     )) match = true;
               else if ((t0.type == lexer::token::e_rcrlbracket) && (t1.type == lexer::token::e_symbol     )) match = true;
               else if ((t0.type == lexer::token::e_rsqrbracket) && (t1.type == lexer::token::e_symbol     )) match = true;

               return (match) ? 1 : -1;
            }

         private:

            std::set<std::string,details::ilesscompare> ignore_set_;
         };

         class operator_joiner : public token_joiner
         {
         public:

            operator_joiner(const std::size_t& stride)
            : token_joiner(stride)
            {}

            inline bool join(const lexer::token& t0, const lexer::token& t1, lexer::token& t)
            {
               //': =' --> ':='
               if ((t0.type == lexer::token::e_colon) && (t1.type == lexer::token::e_eq))
               {
                  t.type = lexer::token::e_assign;
                  t.value = ":=";
                  t.position = t0.position;
                  return true;
               }
               //'> =' --> '>='
               else if ((t0.type == lexer::token::e_gt) && (t1.type == lexer::token::e_eq))
               {
                  t.type = lexer::token::e_gte;
                  t.value = ">=";
                  t.position = t0.position;
                  return true;
               }
               //'< =' --> '<='
               else if ((t0.type == lexer::token::e_lt) && (t1.type == lexer::token::e_eq))
               {
                  t.type = lexer::token::e_lte;
                  t.value = "<=";
                  t.position = t0.position;
                  return true;
               }
               //'= =' --> '=='
               else if ((t0.type == lexer::token::e_eq) && (t1.type == lexer::token::e_eq))
               {
                  t.type = lexer::token::e_eq;
                  t.value = "==";
                  t.position = t0.position;
                  return true;
               }
               //'! =' --> '!='
               else if ((static_cast<char>(t0.type) == '!') && (t1.type == lexer::token::e_eq))
               {
                  t.type = lexer::token::e_ne;
                  t.value = "!=";
                  t.position = t0.position;
                  return true;
               }
               //'< >' --> '<>'
               else if ((t0.type == lexer::token::e_lt) && (t1.type == lexer::token::e_gt))
               {
                  t.type = lexer::token::e_ne;
                  t.value = "<>";
                  t.position = t0.position;
                  return true;
               }
               else
                  return false;
            }

            inline bool join(const lexer::token& t0, const lexer::token& t1, const lexer::token& t2, lexer::token& t)
            {
               if (
                    (t0.type == lexer::token::e_lsqrbracket) &&
                    (t1.type == lexer::token::e_mul        ) &&
                    (t2.type == lexer::token::e_rsqrbracket)
                  )
               {
                  t.type = lexer::token::e_symbol;
                  t.value = "[*]";
                  t.position = t0.position;
                  return true;
               }
               else
                  return false;
            }
         };

         class bracket_checker : public lexer::token_scanner
         {
         public:

            bracket_checker()
            : token_scanner(1),
              state_(true)
            {}

            bool result()
            {
               if (!stack_.empty())
               {
                  lexer::token t;
                  t.value      = stack_.top().first;
                  t.position   = stack_.top().second;
                  error_token_ = t;
                  state_ = false;
                  return false;
               }
               else
                  return state_;
            }

            lexer::token error_token()
            {
               return error_token_;
            }

            void reset()
            {
               //why? because msvc doesn't support swap properly.
               stack_ = std::stack<std::pair<char,std::size_t> >();
               state_ = true;
               error_token_.clear();
            }

            bool operator()(const lexer::token& t)
            {
               if (!t.value.empty()                       &&
                   (lexer::token::e_string != t.type)     &&
                   (lexer::token::e_symbol != t.type)     &&
                   exprtk::details::is_bracket(t.value[0])
                  )
               {
                  char c = t.value[0];
                       if (t.type == lexer::token::e_lbracket)    stack_.push(std::make_pair(')',t.position));
                  else if (t.type == lexer::token::e_lcrlbracket) stack_.push(std::make_pair('}',t.position));
                  else if (t.type == lexer::token::e_lsqrbracket) stack_.push(std::make_pair(']',t.position));
                  else if (exprtk::details::is_right_bracket(c))
                  {
                     if (stack_.empty())
                     {
                        state_ = false;
                        error_token_ = t;
                        return false;
                     }
                     else if (c != stack_.top().first)
                     {
                        state_ = false;
                        error_token_ = t;
                        return false;
                     }
                     else
                        stack_.pop();
                  }
               }

               return true;
            }

         private:

            bool state_;
            std::stack<std::pair<char,std::size_t> > stack_;
            lexer::token error_token_;
         };

         class numeric_checker : public lexer::token_scanner
         {
         public:

            numeric_checker()
            : token_scanner(1),
              current_index_(0)
            {}

            bool result()
            {
               return error_list_.empty();
            }

            void reset()
            {
               error_list_.clear();
               current_index_ = 0;
            }

            bool operator()(const lexer::token& t)
            {
               if (token::e_number == t.type)
               {
                  double v;
                  if (!exprtk::details::string_to_real(t.value,v))
                  {
                     error_list_.push_back(current_index_);
                  }
               }
               ++current_index_;
               return true;
            }

            std::size_t error_count() const
            {
               return error_list_.size();
            }

            std::size_t error_index(const std::size_t& i)
            {
               if (i < error_list_.size())
                  return error_list_[i];
               else
                  return std::numeric_limits<std::size_t>::max();
            }

         private:

            std::size_t current_index_;
            std::deque<std::size_t> error_list_;
         };

         class symbol_replacer : public lexer::token_modifier
         {
         private:

            typedef std::map<std::string,std::pair<std::string,token::token_type>,details::ilesscompare> replace_map_t;

         public:

            bool remove(const std::string& target_symbol)
            {
               replace_map_t::iterator itr = replace_map_.find(target_symbol);
               if (replace_map_.end() == itr)
                  return false;
               replace_map_.erase(itr);
               return true;
            }

            bool add_replace(const std::string& target_symbol,
                             const std::string& replace_symbol,
                             const lexer::token::token_type token_type = lexer::token::e_symbol)
            {
               replace_map_t::iterator itr = replace_map_.find(target_symbol);
               if (replace_map_.end() != itr)
               {
                  return false;
               }
               replace_map_[target_symbol] = std::make_pair(replace_symbol,token_type);
               return true;
            }

            void clear()
            {
               replace_map_.clear();
            }

         private:

            bool modify(lexer::token& t)
            {
               if (lexer::token::e_symbol == t.type)
               {
                  if (replace_map_.empty())
                     return false;
                  replace_map_t::iterator itr = replace_map_.find(t.value);
                  if (replace_map_.end() != itr)
                  {
                     t.value = itr->second.first;
                     t.type  = itr->second.second;
                     return true;
                  }
               }

               return false;
            }

            replace_map_t replace_map_;
         };

         class sequence_validator : public lexer::token_scanner
         {
         private:

            typedef std::pair<lexer::token::token_type,lexer::token::token_type> token_pair_t;
            typedef std::set<token_pair_t> set_t;

         public:

            sequence_validator()
            : lexer::token_scanner(2)
            {
               add_invalid(lexer::token::e_number,lexer::token::e_number);
               add_invalid(lexer::token::e_string,lexer::token::e_string);
               add_invalid(lexer::token::e_number,lexer::token::e_string);
               add_invalid(lexer::token::e_string,lexer::token::e_number);
               add_invalid(lexer::token::e_string,lexer::token::e_colon);
               add_invalid(lexer::token::e_colon,lexer::token::e_string);
               add_invalid_set1(lexer::token::e_assign);
               add_invalid_set1(lexer::token::e_shr);
               add_invalid_set1(lexer::token::e_shl);
               add_invalid_set1(lexer::token::e_lte);
               add_invalid_set1(lexer::token::e_ne);
               add_invalid_set1(lexer::token::e_gte);
               add_invalid_set1(lexer::token::e_lt);
               add_invalid_set1(lexer::token::e_gt);
               add_invalid_set1(lexer::token::e_eq);
               add_invalid_set1(lexer::token::e_comma);
               add_invalid_set1(lexer::token::e_add);
               add_invalid_set1(lexer::token::e_sub);
               add_invalid_set1(lexer::token::e_div);
               add_invalid_set1(lexer::token::e_mul);
               add_invalid_set1(lexer::token::e_mod);
               add_invalid_set1(lexer::token::e_pow);
               add_invalid_set1(lexer::token::e_colon);
            }

            bool result()
            {
               return error_list_.empty();
            }

            bool operator()(const lexer::token& t0, const lexer::token& t1)
            {
               set_t::value_type p = std::make_pair(t0.type,t1.type);
               if (invalid_bracket_check(t0.type,t1.type))
               {
                  error_list_.push_back(std::make_pair(t0,t1));
               }
               else if (invalid_comb_.find(p) != invalid_comb_.end())
                  error_list_.push_back(std::make_pair(t0,t1));
               return true;
            }

            std::size_t error_count()
            {
               return error_list_.size();
            }

            std::pair<lexer::token,lexer::token> error(const std::size_t index)
            {
               if (index < error_list_.size())
               {
                  return error_list_[index];
               }
               else
               {
                  static const lexer::token error_token;
                  return std::make_pair(error_token,error_token);
               }
            }

            void clear_errors()
            {
               error_list_.clear();
            }

         private:

            void add_invalid(lexer::token::token_type base, lexer::token::token_type t)
            {
               invalid_comb_.insert(std::make_pair(base,t));
            }

            void add_invalid_set1(lexer::token::token_type t)
            {
               add_invalid(t,lexer::token::e_assign);
               add_invalid(t,lexer::token::e_shr);
               add_invalid(t,lexer::token::e_shl);
               add_invalid(t,lexer::token::e_lte);
               add_invalid(t,lexer::token::e_ne);
               add_invalid(t,lexer::token::e_gte);
               add_invalid(t,lexer::token::e_lt);
               add_invalid(t,lexer::token::e_gt);
               add_invalid(t,lexer::token::e_eq);
               add_invalid(t,lexer::token::e_comma);
               add_invalid(t,lexer::token::e_div);
               add_invalid(t,lexer::token::e_mul);
               add_invalid(t,lexer::token::e_mod);
               add_invalid(t,lexer::token::e_pow);
               add_invalid(t,lexer::token::e_colon);
            }

            bool invalid_bracket_check(lexer::token::token_type base, lexer::token::token_type t)
            {
               if (details::is_right_bracket(static_cast<char>(base)))
               {
                  switch (t)
                  {
                     case lexer::token::e_string : return true;
                     case lexer::token::e_assign : return true;
                     default                     : return false;
                  }
               }
               else if (details::is_left_bracket(static_cast<char>(base)))
               {
                  if (details::is_right_bracket(static_cast<char>(t)))
                     return false;
                  else if (details::is_left_bracket(static_cast<char>(t)))
                     return false;
                  else
                  {
                     switch (t)
                     {
                        case lexer::token::e_number : return false;
                        case lexer::token::e_symbol : return false;
                        case lexer::token::e_string : return false;
                        case lexer::token::e_add    : return false;
                        case lexer::token::e_sub    : return false;
                        case lexer::token::e_colon  : return false;
                        default                     : return true;
                     }
                  }
               }
               else if (details::is_right_bracket(static_cast<char>(t)))
               {
                  switch (base)
                  {
                     case lexer::token::e_number : return false;
                     case lexer::token::e_symbol : return false;
                     case lexer::token::e_string : return false;
                     case lexer::token::e_eof    : return false;
                     case lexer::token::e_colon  : return false;
                     default                     : return true;
                  }
               }
               else if (details::is_left_bracket(static_cast<char>(t)))
               {
                  switch (base)
                  {
                     case lexer::token::e_rbracket    : return true;
                     case lexer::token::e_rsqrbracket : return true;
                     case lexer::token::e_rcrlbracket : return true;
                     default                          : return false;
                  }
               }

               return false;
            }

            set_t invalid_comb_;
            std::deque<std::pair<lexer::token,lexer::token> > error_list_;

         };

         struct helper_assembly
         {
            inline bool register_scanner(lexer::token_scanner* scanner)
            {
               if (token_scanner_list.end() != std::find(token_scanner_list.begin(),
                                                         token_scanner_list.end(),
                                                         scanner))
               {
                  return false;
               }

               token_scanner_list.push_back(scanner);
               return true;
            }

            inline bool register_modifier(lexer::token_modifier* modifier)
            {
               if (token_modifier_list.end() != std::find(token_modifier_list.begin(),
                                                          token_modifier_list.end(),
                                                          modifier))
               {
                  return false;
               }

               token_modifier_list.push_back(modifier);
               return true;
            }

            inline bool register_joiner(lexer::token_joiner* joiner)
            {
               if (token_joiner_list.end() != std::find(token_joiner_list.begin(),
                                                        token_joiner_list.end(),
                                                        joiner))
               {
                  return false;
               }
               token_joiner_list.push_back(joiner);
               return true;
            }

            inline bool register_inserter(lexer::token_inserter* inserter)
            {
               if (token_inserter_list.end() != std::find(token_inserter_list.begin(),
                                                          token_inserter_list.end(),
                                                          inserter))
               {
                  return false;
               }
               token_inserter_list.push_back(inserter);
               return true;
            }

            inline bool run_modifiers(lexer::generator& g)
            {
               error_token_modifier = reinterpret_cast<lexer::token_modifier*>(0);
               bool result = true;

               for (std::size_t i = 0; i < token_modifier_list.size(); ++i)
               {
                  lexer::token_modifier& modifier = (*token_modifier_list[i]);
                  modifier.reset();
                  modifier.process(g);
                  if (!modifier.result())
                  {
                     error_token_modifier = token_modifier_list[i];
                     return false;
                  }
               }

               return result;
            }

            inline bool run_joiners(lexer::generator& g)
            {
               error_token_joiner = reinterpret_cast<lexer::token_joiner*>(0);
               bool result = true;

               for (std::size_t i = 0; i < token_joiner_list.size(); ++i)
               {
                  lexer::token_joiner& joiner = (*token_joiner_list[i]);
                  joiner.reset();
                  joiner.process(g);
                  if (!joiner.result())
                  {
                     error_token_joiner = token_joiner_list[i];
                     return false;
                  }
               }

               return result;
            }

            inline bool run_inserters(lexer::generator& g)
            {
               error_token_inserter = reinterpret_cast<lexer::token_inserter*>(0);
               bool result = true;

               for (std::size_t i = 0; i < token_inserter_list.size(); ++i)
               {
                  lexer::token_inserter& inserter = (*token_inserter_list[i]);
                  inserter.reset();
                  inserter.process(g);
                  if (!inserter.result())
                  {
                     error_token_inserter = token_inserter_list[i];
                     return false;
                  }
               }

               return result;
            }

            inline bool run_scanners(lexer::generator& g)
            {
               error_token_scanner = reinterpret_cast<lexer::token_scanner*>(0);
               bool result = true;

               for (std::size_t i = 0; i < token_scanner_list.size(); ++i)
               {
                  lexer::token_scanner& scanner = (*token_scanner_list[i]);
                  scanner.reset();
                  scanner.process(g);
                  if (!scanner.result())
                  {
                     error_token_scanner = token_scanner_list[i];
                     return false;
                  }
               }

               return result;
            }

            std::deque<lexer::token_scanner*>  token_scanner_list;
            std::deque<lexer::token_modifier*> token_modifier_list;
            std::deque<lexer::token_joiner*>   token_joiner_list;
            std::deque<lexer::token_inserter*> token_inserter_list;

            lexer::token_scanner*  error_token_scanner;
            lexer::token_modifier* error_token_modifier;
            lexer::token_joiner*   error_token_joiner;
            lexer::token_inserter* error_token_inserter;
         };
      }
   }

   namespace details
   {
      enum operator_type
      {
         e_default , e_null    , e_add     , e_sub     ,
         e_mul     , e_div     , e_mod     , e_pow     ,
         e_atan2   , e_min     , e_max     , e_avg     ,
         e_sum     , e_prod    , e_lt      , e_lte     ,
         e_eq      , e_equal   , e_ne      , e_nequal  ,
         e_gte     , e_gt      , e_and     , e_nand    ,
         e_or      , e_nor     , e_xor     , e_xnor    ,
         e_mand    , e_mor     , e_scand   , e_scor    ,
         e_shr     , e_shl     , e_abs     , e_acos    ,
         e_acosh   , e_asin    , e_asinh   , e_atan    ,
         e_atanh   , e_ceil    , e_cos     , e_cosh    ,
         e_exp     , e_expm1   , e_floor   , e_log     ,
         e_log10   , e_log2    , e_log1p   , e_logn    ,
         e_neg     , e_pos     , e_round   , e_roundn  ,
         e_root    , e_sqrt    , e_sin     , e_sinh    ,
         e_sec     , e_csc     , e_tan     , e_tanh    ,
         e_cot     , e_clamp   , e_iclamp  , e_inrange ,
         e_sgn     , e_r2d     , e_d2r     , e_d2g     ,
         e_g2d     , e_hypot   , e_notl    , e_erf     ,
         e_erfc    , e_frac    , e_trunc   , e_assign  ,
         e_in      , e_like    , e_ilike   , e_multi   ,

         // Do not add new functions/operators after this point.
         e_sf00 = 1000, e_sf01 = 1001, e_sf02 = 1002, e_sf03 = 1003,
         e_sf04 = 1004, e_sf05 = 1005, e_sf06 = 1006, e_sf07 = 1007,
         e_sf08 = 1008, e_sf09 = 1009, e_sf10 = 1010, e_sf11 = 1011,
         e_sf12 = 1012, e_sf13 = 1013, e_sf14 = 1014, e_sf15 = 1015,
         e_sf16 = 1016, e_sf17 = 1017, e_sf18 = 1018, e_sf19 = 1019,
         e_sf20 = 1020, e_sf21 = 1021, e_sf22 = 1022, e_sf23 = 1023,
         e_sf24 = 1024, e_sf25 = 1025, e_sf26 = 1026, e_sf27 = 1027,
         e_sf28 = 1028, e_sf29 = 1029, e_sf30 = 1030, e_sf31 = 1031,
         e_sf32 = 1032, e_sf33 = 1033, e_sf34 = 1034, e_sf35 = 1035,
         e_sf36 = 1036, e_sf37 = 1037, e_sf38 = 1038, e_sf39 = 1039,
         e_sf40 = 1040, e_sf41 = 1041, e_sf42 = 1042, e_sf43 = 1043,
         e_sf44 = 1044, e_sf45 = 1045, e_sf46 = 1046, e_sf47 = 1047,
         e_sf48 = 1048, e_sf49 = 1049, e_sf50 = 1050, e_sf51 = 1051,
         e_sf52 = 1052, e_sf53 = 1053, e_sf54 = 1054, e_sf55 = 1055,
         e_sf56 = 1056, e_sf57 = 1057, e_sf58 = 1058, e_sf59 = 1059,
         e_sf60 = 1060, e_sf61 = 1061, e_sf62 = 1062, e_sf63 = 1063,
         e_sf64 = 1064, e_sf65 = 1065, e_sf66 = 1066, e_sf67 = 1067,
         e_sf68 = 1068, e_sf69 = 1069, e_sf70 = 1070, e_sf71 = 1071,
         e_sf72 = 1072, e_sf73 = 1073, e_sf74 = 1074, e_sf75 = 1075,
         e_sf76 = 1076, e_sf77 = 1077, e_sf78 = 1078, e_sf79 = 1079,
         e_sf80 = 1080, e_sf81 = 1081, e_sf82 = 1082, e_sf83 = 1083,
         e_sf84 = 1084, e_sf85 = 1085, e_sf86 = 1086, e_sf87 = 1087,
         e_sf88 = 1088, e_sf89 = 1089, e_sf90 = 1090, e_sf91 = 1091,
         e_sf92 = 1092, e_sf93 = 1093, e_sf94 = 1094, e_sf95 = 1095,
         e_sf96 = 1096, e_sf97 = 1097, e_sf98 = 1098, e_sf99 = 1099,
         e_sffinal = 1100,
         e_sf4ext00 = 2000, e_sf4ext01 = 2001, e_sf4ext02 = 2002, e_sf4ext03 = 2003,
         e_sf4ext04 = 2004, e_sf4ext05 = 2005, e_sf4ext06 = 2006, e_sf4ext07 = 2007,
         e_sf4ext08 = 2008, e_sf4ext09 = 2009, e_sf4ext10 = 2010, e_sf4ext11 = 2011,
         e_sf4ext12 = 2012, e_sf4ext13 = 2013, e_sf4ext14 = 2014, e_sf4ext15 = 2015,
         e_sf4ext16 = 2016, e_sf4ext17 = 2017, e_sf4ext18 = 2018, e_sf4ext19 = 2019,
         e_sf4ext20 = 2020, e_sf4ext21 = 2021, e_sf4ext22 = 2022, e_sf4ext23 = 2023,
         e_sf4ext24 = 2024, e_sf4ext25 = 2025, e_sf4ext26 = 2026, e_sf4ext27 = 2027,
         e_sf4ext28 = 2028, e_sf4ext29 = 2029, e_sf4ext30 = 2030, e_sf4ext31 = 2031,
         e_sf4ext32 = 2032, e_sf4ext33 = 2033, e_sf4ext34 = 2034, e_sf4ext35 = 2035,
         e_sf4ext36 = 2036, e_sf4ext37 = 2037, e_sf4ext38 = 2038, e_sf4ext39 = 2039,
         e_sf4ext40 = 2040, e_sf4ext41 = 2041, e_sf4ext42 = 2042, e_sf4ext43 = 2043,
         e_sf4ext44 = 2044, e_sf4ext45 = 2045
      };

      struct base_operation_t
      {
         base_operation_t(const operator_type t, const unsigned int& np)
         : type(t),
           num_params(np)
         {}

         operator_type type;
         unsigned int num_params;
      };

      namespace numeric
      {
         namespace details
         {
            template <typename T>
            inline T process_impl(const operator_type operation, const T& arg, real_type_tag)
            {
               switch (operation)
               {
                  case e_abs   : return numeric::abs  (arg);
                  case e_acos  : return numeric::acos (arg);
                  case e_acosh : return numeric::acosh(arg);
                  case e_asin  : return numeric::asin (arg);
                  case e_asinh : return numeric::asinh(arg);
                  case e_atan  : return numeric::atan (arg);
                  case e_atanh : return numeric::atanh(arg);
                  case e_ceil  : return numeric::ceil (arg);
                  case e_cos   : return numeric::cos  (arg);
                  case e_cosh  : return numeric::cosh (arg);
                  case e_exp   : return numeric::exp  (arg);
                  case e_expm1 : return numeric::expm1(arg);
                  case e_floor : return numeric::floor(arg);
                  case e_log   : return numeric::log  (arg);
                  case e_log10 : return numeric::log10(arg);
                  case e_log2  : return numeric::log2 (arg);
                  case e_log1p : return numeric::log1p(arg);
                  case e_neg   : return numeric::neg  (arg);
                  case e_pos   : return numeric::pos  (arg);
                  case e_round : return numeric::round(arg);
                  case e_sin   : return numeric::sin  (arg);
                  case e_sinh  : return numeric::sinh (arg);
                  case e_sqrt  : return numeric::sqrt (arg);
                  case e_tan   : return numeric::tan  (arg);
                  case e_tanh  : return numeric::tanh (arg);
                  case e_cot   : return numeric::cot  (arg);
                  case e_sec   : return numeric::sec  (arg);
                  case e_csc   : return numeric::csc  (arg);
                  case e_r2d   : return numeric::r2d  (arg);
                  case e_d2r   : return numeric::d2r  (arg);
                  case e_d2g   : return numeric::d2g  (arg);
                  case e_g2d   : return numeric::g2d  (arg);
                  case e_notl  : return numeric::notl (arg);
                  case e_sgn   : return numeric::sgn  (arg);
                  case e_erf   : return numeric::erf  (arg);
                  case e_erfc  : return numeric::erfc (arg);
                  case e_frac  : return numeric::frac (arg);
                  case e_trunc : return numeric::trunc(arg);
                  default      : return std::numeric_limits<T>::quiet_NaN();
               }
            }

            template <typename T>
            inline T process_impl(const operator_type operation, const T& arg, int_type_tag)
            {
               switch (operation)
               {
                  case e_abs   : return numeric::abs  (arg);
                  case e_exp   : return numeric::exp  (arg);
                  case e_expm1 : return numeric::expm1(arg);
                  case e_log   : return numeric::log  (arg);
                  case e_log10 : return numeric::log10(arg);
                  case e_log2  : return numeric::log2 (arg);
                  case e_log1p : return numeric::log1p(arg);
                  case e_neg   : return numeric::neg  (arg);
                  case e_pos   : return numeric::pos  (arg);
                  case e_sqrt  : return numeric::sqrt (arg);
                  case e_notl  : return numeric::notl (arg);
                  case e_sgn   : return numeric::sgn  (arg);
                  default      : return std::numeric_limits<T>::quiet_NaN();
               }
            }

            template <typename T>
            inline T process_impl(const operator_type operation, const T& arg0, const T& arg1, real_type_tag)
            {
               switch (operation)
               {
                  case e_add    : return (arg0 + arg1);
                  case e_sub    : return (arg0 - arg1);
                  case e_mul    : return (arg0 * arg1);
                  case e_div    : return (arg0 / arg1);
                  case e_mod    : return modulus<T>(arg0,arg1);
                  case e_pow    : return pow<T>(arg0,arg1);
                  case e_atan2  : return atan2<T>(arg0,arg1);
                  case e_min    : return std::min<T>(arg0,arg1);
                  case e_max    : return std::max<T>(arg0,arg1);
                  case e_logn   : return logn<T>(arg0,arg1);
                  case e_lt     : return (arg0 <  arg1) ? T(1) : T(0);
                  case e_lte    : return (arg0 <= arg1) ? T(1) : T(0);
                  case e_eq     : return (arg0 == arg1) ? T(1) : T(0);
                  case e_ne     : return (arg0 != arg1) ? T(1) : T(0);
                  case e_gte    : return (arg0 >= arg1) ? T(1) : T(0);
                  case e_gt     : return (arg0 >  arg1) ? T(1) : T(0);
                  case e_and    : return and_opr<T> (arg0,arg1);
                  case e_nand   : return nand_opr<T>(arg0,arg1);
                  case e_or     : return or_opr<T>  (arg0,arg1);
                  case e_nor    : return nor_opr<T> (arg0,arg1);
                  case e_xor    : return xor_opr<T> (arg0,arg1);
                  case e_xnor   : return xnor_opr<T>(arg0,arg1);
                  case e_root   : return root<T>    (arg0,arg1);
                  case e_roundn : return roundn<T>  (arg0,arg1);
                  case e_equal  : return equal<T>   (arg0,arg1);
                  case e_nequal : return nequal<T>  (arg0,arg1);
                  case e_hypot  : return hypot<T>   (arg0,arg1);
                  case e_shr    : return shr<T>     (arg0,arg1);
                  case e_shl    : return shl<T>     (arg0,arg1);
                  default       : return std::numeric_limits<T>::quiet_NaN();
               }
            }

            template <typename T>
            inline T process_impl(const operator_type operation, const T& arg0, const T& arg1, int_type_tag)
            {
               switch (operation)
               {
                  case e_add    : return (arg0 + arg1);
                  case e_sub    : return (arg0 - arg1);
                  case e_mul    : return (arg0 * arg1);
                  case e_div    : return (arg0 / arg1);
                  case e_mod    : return arg0 % arg1;
                  case e_pow    : return pow<T>(arg0,arg1);
                  case e_min    : return std::min<T>(arg0,arg1);
                  case e_max    : return std::max<T>(arg0,arg1);
                  case e_logn   : return logn<T>(arg0,arg1);
                  case e_lt     : return (arg0 <  arg1) ? T(1) : T(0);
                  case e_lte    : return (arg0 <= arg1) ? T(1) : T(0);
                  case e_eq     : return (arg0 == arg1) ? T(1) : T(0);
                  case e_ne     : return (arg0 != arg1) ? T(1) : T(0);
                  case e_gte    : return (arg0 >= arg1) ? T(1) : T(0);
                  case e_gt     : return (arg0 >  arg1) ? T(1) : T(0);
                  case e_and    : return ((arg0 != T(0)) && (arg1 != T(0))) ? T(1) : T(0);
                  case e_nand   : return ((arg0 != T(0)) && (arg1 != T(0))) ? T(0) : T(1);
                  case e_or     : return ((arg0 != T(0)) || (arg1 != T(0))) ? T(1) : T(0);
                  case e_nor    : return ((arg0 != T(0)) || (arg1 != T(0))) ? T(0) : T(1);
                  case e_xor    : return arg0 ^ arg1;
                  case e_xnor   : return !(arg0 ^ arg1);
                  case e_root   : return root<T>(arg0,arg1);
                  case e_equal  : return arg0 == arg1;
                  case e_nequal : return arg0 != arg1;
                  case e_hypot  : return hypot<T>(arg0,arg1);
                  case e_shr    : return arg0 >> arg1;
                  case e_shl    : return arg0 << arg1;
                  default       : return std::numeric_limits<T>::quiet_NaN();
               }
            }
         }

         template <typename T>
         inline T process(const operator_type operation, const T& arg)
         {
            typename details::number_type<T>::type num_type;
            return details::process_impl<T>(operation,arg,num_type);
         }

         template <typename T>
         inline T process(const operator_type operation, const T& arg0, const T& arg1)
         {
            typename details::number_type<T>::type num_type;
            return details::process_impl<T>(operation,arg0,arg1,num_type);
         }
      }

      template <typename T>
      class expression_node
      {
      public:

         enum node_type
         {
            e_none         , e_null         , e_constant     , e_unary        ,
            e_binary       , e_binary_ext   , e_trinary      , e_quaternary   ,
            e_quinary      , e_senary       , e_vararg       , e_conditional  ,
            e_while        , e_repeat       , e_switch       , e_mswitch      ,
            e_variable     , e_stringvar    , e_stringconst  , e_stringvarrng ,
            e_cstringvarrng, e_function     , e_vafunction   , e_add          ,
            e_sub          , e_mul          , e_div          , e_mod          ,
            e_pow          , e_lt           , e_lte          , e_gt           ,
            e_gte          , e_eq           , e_ne           , e_and          ,
            e_nand         , e_or           , e_nor          , e_xor          ,
            e_xnor         , e_in           , e_like         , e_ilike        ,
            e_inranges     , e_ipow         , e_ipowinv      , e_abs          ,
            e_acos         , e_acosh        , e_asin         , e_asinh        ,
            e_atan         , e_atanh        , e_ceil         , e_cos          ,
            e_cosh         , e_exp          , e_expm1        , e_floor        ,
            e_log          , e_log10        , e_log2         , e_log1p        ,
            e_neg          , e_pos          , e_round        , e_sin          ,
            e_sinh         , e_sqrt         , e_tan          , e_tanh         ,
            e_cot          , e_sec          , e_csc          , e_r2d          ,
            e_d2r          , e_d2g          , e_g2d          , e_notl         ,
            e_sgn          , e_erf          , e_erfc         , e_frac         ,
            e_trunc        , e_uvouv        , e_vov          , e_cov          ,
            e_voc          , e_vob          , e_bov          , e_cob          ,
            e_boc          , e_vovov        , e_vovoc        , e_vocov        ,
            e_covov        , e_covoc        , e_vovovov      , e_vovovoc      ,
            e_vovocov      , e_vocovov      , e_covovov      , e_covocov      ,
            e_vocovoc      , e_covovoc      , e_vococov      , e_sf3ext       ,
            e_sf4ext
         };

         typedef T value_type;
         typedef expression_node<T>* expression_ptr;

         virtual ~expression_node()
         {}

         virtual inline T value() const
         {
            return std::numeric_limits<T>::quiet_NaN();
         }

         virtual inline bool result() const
         {
            return (T(0) != value());
         }

         virtual inline expression_node<T>* branch(const std::size_t& index = 0) const
         {
            return reinterpret_cast<expression_ptr>(index * 0);
         }

         virtual inline node_type type() const
         {
            return e_none;
         }
      };

      inline bool is_true(const double v)
      {
         return (0.0 != v);
      }

      inline bool is_true(const float v)
      {
         return (0.0f != v);
      }

      template <typename T>
      inline bool is_true(const expression_node<T>* node)
      {
         return (T(0) != node->value());
      }

      template <typename T>
      inline bool is_false(const expression_node<T>* node)
      {
         return (T(0) == node->value());
      }

      template <typename T>
      inline bool is_unary_node(const expression_node<T>* node)
      {
         return (details::expression_node<T>::e_unary == node->type());
      }

      template <typename T>
      inline bool is_binary_node(const expression_node<T>* node)
      {
         return (details::expression_node<T>::e_binary == node->type());
      }

      template <typename T>
      inline bool is_variable_node(const expression_node<T>* node)
      {
         return (details::expression_node<T>::e_variable == node->type());
      }

      template <typename T>
      inline bool is_constant_node(const expression_node<T>* node)
      {
         return (details::expression_node<T>::e_constant == node->type());
      }

      template <typename T>
      inline bool is_null_node(const expression_node<T>* node)
      {
         return (details::expression_node<T>::e_null == node->type());
      }

      template <typename T>
      inline bool is_function(const expression_node<T>* node)
      {
         return (details::expression_node<T>::e_function == node->type());
      }

      template <typename T>
      inline bool branch_deletable(expression_node<T>* node)
      {
         return !is_variable_node(node);
      }

      template <std::size_t N, typename T>
      inline bool all_nodes_valid(expression_node<T>* (&b)[N])
      {
         for (std::size_t i = 0; i < N; ++i)
         {
            if (0 == b[i]) return false;
         }
         return true;
      }

      template <typename T,
                typename Allocator,
                template <typename,typename> class Sequence>
      inline bool all_nodes_valid(const Sequence<expression_node<T>*,Allocator>& b)
      {
         for (std::size_t i = 0; i < b.size(); ++i)
         {
            if (0 == b[i]) return false;
         }
         return true;
      }

      template <std::size_t N, typename T>
      inline bool all_nodes_variables(expression_node<T>* (&b)[N])
      {
         for (std::size_t i = 0; i < N; ++i)
         {
            if (0 == b[i])
               return false;
            else if (!is_variable_node(b[i]))
               return false;
         }
         return true;
      }

      template <typename T,
                typename Allocator,
                template <typename,typename> class Sequence>
      inline bool all_nodes_variables(Sequence<expression_node<T>*,Allocator>& b)
      {
         for (std::size_t i = 0; i < b.size(); ++i)
         {
            if (0 == b[i])
               return false;
            else if (!is_variable_node(b[i]))
               return false;
         }
         return true;
      }

      template <typename NodeAllocator, typename T, std::size_t N>
      inline void free_all_nodes(NodeAllocator& node_allocator, expression_node<T>* (&b)[N])
      {
         for (std::size_t i = 0; i < N; ++i)
         {
            free_node(node_allocator,b[i]);
         }
      }

      template <typename NodeAllocator,
                typename T,
                typename Allocator,
                template <typename,typename> class Sequence>
      inline void free_all_nodes(NodeAllocator& node_allocator, Sequence<expression_node<T>*,Allocator>& b)
      {
         for (std::size_t i = 0; i < b.size(); ++i)
         {
            free_node(node_allocator,b[i]);
         }
         b.clear();
      }

      template <typename NodeAllocator, typename T>
      inline void free_node(NodeAllocator& node_allocator, expression_node<T>*& node)
      {
         if (0 != node)
         {
            if (is_variable_node(node))
               return;
            node_allocator.free(node);
            node = 0;
         }
      }

      template <typename T>
      class null_node : public expression_node<T>
      {
      public:

         inline T value() const
         {
            return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_null;
         }
      };

      template <typename T>
      class literal_node : public expression_node<T>
      {
      public:

         explicit literal_node(const T& value)
         : value_(value)
         {}

         inline T value() const
         {
            return value_;
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_constant;
         }

         inline expression_node<T>* branch(const std::size_t&) const
         {
            return reinterpret_cast<expression_node<T>*>(0);
         }

      private:

         literal_node(literal_node<T>&) {}
         literal_node<T>& operator=(literal_node<T>&) { return *this; }

         const T value_;
      };

      template <typename T>
      class string_base_node : public expression_node<T>
      {
         virtual std::string str() const = 0;
      };

      template <typename T>
      class string_literal_node : public string_base_node<T>
      {
      public:

         explicit string_literal_node(const std::string& value)
         : value_(value)
         {}

         inline T value() const
         {
            return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_stringconst;
         }

         inline expression_node<T>* branch(const std::size_t&) const
         {
            return reinterpret_cast<expression_node<T>*>(0);
         }

         std::string str() const
         {
            return value_;
         }

      private:

         string_literal_node(const string_literal_node<T>&);
         string_literal_node<T>& operator=(const string_literal_node<T>&);

         const std::string value_;
      };

      template <typename T>
      class unary_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         unary_node(const operator_type& operation,
                    expression_ptr branch)
         : operation_(operation),
           branch_(branch),
           branch_deletable_(branch_deletable(branch_))
         {}

        ~unary_node()
         {
            if (branch_ && branch_deletable_)
            {
               delete branch_;
               branch_ = 0;
            }
         }

         inline T value() const
         {
            const T arg = branch_->value();
            return numeric::process<T>(operation_,arg);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_unary;
         }

         inline operator_type operation() const
         {
            return operation_;
         }

         inline expression_node<T>* branch(const std::size_t& index = 0) const
         {
            if (0 == index)
               return branch_;
            else
               return reinterpret_cast<expression_ptr>(0);
         }

      private:

         operator_type operation_;
         expression_ptr branch_;
         bool branch_deletable_;
      };

      template <typename T, std::size_t D, bool B>
      struct construct_branch_pair
      {
         template <std::size_t N>
         static inline void process(std::pair<expression_node<T>*,bool> (&)[N], expression_node<T>*)
         {}
      };

      template <typename T, std::size_t D>
      struct construct_branch_pair<T,D,true>
      {
         template <std::size_t N>
         static inline void process(std::pair<expression_node<T>*,bool> (&branch)[N], expression_node<T>* b)
         {
            if (b)
            {
               branch[D] = std::make_pair(b,branch_deletable(b));
            }
         }
      };

      template <std::size_t N, typename T>
      inline void init_branches(std::pair<expression_node<T>*,bool> (&branch)[N],
                                expression_node<T>* b0,
                                expression_node<T>* b1 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b2 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b3 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b4 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b5 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b6 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b7 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b8 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b9 = reinterpret_cast<expression_node<T>*>(0))
      {
         construct_branch_pair<T,0,(N > 0)>::process(branch,b0);
         construct_branch_pair<T,1,(N > 1)>::process(branch,b1);
         construct_branch_pair<T,2,(N > 2)>::process(branch,b2);
         construct_branch_pair<T,3,(N > 3)>::process(branch,b3);
         construct_branch_pair<T,4,(N > 4)>::process(branch,b4);
         construct_branch_pair<T,5,(N > 5)>::process(branch,b5);
         construct_branch_pair<T,6,(N > 6)>::process(branch,b6);
         construct_branch_pair<T,7,(N > 7)>::process(branch,b7);
         construct_branch_pair<T,8,(N > 8)>::process(branch,b8);
         construct_branch_pair<T,9,(N > 9)>::process(branch,b9);
      }

      template <typename T, std::size_t N>
      struct cleanup_branches
      {
         static inline void execute(std::pair<expression_node<T>*,bool> (&branch)[N])
         {
            for (std::size_t i = 0; i < N; ++i)
            {
               if (branch[i].first && branch[i].second)
               {
                  delete branch[i].first;
                  branch[i].first = 0;
               }
            }
         }
      };

      template <typename T>
      class binary_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         binary_node(const operator_type& operation,
                     expression_ptr branch0,
                     expression_ptr branch1)
         : operation_(operation)
         {
            init_branches<2>(branch_,branch0,branch1);
         }

        ~binary_node()
         {
            cleanup_branches<T,2>::execute(branch_);
         }

         inline T value() const
         {
            const T arg0 = branch_[0].first->value();
            const T arg1 = branch_[1].first->value();
            return numeric::process<T>(operation_,arg0,arg1);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_binary;
         }

         inline operator_type operation()
         {
            return operation_;
         }

         inline expression_node<T>* branch(const std::size_t& index = 0) const
         {
            if (0 == index)
               return branch_[0].first;
            else if (1 == index)
               return branch_[1].first;
            else
               return reinterpret_cast<expression_ptr>(0);
         }

      protected:

         operator_type operation_;
         branch_t branch_[2];
      };

      template <typename T, typename Operation>
      class binary_ext_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         binary_ext_node(expression_ptr branch0, expression_ptr branch1)
         {
            init_branches<2>(branch_,branch0,branch1);
         }

        ~binary_ext_node()
         {
            cleanup_branches<T,2>::execute(branch_);
         }

         inline T value() const
         {
            const T arg0 = branch_[0].first->value();
            const T arg1 = branch_[1].first->value();
            return Operation::process(arg0,arg1);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_binary_ext;
         }

         inline operator_type operation()
         {
            return Operation::operation();
         }

         inline expression_node<T>* branch(const std::size_t& index = 0) const
         {
            if (0 == index)
               return branch_[0].first;
            else if (1 == index)
               return branch_[1].first;
            else
               return reinterpret_cast<expression_ptr>(0);
         }

      protected:

         branch_t branch_[2];
      };

      template <typename T>
      class trinary_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         trinary_node(const operator_type& operation,
                      expression_ptr branch0,
                      expression_ptr branch1,
                      expression_ptr branch2)
         : operation_(operation)
         {
            init_branches<3>(branch_,branch0,branch1,branch2);
         }

        ~trinary_node()
         {
            cleanup_branches<T,3>::execute(branch_);
         }

         inline T value() const
         {
            const T arg0 = branch_[0].first->value();
            const T arg1 = branch_[1].first->value();
            const T arg2 = branch_[2].first->value();
            switch (operation_)
            {
               case e_inrange : return (arg1 < arg0) ? T(0) : ((arg1 > arg2) ? T(0) : T(1));
               case e_min     : return std::min<T>(std::min<T>(arg0,arg1),arg2);
               case e_max     : return std::max<T>(std::max<T>(arg0,arg1),arg2);
               case e_clamp   : return (arg1 < arg0) ? arg0 : (arg1 > arg2 ? arg2 : arg1);
               case e_iclamp  : if ((arg1 <= arg0) || (arg1 >= arg2))
                                   return arg1;
                                else
                                   return ((T(2) * arg1  <= (arg2 + arg0)) ? arg0 : arg2);
               default        : return std::numeric_limits<T>::quiet_NaN();
            }
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_trinary;
         }

      protected:

         operator_type operation_;
         branch_t branch_[3];
      };

      template <typename T>
      class quaternary_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         quaternary_node(const operator_type& operation,
                         expression_ptr branch0,
                         expression_ptr branch1,
                         expression_ptr branch2,
                         expression_ptr branch3)
         : operation_(operation)
         {
            init_branches<4>(branch_,branch0,branch1,branch2,branch3);
         }

        ~quaternary_node()
         {
            cleanup_branches<T,4>::execute(branch_);
         }

         inline T value() const
         {
            const T arg0 = branch_[0].first->value();
            const T arg1 = branch_[1].first->value();
            const T arg2 = branch_[2].first->value();
            const T arg3 = branch_[3].first->value();
            switch (operation_)
            {
               case e_min : return std::min<T>(std::min<T>(arg0,arg1),std::min<T>(arg2,arg3));
               case e_max : return std::max<T>(std::max<T>(arg0,arg1),std::max<T>(arg2,arg3));
               default    : return std::numeric_limits<T>::quiet_NaN();
            }
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_quaternary;
         }

      protected:

         operator_type operation_;
         branch_t branch_[4];
      };

      template <typename T>
      class quinary_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         quinary_node(const operator_type& operation,
                      expression_ptr branch0,
                      expression_ptr branch1,
                      expression_ptr branch2,
                      expression_ptr branch3,
                      expression_ptr branch4)
         : operation_(operation)
         {
            init_branches<5>(branch_,branch0,branch1,branch2,branch3,branch4);
         }

        ~quinary_node()
         {
            cleanup_branches<T,5>::execute(branch_);
         }

         inline T value() const
         {
            const T arg0 = branch_[0].first->value();
            const T arg1 = branch_[1].first->value();
            const T arg2 = branch_[2].first->value();
            const T arg3 = branch_[3].first->value();
            const T arg4 = branch_[4].first->value();

            switch (operation_)
            {
               case e_min  : return std::min<T>(std::min<T>(std::min<T>(arg0,arg1),std::min<T>(arg2,arg3)),arg4);
               case e_max  : return std::max<T>(std::max<T>(std::max<T>(arg0,arg1),std::max<T>(arg2,arg3)),arg4);
               default     : return std::numeric_limits<T>::quiet_NaN();
            }
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_quinary;
         }

      private:

         operator_type operation_;
         branch_t branch_[5];
      };

      template <typename T>
      class senary_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         senary_node(const operator_type& operation,
                     expression_ptr branch0,
                     expression_ptr branch1,
                     expression_ptr branch2,
                     expression_ptr branch3,
                     expression_ptr branch4,
                     expression_ptr branch5)
         : operation_(operation)
         {
            init_branches<6>(branch_,branch0,branch1,branch2,branch3,branch4,branch5);
         }

        ~senary_node()
         {
            cleanup_branches<T,6>::execute(branch_);
         }

         inline T value() const
         {
            const T arg0 = branch_[0].first->value();
            const T arg1 = branch_[1].first->value();
            const T arg2 = branch_[2].first->value();
            const T arg3 = branch_[3].first->value();
            const T arg4 = branch_[4].first->value();
            const T arg5 = branch_[5].first->value();
            switch (operation_)
            {
               case e_min     : return std::min<T>(std::min<T>(std::min<T>(arg0,arg1),std::min<T>(arg2,arg3)),std::min<T>(arg4,arg5));
               case e_max     : return std::max<T>(std::max<T>(std::max<T>(arg0,arg1),std::max<T>(arg2,arg3)),std::max<T>(arg4,arg5));
               case e_default :
               default        : return std::numeric_limits<T>::quiet_NaN();
            }
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_senary;
         }

      private:

         operator_type operation_;
         branch_t branch_[6];
      };

      template <typename T>
      class conditional_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         conditional_node(expression_ptr test,
                          expression_ptr consequent,
                          expression_ptr alternative)
         : test_(test),
           consequent_(consequent),
           alternative_(alternative),
           test_deletable_(!is_variable_node(test_)),
           consequent_deletable_(!is_variable_node(consequent_)),
           alternative_deletable_(!is_variable_node(alternative_))
         {}

        ~conditional_node()
         {
            if (test_        &&        test_deletable_) delete test_;
            if (consequent_  &&  consequent_deletable_) delete consequent_;
            if (alternative_ && alternative_deletable_) delete alternative_;
         }

         inline T value() const
         {
            if (is_true(test_))
               return consequent_->value();
            else
               return alternative_->value();
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_conditional;
         }

      private:

         expression_ptr test_;
         expression_ptr consequent_;
         expression_ptr alternative_;
         bool test_deletable_;
         bool consequent_deletable_;
         bool alternative_deletable_;
      };

      template <typename T>
      class while_loop_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         while_loop_node(expression_ptr test, expression_ptr branch)
         : test_(test),
           branch_(branch),
           test_deletable_(!is_variable_node(test_)),
           branch_deletable_(!is_variable_node(branch_))
         {}

        ~while_loop_node()
         {
            if (test_   &&   test_deletable_) delete test_;
            if (branch_ && branch_deletable_) delete branch_;
         }

         inline T value() const
         {
            T result = T(0);
            while (is_true(test_))
            {
               result = branch_->value();
            }
            return result;
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_while;
         }

      private:

         expression_ptr test_;
         expression_ptr branch_;
         bool test_deletable_;
         bool branch_deletable_;
      };

      template <typename T>
      class repeat_until_loop_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         repeat_until_loop_node(expression_ptr test, expression_ptr branch)
         : test_(test),
           branch_(branch),
           test_deletable_(!is_variable_node(test_)),
           branch_deletable_(!is_variable_node(branch_))
         {}

        ~repeat_until_loop_node()
         {
            if (test_   &&   test_deletable_) delete test_;
            if (branch_ && branch_deletable_) delete branch_;
         }

         inline T value() const
         {
            T result = T(0);
            do
            {
               result = branch_->value();
            }
            while (is_false(test_));
            return result;
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_repeat;
         }

      private:

         expression_ptr test_;
         expression_ptr branch_;
         bool test_deletable_;
         bool branch_deletable_;
      };

      template <typename T>
      class switch_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         template <typename Allocator,
                   template <typename,typename> class Sequence>
         switch_node(const Sequence<expression_ptr,Allocator>& arglist)
         {
            if (1 != (arglist.size() & 1))
               return;
            arg_list_.resize(arglist.size());
            delete_branch_.resize(arglist.size());
            for (std::size_t i = 0; i < arglist.size(); ++i)
            {
               if (arglist[i])
               {
                       arg_list_[i] = arglist[i];
                  delete_branch_[i] = static_cast<unsigned char>(branch_deletable(arg_list_[i]) ? 1 : 0);
               }
               else
               {
                  arg_list_.clear();
                  delete_branch_.clear();
                  return;
               }
            }
         }

        ~switch_node()
         {
            for (std::size_t i = 0; i < arg_list_.size(); ++i)
            {
               if (arg_list_[i] && delete_branch_[i])
               {
                  delete arg_list_[i];
                  arg_list_[i] = 0;
               }
            }
         }

         inline T value() const
         {
            if (!arg_list_.empty())
            {
               if (1 != (arg_list_.size() & 1))
               {
                  return std::numeric_limits<T>::quiet_NaN();
               }

               for (std::size_t i = 0; i < arg_list_.size() / 2; ++i)
               {
                  expression_ptr condition  = arg_list_[(2 * i)    ];
                  expression_ptr consequent = arg_list_[(2 * i) + 1];
                  if (is_true(condition))
                  {
                     return consequent->value();
                  }
               }

               return arg_list_.back()->value();
            }
            else
               return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_switch;
         }

      private:

         std::vector<expression_ptr> arg_list_;
         std::vector<unsigned char> delete_branch_;
      };

      template <typename T>
      class multi_switch_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         template <typename Allocator,
                   template <typename,typename> class Sequence>
         multi_switch_node(const Sequence<expression_ptr,Allocator>& arglist)
         {
            if (0 != (arglist.size() & 1))
               return;
            arg_list_.resize(arglist.size());
            delete_branch_.resize(arglist.size());
            for (std::size_t i = 0; i < arglist.size(); ++i)
            {
               if (arglist[i])
               {
                       arg_list_[i] = arglist[i];
                  delete_branch_[i] = static_cast<unsigned char>(branch_deletable(arg_list_[i]) ? 1 : 0);
               }
               else
               {
                  arg_list_.clear();
                  delete_branch_.clear();
                  return;
               }
            }
         }

        ~multi_switch_node()
         {
            for (std::size_t i = 0; i < arg_list_.size(); ++i)
            {
               if (arg_list_[i] && delete_branch_[i])
               {
                  delete arg_list_[i];
                  arg_list_[i] = 0;
               }
            }
         }

         inline T value() const
         {
            T result = T(0);
            if (!arg_list_.empty())
            {
               if (0 != (arg_list_.size() & 1))
               {
                  return std::numeric_limits<T>::quiet_NaN();
               }

               for (std::size_t i = 0; i < arg_list_.size() / 2; ++i)
               {
                  expression_ptr condition  = arg_list_[(2 * i)    ];
                  expression_ptr consequent = arg_list_[(2 * i) + 1];

                  if (is_true(condition))
                  {
                     result = consequent->value();
                  }
               }
            }

            return result;
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_mswitch;
         }

      private:

         std::vector<expression_ptr> arg_list_;
         std::vector<unsigned char> delete_branch_;
      };

      template <typename T>
      class variable_node : public expression_node<T>
      {
      public:

         static T null_value;

         explicit variable_node()
         : value_(&null_value)
         {}

         explicit variable_node(T& value)
         : value_(&value)
         {}

         inline bool operator <(const variable_node<T>& v) const
         {
            return this < (&v);
         }

         inline T value() const
         {
            return (*value_);
         }

         inline T& ref()
         {
            return (*value_);
         }

         inline const T& ref() const
         {
            return (*value_);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_variable;
         }

      private:

         T* value_;
      };

      template <typename T>
      T variable_node<T>::null_value = T(std::numeric_limits<T>::quiet_NaN());

      #ifndef exprtk_disable_string_capabilities
      template <typename T>
      class stringvar_node : public string_base_node<T>
      {
      public:

         static std::string null_value;

         explicit stringvar_node()
         : value_(&null_value)
         {}

         explicit stringvar_node(std::string& value)
         : value_(&value)
         {}

         inline bool operator <(const stringvar_node<T>& v) const
         {
            return this < (&v);
         }

         inline T value() const
         {
            return std::numeric_limits<T>::quiet_NaN();
         }

         inline std::string str() const
         {
            return (*value_);
         }

         inline virtual std::string& ref()
         {
            return (*value_);
         }

         inline virtual const std::string& ref() const
         {
            return (*value_);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_stringvar;
         }

      private:

         std::string* value_;
      };

      template <typename T>
      std::string stringvar_node<T>::null_value = std::string("");

      template <typename T, typename RangePack>
      class string_range_node : public string_base_node<T>
      {
      public:

         static std::string null_value;

         explicit string_range_node(std::string& value, RangePack rp)
         : value_(&value),
           rp_(rp)
         {}

        ~string_range_node()
         {
            rp_.free();
         }

         inline bool operator <(const string_range_node<T,RangePack>& v) const
         {
            return this < (&v);
         }

         inline T value() const
         {
            return std::numeric_limits<T>::quiet_NaN();
         }

         inline std::string str() const
         {
            return (*value_);
         }

         inline RangePack range() const
         {
            return rp_;
         }

         inline virtual std::string& ref()
         {
            return (*value_);
         }

         inline virtual const std::string& ref() const
         {
            return (*value_);
         }

         inline virtual RangePack& range_ref()
         {
            return rp_;
         }

         inline virtual const RangePack& range_ref() const
         {
            return rp_;
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_stringvarrng;
         }

      private:

         std::string* value_;
         RangePack    rp_;
      };

      template <typename T, typename RangePack>
      std::string string_range_node<T,RangePack>::null_value = std::string("");

      template <typename T, typename RangePack>
      class const_string_range_node : public string_base_node<T>
      {
      public:

         explicit const_string_range_node(const std::string& value, RangePack rp)
         : value_(value),
           rp_(rp)
         {}

        ~const_string_range_node()
         {
            rp_.free();
         }

         inline T value() const
         {
            return std::numeric_limits<T>::quiet_NaN();
         }

         inline std::string str() const
         {
            return value_;
         }

         inline RangePack range() const
         {
            return rp_;
         }

         inline virtual RangePack& range_ref()
         {
            return rp_;
         }

         inline virtual const RangePack& range_ref() const
         {
            return rp_;
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_cstringvarrng;
         }

      private:

         const_string_range_node<T,RangePack>& operator=(const const_string_range_node<T,RangePack>&);

         const std::string value_;
         RangePack    rp_;
      };
      #endif

      template <typename T, std::size_t N> inline T axn(T a, T x)       { return a * exprtk::details::numeric::fast_exp<T,N>::result(x); } // a*x^n
      template <typename T, std::size_t N> inline T axnb(T a, T x, T b) { return a * exprtk::details::numeric::fast_exp<T,N>::result(x) + b; } // a*x^n+b

      template<typename T>
      struct sf_base
      {
         typedef typename details::functor_t<T>::Type Type;
         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::qfunc_t quaternary_functor_t;
         typedef typename functor_t::tfunc_t trinary_functor_t;
         typedef typename functor_t::bfunc_t binary_functor_t;
         typedef typename functor_t::ufunc_t unary_functor_t;
      };

      template <typename T> struct sf00_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return (x + y) / z; } static inline std::string id() { return "(t+t)/t";} };
      template <typename T> struct sf01_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return (x + y) * z; } static inline std::string id() { return "(t+t)*t";} };
      template <typename T> struct sf02_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return (x + y) - z; } static inline std::string id() { return "(t+t)-t";} };
      template <typename T> struct sf03_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return (x + y) + z; } static inline std::string id() { return "(t+t)+t";} };
      template <typename T> struct sf04_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return (x - y) + z; } static inline std::string id() { return "(t-t)+t";} };
      template <typename T> struct sf05_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return (x - y) / z; } static inline std::string id() { return "(t-t)/t";} };
      template <typename T> struct sf06_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return (x - y) * z; } static inline std::string id() { return "(t-t)*t";} };
      template <typename T> struct sf07_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return (x * y) + z; } static inline std::string id() { return "(t*t)+t";} };
      template <typename T> struct sf08_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return (x * y) - z; } static inline std::string id() { return "(t*t)-t";} };
      template <typename T> struct sf09_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return (x * y) / z; } static inline std::string id() { return "(t*t)/t";} };
      template <typename T> struct sf10_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return (x * y) * z; } static inline std::string id() { return "(t*t)*t";} };
      template <typename T> struct sf11_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return (x / y) + z; } static inline std::string id() { return "(t/t)+t";} };
      template <typename T> struct sf12_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return (x / y) - z; } static inline std::string id() { return "(t/t)-t";} };
      template <typename T> struct sf13_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return (x / y) / z; } static inline std::string id() { return "(t/t)/t";} };
      template <typename T> struct sf14_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return (x / y) * z; } static inline std::string id() { return "(t/t)*t";} };
      template <typename T> struct sf15_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x / (y + z); } static inline std::string id() { return "t/(t+t)";} };
      template <typename T> struct sf16_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x / (y - z); } static inline std::string id() { return "t/(t-t)";} };
      template <typename T> struct sf17_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x / (y * z); } static inline std::string id() { return "t/(t*t)";} };
      template <typename T> struct sf18_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x / (y / z); } static inline std::string id() { return "t/(t/t)";} };
      template <typename T> struct sf19_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x * (y + z); } static inline std::string id() { return "t*(t+t)";} };
      template <typename T> struct sf20_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x * (y - z); } static inline std::string id() { return "t*(t-t)";} };
      template <typename T> struct sf21_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x * (y * z); } static inline std::string id() { return "t*(t*t)";} };
      template <typename T> struct sf22_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x * (y / z); } static inline std::string id() { return "t*(t/t)";} };
      template <typename T> struct sf23_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x - (y + z); } static inline std::string id() { return "t-(t+t)";} };
      template <typename T> struct sf24_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x - (y - z); } static inline std::string id() { return "t-(t-t)";} };
      template <typename T> struct sf25_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x - (y / z); } static inline std::string id() { return "t-(t/t)";} };
      template <typename T> struct sf26_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x - (y * z); } static inline std::string id() { return "t-(t*t)";} };
      template <typename T> struct sf27_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x + (y * z); } static inline std::string id() { return "t+(t*t)";} };
      template <typename T> struct sf28_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x + (y / z); } static inline std::string id() { return "t+(t/t)";} };
      template <typename T> struct sf29_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x + (y + z); } static inline std::string id() { return "t+(t+t)";} };
      template <typename T> struct sf30_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x + (y - z); } static inline std::string id() { return "t+(t-t)";} };
      template <typename T> struct sf31_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return axnb<T,2>(x,y,z); } }; //x * y^2 + z
      template <typename T> struct sf32_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return axnb<T,3>(x,y,z); } }; //x * y^3 + z
      template <typename T> struct sf33_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return axnb<T,4>(x,y,z); } }; //x * y^4 + z
      template <typename T> struct sf34_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return axnb<T,5>(x,y,z); } }; //x * y^5 + z
      template <typename T> struct sf35_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return axnb<T,6>(x,y,z); } }; //x * y^6 + z
      template <typename T> struct sf36_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return axnb<T,7>(x,y,z); } }; //x * y^7 + z
      template <typename T> struct sf37_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return axnb<T,8>(x,y,z); } }; //x * y^8 + z
      template <typename T> struct sf38_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return axnb<T,9>(x,y,z); } }; //x * y^9 + z
      template <typename T> struct sf39_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x * numeric::log(y)   + z; } };
      template <typename T> struct sf40_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x * numeric::log(y)   - z; } };
      template <typename T> struct sf41_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x * numeric::log10(y) + z; } };
      template <typename T> struct sf42_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x * numeric::log10(y) - z; } };
      template <typename T> struct sf43_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x * numeric::sin(y) + z; } };
      template <typename T> struct sf44_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x * numeric::sin(y) - z; } };
      template <typename T> struct sf45_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x * numeric::cos(y) + z; } };
      template <typename T> struct sf46_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return x * numeric::cos(y) - z; } };
      template <typename T> struct sf47_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z) { return is_true(x) ? y : z;      } };
      template <typename T> struct sf48_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x + ((y + z) / w); } static inline std::string id() { return "t+((t+t)/t)";} };
      template <typename T> struct sf49_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x + ((y + z) * w); } static inline std::string id() { return "t+((t+t)*t)";} };
      template <typename T> struct sf50_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x + ((y - z) / w); } static inline std::string id() { return "t+((t-t)/t)";} };
      template <typename T> struct sf51_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x + ((y - z) * w); } static inline std::string id() { return "t+((t-t)*t)";} };
      template <typename T> struct sf52_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x + ((y * z) / w); } static inline std::string id() { return "t+((t*t)/t)";} };
      template <typename T> struct sf53_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x + ((y * z) * w); } static inline std::string id() { return "t+((t*t)*t)";} };
      template <typename T> struct sf54_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x + ((y / z) + w); } static inline std::string id() { return "t+((t/t)+t)";} };
      template <typename T> struct sf55_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x + ((y / z) / w); } static inline std::string id() { return "t+((t/t)/t)";} };
      template <typename T> struct sf56_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x + ((y / z) * w); } static inline std::string id() { return "t+((t/t)*t)";} };
      template <typename T> struct sf57_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x - ((y + z) / w); } static inline std::string id() { return "t-((t+t)/t)";} };
      template <typename T> struct sf58_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x - ((y + z) * w); } static inline std::string id() { return "t-((t+t)*t)";} };
      template <typename T> struct sf59_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x - ((y - z) / w); } static inline std::string id() { return "t-((t-t)/t)";} };
      template <typename T> struct sf60_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x - ((y - z) * w); } static inline std::string id() { return "t-((t-t)*t)";} };
      template <typename T> struct sf61_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x - ((y * z) / w); } static inline std::string id() { return "t-((t*t)/t)";} };
      template <typename T> struct sf62_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x - ((y * z) * w); } static inline std::string id() { return "t-((t*t)*t)";} };
      template <typename T> struct sf63_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x - ((y / z) / w); } static inline std::string id() { return "t-((t/t)/t)";} };
      template <typename T> struct sf64_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x - ((y / z) * w); } static inline std::string id() { return "t-((t/t)*t)";} };
      template <typename T> struct sf65_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return ((x + y) * z) - w; } static inline std::string id() { return "((t+t)*t)-t";} };
      template <typename T> struct sf66_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return ((x - y) * z) - w; } static inline std::string id() { return "((t-t)*t)-t";} };
      template <typename T> struct sf67_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return ((x * y) * z) - w; } static inline std::string id() { return "((t*t)*t)-t";} };
      template <typename T> struct sf68_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return ((x / y) * z) - w; } static inline std::string id() { return "((t/t)*t)-t";} };
      template <typename T> struct sf69_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return ((x + y) / z) - w; } static inline std::string id() { return "((t+t)/t)-t";} };
      template <typename T> struct sf70_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return ((x - y) / z) - w; } static inline std::string id() { return "((t-t)/t)-t";} };
      template <typename T> struct sf71_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return ((x * y) / z) - w; } static inline std::string id() { return "((t*t)/t)-t";} };
      template <typename T> struct sf72_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return ((x / y) / z) - w; } static inline std::string id() { return "((t/t)/t)-t";} };
      template <typename T> struct sf73_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x * y) + (z * w); } static inline std::string id() { return "(t*t)+(t*t)";} };
      template <typename T> struct sf74_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x * y) - (z * w); } static inline std::string id() { return "(t*t)-(t*t)";} };
      template <typename T> struct sf75_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x * y) + (z / w); } static inline std::string id() { return "(t*t)+(t/t)";} };
      template <typename T> struct sf76_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x * y) - (z / w); } static inline std::string id() { return "(t*t)-(t/t)";} };
      template <typename T> struct sf77_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x / y) + (z / w); } static inline std::string id() { return "(t/t)+(t/t)";} };
      template <typename T> struct sf78_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x / y) - (z / w); } static inline std::string id() { return "(t/t)-(t/t)";} };
      template <typename T> struct sf79_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x / y) - (z * w); } static inline std::string id() { return "(t/t)-(t*t)";} };
      template <typename T> struct sf80_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x / (y + (z * w)); } static inline std::string id() { return "t/(t+(t*t))";} };
      template <typename T> struct sf81_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x / (y - (z * w)); } static inline std::string id() { return "t/(t-(t*t))";} };
      template <typename T> struct sf82_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x * (y + (z * w)); } static inline std::string id() { return "t*(t+(t*t))";} };
      template <typename T> struct sf83_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x * (y - (z * w)); } static inline std::string id() { return "t*(t-(t*t))";} };
      template <typename T> struct sf84_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return axn<T,2>(x,y) + axn<T,2>(z,w); } }; //x*y^2+z*w^2
      template <typename T> struct sf85_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return axn<T,3>(x,y) + axn<T,3>(z,w); } }; //x*y^3+z*w^3
      template <typename T> struct sf86_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return axn<T,4>(x,y) + axn<T,4>(z,w); } }; //x*y^4+z*w^4
      template <typename T> struct sf87_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return axn<T,5>(x,y) + axn<T,5>(z,w); } }; //x*y^5+z*w^5
      template <typename T> struct sf88_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return axn<T,6>(x,y) + axn<T,6>(z,w); } }; //x*y^6+z*w^6
      template <typename T> struct sf89_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return axn<T,7>(x,y) + axn<T,7>(z,w); } }; //x*y^7+z*w^7
      template <typename T> struct sf90_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return axn<T,8>(x,y) + axn<T,8>(z,w); } }; //x*y^8+z*w^8
      template <typename T> struct sf91_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return axn<T,9>(x,y) + axn<T,9>(z,w); } }; //x*y^9+z*w^9
      template <typename T> struct sf92_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (is_true(x) && is_true(y)) ? z : w; } };
      template <typename T> struct sf93_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (is_true(x) || is_true(y)) ? z : w; } };
      template <typename T> struct sf94_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x <  y) ? z : w; } };
      template <typename T> struct sf95_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x <= y) ? z : w; } };
      template <typename T> struct sf96_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x >  y) ? z : w; } };
      template <typename T> struct sf97_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x >= y) ? z : w; } };
      template <typename T> struct sf98_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return numeric::equal(x,y) ? z : w; } };
      template <typename T> struct sf99_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x * numeric::sin(y) + z * numeric::cos(w); } };

      template <typename T> struct sfext00_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x + y) - (z * w); } static inline std::string id() { return "(t+t)-(t*t)";} };
      template <typename T> struct sfext01_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x + y) - (z / w); } static inline std::string id() { return "(t+t)-(t/t)";} };
      template <typename T> struct sfext02_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x + y) + (z * w); } static inline std::string id() { return "(t+t)+(t*t)";} };
      template <typename T> struct sfext03_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x + y) + (z / w); } static inline std::string id() { return "(t+t)+(t/t)";} };
      template <typename T> struct sfext04_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x - y) + (z * w); } static inline std::string id() { return "(t-t)+(t*t)";} };
      template <typename T> struct sfext05_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x - y) + (z / w); } static inline std::string id() { return "(t-t)+(t/t)";} };
      template <typename T> struct sfext06_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x - y) - (z * w); } static inline std::string id() { return "(t-t)-(t*t)";} };
      template <typename T> struct sfext07_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x - y) - (z / w); } static inline std::string id() { return "(t-t)-(t/t)";} };
      template <typename T> struct sfext08_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x + y) - (z - w); } static inline std::string id() { return "(t+t)-(t-t)";} };
      template <typename T> struct sfext09_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x + y) + (z - w); } static inline std::string id() { return "(t+t)+(t-t)";} };
      template <typename T> struct sfext10_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x + y) * (z - w); } static inline std::string id() { return "(t+t)*(t-t)";} };
      template <typename T> struct sfext11_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x + y) / (z - w); } static inline std::string id() { return "(t+t)/(t-t)";} };
      template <typename T> struct sfext12_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x - y) - (z + w); } static inline std::string id() { return "(t-t)-(t+t)";} };
      template <typename T> struct sfext13_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x - y) + (z + w); } static inline std::string id() { return "(t-t)+(t+t)";} };
      template <typename T> struct sfext14_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x - y) * (z + w); } static inline std::string id() { return "(t-t)*(t+t)";} };
      template <typename T> struct sfext15_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x - y) / (z + w); } static inline std::string id() { return "(t-t)/(t+t)";} };
      template <typename T> struct sfext16_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x * y) - (z + w); } static inline std::string id() { return "(t*t)-(t+t)";} };
      template <typename T> struct sfext17_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x / y) - (z + w); } static inline std::string id() { return "(t/t)-(t+t)";} };
      template <typename T> struct sfext18_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x * y) + (z + w); } static inline std::string id() { return "(t*t)+(t+t)";} };
      template <typename T> struct sfext19_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x / y) + (z + w); } static inline std::string id() { return "(t/t)+(t+t)";} };
      template <typename T> struct sfext20_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x * y) + (z - w); } static inline std::string id() { return "(t*t)+(t-t)";} };
      template <typename T> struct sfext21_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x / y) + (z - w); } static inline std::string id() { return "(t/t)+(t-t)";} };
      template <typename T> struct sfext22_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x * y) - (z - w); } static inline std::string id() { return "(t*t)-(t-t)";} };
      template <typename T> struct sfext23_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x / y) - (z - w); } static inline std::string id() { return "(t/t)-(t-t)";} };
      template <typename T> struct sfext24_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x + y) * (z * w); } static inline std::string id() { return "(t+t)*(t*t)";} };
      template <typename T> struct sfext25_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x + y) * (z / w); } static inline std::string id() { return "(t+t)*(t/t)";} };
      template <typename T> struct sfext26_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x + y) / (z * w); } static inline std::string id() { return "(t+t)/(t*t)";} };
      template <typename T> struct sfext27_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x + y) / (z / w); } static inline std::string id() { return "(t+t)/(t/t)";} };
      template <typename T> struct sfext28_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x - y) / (z * w); } static inline std::string id() { return "(t-t)/(t*t)";} };
      template <typename T> struct sfext29_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x - y) / (z / w); } static inline std::string id() { return "(t-t)/(t/t)";} };
      template <typename T> struct sfext30_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x - y) * (z * w); } static inline std::string id() { return "(t-t)*(t*t)";} };
      template <typename T> struct sfext31_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x - y) * (z / w); } static inline std::string id() { return "(t-t)*(t/t)";} };
      template <typename T> struct sfext32_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x * y) * (z + w); } static inline std::string id() { return "(t*t)*(t+t)";} };
      template <typename T> struct sfext33_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x / y) * (z + w); } static inline std::string id() { return "(t/t)*(t+t)";} };
      template <typename T> struct sfext34_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x * y) / (z + w); } static inline std::string id() { return "(t*t)/(t+t)";} };
      template <typename T> struct sfext35_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x / y) / (z + w); } static inline std::string id() { return "(t/t)/(t+t)";} };
      template <typename T> struct sfext36_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x * y) / (z - w); } static inline std::string id() { return "(t*t)/(t-t)";} };
      template <typename T> struct sfext37_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x / y) / (z - w); } static inline std::string id() { return "(t/t)/(t-t)";} };
      template <typename T> struct sfext38_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x * y) * (z - w); } static inline std::string id() { return "(t*t)*(t-t)";} };
      template <typename T> struct sfext39_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x * y) / (z * w); } static inline std::string id() { return "(t*t)/(t*t)";} };
      template <typename T> struct sfext40_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x / y) * (z - w); } static inline std::string id() { return "(t/t)*(t-t)";} };
      template <typename T> struct sfext41_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return (x * y) * (z * w); } static inline std::string id() { return "(t*t)*(t*t)";} };
      template <typename T> struct sfext42_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x + (y * (z / w)); } static inline std::string id() { return "t+(t*(t/t))";} };
      template <typename T> struct sfext43_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x - (y * (z / w)); } static inline std::string id() { return "t-(t*(t/t))";} };
      template <typename T> struct sfext44_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x + (y / (z * w)); } static inline std::string id() { return "t+(t/(t*t))";} };
      template <typename T> struct sfext45_op : public sf_base<T> { typedef typename sf_base<T>::Type Type; static inline T process(Type x, Type y, Type z, Type w) { return x - (y / (z * w)); } static inline std::string id() { return "t-(t/(t*t))";} };

      template <typename T, typename SpecialFunction>
      class sf3_node : public trinary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         sf3_node(const operator_type& operation,
                  expression_ptr branch0,
                  expression_ptr branch1,
                  expression_ptr branch2)
         : trinary_node<T>(operation,branch0,branch1,branch2)
         {}

         inline T value() const
         {
            const T x = trinary_node<T>::branch_[0].first->value();
            const T y = trinary_node<T>::branch_[1].first->value();
            const T z = trinary_node<T>::branch_[2].first->value();
            return SpecialFunction::process(x,y,z);
         }
      };

      template <typename T, typename SpecialFunction>
      class sf4_node : public quaternary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         sf4_node(const operator_type& operation,
                  expression_ptr branch0,
                  expression_ptr branch1,
                  expression_ptr branch2,
                  expression_ptr branch3)
         : quaternary_node<T>(operation,branch0,branch1,branch2,branch3)
         {}

         inline T value() const
         {
            const T x = quaternary_node<T>::branch_[0].first->value();
            const T y = quaternary_node<T>::branch_[1].first->value();
            const T z = quaternary_node<T>::branch_[2].first->value();
            const T w = quaternary_node<T>::branch_[3].first->value();
            return SpecialFunction::process(x,y,z,w);
         }
      };

      template <typename T, typename SpecialFunction>
      class sf3_var_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         sf3_var_node(const T& v0, const T& v1, const T& v2)
         : v0_(v0),
           v1_(v1),
           v2_(v2)
         {}

         inline T value() const
         {
            return SpecialFunction::process(v0_,v1_,v2_);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_trinary;
         }

      private:

         sf3_var_node(sf3_var_node<T,SpecialFunction>&);
         sf3_var_node<T,SpecialFunction>& operator=(sf3_var_node<T,SpecialFunction>&);

         const T& v0_;
         const T& v1_;
         const T& v2_;
      };

      template <typename T, typename SpecialFunction>
      class sf4_var_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         sf4_var_node(const T& v0, const T& v1, const T& v2, const T& v3)
         : v0_(v0),
           v1_(v1),
           v2_(v2),
           v3_(v3)
         {}

         inline T value() const
         {
            return SpecialFunction::process(v0_,v1_,v2_,v3_);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_trinary;
         }

      private:

         sf4_var_node(sf4_var_node<T,SpecialFunction>&);
         sf4_var_node<T,SpecialFunction>& operator=(sf4_var_node<T,SpecialFunction>&);

         const T& v0_;
         const T& v1_;
         const T& v2_;
         const T& v3_;
      };

      template <typename T, typename VarArgFunction>
      class vararg_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         template <typename Allocator,
                   template <typename,typename> class Sequence>
         vararg_node(const Sequence<expression_ptr,Allocator>& arglist)
         {
            arg_list_.resize(arglist.size());
            delete_branch_.resize(arglist.size());
            for (std::size_t i = 0; i < arglist.size(); ++i)
            {
               if (arglist[i])
               {
                       arg_list_[i] = arglist[i];
                  delete_branch_[i] = static_cast<unsigned char>(branch_deletable(arg_list_[i]) ? 1 : 0);
               }
               else
               {
                  arg_list_.clear();
                  delete_branch_.clear();
                  return;
               }
            }
         }

        ~vararg_node()
         {
            for (std::size_t i = 0; i < arg_list_.size(); ++i)
            {
               if (arg_list_[i] && delete_branch_[i])
               {
                  delete arg_list_[i];
                  arg_list_[i] = 0;
               }
            }
         }

         inline T value() const
         {
            if (!arg_list_.empty())
               return VarArgFunction::process(arg_list_);
            else
               return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_vararg;
         }

      private:

         std::vector<expression_ptr> arg_list_;
         std::vector<unsigned char> delete_branch_;
      };

      template <typename T, typename VarArgFunction>
      class vararg_varnode : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         template <typename Allocator,
                   template <typename,typename> class Sequence>
         vararg_varnode(const Sequence<expression_ptr,Allocator>& arglist)
         {
            arg_list_.resize(arglist.size());
            for (std::size_t i = 0; i < arglist.size(); ++i)
            {
               if (arglist[i] && is_variable_node(arglist[i]))
               {
                  variable_node<T>* var_node_ptr = dynamic_cast<variable_node<T>*>(arglist[i]);
                  arg_list_[i] = (&var_node_ptr->ref());
               }
               else
               {
                  arg_list_.clear();
                  return;
               }
            }
         }

         inline T value() const
         {
            if (!arg_list_.empty())
               return VarArgFunction::process(arg_list_);
            else
               return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_vararg;
         }

      private:

         std::vector<const T*> arg_list_;
      };

      template <typename T>
      class assignment_node : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         assignment_node(const operator_type& operation,
                         expression_ptr branch0,
                         expression_ptr branch1)
         : binary_node<T>(operation,branch0,branch1),
           var_node_ptr_(0)
         {
            if (is_variable_node(binary_node<T>::branch_[0].first))
            {
               var_node_ptr_ = dynamic_cast<variable_node<T>*>(binary_node<T>::branch_[0].first);
            }
         }

         inline T value() const
         {
            if (var_node_ptr_)
            {
               T& result = var_node_ptr_->ref();
               result = binary_node<T>::branch_[1].first->value();
               return result;
            }
            return std::numeric_limits<T>::quiet_NaN();
         }

      private:

         variable_node<T>* var_node_ptr_;
      };

      template <typename T>
      class scand_node : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         scand_node(const operator_type& operation,
                    expression_ptr branch0,
                    expression_ptr branch1)
         : binary_node<T>(operation,branch0,branch1)
         {}

         inline T value() const
         {
            return (
                    (T(0) != binary_node<T>::branch_[0].first->value()) &&
                    (T(0) != binary_node<T>::branch_[1].first->value())
                   ) ? T(1) : T(0);
         }
      };

      template <typename T>
      class scor_node : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         scor_node(const operator_type& operation,
                   expression_ptr branch0,
                   expression_ptr branch1)
         : binary_node<T>(operation,branch0,branch1)
         {}

         inline T value() const
         {
            return (
                    (T(0) != binary_node<T>::branch_[0].first->value()) ||
                    (T(0) != binary_node<T>::branch_[1].first->value())
                   ) ? T(1) : T(0);
         }
      };

      template <typename T, typename IFunction, std::size_t N>
      class function_N_node : public expression_node<T>
      {
      public:

         // Function of N paramters.
         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;
         typedef IFunction ifunction;

         function_N_node(ifunction* func)
         : function_((N == func->param_count) ? func : reinterpret_cast<ifunction*>(0)),
           parameter_count_(func->param_count)
         {}

        ~function_N_node()
         {
            cleanup_branches<T,N>::execute(branch_);
         }

         template <std::size_t NumBranches>
         bool init_branches(expression_ptr (&b)[NumBranches])
         {
            // Needed for incompetent and broken msvc compiler versions
            #ifdef _MSC_VER
             #pragma warning(push)
             #pragma warning(disable: 4127)
            #endif
            if (N != NumBranches)
               return false;
            else
            {
               for (std::size_t i = 0; i < NumBranches; ++i)
               {
                  if (b[i])
                     branch_[i] = std::make_pair(b[i],branch_deletable(b[i]));
                  else
                     return false;
               }
               return true;
            }
            #ifdef _MSC_VER
             #pragma warning(pop)
            #endif
         }

         inline bool operator <(const function_N_node<T,IFunction,N>& fn) const
         {
            return this < (&fn);
         }

         inline T value() const
         {
            // Needed for incompetent and broken msvc compiler versions
            #ifdef _MSC_VER
             #pragma warning(push)
             #pragma warning(disable: 4127)
            #endif
            if ((0 == function_) || (0 == N))
               return std::numeric_limits<T>::quiet_NaN();
            else
            {
               T v[N];
               evaluate_branches<T,N>::execute(v,branch_);
               return invoke<T,N>::execute(*function_,v);
            }
            #ifdef _MSC_VER
             #pragma warning(pop)
            #endif
         }

         template <typename T_, std::size_t BranchCount>
         struct evaluate_branches
         {
            static inline void execute(T_ (&v)[BranchCount], const branch_t (&b)[BranchCount])
            {
               for (std::size_t i = 0; i < BranchCount; ++i)
               {
                  v[i] = b[i].first->value();
               }
            }
         };

         template <typename T_>
         struct evaluate_branches <T_,5>
         {
            static inline void execute(T_ (&v)[5], const branch_t (&b)[5])
            {
               v[0] = b[0].first->value();
               v[1] = b[1].first->value();
               v[2] = b[2].first->value();
               v[3] = b[3].first->value();
               v[4] = b[4].first->value();
            }
         };

         template <typename T_>
         struct evaluate_branches <T_,4>
         {
            static inline void execute(T_ (&v)[4], const branch_t (&b)[4])
            {
               v[0] = b[0].first->value();
               v[1] = b[1].first->value();
               v[2] = b[2].first->value();
               v[3] = b[3].first->value();
            }
         };

         template <typename T_>
         struct evaluate_branches <T_,3>
         {
            static inline void execute(T_ (&v)[3], const branch_t (&b)[3])
            {
               v[0] = b[0].first->value();
               v[1] = b[1].first->value();
               v[2] = b[2].first->value();
            }
         };

         template <typename T_>
         struct evaluate_branches <T_,2>
         {
            static inline void execute(T_ (&v)[2], const branch_t (&b)[2])
            {
               v[0] = b[0].first->value();
               v[1] = b[1].first->value();
            }
         };

         template <typename T_>
         struct evaluate_branches <T_,1>
         {
            static inline void execute(T_ (&v)[1], const branch_t (&b)[1])
            {
               v[0] = b[0].first->value();
            }
         };

         template <typename T_, std::size_t ParamCount>
         struct invoke { static inline T execute(ifunction*, branch_t (&)[ParamCount]) { return std::numeric_limits<T_>::quiet_NaN(); } };

         template <typename T_>
         struct invoke<T_,20>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[20])
            { return f(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],v[13],v[14],v[15],v[16],v[17],v[18],v[19]); }
         };

         template <typename T_>
         struct invoke<T_,19>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[19])
            { return f(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],v[13],v[14],v[15],v[16],v[17],v[18]); }
         };

         template <typename T_>
         struct invoke<T_,18>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[18])
            { return f(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],v[13],v[14],v[15],v[16],v[17]); }
         };

         template <typename T_>
         struct invoke<T_,17>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[17])
            { return f(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],v[13],v[14],v[15],v[16]); }
         };

         template <typename T_>
         struct invoke<T_,16>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[16])
            { return f(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],v[13],v[14],v[15]); }
         };

         template <typename T_>
         struct invoke<T_,15>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[15])
            { return f(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],v[13],v[14]); }
         };

         template <typename T_>
         struct invoke<T_,14>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[14])
            { return f(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],v[13]); }
         };

         template <typename T_>
         struct invoke<T_,13>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[13])
            { return f(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12]); }
         };

         template <typename T_>
         struct invoke<T_,12>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[12])
            { return f(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11]); }
         };

         template <typename T_>
         struct invoke<T_,11>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[11])
            { return f(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10]); }
         };

         template <typename T_>
         struct invoke<T_,10>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[10])
            { return f(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9]); }
         };

         template <typename T_>
         struct invoke<T_,9>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[9])
            { return f(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8]); }
         };

         template <typename T_>
         struct invoke<T_,8>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[8])
            { return f(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7]); }
         };

         template <typename T_>
         struct invoke<T_,7>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[7])
            { return f(v[0],v[1],v[2],v[3],v[4],v[5],v[6]); }
         };

         template <typename T_>
         struct invoke<T_,6>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[6])
            { return f(v[0],v[1],v[2],v[3],v[4],v[5]); }
         };

         template <typename T_>
         struct invoke<T_,5>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[5])
            { return f(v[0],v[1],v[2],v[3],v[4]); }
         };

         template <typename T_>
         struct invoke<T_,4>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[4])
            { return f(v[0],v[1],v[2],v[3]); }
         };

         template <typename T_>
         struct invoke<T_,3>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[3])
            { return f(v[0],v[1],v[2]); }
         };

         template <typename T_>
         struct invoke<T_,2>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[2])
            { return f(v[0],v[1]); }
         };

         template <typename T_>
         struct invoke<T_,1>
         {
            static inline T_ execute(ifunction& f, T_ (&v)[1])
            { return f(v[0]); }
         };

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_function;
         }

      private:

         ifunction* function_;
         std::size_t parameter_count_;
         branch_t branch_[N];
      };

      template <typename T, typename IFunction>
      class function_N_node<T,IFunction,0> : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef IFunction ifunction;

         function_N_node(ifunction* func)
         : function_((0 == func->param_count) ? func : reinterpret_cast<ifunction*>(0))
         {}

         inline bool operator <(const function_N_node<T,IFunction,0>& fn) const
         {
            return this < (&fn);
         }

         inline T value() const
         {
            if (function_)
               return (*function_)();
            else
               return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_function;
         }

      private:

         ifunction* function_;
         std::size_t parameter_count_;
      };

      template <typename T, typename VarArgFunction>
      class vararg_function_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         vararg_function_node(VarArgFunction*  func,
                              const std::vector<expression_ptr>& arg_list)
         : function_(func),
           arg_list_(arg_list)
         {
            value_list_.resize(arg_list.size(),std::numeric_limits<T>::quiet_NaN());
         }

        ~vararg_function_node()
         {
            for (std::size_t i = 0; i < arg_list_.size(); ++i)
            {
               if (arg_list_[i] && !details::is_variable_node(arg_list_[i]))
               {
                  delete arg_list_[i];
                  arg_list_[i] = 0;
               }
            }
         }

         inline bool operator <(const vararg_function_node<T,VarArgFunction>& fn) const
         {
            return this < (&fn);
         }

         inline T value() const
         {
            if (function_)
            {
               populate_value_list();
               return (*function_)(value_list_);
            }
            else
               return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_vafunction;
         }

      private:

         inline void populate_value_list() const
         {
            for (std::size_t i = 0; i < arg_list_.size(); ++i)
            {
               value_list_[i] = arg_list_[i]->value();
            }
         }

         VarArgFunction* function_;
         std::vector<expression_ptr> arg_list_;
         mutable std::vector<T> value_list_;
      };

      #define exprtk_define_unary_op(OpName)                                                                    \
      template <typename T>                                                                                     \
      struct OpName##_op                                                                                        \
      {                                                                                                         \
         typedef typename functor_t<T>::Type Type;                                                              \
         static inline T process(Type v) { return numeric:: OpName (v); }                                       \
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_##OpName; } \
         static inline details::operator_type operation() { return details::e_##OpName; }                       \
      };                                                                                                        \

      exprtk_define_unary_op(abs  )
      exprtk_define_unary_op(acos )
      exprtk_define_unary_op(acosh)
      exprtk_define_unary_op(asin )
      exprtk_define_unary_op(asinh)
      exprtk_define_unary_op(atan )
      exprtk_define_unary_op(atanh)
      exprtk_define_unary_op(ceil )
      exprtk_define_unary_op(cos  )
      exprtk_define_unary_op(cosh )
      exprtk_define_unary_op(cot  )
      exprtk_define_unary_op(csc  )
      exprtk_define_unary_op(d2g  )
      exprtk_define_unary_op(d2r  )
      exprtk_define_unary_op(erf  )
      exprtk_define_unary_op(erfc )
      exprtk_define_unary_op(exp  )
      exprtk_define_unary_op(expm1)
      exprtk_define_unary_op(floor)
      exprtk_define_unary_op(frac )
      exprtk_define_unary_op(g2d  )
      exprtk_define_unary_op(log  )
      exprtk_define_unary_op(log10)
      exprtk_define_unary_op(log2 )
      exprtk_define_unary_op(log1p)
      exprtk_define_unary_op(neg  )
      exprtk_define_unary_op(notl )
      exprtk_define_unary_op(pos  )
      exprtk_define_unary_op(r2d  )
      exprtk_define_unary_op(round)
      exprtk_define_unary_op(sec  )
      exprtk_define_unary_op(sgn  )
      exprtk_define_unary_op(sin  )
      exprtk_define_unary_op(sinh )
      exprtk_define_unary_op(sqrt )
      exprtk_define_unary_op(tan  )
      exprtk_define_unary_op(tanh )
      exprtk_define_unary_op(trunc)
      #undef exprtk_define_unary_op

      template<typename T>
      struct opr_base
      {
         typedef typename details::functor_t<T>::Type Type;
         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::qfunc_t quaternary_functor_t;
         typedef typename functor_t::tfunc_t trinary_functor_t;
         typedef typename functor_t::bfunc_t binary_functor_t;
         typedef typename functor_t::ufunc_t unary_functor_t;
      };

      template <typename T>
      struct add_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(Type t1, Type t2) { return t1 + t2; }
         static inline T process(Type t1, Type t2, Type t3) { return t1 + t2 + t3; }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_add; }
         static inline details::operator_type operation() { return details::e_add; }
      };

      template <typename T>
      struct mul_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(Type t1, Type t2) { return t1 * t2; }
         static inline T process(Type t1, Type t2, Type t3) { return t1 * t2 * t3; }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_mul; }
         static inline details::operator_type operation() { return details::e_mul; }
      };

      template <typename T>
      struct sub_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(Type t1, Type t2) { return t1 - t2; }
         static inline T process(Type t1, Type t2, Type t3) { return t1 - t2 - t3; }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_sub; }
         static inline details::operator_type operation() { return details::e_sub; }
      };

      template <typename T>
      struct div_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(Type t1, Type t2) { return t1 / t2; }
         static inline T process(Type t1, Type t2, Type t3) { return t1 / t2 / t3; }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_div; }
         static inline details::operator_type operation() { return details::e_div; }
      };

      template <typename T>
      struct mod_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(Type t1, Type t2) { return numeric::modulus<T>(t1,t2); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_mod; }
         static inline details::operator_type operation() { return details::e_mod; }
      };

      template <typename T>
      struct pow_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(Type t1, Type t2) { return numeric::pow<T>(t1,t2); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_pow; }
         static inline details::operator_type operation() { return details::e_pow; }
      };

      template <typename T>
      struct lt_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(Type t1, Type t2) { return ((t1 < t2) ? T(1) : T(0)); }
         static inline T process(const std::string& t1, const std::string& t2) { return ((t1 < t2) ? T(1) : T(0)); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_lt; }
         static inline details::operator_type operation() { return details::e_lt; }
      };

      template <typename T>
      struct lte_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(Type t1, Type t2) { return ((t1 <= t2) ? T(1) : T(0)); }
         static inline T process(const std::string& t1, const std::string& t2) { return ((t1 <= t2) ? T(1) : T(0)); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_lte; }
         static inline details::operator_type operation() { return details::e_lte; }
      };

      template <typename T>
      struct gt_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(Type t1, Type t2) { return ((t1 > t2) ? T(1) : T(0)); }
         static inline T process(const std::string& t1, const std::string& t2) { return ((t1 > t2) ? T(1) : T(0)); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_gt; }
         static inline details::operator_type operation() { return details::e_gt; }
      };

      template <typename T>
      struct gte_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(Type t1, Type t2) { return ((t1 >= t2) ? T(1) : T(0)); }
         static inline T process(const std::string& t1, const std::string& t2) { return ((t1 >= t2) ? T(1) : T(0)); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_gte; }
         static inline details::operator_type operation() { return details::e_gte; }
      };

      template <typename T>
      struct eq_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(Type t1, Type t2) { return ((t1 == t2) ? T(1) : T(0)); }
         static inline T process(const std::string& t1, const std::string& t2) { return ((t1 == t2) ? T(1) : T(0)); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_eq; }
         static inline details::operator_type operation() { return details::e_eq; }
      };

      template <typename T>
      struct ne_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(Type t1, Type t2) { return ((t1 != t2) ? T(1) : T(0)); }
         static inline T process(const std::string& t1, const std::string& t2) { return ((t1 != t2) ? T(1) : T(0)); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_ne; }
         static inline details::operator_type operation() { return details::e_ne; }
      };

      template <typename T>
      struct and_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(Type t1, Type t2) { return (details::is_true(t1) && details::is_true(t2)) ? T(1) : T(0); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_and; }
         static inline details::operator_type operation() { return details::e_and; }
      };

      template <typename T>
      struct nand_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(Type t1, Type t2) { return (details::is_true(t1) && details::is_true(t2)) ? T(0) : T(1); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_nand; }
         static inline details::operator_type operation() { return details::e_nand; }
      };

      template <typename T>
      struct or_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(Type t1, Type t2) { return (details::is_true(t1) || details::is_true(t2)) ? T(1) : T(0); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_or; }
         static inline details::operator_type operation() { return details::e_or; }
      };

      template <typename T>
      struct nor_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(Type t1, Type t2) { return (details::is_true(t1) || details::is_true(t2)) ? T(0) : T(1); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_nor; }
         static inline details::operator_type operation() { return details::e_nor; }
      };

      template <typename T>
      struct xor_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(Type t1, Type t2) { return numeric::xor_opr<T>(t1,t2); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_nor; }
         static inline details::operator_type operation() { return details::e_xor; }
      };

      template <typename T>
      struct xnor_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(Type t1, Type t2) { return numeric::xnor_opr<T>(t1,t2); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_nor; }
         static inline details::operator_type operation() { return details::e_xnor; }
      };

      template <typename T>
      struct in_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(const T&, const T&) { return std::numeric_limits<T>::quiet_NaN(); }
         static inline T process(const std::string& t1, const std::string& t2) { return ((std::string::npos != t2.find(t1)) ? T(1) : T(0)); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_in; }
         static inline details::operator_type operation() { return details::e_in; }
      };

      template <typename T>
      struct like_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(const T&, const T&) { return std::numeric_limits<T>::quiet_NaN(); }
         static inline T process(const std::string& t1, const std::string& t2) { return (details::wc_match(t2,t1) ? T(1) : T(0)); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_like; }
         static inline details::operator_type operation() { return details::e_like; }
      };

      template <typename T>
      struct ilike_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(const T&, const T&) { return std::numeric_limits<T>::quiet_NaN(); }
         static inline T process(const std::string& t1, const std::string& t2) { return (details::wc_imatch(t2,t1) ? T(1) : T(0)); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_ilike; }
         static inline details::operator_type operation() { return details::e_ilike; }
      };

      template <typename T>
      struct inrange_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;
         static inline T process(const T& t0, const T& t1, const T& t2) { return ((t0 <= t1) && (t1 <= t2)) ? T(1) : T(0); }
         static inline T process(const std::string& t0, const std::string& t1, const std::string& t2)
         {
            return ((t0 <= t1) && (t1 <= t2)) ? T(1) : T(0);
         }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_inranges; }
         static inline details::operator_type operation() { return details::e_inrange; }
      };

      template <typename T>
      inline T value(details::expression_node<T>* n)
      {
         return n->value();
      }

      template <typename T>
      inline T value(T* t)
      {
         return (*t);
      }

      template <typename T>
      struct vararg_add_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;

         template <typename Type,
                   typename Allocator,
                   template <typename,typename> class Sequence>
         static inline T process(const Sequence<Type,Allocator>& arglist)
         {
            switch (arglist.size())
            {
               case 0  : return T(0);
               case 1  : return process_1(arglist);
               case 2  : return process_2(arglist);
               case 3  : return process_3(arglist);
               case 4  : return process_4(arglist);
               case 5  : return process_5(arglist);
               default :
                         {
                            T result = T(0);
                            for (std::size_t i = 0; i < arglist.size(); ++i)
                            {
                              result += value(arglist[i]);
                            }
                            return result;
                         }
            }
         }

         template <typename Sequence>
         static inline T process_1(const Sequence& arglist)
         {
            return value(arglist[0]);
         }

         template <typename Sequence>
         static inline T process_2(const Sequence& arglist)
         {
            return value(arglist[0]) + value(arglist[1]);
         }

         template <typename Sequence>
         static inline T process_3(const Sequence& arglist)
         {
            return value(arglist[0]) + value(arglist[1]) +
                   value(arglist[2]);
         }

         template <typename Sequence>
         static inline T process_4(const Sequence& arglist)
         {
            return value(arglist[0]) + value(arglist[1]) +
                   value(arglist[2]) + value(arglist[3]);
         }

         template <typename Sequence>
         static inline T process_5(const Sequence& arglist)
         {
            return value(arglist[0]) + value(arglist[1]) +
                   value(arglist[2]) + value(arglist[3]) +
                   value(arglist[4]);
         }
      };

      template <typename T>
      struct vararg_mul_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;

         template <typename Type,
                   typename Allocator,
                   template <typename,typename> class Sequence>
         static inline T process(const Sequence<Type,Allocator>& arglist)
         {
            switch (arglist.size())
            {
               case 0  : return T(0);
               case 1  : return process_1(arglist);
               case 2  : return process_2(arglist);
               case 3  : return process_3(arglist);
               case 4  : return process_4(arglist);
               case 5  : return process_5(arglist);
               default :
                         {
                            T result = T(value(arglist[0]));
                            for (std::size_t i = 1; i < arglist.size(); ++i)
                            {
                               result *= value(arglist[i]);
                            }
                            return result;
                         }
            }
         }

         template <typename Sequence>
         static inline T process_1(const Sequence& arglist)
         {
            return value(arglist[0]);
         }

         template <typename Sequence>
         static inline T process_2(const Sequence& arglist)
         {
            return value(arglist[0]) * value(arglist[1]);
         }

         template <typename Sequence>
         static inline T process_3(const Sequence& arglist)
         {
            return value(arglist[0]) * value(arglist[1]) *
                   value(arglist[2]);
         }

         template <typename Sequence>
         static inline T process_4(const Sequence& arglist)
         {
            return value(arglist[0]) * value(arglist[1]) *
                   value(arglist[2]) * value(arglist[3]);
         }

         template <typename Sequence>
         static inline T process_5(const Sequence& arglist)
         {
            return value(arglist[0]) * value(arglist[1]) *
                   value(arglist[2]) * value(arglist[3]) *
                   value(arglist[4]);
         }
      };

      template <typename T>
      struct vararg_avg_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;

         template <typename Type,
                   typename Allocator,
                   template <typename,typename> class Sequence>
         static inline T process(const Sequence<Type,Allocator>& arglist)
         {
            switch (arglist.size())
            {
               case 0  : return T(0);
               case 1  : return process_1(arglist);
               case 2  : return process_2(arglist);
               case 3  : return process_3(arglist);
               case 4  : return process_4(arglist);
               case 5  : return process_5(arglist);
               default : return vararg_add_op<T>::process(arglist) / arglist.size();
            }
         }

         template <typename Sequence>
         static inline T process_1(const Sequence& arglist)
         {
            return value(arglist[0]);
         }

         template <typename Sequence>
         static inline T process_2(const Sequence& arglist)
         {
            return (value(arglist[0]) + value(arglist[1])) / T(2);
         }

         template <typename Sequence>
         static inline T process_3(const Sequence& arglist)
         {
            return (value(arglist[0]) + value(arglist[1]) + value(arglist[2])) / T(3);
         }

         template <typename Sequence>
         static inline T process_4(const Sequence& arglist)
         {
            return (value(arglist[0]) + value(arglist[1]) +
                    value(arglist[2]) + value(arglist[3])) / T(4);
         }

         template <typename Sequence>
         static inline T process_5(const Sequence& arglist)
         {
            return (value(arglist[0]) + value(arglist[1]) +
                    value(arglist[2]) + value(arglist[3]) +
                    value(arglist[4])) / T(5);
         }
      };

      template <typename T>
      struct vararg_min_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;

         template <typename Type,
                   typename Allocator,
                   template <typename,typename> class Sequence>
         static inline T process(const Sequence<Type,Allocator>& arglist)
         {
            switch (arglist.size())
            {
               case 0  : return T(0);
               case 1  : return process_1(arglist);
               case 2  : return process_2(arglist);
               case 3  : return process_3(arglist);
               case 4  : return process_4(arglist);
               case 5  : return process_5(arglist);
               default :
                         {
                            T result = T(value(arglist[0]));
                            for (std::size_t i = 1; i < arglist.size(); ++i)
                            {
                               const T v = value(arglist[i]);
                               if (v < result)
                                  result = v;
                            }
                            return result;
                         }
            }
         }

         template <typename Sequence>
         static inline T process_1(const Sequence& arglist)
         {
            return value(arglist[0]);
         }

         template <typename Sequence>
         static inline T process_2(const Sequence& arglist)
         {
            return std::min<T>(value(arglist[0]),value(arglist[1]));
         }

         template <typename Sequence>
         static inline T process_3(const Sequence& arglist)
         {
            return std::min<T>(std::min<T>(value(arglist[0]),value(arglist[1])),value(arglist[2]));
         }

         template <typename Sequence>
         static inline T process_4(const Sequence& arglist)
         {
            return std::min<T>(
                        std::min<T>(value(arglist[0]),value(arglist[1])),
                        std::min<T>(value(arglist[2]),value(arglist[3])));
         }

         template <typename Sequence>
         static inline T process_5(const Sequence& arglist)
         {
            return std::min<T>(
                   std::min<T>(std::min<T>(value(arglist[0]),value(arglist[1])),
                               std::min<T>(value(arglist[2]),value(arglist[3]))),
                               value(arglist[4]));
         }
      };

      template <typename T>
      struct vararg_max_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;

         template <typename Type,
                   typename Allocator,
                   template <typename,typename> class Sequence>
         static inline T process(const Sequence<Type,Allocator>& arglist)
         {
            switch (arglist.size())
            {
               case 0  : return T(0);
               case 1  : return process_1(arglist);
               case 2  : return process_2(arglist);
               case 3  : return process_3(arglist);
               case 4  : return process_4(arglist);
               case 5  : return process_5(arglist);
               default :
                         {
                            T result = T(value(arglist[0]));
                            for (std::size_t i = 1; i < arglist.size(); ++i)
                            {
                               const T v = value(arglist[i]);
                               if (v > result)
                                  result = v;
                            }
                            return result;
                         }
            }
         }

         template <typename Sequence>
         static inline T process_1(const Sequence& arglist)
         {
            return value(arglist[0]);
         }

         template <typename Sequence>
         static inline T process_2(const Sequence& arglist)
         {
            return std::max<T>(value(arglist[0]),value(arglist[1]));
         }

         template <typename Sequence>
         static inline T process_3(const Sequence& arglist)
         {
            return std::max<T>(std::max<T>(value(arglist[0]),value(arglist[1])),value(arglist[2]));
         }

         template <typename Sequence>
         static inline T process_4(const Sequence& arglist)
         {
            return std::max<T>(
                        std::max<T>(value(arglist[0]),value(arglist[1])),
                        std::max<T>(value(arglist[2]),value(arglist[3])));
         }

         template <typename Sequence>
         static inline T process_5(const Sequence& arglist)
         {
            return std::max<T>(
                   std::max<T>(std::max<T>(value(arglist[0]),value(arglist[1])),
                               std::max<T>(value(arglist[2]),value(arglist[3]))),
                               value(arglist[4]));
         }
      };

      template <typename T>
      struct vararg_mand_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;

         template <typename Type,
                   typename Allocator,
                   template <typename,typename> class Sequence>
         static inline T process(const Sequence<Type,Allocator>& arglist)
         {
            switch (arglist.size())
            {
               case 1  : return process_1(arglist);
               case 2  : return process_2(arglist);
               case 3  : return process_3(arglist);
               case 4  : return process_4(arglist);
               case 5  : return process_5(arglist);
               default :
                         {
                            for (std::size_t i = 0; i < arglist.size(); ++i)
                            {
                               if (T(0) == value(arglist[i]))
                                  return T(0);
                            }
                            return T(1);
                         }
            }
         }

         template <typename Sequence>
         static inline T process_1(const Sequence& arglist)
         {
            return (T(0) != value(arglist[0])) ? T(1) : T(0);
         }

         template <typename Sequence>
         static inline T process_2(const Sequence& arglist)
         {
            return (
                    (T(0) != value(arglist[0])) &&
                    (T(0) != value(arglist[1]))
                   ) ? T(1) : T(0);
         }

         template <typename Sequence>
         static inline T process_3(const Sequence& arglist)
         {
            return (
                    (T(0) != value(arglist[0])) &&
                    (T(0) != value(arglist[1])) &&
                    (T(0) != value(arglist[2]))
                   ) ? T(1) : T(0);
         }

         template <typename Sequence>
         static inline T process_4(const Sequence& arglist)
         {
            return (
                    (T(0) != value(arglist[0])) &&
                    (T(0) != value(arglist[1])) &&
                    (T(0) != value(arglist[2])) &&
                    (T(0) != value(arglist[3]))
                   ) ? T(1) : T(0);
         }

         template <typename Sequence>
         static inline T process_5(const Sequence& arglist)
         {
            return (
                    (T(0) != value(arglist[0])) &&
                    (T(0) != value(arglist[1])) &&
                    (T(0) != value(arglist[2])) &&
                    (T(0) != value(arglist[3])) &&
                    (T(0) != value(arglist[4]))
                   ) ? T(1) : T(0);
         }
      };

      template <typename T>
      struct vararg_mor_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;

         template <typename Type,
                   typename Allocator,
                   template <typename,typename> class Sequence>
         static inline T process(const Sequence<Type,Allocator>& arglist)
         {
            switch (arglist.size())
            {
               case 1  : return process_1(arglist);
               case 2  : return process_2(arglist);
               case 3  : return process_3(arglist);
               case 4  : return process_4(arglist);
               case 5  : return process_5(arglist);
               default :
                         {
                            for (std::size_t i = 0; i < arglist.size(); ++i)
                            {
                               if (T(0) != value(arglist[i]))
                                  return T(1);
                            }
                            return T(0);
                         }
            }
         }

         template <typename Sequence>
         static inline T process_1(const Sequence& arglist)
         {
            return (T(0) != value(arglist[0])) ? T(1) : T(0);
         }

         template <typename Sequence>
         static inline T process_2(const Sequence& arglist)
         {
            return (
                    (T(0) != value(arglist[0])) ||
                    (T(0) != value(arglist[1]))
                   ) ? T(1) : T(0);
         }

         template <typename Sequence>
         static inline T process_3(const Sequence& arglist)
         {
            return (
                    (T(0) != value(arglist[0])) ||
                    (T(0) != value(arglist[1])) ||
                    (T(0) != value(arglist[2]))
                   ) ? T(1) : T(0);
         }

         template <typename Sequence>
         static inline T process_4(const Sequence& arglist)
         {
            return (
                    (T(0) != value(arglist[0])) ||
                    (T(0) != value(arglist[1])) ||
                    (T(0) != value(arglist[2])) ||
                    (T(0) != value(arglist[3]))
                   ) ? T(1) : T(0);
         }

         template <typename Sequence>
         static inline T process_5(const Sequence& arglist)
         {
            return (
                    (T(0) != value(arglist[0])) ||
                    (T(0) != value(arglist[1])) ||
                    (T(0) != value(arglist[2])) ||
                    (T(0) != value(arglist[3])) ||
                    (T(0) != value(arglist[4]))
                   ) ? T(1) : T(0);
         }
      };

      template <typename T>
      struct vararg_multi_op : public opr_base<T>
      {
         typedef typename opr_base<T>::Type Type;

         template <typename Type,
                   typename Allocator,
                   template <typename,typename> class Sequence>
         static inline T process(const Sequence<Type,Allocator>& arglist)
         {
            switch (arglist.size())
            {
               case 0  : return std::numeric_limits<T>::quiet_NaN();
               case 1  : return process_1(arglist);
               case 2  : return process_2(arglist);
               case 3  : return process_3(arglist);
               case 4  : return process_4(arglist);
               case 5  : return process_5(arglist);
               case 6  : return process_6(arglist);
               case 7  : return process_7(arglist);
               case 8  : return process_8(arglist);
               default :
                         {
                            for (std::size_t i = 0; i < (arglist.size() - 1); ++i)
                            {
                               value(arglist[i]);
                            }
                            return value(arglist.back());
                         }
            }
         }

         template <typename Sequence>
         static inline T process_1(const Sequence& arglist)
         {
            return value(arglist[0]);
         }

         template <typename Sequence>
         static inline T process_2(const Sequence& arglist)
         {
                   value(arglist[0]);
            return value(arglist[1]);
         }

         template <typename Sequence>
         static inline T process_3(const Sequence& arglist)
         {
                   value(arglist[0]);
                   value(arglist[1]);
            return value(arglist[2]);
         }

         template <typename Sequence>
         static inline T process_4(const Sequence& arglist)
         {
                   value(arglist[0]);
                   value(arglist[1]);
                   value(arglist[2]);
            return value(arglist[3]);
         }

         template <typename Sequence>
         static inline T process_5(const Sequence& arglist)
         {
                   value(arglist[0]);
                   value(arglist[1]);
                   value(arglist[2]);
                   value(arglist[3]);
            return value(arglist[4]);
         }

         template <typename Sequence>
         static inline T process_6(const Sequence& arglist)
         {
                   value(arglist[0]);
                   value(arglist[1]);
                   value(arglist[2]);
                   value(arglist[3]);
                   value(arglist[4]);
            return value(arglist[5]);
         }

         template <typename Sequence>
         static inline T process_7(const Sequence& arglist)
         {
                   value(arglist[0]);
                   value(arglist[1]);
                   value(arglist[2]);
                   value(arglist[3]);
                   value(arglist[4]);
                   value(arglist[5]);
            return value(arglist[6]);
         }

         template <typename Sequence>
         static inline T process_8(const Sequence& arglist)
         {
                   value(arglist[0]);
                   value(arglist[1]);
                   value(arglist[2]);
                   value(arglist[3]);
                   value(arglist[4]);
                   value(arglist[5]);
                   value(arglist[6]);
            return value(arglist[7]);
         }
      };

      template <typename T>
      class vov_base_node : public expression_node<T>
      {
      public:

         inline virtual operator_type operation() const
         {
            return details::e_default;
         }

         virtual const T& v0() const = 0;

         virtual const T& v1() const = 0;
      };

      template <typename T>
      class cov_base_node : public expression_node<T>
      {
      public:

         inline virtual operator_type operation() const
         {
            return details::e_default;
         }

         virtual const T c() const = 0;

         virtual const T& v() const = 0;
      };

      template <typename T>
      class voc_base_node : public expression_node<T>
      {
      public:

         inline virtual operator_type operation() const
         {
            return details::e_default;
         }

         virtual const T c() const = 0;

         virtual const T& v() const = 0;
      };

      template <typename T>
      class vob_base_node : public expression_node<T>
      {
      public:

         virtual const T& v() const = 0;
      };

      template <typename T>
      class bov_base_node : public expression_node<T>
      {
      public:

         virtual const T& v() const = 0;
      };

      template <typename T>
      class cob_base_node : public expression_node<T>
      {
      public:

         inline virtual operator_type operation() const
         {
            return details::e_default;
         }

         virtual const T c() const = 0;

         virtual void set_c(const T) = 0;

         virtual expression_node<T>* move_branch(const std::size_t& index) = 0;
      };

      template <typename T>
      class boc_base_node : public expression_node<T>
      {
      public:

         inline virtual operator_type operation() const
         {
            return details::e_default;
         }

         virtual const T c() const = 0;

         virtual void set_c(const T) = 0;
      };

      template <typename T>
      class uv_base_node : public expression_node<T>
      {
      public:

         inline virtual operator_type operation() const
         {
            return details::e_default;
         }

         virtual const T& v() const = 0;
      };

      template <typename T>
      class sos_base_node : public expression_node<T>
      {
      public:

         inline virtual operator_type operation() const
         {
            return details::e_default;
         }
      };

      template <typename T>
      class sosos_base_node : public expression_node<T>
      {
      public:

         inline virtual operator_type operation() const
         {
            return details::e_default;
         }
      };

      template <typename T>
      class T0oT1oT2_base_node : public expression_node<T>
      {
      public:

         virtual std::string type_id() const = 0;
      };

      template <typename T>
      class T0oT1oT2oT3_base_node : public expression_node<T>
      {
      public:

         virtual std::string type_id() const = 0;
      };

      template <typename T, typename Operation>
      class unary_variable_node : public uv_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         explicit unary_variable_node(const T& v)
         : v_(v)
         {}

         inline T value() const
         {
            return Operation::process(v_);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return Operation::type();
         }

         inline operator_type operation() const
         {
            return Operation::operation();
         }

         inline const T& v() const
         {
            return v_;
         }

      private:

         unary_variable_node(unary_variable_node<T,Operation>&);
         unary_variable_node<T,Operation>& operator=(unary_variable_node<T,Operation>&);

         const T& v_;
      };

      template <typename T>
      class uvouv_node : public expression_node<T>
      {
      public:

         // UOpr1(v0) Op UOpr2(v1)

         typedef expression_node<T>* expression_ptr;
         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::bfunc_t bfunc_t;
         typedef typename functor_t::ufunc_t ufunc_t;

         explicit uvouv_node(const T& v0,const T& v1, ufunc_t u0, ufunc_t u1, bfunc_t f)
         : v0_(v0),
           v1_(v1),
           u0_(u0),
           u1_(u1),
           f_ (f )
         {}

         inline T value() const
         {
            return f_(u0_(v0_),u1_(v1_));
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_uvouv;
         }

         inline operator_type operation() const
         {
            return details::e_default;
         }

         inline const T& v0()
         {
            return v0_;
         }

         inline const T& v1()
         {
            return v1_;
         }

         inline ufunc_t u0()
         {
            return u0_;
         }

         inline ufunc_t u1()
         {
            return u1_;
         }

         inline ufunc_t f()
         {
            return f_;
         }

      private:

         uvouv_node(uvouv_node<T>&);
         uvouv_node<T>& operator=(uvouv_node<T>&);

         const T& v0_;
         const T& v1_;
         const ufunc_t u0_;
         const ufunc_t u1_;
         const bfunc_t f_;
      };

      template <typename T, typename Operation>
      class unary_branch_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         explicit unary_branch_node(expression_ptr branch)
         : branch_(branch),
           branch_deletable_(branch_deletable(branch_))
         {}

        ~unary_branch_node()
         {
            if (branch_ && branch_deletable_)
            {
               delete branch_;
               branch_ = 0;
            }
         }

         inline T value() const
         {
            return Operation::process(branch_->value());
         }

         inline typename expression_node<T>::node_type type() const
         {
            return Operation::type();
         }

         inline operator_type operation() const
         {
            return Operation::operation();
         }

      private:

         unary_branch_node(unary_branch_node<T,Operation>&);
         unary_branch_node<T,Operation>& operator=(unary_branch_node<T,Operation>&);

         expression_ptr branch_;
         bool           branch_deletable_;
      };

      template <typename T> struct is_const                { enum {result = 0}; };
      template <typename T> struct is_const <const T>      { enum {result = 1}; };
      template <typename T> struct is_const_ref            { enum {result = 0}; };
      template <typename T> struct is_const_ref <const T&> { enum {result = 1}; };
      template <typename T> struct is_ref                  { enum {result = 0}; };
      template <typename T> struct is_ref<T&>              { enum {result = 1}; };
      template <typename T> struct is_ref<const T&>        { enum {result = 0}; };

      template <std::size_t State>
      struct param_to_str { static std::string result() { static const std::string r("v"); return r; } };

      template <>
      struct param_to_str<0> { static std::string result() { static const std::string r("c"); return r; } };

      template <typename T>
      struct T0oT1oT2process
      {
         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::bfunc_t bfunc_t;

         struct mode0
         {
            static inline T process(const T& t0, const T& t1, const T& t2, const bfunc_t bf0, const bfunc_t bf1)
            {
               // (T0 o0 T1) o1 T2
               return bf1(bf0(t0,t1),t2);
            }

            template <typename T0, typename T1, typename T2>
            static inline std::string id()
            {
               static const std::string result = "(" + param_to_str<is_const_ref<T0>::result>::result() + "o"   +
                                                       param_to_str<is_const_ref<T1>::result>::result() + ")o(" +
                                                       param_to_str<is_const_ref<T2>::result>::result() + ")"   ;
               return result;
            }
         };

         struct mode1
         {
            static inline T process(const T& t0, const T& t1, const T& t2, const bfunc_t bf0, const bfunc_t bf1)
            {
               // T0 o0 (T1 o1 T2)
               return bf0(t0,bf1(t1,t2));
            }

            template <typename T0, typename T1, typename T2>
            static inline std::string id()
            {
               static const std::string result = "(" + param_to_str<is_const_ref<T0>::result>::result() + ")o(" +
                                                       param_to_str<is_const_ref<T1>::result>::result() + "o"   +
                                                       param_to_str<is_const_ref<T2>::result>::result() + ")"   ;
               return result;
            }
         };
      };

      template <typename T>
      struct T0oT1oT20T3process
      {
         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::bfunc_t bfunc_t;

         struct mode0
         {
            static inline T process(const T& t0, const T& t1, const T& t2, const T& t3, const bfunc_t bf0, const bfunc_t bf1, const bfunc_t bf2)
            {
               // (T0 o0 T1) o1 (T2 o2 T3)
               return bf1(bf0(t0,t1),bf2(t2,t3));
            }

            template <typename T0, typename T1, typename T2, typename T3>
            static inline std::string id()
            {
               static const std::string result = "(" + param_to_str<is_const_ref<T0>::result>::result() + "o"  +
                                                       param_to_str<is_const_ref<T1>::result>::result() + ")o" +
                                                 "(" + param_to_str<is_const_ref<T2>::result>::result() + "o"  +
                                                       param_to_str<is_const_ref<T3>::result>::result() + ")"  ;
               return result;
            }
         };

         struct mode1
         {
            static inline T process(const T& t0, const T& t1, const T& t2, const T& t3, const bfunc_t bf0, const bfunc_t bf1, const bfunc_t bf2)
            {
               // (T0 o0 (T1 o1 (T2 o2 T3))
               return bf0(t0,bf1(t1,bf2(t2,t3)));
            }
            template <typename T0, typename T1, typename T2, typename T3>
            static inline std::string id()
            {
               static const std::string result = "(" + param_to_str<is_const_ref<T0>::result>::result() +  ")o((" +
                                                       param_to_str<is_const_ref<T1>::result>::result() +  ")o("  +
                                                       param_to_str<is_const_ref<T2>::result>::result() +  "o"    +
                                                       param_to_str<is_const_ref<T3>::result>::result() +  "))"   ;
               return result;
            }
         };

         struct mode2
         {
            static inline T process(const T& t0, const T& t1, const T& t2, const T& t3, const bfunc_t bf0, const bfunc_t bf1, const bfunc_t bf2)
            {
               // (T0 o0 ((T1 o1 T2) o2 T3)
               return bf0(t0,bf2(bf1(t1,t2),t3));
            }

            template <typename T0, typename T1, typename T2, typename T3>
            static inline std::string id()
            {
               static const std::string result = "(" + param_to_str<is_const_ref<T0>::result>::result() + ")o((" +
                                                       param_to_str<is_const_ref<T1>::result>::result() + "o"    +
                                                       param_to_str<is_const_ref<T2>::result>::result() + ")o("  +
                                                       param_to_str<is_const_ref<T3>::result>::result() + "))"   ;
               return result;
            }
         };

         struct mode3
         {
            static inline T process(const T& t0, const T& t1, const T& t2, const T& t3, const bfunc_t bf0, const bfunc_t bf1, const bfunc_t bf2)
            {
               // (((T0 o0 T1) o1 T2) o2 T3)
               return bf2(bf1(bf0(t0,t1),t2),t3);
            }

            template <typename T0, typename T1, typename T2, typename T3>
            static inline std::string id()
            {
               static const std::string result = "((" + param_to_str<is_const_ref<T0>::result>::result() + "o"    +
                                                        param_to_str<is_const_ref<T1>::result>::result() + ")o("  +
                                                        param_to_str<is_const_ref<T2>::result>::result() + "))o(" +
                                                        param_to_str<is_const_ref<T3>::result>::result() + ")";
               return result;
            }
         };

         struct mode4
         {
            static inline T process(const T& t0, const T& t1, const T& t2, const T& t3, const bfunc_t bf0, const bfunc_t bf1, const bfunc_t bf2)
            {
               // ((T0 o0 (T1 o1 T2)) o2 T3
               return bf2(bf0(t0,bf1(t1,t2)),t3);
            }

            template <typename T0, typename T1, typename T2, typename T3>
            static inline std::string id()
            {
               static const std::string result = "((" + param_to_str<is_const_ref<T0>::result>::result() + ")o("  +
                                                        param_to_str<is_const_ref<T1>::result>::result() + "o"    +
                                                        param_to_str<is_const_ref<T2>::result>::result() + "))o(" +
                                                        param_to_str<is_const_ref<T3>::result>::result() + ")"    ;
               return result;
            }
         };
      };

      template <typename T, typename T0, typename T1>
      struct nodetype_T0oT1 { static const typename expression_node<T>::node_type result; };
      template <typename T, typename T0, typename T1>
      const typename expression_node<T>::node_type nodetype_T0oT1<T,T0,T1>::result = expression_node<T>::e_none;

      #define synthnode_type_define(T0_,T1_,v_)                                                                 \
      template <typename T, typename T0, typename T1>                                                           \
      struct nodetype_T0oT1<T,T0_,T1_> { static const typename expression_node<T>::node_type result; };         \
      template <typename T, typename T0, typename T1>                                                           \
      const typename expression_node<T>::node_type nodetype_T0oT1<T,T0_,T1_>::result = expression_node<T>:: v_; \

      synthnode_type_define(const T0&,const T1&, e_vov)
      synthnode_type_define(const T0&,const T1 , e_voc)
      synthnode_type_define(const T0 ,const T1&, e_cov)
      synthnode_type_define(      T0&,      T1&,e_none)
      synthnode_type_define(const T0 ,const T1 ,e_none)
      synthnode_type_define(      T0&,const T1 ,e_none)
      synthnode_type_define(const T0 ,      T1&,e_none)
      synthnode_type_define(const T0&,      T1&,e_none)
      synthnode_type_define(      T0&,const T1&,e_none)
      #undef synthnode_type_define

      template <typename T, typename T0, typename T1, typename T2>
      struct nodetype_T0oT1oT2 { static const typename expression_node<T>::node_type result; };
      template <typename T, typename T0, typename T1, typename T2>
      const typename expression_node<T>::node_type nodetype_T0oT1oT2<T,T0,T1,T2>::result = expression_node<T>::e_none;

      #define synthnode_type_define(T0_,T1_,T2_,v_)                                                                    \
      template <typename T, typename T0, typename T1, typename T2>                                                     \
      struct nodetype_T0oT1oT2<T,T0_,T1_,T2_> { static const typename expression_node<T>::node_type result; };         \
      template <typename T, typename T0, typename T1, typename T2>                                                     \
      const typename expression_node<T>::node_type nodetype_T0oT1oT2<T,T0_,T1_,T2_>::result = expression_node<T>:: v_; \

      synthnode_type_define(const T0&,const T1&,const T2&, e_vovov)
      synthnode_type_define(const T0&,const T1&,const T2 , e_vovoc)
      synthnode_type_define(const T0&,const T1 ,const T2&, e_vocov)
      synthnode_type_define(const T0 ,const T1&,const T2&, e_covov)
      synthnode_type_define(const T0 ,const T1&,const T2 , e_covoc)
      synthnode_type_define(const T0 ,const T1 ,const T2 , e_none )
      synthnode_type_define(const T0 ,const T1 ,const T2&, e_none )
      synthnode_type_define(const T0&,const T1 ,const T2 , e_none )
      synthnode_type_define(      T0&,      T1&,      T2&, e_none )
      #undef synthnode_type_define

      template <typename T, typename T0, typename T1, typename T2, typename T3>
      struct nodetype_T0oT1oT2oT3 { static const typename expression_node<T>::node_type result; };
      template <typename T, typename T0, typename T1, typename T2, typename T3>
      const typename expression_node<T>::node_type nodetype_T0oT1oT2oT3<T,T0,T1,T2,T3>::result = expression_node<T>::e_none;

      #define synthnode_type_define(T0_,T1_,T2_,T3_,v_)                                                                       \
      template <typename T, typename T0, typename T1, typename T2, typename T3>                                               \
      struct nodetype_T0oT1oT2oT3<T,T0_,T1_,T2_,T3_> { static const typename expression_node<T>::node_type result; };         \
      template <typename T, typename T0, typename T1, typename T2, typename T3>                                               \
      const typename expression_node<T>::node_type nodetype_T0oT1oT2oT3<T,T0_,T1_,T2_,T3_>::result = expression_node<T>:: v_; \

      synthnode_type_define(const T0&,const T1&,const T2&, const T3&,e_vovovov)
      synthnode_type_define(const T0&,const T1&,const T2&, const T3 ,e_vovovoc)
      synthnode_type_define(const T0&,const T1&,const T2 , const T3&,e_vovocov)
      synthnode_type_define(const T0&,const T1 ,const T2&, const T3&,e_vocovov)
      synthnode_type_define(const T0 ,const T1&,const T2&, const T3&,e_covovov)
      synthnode_type_define(const T0 ,const T1&,const T2 , const T3&,e_covocov)
      synthnode_type_define(const T0&,const T1 ,const T2&, const T3 ,e_vocovoc)
      synthnode_type_define(const T0 ,const T1&,const T2&, const T3 ,e_covovoc)
      synthnode_type_define(const T0&,const T1 ,const T2 , const T3&,e_vococov)
      synthnode_type_define(const T0 ,const T1 ,const T2 , const T3 ,e_none   )
      synthnode_type_define(const T0 ,const T1 ,const T2 , const T3&,e_none   )
      synthnode_type_define(const T0 ,const T1 ,const T2&, const T3 ,e_none   )
      synthnode_type_define(const T0 ,const T1&,const T2 , const T3 ,e_none   )
      synthnode_type_define(const T0&,const T1 ,const T2 , const T3 ,e_none   )
      synthnode_type_define(const T0 ,const T1 ,const T2&, const T3&,e_none   )
      synthnode_type_define(const T0&,const T1&,const T2 , const T3 ,e_none   )
      #undef synthnode_type_define

      template <typename T, typename T0, typename T1>
      class T0oT1 : public expression_node<T>
      {
      public:

         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::bfunc_t bfunc_t;
         typedef T value_type;
         typedef T0oT1<T,T0,T1> node_type;

         T0oT1(T0 t0, T1 t1, const bfunc_t f)
         : t0_(t0),
           t1_(t1),
           f_(f)
         {}

         inline typename expression_node<T>::node_type type() const
         {
            static const typename expression_node<T>::node_type result = nodetype_T0oT1<T,T0,T1>::result;
            return result;
         }

         inline operator_type operation() const
         {
            return e_default;
         }

         inline T value() const
         {
            return f_(t0_,t1_);
         }

         inline T0 t0() const
         {
            return t0_;
         }

         inline T1 t1() const
         {
            return t1_;
         }

         inline bfunc_t f() const
         {
            return f_;
         }

         template <typename Allocator>
         static inline expression_node<T>* allocate(Allocator& allocator,
                                                    T0 t0, T1 t1,
                                                    bfunc_t f)
         {
            return allocator.template allocate_type<node_type,T0,T1,bfunc_t&>(t0,t1,f);
         }

      private:

         T0oT1(T0oT1<T,T0,T1>&) {}
         T0oT1<T,T0,T1>& operator=(T0oT1<T,T0,T1>&) { return *this; }

         T0 t0_;
         T1 t1_;
         const bfunc_t f_;
      };

      template <typename T, typename T0, typename T1, typename T2, typename ProcessMode>
      class T0oT1oT2 : public T0oT1oT2_base_node<T>
      {
      public:

         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::bfunc_t bfunc_t;
         typedef T value_type;
         typedef T0oT1oT2<T,T0,T1,T2,ProcessMode> node_type;
         typedef ProcessMode process_mode_t;

         T0oT1oT2(T0 t0, T1 t1, T2 t2, const bfunc_t f0, const bfunc_t f1)
         : t0_(t0),
           t1_(t1),
           t2_(t2),
           f0_(f0),
           f1_(f1)
         {}

         inline typename expression_node<T>::node_type type() const
         {
            static const typename expression_node<T>::node_type result = nodetype_T0oT1oT2<T,T0,T1,T2>::result;
            return result;
         }

         inline operator_type operation() const
         {
            return e_default;
         }

         inline T value() const
         {
            return ProcessMode::process(t0_,t1_,t2_,f0_,f1_);
         }

         inline T0 t0() const
         {
            return t0_;
         }

         inline T1 t1() const
         {
            return t1_;
         }

         inline T2 t2() const
         {
            return t2_;
         }

         bfunc_t f0() const
         {
            return f0_;
         }

         bfunc_t f1() const
         {
            return f1_;
         }

         std::string type_id() const
         {
            return id();
         }

         static inline std::string id()
         {
            return process_mode_t::template id<T0,T1,T2>();
         }

         template <typename Allocator>
         static inline expression_node<T>* allocate(Allocator& allocator, T0 t0, T1 t1, T2 t2, bfunc_t f0, bfunc_t f1)
         {
            return allocator.template allocate_type<node_type,T0,T1,T2,bfunc_t,bfunc_t>(t0,t1,t2,f0,f1);
         }

      private:

         T0oT1oT2(node_type&) {}
         node_type& operator=(node_type&) { return *this; }

         T0 t0_;
         T1 t1_;
         T2 t2_;
         const bfunc_t f0_;
         const bfunc_t f1_;
      };

      template <typename T, typename T0_, typename T1_, typename T2_, typename T3_, typename ProcessMode>
      class T0oT1oT2oT3 : public T0oT1oT2oT3_base_node<T>
      {
      public:

         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::bfunc_t bfunc_t;
         typedef T value_type;
         typedef T0_ T0;
         typedef T1_ T1;
         typedef T2_ T2;
         typedef T3_ T3;
         typedef T0oT1oT2oT3<T,T0,T1,T2,T3,ProcessMode> node_type;
         typedef ProcessMode process_mode_t;

         T0oT1oT2oT3(T0 t0, T1 t1, T2 t2, T3 t3, bfunc_t f0, bfunc_t f1, bfunc_t f2)
         : t0_(t0),
           t1_(t1),
           t2_(t2),
           t3_(t3),
           f0_(f0),
           f1_(f1),
           f2_(f2)
         {}

         inline T value() const
         {
            return ProcessMode::process(t0_,t1_,t2_,t3_,f0_,f1_,f2_);
         }

         inline T0 t0() const
         {
            return t0_;
         }

         inline T1 t1() const
         {
            return t1_;
         }

         inline T2 t2() const
         {
            return t2_;
         }

         inline T3 t3() const
         {
            return t3_;
         }

         inline bfunc_t f0() const
         {
            return f0_;
         }

         inline bfunc_t f1() const
         {
            return f1_;
         }

         inline bfunc_t f2() const
         {
            return f2_;
         }

         inline std::string type_id() const
         {
            return id();
         }

         static inline std::string id()
         {
            return process_mode_t::template id<T0,T1,T2,T3>();
         }

         template <typename Allocator>
         static inline expression_node<T>* allocate(Allocator& allocator,
                                                    T0 t0, T1 t1, T2 t2, T3 t3,
                                                    bfunc_t f0, bfunc_t f1, bfunc_t f2)
         {
            return allocator.template allocate_type<node_type,T0,T1,T2,T3,bfunc_t,bfunc_t>(t0,t1,t2,t3,f0,f1,f2);
         }

      private:

         T0oT1oT2oT3(node_type&) {}
         node_type& operator=(node_type&) { return *this; }

         T0 t0_;
         T1 t1_;
         T2 t2_;
         T3 t3_;
         const bfunc_t f0_;
         const bfunc_t f1_;
         const bfunc_t f2_;
      };

      template <typename T, typename T0, typename T1, typename T2>
      class T0oT1oT2_sf3 : public T0oT1oT2_base_node<T>
      {
      public:

         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::tfunc_t tfunc_t;
         typedef T value_type;
         typedef T0oT1oT2_sf3<T,T0,T1,T2> node_type;

         T0oT1oT2_sf3(T0 t0, T1 t1, T2 t2, const tfunc_t f)
         : t0_(t0),
           t1_(t1),
           t2_(t2),
           f_(f)
         {}

         inline typename expression_node<T>::node_type type() const
         {
            static const typename expression_node<T>::node_type result = nodetype_T0oT1oT2<T,T0,T1,T2>::result;
            return result;
         }

         inline operator_type operation() const
         {
            return e_default;
         }

         inline T value() const
         {
            return f_(t0_,t1_,t2_);
         }

         inline T0 t0() const
         {
            return t0_;
         }

         inline T1 t1() const
         {
            return t1_;
         }

         inline T2 t2() const
         {
            return t2_;
         }

         tfunc_t f() const
         {
            return f_;
         }

         std::string type_id() const
         {
            return id();
         }

         static inline std::string id()
         {
            return "sf3";
         }

         template <typename Allocator>
         static inline expression_node<T>* allocate(Allocator& allocator, T0 t0, T1 t1, T2 t2, tfunc_t f)
         {
            return allocator.template allocate_type<node_type,T0,T1,T2,tfunc_t>(t0,t1,t2,f);
         }

      private:

         T0oT1oT2_sf3(node_type&) {}
         node_type& operator=(node_type&) { return *this; }

         T0 t0_;
         T1 t1_;
         T2 t2_;
         const tfunc_t f_;
      };

      template <typename T, typename T0, typename T1, typename T2>
      class sf3ext_type_node : public T0oT1oT2_base_node<T>
      {
      public:

         virtual T0 t0() const = 0;

         virtual T1 t1() const = 0;

         virtual T2 t2() const = 0;
      };

      template <typename T, typename T0, typename T1, typename T2, typename SF3Operation>
      class T0oT1oT2_sf3ext : public sf3ext_type_node<T,T0,T1,T2>
      {
      public:

         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::tfunc_t tfunc_t;
         typedef T value_type;
         typedef T0oT1oT2_sf3ext<T,T0,T1,T2,SF3Operation> node_type;

         T0oT1oT2_sf3ext(T0 t0, T1 t1, T2 t2)
         : t0_(t0),
           t1_(t1),
           t2_(t2)
         {}

         inline typename expression_node<T>::node_type type() const
         {
            static const typename expression_node<T>::node_type result = nodetype_T0oT1oT2<T,T0,T1,T2>::result;
            return result;
         }

         inline operator_type operation() const
         {
            return e_default;
         }

         inline T value() const
         {
            return SF3Operation::process(t0_,t1_,t2_);
         }

         T0 t0() const
         {
            return t0_;
         }

         T1 t1() const
         {
            return t1_;
         }

         T2 t2() const
         {
            return t2_;
         }

         std::string type_id() const
         {
            return id();
         }

         static inline std::string id()
         {
            return SF3Operation::id();
         }

         template <typename Allocator>
         static inline expression_node<T>* allocate(Allocator& allocator, T0 t0, T1 t1, T2 t2)
         {
            return allocator.template allocate_type<node_type,T0,T1,T2>(t0,t1,t2);
         }

      private:

         T0oT1oT2_sf3ext(node_type&) {}
         node_type& operator=(node_type&) { return *this; }

         T0 t0_;
         T1 t1_;
         T2 t2_;
      };

      template <typename T>
      inline bool is_sf3ext_node(const expression_node<T>* n)
      {
         switch (n->type())
         {
            case expression_node<T>::e_vovov : return true;
            case expression_node<T>::e_vovoc : return true;
            case expression_node<T>::e_vocov : return true;
            case expression_node<T>::e_covov : return true;
            case expression_node<T>::e_covoc : return true;
            default                          : return false;
         }
      }

      template <typename T, typename T0, typename T1, typename T2, typename T3>
      class T0oT1oT2oT3_sf4 : public T0oT1oT2_base_node<T>
      {
      public:

         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::qfunc_t qfunc_t;
         typedef T value_type;
         typedef T0oT1oT2oT3_sf4<T,T0,T1,T2,T3> node_type;

         T0oT1oT2oT3_sf4(T0 t0, T1 t1, T2 t2, T3 t3, const qfunc_t f)
         : t0_(t0),
           t1_(t1),
           t2_(t2),
           t3_(t3),
           f_(f)
         {}

         inline typename expression_node<T>::node_type type() const
         {
            static const typename expression_node<T>::node_type result = nodetype_T0oT1oT2oT3<T,T0,T1,T2,T3>::result;
            return result;
         }

         inline operator_type operation() const
         {
            return e_default;
         }

         inline T value() const
         {
            return f_(t0_,t1_,t2_,t3_);
         }

         inline T0 t0() const
         {
            return t0_;
         }

         inline T1 t1() const
         {
            return t1_;
         }

         inline T2 t2() const
         {
            return t2_;
         }

         inline T3 t3() const
         {
            return t3_;
         }

         qfunc_t f() const
         {
            return f_;
         }

         std::string type_id() const
         {
            return id();
         }

         static inline std::string id()
         {
            return "sf4";
         }

         template <typename Allocator>
         static inline expression_node<T>* allocate(Allocator& allocator, T0 t0, T1 t1, T2 t2, T3 t3, qfunc_t f)
         {
            return allocator.template allocate_type<node_type,T0,T1,T2,T3,qfunc_t>(t0,t1,t2,t3,f);
         }

      private:

         T0oT1oT2oT3_sf4(node_type&) {}
         node_type& operator=(node_type&) { return *this; }

         T0 t0_;
         T1 t1_;
         T2 t2_;
         T3 t3_;
         const qfunc_t f_;
      };

      template <typename T, typename T0, typename T1, typename T2, typename T3, typename SF4Operation>
      class T0oT1oT2oT3_sf4ext : public T0oT1oT2oT3_base_node<T>
      {
      public:

         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::tfunc_t tfunc_t;
         typedef T value_type;
         typedef T0oT1oT2oT3_sf4ext<T,T0,T1,T2,T3,SF4Operation> node_type;

         T0oT1oT2oT3_sf4ext(T0 t0, T1 t1, T2 t2, T3 t3)
         : t0_(t0),
           t1_(t1),
           t2_(t2),
           t3_(t3)
         {}

         inline typename expression_node<T>::node_type type() const
         {
            static const typename expression_node<T>::node_type result = nodetype_T0oT1oT2oT3<T,T0,T1,T2,T3>::result;
            return result;
         }

         inline operator_type operation() const
         {
            return e_default;
         }

         inline T value() const
         {
            return SF4Operation::process(t0_,t1_,t2_,t3_);
         }

         inline T0 t0() const
         {
            return t0_;
         }

         inline T1 t1() const
         {
            return t1_;
         }

         inline T2 t2() const
         {
            return t2_;
         }

         inline T3 t3() const
         {
            return t2_;
         }

         std::string type_id() const
         {
            return id();
         }

         static inline std::string id()
         {
            return SF4Operation::id();
         }

         template <typename Allocator>
         static inline expression_node<T>* allocate(Allocator& allocator, T0 t0, T1 t1, T2 t2, T3 t3)
         {
            return allocator.template allocate_type<node_type,T0,T1,T2,T3>(t0,t1,t2,t3);
         }

      private:

         T0oT1oT2oT3_sf4ext(node_type&) {}
         node_type& operator=(node_type&) { return *this; }

         T0 t0_;
         T1 t1_;
         T2 t2_;
         T3 t3_;
      };

      template <typename T>
      inline bool is_sf4ext_node(const expression_node<T>* n)
      {
         switch (n->type())
         {
            case expression_node<T>::e_vovovov : return true;
            case expression_node<T>::e_vovovoc : return true;
            case expression_node<T>::e_vovocov : return true;
            case expression_node<T>::e_vocovov : return true;
            case expression_node<T>::e_covovov : return true;
            case expression_node<T>::e_covocov : return true;
            case expression_node<T>::e_vocovoc : return true;
            case expression_node<T>::e_covovoc : return true;
            case expression_node<T>::e_vococov : return true;
            default                            : return false;
         }
      }

      template <typename T, typename T0, typename T1>
      struct T0oT1_define
      {
         typedef details::T0oT1<T,T0,T1> type0;
      };

      template <typename T, typename T0, typename T1, typename T2>
      struct T0oT1oT2_define
      {
         typedef details::T0oT1oT2<T,T0,T1,T2,typename T0oT1oT2process<T>::mode0> type0;
         typedef details::T0oT1oT2<T,T0,T1,T2,typename T0oT1oT2process<T>::mode1> type1;
         typedef details::T0oT1oT2_sf3<T,T0,T1,T2> sf3_type;
         typedef details::sf3ext_type_node<T,T0,T1,T2> sf3_type_node;
      };

      template <typename T, typename T0, typename T1, typename T2, typename T3>
      struct T0oT1oT2oT3_define
      {
         typedef details::T0oT1oT2oT3<T,T0,T1,T2,T3,typename T0oT1oT20T3process<T>::mode0> type0;
         typedef details::T0oT1oT2oT3<T,T0,T1,T2,T3,typename T0oT1oT20T3process<T>::mode1> type1;
         typedef details::T0oT1oT2oT3<T,T0,T1,T2,T3,typename T0oT1oT20T3process<T>::mode2> type2;
         typedef details::T0oT1oT2oT3<T,T0,T1,T2,T3,typename T0oT1oT20T3process<T>::mode3> type3;
         typedef details::T0oT1oT2oT3<T,T0,T1,T2,T3,typename T0oT1oT20T3process<T>::mode4> type4;
         typedef details::T0oT1oT2oT3_sf4<T,T0,T1,T2,T3> sf4_type;
      };

      template <typename T, typename Operation>
      class vov_node : public vov_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         // variable op variable node
         explicit vov_node(const T& v0, const T& v1)
         : v0_(v0),
           v1_(v1)
         {}

         inline T value() const
         {
            return Operation::process(v0_,v1_);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return Operation::type();
         }

         inline operator_type operation() const
         {
            return Operation::operation();
         }

         inline const T& v0() const
         {
            return v0_;
         }

         inline const T& v1() const
         {
            return v1_;
         }

      protected:

         const T& v0_;
         const T& v1_;

      private:

         vov_node(vov_node<T,Operation>&);
         vov_node<T,Operation>& operator=(vov_node<T,Operation>&);
      };

      template <typename T, typename Operation>
      class cov_node : public cov_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         // constant op variable node
         explicit cov_node(const T& c, const T& v)
         : c_(c),
           v_(v)
         {}

         inline T value() const
         {
            return Operation::process(c_,v_);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return Operation::type();
         }

         inline operator_type operation() const
         {
            return Operation::operation();
         }

         inline const T c() const
         {
            return c_;
         }

         inline const T& v() const
         {
            return v_;
         }

      protected:

         const T  c_;
         const T& v_;

      private:

         cov_node(const cov_node<T,Operation>&);
         cov_node<T,Operation>& operator=(const cov_node<T,Operation>&);
      };

      template <typename T, typename Operation>
      class voc_node : public voc_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         // variable op constant node
         explicit voc_node(const T& v, const T& c)
         : v_(v),
           c_(c)
         {}

         inline T value() const
         {
            return Operation::process(v_,c_);
         }

         inline operator_type operation() const
         {
            return Operation::operation();
         }

         inline const T c() const
         {
            return c_;
         }

         inline const T& v() const
         {
            return v_;
         }

      protected:

         const T& v_;
         const T  c_;

      private:

         voc_node(const voc_node<T,Operation>&);
         voc_node<T,Operation>& operator=(const voc_node<T,Operation>&);
      };

      template <typename T, typename Operation>
      class vob_node : public vob_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;
         typedef Operation operation_t;

         // variable op constant node
         explicit vob_node(const T& v, const expression_ptr branch)
         : v_(v)
         {
            init_branches<1>(branch_,branch);
         }

        ~vob_node()
         {
            cleanup_branches<T,1>::execute(branch_);
         }

         inline T value() const
         {
            return Operation::process(v_,branch_[0].first->value());
         }

         inline operator_type operation() const
         {
            return Operation::operation();
         }

         inline const T& v() const
         {
            return v_;
         }

         inline expression_node<T>* branch(const std::size_t&) const
         {
            return branch_[0].first;
         }

      private:

         vob_node(const vob_node<T,Operation>&);
         vob_node<T,Operation>& operator=(const vob_node<T,Operation>&);

         const T& v_;
         branch_t branch_[1];
      };

      template <typename T, typename Operation>
      class bov_node : public bov_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;
         typedef Operation operation_t;

         // variable op constant node
         explicit bov_node(const expression_ptr branch, const T& v)
         : v_(v)
         {
            init_branches<1>(branch_,branch);
         }

        ~bov_node()
         {
            cleanup_branches<T,1>::execute(branch_);
         }

         inline T value() const
         {
            return Operation::process(branch_[0].first->value(),v_);
         }

         inline operator_type operation() const
         {
            return Operation::operation();
         }

         inline const T& v() const
         {
            return v_;
         }

         inline expression_node<T>* branch(const std::size_t&) const
         {
            return branch_[0].first;
         }

      private:

         bov_node(const bov_node<T,Operation>&);
         bov_node<T,Operation>& operator=(const bov_node<T,Operation>&);

         const T& v_;
         branch_t branch_[1];
      };

      template <typename T, typename Operation>
      class cob_node : public cob_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;
         typedef Operation operation_t;

         // variable op constant node
         explicit cob_node(const T c, const expression_ptr branch)
         : c_(c)
         {
            init_branches<1>(branch_,branch);
         }

        ~cob_node()
         {
            cleanup_branches<T,1>::execute(branch_);
         }

         inline T value() const
         {
            return Operation::process(c_,branch_[0].first->value());
         }

         inline operator_type operation() const
         {
            return Operation::operation();
         }

         inline const T c() const
         {
            return c_;
         }

         inline void set_c(const T new_c)
         {
            (*const_cast<T*>(&c_)) = new_c;
         }

         inline expression_node<T>* branch(const std::size_t&) const
         {
            return branch_[0].first;
         }

         inline expression_node<T>* move_branch(const std::size_t&)
         {
            branch_[0].second = false;
            return branch_[0].first;
         }

      private:

         cob_node(const cob_node<T,Operation>&);
         cob_node<T,Operation>& operator=(const cob_node<T,Operation>&);

         const T c_;
         branch_t branch_[1];
      };

      template <typename T, typename Operation>
      class boc_node : public boc_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;
         typedef Operation operation_t;

         // variable op constant node
         explicit boc_node(const expression_ptr branch, const T c)
         : c_(c)
         {
            init_branches<1>(branch_,branch);
         }

        ~boc_node()
         {
            cleanup_branches<T,1>::execute(branch_);
         }

         inline T value() const
         {
            return Operation::process(branch_[0].first->value(),c_);
         }

         inline operator_type operation() const
         {
            return Operation::operation();
         }

         inline const T c() const
         {
            return c_;
         }

         inline void set_c(const T new_c)
         {
            (*const_cast<T*>(&c_)) = new_c;
         }

         inline expression_node<T>* branch(const std::size_t&) const
         {
            return branch_[0].first;
         }

      private:

         boc_node(const boc_node<T,Operation>&);
         boc_node<T,Operation>& operator=(const boc_node<T,Operation>&);

         const T c_;
         branch_t branch_[1];
      };

      #ifndef exprtk_disable_string_capabilities
      template <typename T, typename SType0, typename SType1, typename Operation>
      class sos_node : public sos_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         // string op string node
         explicit sos_node(SType0 s0, SType1 s1)
         : s0_(s0),
           s1_(s1)
         {}

         inline T value() const
         {
            return Operation::process(s0_,s1_);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return Operation::type();
         }

         inline operator_type operation() const
         {
            return Operation::operation();
         }

         inline std::string& s0()
         {
            return s0_;
         }

         inline std::string& s1()
         {
            return s1_;
         }

      protected:

         SType0 s0_;
         SType1 s1_;

      private:

         sos_node(sos_node<T,SType0,SType1,Operation>&);
         sos_node<T,SType0,SType1,Operation>& operator=(sos_node<T,SType0,SType1,Operation>&);
      };

      template <typename T, typename SType0, typename SType1, typename RangePack, typename Operation>
      class str_xrox_node : public sos_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         // string-range op string node
         explicit str_xrox_node(SType0 s0, SType1 s1, RangePack rp0)
         : s0_(s0),
           s1_(s1),
           rp0_(rp0)
         {}

        ~str_xrox_node()
         {
            rp0_.free();
         }

         inline T value() const
         {
            std::size_t r0 = 0;
            std::size_t r1 = 0;
            if (rp0_(r0,r1,s0_.size()))
               return Operation::process(s0_.substr(r0,(r1 - r0) + 1),s1_);
            else
               return T(0);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return Operation::type();
         }

         inline operator_type operation() const
         {
            return Operation::operation();
         }

         inline std::string& s0()
         {
            return s0_;
         }

         inline std::string& s1()
         {
            return s1_;
         }

      protected:

         SType0    s0_;
         SType1    s1_;
         RangePack rp0_;

      private:

         str_xrox_node(str_xrox_node<T,SType0,SType1,RangePack,Operation>&);
         str_xrox_node<T,SType0,SType1,RangePack,Operation>& operator=(str_xrox_node<T,SType0,SType1,RangePack,Operation>&);
      };

      template <typename T, typename SType0, typename SType1, typename RangePack, typename Operation>
      class str_xoxr_node : public sos_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         // string op string range node
         explicit str_xoxr_node(SType0 s0, SType1 s1, RangePack rp1)
         : s0_(s0),
           s1_(s1),
           rp1_(rp1)
         {}

        ~str_xoxr_node()
         {
            rp1_.free();
         }

         inline T value() const
         {
            std::size_t r0 = 0;
            std::size_t r1 = 0;
            if (rp1_(r0,r1,s1_.size()))
               return Operation::process(s0_,s1_.substr(r0,(r1 - r0) + 1));
            else
               return T(0);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return Operation::type();
         }

         inline operator_type operation() const
         {
            return Operation::operation();
         }

         inline std::string& s0()
         {
            return s0_;
         }

         inline std::string& s1()
         {
            return s1_;
         }

      protected:

         SType0    s0_;
         SType1    s1_;
         RangePack rp1_;

      private:

         str_xoxr_node(str_xoxr_node<T,SType0,SType1,RangePack,Operation>&);
         str_xoxr_node<T,SType0,SType1,RangePack,Operation>& operator=(str_xoxr_node<T,SType0,SType1,RangePack,Operation>&);
      };

      template <typename T, typename SType0, typename SType1, typename RangePack, typename Operation>
      class str_xroxr_node : public sos_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         // string-range op string-range node
         explicit str_xroxr_node(SType0 s0, SType1 s1, RangePack rp0, RangePack rp1)
         : s0_(s0),
           s1_(s1),
           rp0_(rp0),
           rp1_(rp1)
         {}

        ~str_xroxr_node()
         {
            rp0_.free();
            rp1_.free();
         }

         inline T value() const
         {
            std::size_t r0_0 = 0;
            std::size_t r0_1 = 0;
            std::size_t r1_0 = 0;
            std::size_t r1_1 = 0;
            if (rp0_(r0_0,r1_0,s0_.size()) && rp1_(r0_1,r1_1,s1_.size()))
               return Operation::process(s0_.substr(r0_0,(r1_0 - r0_0) + 1),
                                         s1_.substr(r0_1,(r1_1 - r0_1) + 1));
            else
               return T(0);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return Operation::type();
         }

         inline operator_type operation() const
         {
            return Operation::operation();
         }

         inline std::string& s0()
         {
            return s0_;
         }

         inline std::string& s1()
         {
            return s1_;
         }

      protected:

         SType0    s0_;
         SType1    s1_;
         RangePack rp0_;
         RangePack rp1_;

      private:

         str_xroxr_node(str_xroxr_node<T,SType0,SType1,RangePack,Operation>&);
         str_xroxr_node<T,SType0,SType1,RangePack,Operation>& operator=(str_xroxr_node<T,SType0,SType1,RangePack,Operation>&);
      };

      template <typename T, typename SType0, typename SType1, typename SType2, typename Operation>
      class sosos_node : public sosos_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         // variable op variable node
         explicit sosos_node(SType0 s0, SType1 s1, SType2 s2)
         : s0_(s0),
           s1_(s1),
           s2_(s2)
         {}

         inline T value() const
         {
            return Operation::process(s0_,s1_,s2_);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return Operation::type();
         }

         inline operator_type operation() const
         {
            return Operation::operation();
         }

         inline std::string& s0()
         {
            return s0_;
         }

         inline std::string& s1()
         {
            return s1_;
         }

         inline std::string& s2()
         {
            return s2_;
         }

      protected:

         SType0 s0_;
         SType1 s1_;
         SType2 s2_;

      private:

         sosos_node(sosos_node<T,SType0,SType1,SType2,Operation>&);
         sosos_node<T,SType0,SType1,SType2,Operation>& operator=(sosos_node<T,SType0,SType1,SType2,Operation>&);
      };
      #endif

      template <typename T, typename PowOp>
      class ipow_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef PowOp operation_t;

         explicit ipow_node(const T& v)
         : v_(v)
         {}

         inline T value() const
         {
            return PowOp::result(v_);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_ipow;
         }

      private:

         ipow_node(const ipow_node<T,PowOp>&);
         ipow_node<T,PowOp>& operator=(const ipow_node<T,PowOp>&);

         const T& v_;
      };

      template <typename T, typename PowOp>
      class ipowinv_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef PowOp operation_t;

         explicit ipowinv_node(const T& v)
         : v_(v)
         {}

         inline T value() const
         {
            return (T(1) / PowOp::result(v_));
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_ipowinv;
         }

      private:

         ipowinv_node(const ipowinv_node<T,PowOp>&);
         ipowinv_node<T,PowOp>& operator=(const ipowinv_node<T,PowOp>&);

         const T& v_;
      };

      template <typename T>
      inline bool is_vov_node(const expression_node<T>* node)
      {
         return (0 != dynamic_cast<const vov_base_node<T>*>(node));
      }

      template <typename T>
      inline bool is_cov_node(const expression_node<T>* node)
      {
         return (0 != dynamic_cast<const cov_base_node<T>*>(node));
      }

      template <typename T>
      inline bool is_voc_node(const expression_node<T>* node)
      {
         return (0 != dynamic_cast<const voc_base_node<T>*>(node));
      }

      template <typename T>
      inline bool is_cob_node(const expression_node<T>* node)
      {
         return (0 != dynamic_cast<const cob_base_node<T>*>(node));
      }

      template <typename T>
      inline bool is_boc_node(const expression_node<T>* node)
      {
         return (0 != dynamic_cast<const boc_base_node<T>*>(node));
      }

      template <typename T>
      inline bool is_t0ot1ot2_node(const expression_node<T>* node)
      {
         return (0 != dynamic_cast<const T0oT1oT2_base_node<T>*>(node));
      }

      template <typename T>
      inline bool is_t0ot1ot2ot3_node(const expression_node<T>* node)
      {
         return (0 != dynamic_cast<const T0oT1oT2oT3_base_node<T>*>(node));
      }

      template <typename T>
      inline bool is_uv_node(const expression_node<T>* node)
      {
         return (0 != dynamic_cast<const uv_base_node<T>*>(node));
      }

      template <typename T>
      inline bool is_string_node(const expression_node<T>* node)
      {
         return (expression_node<T>::e_stringvar == node->type());
      }

      template <typename T>
      inline bool is_string_range_node(const expression_node<T>* node)
      {
         return (expression_node<T>::e_stringvarrng == node->type());
      }

      template <typename T>
      inline bool is_const_string_node(const expression_node<T>* node)
      {
         return (expression_node<T>::e_stringconst == node->type());
      }

      template <typename T>
      inline bool is_const_string_range_node(const expression_node<T>* node)
      {
         return (expression_node<T>::e_cstringvarrng == node->type());
      }

      template <typename T>
      inline bool is_generally_string_node(const expression_node<T>* node)
      {
         return is_string_node            (node) ||
                is_const_string_node      (node) ||
                is_string_range_node      (node) ||
                is_const_string_range_node(node);
      }

      class node_allocator
      {
      public:

         template <typename ResultNode, typename OpType, typename ExprNode>
         inline expression_node<typename ResultNode::value_type>* allocate(OpType& operation, ExprNode (&branch)[1])
         {
            return allocate<ResultNode>(operation,branch[0]);
         }

         template <typename ResultNode, typename OpType, typename ExprNode>
         inline expression_node<typename ResultNode::value_type>* allocate(OpType& operation, ExprNode (&branch)[2])
         {
            return allocate<ResultNode>(operation,branch[0],branch[1]);
         }

         template <typename ResultNode, typename OpType, typename ExprNode>
         inline expression_node<typename ResultNode::value_type>* allocate(OpType& operation, ExprNode (&branch)[3])
         {
            return allocate<ResultNode>(operation,branch[0],branch[1],branch[2]);
         }

         template <typename ResultNode, typename OpType, typename ExprNode>
         inline expression_node<typename ResultNode::value_type>* allocate(OpType& operation, ExprNode (&branch)[4])
         {
            return allocate<ResultNode>(operation,branch[0],branch[1],branch[2],branch[3]);
         }

         template <typename ResultNode, typename OpType, typename ExprNode>
         inline expression_node<typename ResultNode::value_type>* allocate(OpType& operation, ExprNode (&branch)[5])
         {
            return allocate<ResultNode>(operation,branch[0],branch[1],branch[2],branch[3],branch[4]);
         }

         template <typename ResultNode, typename OpType, typename ExprNode>
         inline expression_node<typename ResultNode::value_type>* allocate(OpType& operation, ExprNode (&branch)[6])
         {
            return allocate<ResultNode>(operation,branch[0],branch[1],branch[2],branch[3],branch[4],branch[5]);
         }

         template <typename node_type>
         inline expression_node<typename node_type::value_type>* allocate() const
         {
            return new node_type();
         }

         template <typename node_type,
                   typename Type,
                   typename Allocator,
                   template<typename,typename> class Sequence>
         inline expression_node<typename node_type::value_type>* allocate(const Sequence<Type,Allocator>& seq) const
         {
            return new node_type(seq);
         }

         template <typename node_type, typename T1>
         inline expression_node<typename node_type::value_type>* allocate(T1& t1) const
         {
            return new node_type(t1);
         }

         template <typename node_type, typename T1>
         inline expression_node<typename node_type::value_type>* allocate_c(const T1& t1) const
         {
            return new node_type(t1);
         }

         template <typename node_type,
                   typename T1, typename T2>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const T2& t2) const
         {
            return new node_type(t1,t2);
         }

         template <typename node_type,
                   typename T1, typename T2>
         inline expression_node<typename node_type::value_type>* allocate_cr(const T1& t1, T2& t2) const
         {
            return new node_type(t1,t2);
         }

         template <typename node_type,
                   typename T1, typename T2>
         inline expression_node<typename node_type::value_type>* allocate_rc(T1& t1, const T2& t2) const
         {
            return new node_type(t1,t2);
         }

         template <typename node_type,
                   typename T1, typename T2>
         inline expression_node<typename node_type::value_type>* allocate_rr(T1& t1, T2& t2) const
         {
            return new node_type(t1,t2);
         }

         template <typename node_type,
                   typename T1, typename T2>
         inline expression_node<typename node_type::value_type>* allocate_tt(T1 t1, T2 t2) const
         {
            return new node_type(t1,t2);
         }

         template <typename node_type,
                   typename T1, typename T2, typename T3>
         inline expression_node<typename node_type::value_type>* allocate_ttt(T1 t1, T2 t2, T3 t3) const
         {
            return new node_type(t1,t2,t3);
         }

         template <typename node_type,
                   typename T1, typename T2, typename T3, typename T4>
         inline expression_node<typename node_type::value_type>* allocate_tttt(T1 t1, T2 t2, T3 t3, T4 t4) const
         {
            return new node_type(t1,t2,t3,t4);
         }

         template <typename node_type,
                   typename T1, typename T2, typename T3>
         inline expression_node<typename node_type::value_type>* allocate_rrr(T1& t1, T2& t2, T3& t3) const
         {
            return new node_type(t1,t2,t3);
         }

         template <typename node_type,
                   typename T1, typename T2, typename T3, typename T4>
         inline expression_node<typename node_type::value_type>* allocate_rrrr(T1& t1, T2& t2, T3& t3, T4& t4) const
         {
            return new node_type(t1,t2,t3,t4);
         }

         template <typename node_type,
                   typename T1, typename T2, typename T3, typename T4, typename T5>
         inline expression_node<typename node_type::value_type>* allocate_rrrrr(T1& t1, T2& t2, T3& t3, T4& t4, T5& t5) const
         {
            return new node_type(t1,t2,t3,t4,t5);
         }

         template <typename node_type,
                   typename T1, typename T2, typename T3>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const T2& t2,
                                                                          const T3& t3) const
         {
            return new node_type(t1,t2,t3);
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const T2& t2,
                                                                          const T3& t3, const T4& t4) const
         {
            return new node_type(t1,t2,t3,t4);
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4, typename T5>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const T2& t2,
                                                                          const T3& t3, const T4& t4,
                                                                          const T5& t5) const
         {
            return new node_type(t1,t2,t3,t4,t5);
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4, typename T5, typename T6>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const T2& t2,
                                                                          const T3& t3, const T4& t4,
                                                                          const T5& t5, const T6& t6) const
         {
            return new node_type(t1,t2,t3,t4,t5,t6);
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4,
                   typename T5, typename T6, typename T7>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const T2& t2,
                                                                          const T3& t3, const T4& t4,
                                                                          const T5& t5, const T6& t6,
                                                                          const T7& t7) const
         {
            return new node_type(t1,t2,t3,t4,t5,t6,t7);
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4,
                   typename T5, typename T6,
                   typename T7, typename T8>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const T2& t2,
                                                                          const T3& t3, const T4& t4,
                                                                          const T5& t5, const T6& t6,
                                                                          const T7& t7, const T8& t8) const
         {
            return new node_type(t1,t2,t3,t4,t5,t6,t7,t8);
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4,
                   typename T5, typename T6,
                   typename T7, typename T8, typename T9>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const T2& t2,
                                                                          const T3& t3, const T4& t4,
                                                                          const T5& t5, const T6& t6,
                                                                          const T7& t7, const T8& t8,
                                                                          const T9& t9) const
         {
            return new node_type(t1,t2,t3,t4,t5,t6,t7,t8,t9);
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4,
                   typename T5, typename T6,
                   typename T7, typename T8,
                   typename T9, typename T10>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const  T2&  t2,
                                                                          const T3& t3, const  T4&  t4,
                                                                          const T5& t5, const  T6&  t6,
                                                                          const T7& t7, const  T8&  t8,
                                                                          const T9& t9, const T10& t10) const
         {
            return new node_type(t1,t2,t3,t4,t5,t6,t7,t8,t9,t10);
         }

         template <typename node_type,
                   typename T1, typename T2, typename T3>
         inline expression_node<typename node_type::value_type>* allocate_type(T1 t1, T2 t2, T3 t3) const
         {
            return new node_type(t1,t2,t3);
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4>
         inline expression_node<typename node_type::value_type>* allocate_type(T1 t1, T2 t2,
                                                                               T3 t3, T4 t4) const
         {
            return new node_type(t1,t2,t3,t4);
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4,
                   typename T5>
         inline expression_node<typename node_type::value_type>* allocate_type(T1 t1, T2 t2,
                                                                               T3 t3, T4 t4,
                                                                               T5 t5) const
         {
            return new node_type(t1,t2,t3,t4,t5);
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3, typename T4,
                   typename T5, typename T6, typename T7>
         inline expression_node<typename node_type::value_type>* allocate_type(T1 t1, T2 t2,
                                                                               T3 t3, T4 t4,
                                                                               T5 t5, T6 t6,
                                                                               T7 t7) const
         {
            return new node_type(t1,t2,t3,t4,t5,t6,t7);
         }

         template <typename T>
         void inline free(expression_node<T>*& e) const
         {
            delete e;
            e = 0;
         }
      };

      inline void load_operations_map(std::multimap<std::string,details::base_operation_t,details::ilesscompare>& m)
      {
         #define register_op(Symbol,Type,Args)                                               \
         m.insert(std::make_pair(std::string(Symbol),details::base_operation_t(Type,Args))); \

         register_op(      "abs",e_abs     , 1)
         register_op(     "acos",e_acos    , 1)
         register_op(    "acosh",e_acosh   , 1)
         register_op(     "asin",e_asin    , 1)
         register_op(    "asinh",e_asinh   , 1)
         register_op(     "atan",e_atan    , 1)
         register_op(    "atanh",e_atanh   , 1)
         register_op(     "ceil",e_ceil    , 1)
         register_op(      "cos",e_cos     , 1)
         register_op(     "cosh",e_cosh    , 1)
         register_op(      "exp",e_exp     , 1)
         register_op(    "expm1",e_expm1   , 1)
         register_op(    "floor",e_floor   , 1)
         register_op(      "log",e_log     , 1)
         register_op(    "log10",e_log10   , 1)
         register_op(     "log2",e_log2    , 1)
         register_op(    "log1p",e_log1p   , 1)
         register_op(    "round",e_round   , 1)
         register_op(      "sin",e_sin     , 1)
         register_op(     "sinh",e_sinh    , 1)
         register_op(      "sec",e_sec     , 1)
         register_op(      "csc",e_csc     , 1)
         register_op(     "sqrt",e_sqrt    , 1)
         register_op(      "tan",e_tan     , 1)
         register_op(     "tanh",e_tanh    , 1)
         register_op(      "cot",e_cot     , 1)
         register_op(  "rad2deg",e_r2d     , 1)
         register_op(  "deg2rad",e_d2r     , 1)
         register_op( "deg2grad",e_d2g     , 1)
         register_op( "grad2deg",e_g2d     , 1)
         register_op(      "sgn",e_sgn     , 1)
         register_op(      "not",e_notl    , 1)
         register_op(      "erf",e_erf     , 1)
         register_op(     "erfc",e_erfc    , 1)
         register_op(     "frac",e_frac    , 1)
         register_op(    "trunc",e_trunc   , 1)
         register_op(    "atan2",e_atan2   , 2)
         register_op(      "mod",e_mod     , 2)
         register_op(     "logn",e_logn    , 2)
         register_op(      "pow",e_pow     , 2)
         register_op(     "root",e_root    , 2)
         register_op(   "roundn",e_roundn  , 2)
         register_op(    "equal",e_equal   , 2)
         register_op("not_equal",e_nequal  , 2)
         register_op(    "hypot",e_hypot   , 2)
         register_op(      "shr",e_shr     , 2)
         register_op(      "shl",e_shl     , 2)
         register_op(    "clamp",e_clamp   , 3)
         register_op(   "iclamp",e_iclamp  , 3)
         register_op(  "inrange",e_inrange , 3)
         #undef register_op
      }

   } // namespace details

   template <typename T>
   class ifunction
   {
   public:

      explicit ifunction(const std::size_t& pc)
      : param_count(pc)
      {}

      virtual ~ifunction()
      {}

      std::size_t param_count;

      inline virtual T operator()()
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&,const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                  const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                  const T&, const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                  const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                  const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                  const T&, const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                  const T&, const T&, const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                  const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                  const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                  const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                  const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }
   };

   template <typename T>
   class ivararg_function
   {
   public:

      virtual ~ivararg_function()
      {}

      inline virtual T operator()(const std::vector<T>&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }
   };

   template <typename T>
   class symbol_table
   {
   private:

      template <typename Type>
      class vector_holder
      {
      private:

         typedef Type value_type;
         typedef value_type* value_ptr;
         typedef const value_ptr const_value_ptr;

         class vector_holder_base
         {
         public:

            virtual ~vector_holder_base(){}

            inline value_ptr operator[](const std::size_t& index) const
            {
               return value_at(index);
            }

         protected:

            virtual value_ptr value_at(const std::size_t&) const = 0;

         };

         class array_vector_impl : public vector_holder_base
         {
         public:

            array_vector_impl(const T* vec, const std::size_t& vec_size)
            : vec_(vec),
              size_(vec_size)
            {}

         protected:

            value_ptr value_at(const std::size_t& index) const
            {
               if (index < size_)
                  return const_cast<const_value_ptr>(vec_ + index);
               else
                  return const_value_ptr(0);
            }

         private:

            array_vector_impl operator=(const array_vector_impl&);

            const T* vec_;
            const std::size_t size_;
         };

         template <typename Allocator,
                   template <typename,typename> class Sequence>
         class sequence_vector_impl : public vector_holder_base
         {
         public:

            typedef Sequence<Type,Allocator> sequence_t;

            sequence_vector_impl(sequence_t& seq)
            : sequence_(seq)
            {}

         protected:

            value_ptr value_at(const std::size_t& index) const
            {
               return (index < sequence_.size()) ? (&sequence_[index]) : const_value_ptr(0);
            }

         private:

            sequence_vector_impl operator=(const sequence_vector_impl&);

            sequence_t& sequence_;
         };

      public:

         vector_holder(Type* vec, const std::size_t& vec_size)
         : vector_holder_base_(new(buffer)array_vector_impl(vec,vec_size))
         {}

         template <typename Allocator>
         vector_holder(std::vector<Type,Allocator>& vec)
         : vector_holder_base_(new(buffer)sequence_vector_impl<Allocator,std::vector>(vec))
         {}

         template <typename Allocator>
         vector_holder(std::deque<Type,Allocator>& deq)
         : vector_holder_base_(new(buffer)sequence_vector_impl<Allocator,std::deque>(deq))
         {}

         inline value_ptr operator[](const std::size_t& index) const
         {
            return (*vector_holder_base_)[index];
         }

      private:

         mutable vector_holder_base* vector_holder_base_;
         unsigned char buffer[64];
      };

      template <typename Type, typename RawType>
      struct type_store
      {
         typedef typename details::variable_node<T>  variable_node_t;
         typedef ifunction<T>                        ifunction_t;
         typedef ivararg_function<T>                 ivararg_function_t;
         typedef vector_holder<T>                    vector_t;
         #ifndef exprtk_disable_string_capabilities
         typedef typename details::stringvar_node<T> stringvar_node_t;
         #endif

         typedef Type type_t;
         typedef type_t* type_ptr;
         typedef std::pair<bool,type_ptr> type_pair_t;
         typedef std::map<std::string,type_pair_t,details::ilesscompare> type_map_t;
         typedef typename type_map_t::iterator tm_itr_t;
         typedef typename type_map_t::const_iterator tm_const_itr_t;

         enum { lut_size = 256 };

         type_pair_t short_type_lut[lut_size];
         type_map_t  map;
         std::size_t size;

         type_store()
         : size(0)
         {}

         inline bool symbol_exists(const std::string& symbol_name) const
         {
            if (symbol_name.empty())
               return false;
            else if ((1 == symbol_name.size()) && short_type_lut[static_cast<std::size_t>(std::tolower(symbol_name[0]))].second)
               return true;
            else if (map.end() != map.find(symbol_name))
               return true;
            else
               return false;
         }

         inline bool is_constant(const std::string& symbol_name) const
         {
            if (symbol_name.empty())
               return false;
            else if (1 == symbol_name.size() && short_type_lut[static_cast<std::size_t>(std::tolower(symbol_name[0]))].first)
               return true;
            else
            {
               tm_const_itr_t itr = map.find(symbol_name);
               if (map.end() == itr)
                  return false;
               else
                  return (*itr).second.first;
            }
         }

         template <typename Tie, typename RType>
         inline bool add_impl(const std::string& symbol_name, RType t, const bool is_constant)
         {
            if (1 == symbol_name.size())
            {
               short_type_lut[static_cast<std::size_t>(std::tolower(symbol_name[0]))] = Tie::make(t,is_constant);
               ++size;
            }
            else
            {
               for (std::size_t i = 0; i < details::reserved_symbols_size; ++i)
               {
                  if (details::imatch(symbol_name,details::reserved_symbols[i]))
                  {
                     return false;
                  }
               }
               tm_itr_t itr = map.find(symbol_name);
               if (map.end() == itr)
               {
                  map[symbol_name] = Tie::make(t,is_constant);
                  ++size;
               }
            }
            return true;
         }

         inline bool add(const std::string& symbol_name, T* v, const std::size_t& v_size, const bool is_constant = false)
         {
            struct tie
            {
               static inline std::pair<bool,vector_t*> make(std::pair<T*,std::size_t> v, const bool is_constant = false)
               {
                  return std::make_pair(is_constant,new vector_t(v.first,v.second));
               }
            };
            return add_impl<tie,std::pair<T*,std::size_t> >(symbol_name,std::make_pair(v,v_size),is_constant);
         }

         template <typename Allocator>
         inline bool add(const std::string& symbol_name, std::vector<T,Allocator>& v, const bool is_constant = false)
         {
            struct tie
            {
               static inline std::pair<bool,vector_t*> make(std::vector<T,Allocator>& v, const bool is_constant = false)
               {
                  return std::make_pair(is_constant,new vector_t(v));
               }
            };
            return add_impl<tie,std::vector<T,Allocator>&>(symbol_name,v,is_constant);
         }

         template <typename Allocator>
         inline bool add(const std::string& symbol_name, std::deque<T,Allocator>& v, const bool is_constant = false)
         {
            struct tie
            {
               static inline std::pair<bool,vector_t*> make(std::deque<T,Allocator>& v, const bool is_constant = false)
               {
                  return std::make_pair(is_constant,new vector_t(v));
               }
            };
            return add_impl<tie,std::deque<T,Allocator>&>(symbol_name,v,is_constant);
         }

         inline bool add(const std::string& symbol_name, RawType& t, const bool is_constant = false)
         {
            struct tie
            {
               static inline std::pair<bool,variable_node_t*> make(T& t,const bool is_constant = false)
               {
                  return std::make_pair(is_constant,new variable_node_t(t));
               }

               #ifndef exprtk_disable_string_capabilities
               static inline std::pair<bool,stringvar_node_t*> make(std::string& t,const bool is_constant = false)
               {
                  return std::make_pair(is_constant,new stringvar_node_t(t));
               }
               #endif

               static inline std::pair<bool,function_t*> make(function_t& t, const bool is_constant = false)
               {
                  return std::make_pair(is_constant,&t);
               }

               static inline std::pair<bool,vararg_function_t*> make(vararg_function_t& t, const bool is_constant = false)
               {
                  return std::make_pair(is_constant,&t);
               }
            };

            if (1 == symbol_name.size())
            {
               short_type_lut[static_cast<std::size_t>(std::tolower(symbol_name[0]))] = tie::make(t,is_constant);
               ++size;
            }
            else
            {
               for (std::size_t i = 0; i < details::reserved_symbols_size; ++i)
               {
                  if (details::imatch(symbol_name,details::reserved_symbols[i]))
                  {
                     return false;
                  }
               }
               tm_itr_t itr = map.find(symbol_name);
               if (map.end() == itr)
               {
                  map[symbol_name] = tie::make(t,is_constant);
                  ++size;
               }
            }
            return true;
         }

         inline type_ptr get(const std::string& symbol_name)
         {
            if (1 == symbol_name.size())
            {
               type_pair_t& tp = short_type_lut[static_cast<std::size_t>(std::tolower(symbol_name[0]))];
               if (tp.second)
                  return tp.second;
               else
                  return reinterpret_cast<type_ptr>(0);
            }
            else
            {
               tm_const_itr_t itr = map.find(symbol_name);
               if (map.end() == itr)
                  return reinterpret_cast<type_ptr>(0);
               else
                  return itr->second.second;
            }
         }

         inline bool remove(const std::string& symbol_name)
         {
            if (1 == symbol_name.size())
            {
               type_pair_t& tp = short_type_lut[static_cast<std::size_t>(std::tolower(symbol_name[0]))];
               if (0 == tp.second)
                  return false;
               struct deleter
               {
                  static inline void process(std::pair<bool,variable_node_t*>& n)  { delete n.second; }
                  #ifndef exprtk_disable_string_capabilities
                  static inline void process(std::pair<bool,stringvar_node_t*>& n) { delete n.second; }
                  #endif
                  static inline void process(std::pair<bool,function_t*>&)         {                  }
               };
               deleter::process(tp);
               tp.first = false;
               tp.second = 0;
               --size;
               return true;
            }
            else
            {
               tm_itr_t itr = map.find(symbol_name);
               if (map.end() != itr)
               {
                  struct deleter
                  {
                     static inline void process(std::pair<bool,variable_node_t*>& n)  { delete n.second; }
                     static inline void process(std::pair<bool,vector_t*>& n)         { delete n.second; }
                     #ifndef exprtk_disable_string_capabilities
                     static inline void process(std::pair<bool,stringvar_node_t*>& n) { delete n.second; }
                     #endif
                     static inline void process(std::pair<bool,function_t*>&)         {                  }
                  };
                  deleter::process((*itr).second);
                  map.erase(itr);
                  --size;
                  return true;
               }
               else
                  return false;
            }
         }

         inline RawType& type_ref(const std::string& symbol_name)
         {
            struct init_type
            {
               static inline double set(long double)      { return (0.0);           }
               static inline float  set(float)            { return (0.0f);          }
               static inline std::string set(std::string) { return std::string(""); }
            };

            static RawType null_type = init_type::set(RawType());

            if (1 == symbol_name.size())
            {
               type_pair_t& tp = short_type_lut[static_cast<std::size_t>(std::tolower(symbol_name[0]))];
               if (tp.second)
                  return tp->second.ref();
               else
                  return null_type;
            }
            else
            {
               tm_const_itr_t itr = map.find(symbol_name);
               if (map.end() == itr)
                  return null_type;
               else
                  return itr->second.second->ref();
            }
         }

         inline void clear()
         {
            struct deleter
            {
               static inline void process(std::pair<bool,variable_node_t*>& n)  { delete n.second; }
               static inline void process(std::pair<bool,function_t*>&)         {                  }
               #ifndef exprtk_disable_string_capabilities
               static inline void process(std::pair<bool,stringvar_node_t*>& n) { delete n.second; }
               #endif
            };

            for (std::size_t i = 0; i < lut_size; ++i)
            {
               if (short_type_lut[i].second)
               {
                  deleter::process(short_type_lut[i]);
                  short_type_lut[i].first = false;
                  short_type_lut[i].second = reinterpret_cast<type_ptr>(0);
               }
            }

            if (!map.empty())
            {
               tm_itr_t itr = map.begin();
               tm_itr_t end = map.end();
               while (end != itr)
               {

                  deleter::process((*itr).second);
                  ++itr;
               }
               map.clear();
            }

            size = 0;
         }

         template <typename Allocator,
                   template <typename, typename> class Sequence>
         inline std::size_t get_list(Sequence<std::pair<std::string,RawType>,Allocator>& list) const
         {
            std::size_t count = 0;
            for (std::size_t i = 0; i < lut_size; ++i)
            {
               const type_pair_t& tp = short_type_lut[static_cast<std::size_t>(i)];
               if (0 != tp.second)
               {
                  list.push_back(std::make_pair(std::string("") + static_cast<char>(i),tp.second->ref()));
                  ++count;
               }
            }

            if (!map.empty())
            {
               tm_const_itr_t itr = map.begin();
               tm_const_itr_t end = map.end();
               while (end != itr)
               {
                  list.push_back(std::make_pair((*itr).first,itr->second.second->ref()));
                  ++itr;
                  ++count;
               }
            }

            return count;
         }

         template <typename Allocator,
                   template <typename, typename> class Sequence>
         inline std::size_t get_list(Sequence<std::string,Allocator>& vlist) const
         {
            std::size_t count = 0;
            for (std::size_t i = 0; i < lut_size; ++i)
            {
               const type_pair_t& tp = short_type_lut[static_cast<std::size_t>(i)];
               if (0 != tp.second)
               {
                  vlist.push_back(std::string("") + static_cast<char>(i));
                  ++count;
               }
            }

            if (!map.empty())
            {
               tm_const_itr_t itr = map.begin();
               tm_const_itr_t end = map.end();
               while (end != itr)
               {
                  vlist.push_back((*itr).first);
                  ++itr;
                  ++count;
               }
            }

            return count;
         }
      };

      typedef typename details::variable_node<T> variable_t;
      typedef variable_t* variable_ptr;
      #ifndef exprtk_disable_string_capabilities
      typedef typename details::stringvar_node<T> stringvar_t;
      typedef stringvar_t* stringvar_ptr;
      #endif
      typedef ifunction<T> function_t;
      typedef ivararg_function<T> vararg_function_t;
      typedef function_t* function_ptr;
      typedef vararg_function_t* vararg_function_ptr;
      typedef vector_holder<T>* vector_ptr;

      static const std::size_t lut_size = 256;

      //Symbol Table Holder
      struct st_holder
      {
         struct st_data
         {
            type_store<typename details::variable_node<T>,T> variable_store;
            #ifndef exprtk_disable_string_capabilities
            type_store<typename details::stringvar_node<T>,std::string> stringvar_store;
            #endif
            type_store<ifunction<T>,ifunction<T> > function_store;
            type_store<ivararg_function<T>,ivararg_function<T> > vararg_function_store;
            type_store<vector_holder<T>,vector_holder<T> > vector_store;

            st_data()
            {
               for (std::size_t i = 0; i < details::reserved_words_size; ++i)
               {
                  reserved_symbol_table_.insert(details::reserved_words[i]);
               }
               for (std::size_t i = 0; i < details::reserved_symbols_size; ++i)
               {
                  reserved_symbol_table_.insert(details::reserved_symbols[i]);
               }
            }

            inline bool is_reserved_symbol(const std::string& symbol) const
            {
               return (reserved_symbol_table_.end() != reserved_symbol_table_.find(symbol));
            }

            std::list<T> local_symbol_list_;
            std::list<std::string> local_stringvar_list_;
            std::set<std::string> reserved_symbol_table_;
         };

         st_holder()
         : ref_count(1),
           data_(new st_data)
         {}

         st_holder(st_data* data)
         : ref_count(1),
           data_(data)
         {}

        ~st_holder()
         {
            if (data_ && (0 == ref_count))
            {
               delete data_;
               data_ = 0;
            }
         }

         std::size_t ref_count;
         st_data* data_;
      };

   public:

      symbol_table()
      : holder_(new st_holder)
      {
         clear();
      }

     ~symbol_table()
      {
         if (holder_)
         {
            if (0 == --holder_->ref_count)
            {
               clear();
               delete holder_;
            }
         }
      }

      symbol_table(const symbol_table<T>& st)
      {
         holder_ = st.holder_;
         holder_->ref_count++;
      }

      inline symbol_table<T>& operator=(const symbol_table<T>& st)
      {
         if (holder_)
         {
            if (0 == --holder_->ref_count)
            {
               delete holder_;
            }
            holder_ = 0;
         }
         holder_ = st.holder_;
         holder_->ref_count++;
         return *this;
      }

      inline bool operator==(const symbol_table<T>& st)
      {
         return (this == *st);
      }

      inline void clear()
      {
         if (!valid()) return;
         local_data().variable_store.clear();
         local_data().function_store.clear();
         #ifndef exprtk_disable_string_capabilities
         local_data().stringvar_store.clear();
         #endif
      }

      inline std::size_t variable_count() const
      {
         if (valid())
            return local_data().variable_store.size;
         else
            return 0;
      }

      #ifndef exprtk_disable_string_capabilities
      inline std::size_t stringvar_count() const
      {
         if (valid())
            return local_data().stringvar_store.size;
         else
            return 0;
      }
      #endif

      inline std::size_t function_count() const
      {
         if (valid())
            return local_data().function_store.size;
         else
            return 0;
      }

      inline std::size_t vector_count() const
      {
         if (valid())
            return local_data().vector_store.size;
         else
            return 0;
      }

      inline variable_ptr get_variable(const std::string& variable_name)
      {
         if (!valid())
            return reinterpret_cast<variable_ptr>(0);
         else if (!valid_symbol(variable_name))
            return reinterpret_cast<variable_ptr>(0);
         else
            return local_data().variable_store.get(variable_name);
      }

      #ifndef exprtk_disable_string_capabilities
      inline stringvar_ptr get_stringvar(const std::string& string_name)
      {
         if (!valid())
            return reinterpret_cast<stringvar_ptr>(0);
         else if (!valid_symbol(string_name))
            return reinterpret_cast<stringvar_ptr>(0);
         else
            return local_data().stringvar_store.get(string_name);
      }
      #endif

      inline function_ptr get_function(const std::string& function_name)
      {
         if (!valid())
            return reinterpret_cast<function_ptr>(0);
         else if (!valid_symbol(function_name))
            return reinterpret_cast<function_ptr>(0);
         else
            return local_data().function_store.get(function_name);
      }

      inline vararg_function_ptr get_vararg_function(const std::string& vararg_function_name)
      {
         if (!valid())
            return reinterpret_cast<vararg_function_ptr>(0);
         else if (!valid_symbol(vararg_function_name))
            return reinterpret_cast<vararg_function_ptr>(0);
         else
            return local_data().vararg_function_store.get(vararg_function_name);
      }

      inline vector_ptr get_vector(const std::string& vector_name)
      {
         if (!valid())
            return reinterpret_cast<vector_ptr>(0);
         else if (!valid_symbol(vector_name))
            return reinterpret_cast<vector_ptr>(0);
         else
            return local_data().vector_store.get(vector_name);
      }

      inline T& variable_ref(const std::string& symbol_name)
      {
         static T null_var = T(0);
         if (!valid())
            return null_var;
         else if (!valid_symbol(symbol_name))
            return null_var;
         else
            return local_data().variable_store.type_ref(symbol_name);
      }

      #ifndef exprtk_disable_string_capabilities
      inline std::string& stringvar_ref(const std::string& symbol_name)
      {
         static std::string null_stringvar;
         if (!valid())
            return null_stringvar;
         else if (!valid_symbol(symbol_name))
            return null_stringvar;
         else
            return local_data().stringvar_store.type_ref(symbol_name);
      }
      #endif

      inline bool is_constant_node(const std::string& symbol_name) const
      {
         if (!valid())
            return false;
         else if (!valid_symbol(symbol_name))
            return false;
         else if (!local_data().variable_store.symbol_exists(symbol_name))
            return false;
         else
            return local_data().variable_store.is_constant(symbol_name);
      }

      #ifndef exprtk_disable_string_capabilities
      inline bool is_constant_string(const std::string& symbol_name) const
      {
         if (!valid())
            return false;
         else if (!valid_symbol(symbol_name))
            return false;
         else if (!local_data().stringvar_store.symbol_exists(symbol_name))
            return false;
         else
            return local_data().stringvar_store.is_constant(symbol_name);
      }
      #endif

      inline bool create_variable(const std::string& variable_name, const T& value = T(0))
      {
         if (!valid())
            return false;
         else if (!valid_symbol(variable_name))
            return false;
         else if (symbol_exists(variable_name))
            return false;
         local_data().local_symbol_list_.push_back(value);
         T& t = local_data().local_symbol_list_.back();
         return add_variable(variable_name,t);
      }

      #ifndef exprtk_disable_string_capabilities
      inline bool create_stringvar(const std::string& stringvar_name, const std::string& value = std::string(""))
      {
         if (!valid())
            return false;
         else if (!valid_symbol(stringvar_name))
            return false;
         else if (symbol_exists(stringvar_name))
            return false;
         local_data().local_stringvar_list_.push_back(value);
         std::string& s = local_data().local_stringvar_list_.back();
         return add_stringvar(stringvar_name,s);
      }
      #endif

      inline bool add_variable(const std::string& variable_name, T& t, const bool is_constant = false)
      {
         if (!valid())
            return false;
         else if (!valid_symbol(variable_name))
            return false;
         else if (symbol_exists(variable_name))
            return false;
         else
            return local_data().variable_store.add(variable_name,t,is_constant);
      }

      inline bool add_constant(const std::string& constant_name, const T& value)
      {
         if (!valid())
            return false;
         else if (!valid_symbol(constant_name))
            return false;
         else if (symbol_exists(constant_name))
            return false;
         local_data().local_symbol_list_.push_back(value);
         T& t = local_data().local_symbol_list_.back();
         return add_variable(constant_name,t,true);
      }

      #ifndef exprtk_disable_string_capabilities
      inline bool add_stringvar(const std::string& stringvar_name, std::string& s, const bool is_constant = false)
      {
         if (!valid())
            return false;
         else if (!valid_symbol(stringvar_name))
            return false;
         else if (symbol_exists(stringvar_name))
            return false;
         else
            return local_data().stringvar_store.add(stringvar_name,s,is_constant);
      }
      #endif

      inline bool add_function(const std::string& function_name, function_t& function)
      {
         if (!valid())
            return false;
         else if (!valid_symbol(function_name))
            return false;
         else if (symbol_exists(function_name))
            return false;
         else
            return local_data().function_store.add(function_name,function);
      }

      inline bool add_vararg_function(const std::string& vararg_function_name, vararg_function_t& vararg_function)
      {
         if (!valid())
            return false;
         else if (!valid_symbol(vararg_function_name))
            return false;
         else if (symbol_exists(vararg_function_name))
            return false;
         else
            return local_data().vararg_function_store.add(vararg_function_name,vararg_function);
      }

      inline bool add_vector(const std::string& vector_name, T* v, const std::size_t& v_size)
      {
         if (!valid())
            return false;
         else if (!valid_symbol(vector_name))
            return false;
         else if (symbol_exists(vector_name))
            return false;
         else
            return local_data().vector_store.add(vector_name,v,v_size);
      }

      template <typename Allocator>
      inline bool add_vector(const std::string& vector_name, std::vector<T,Allocator>& v)
      {
         if (!valid())
            return false;
         else if (!valid_symbol(vector_name))
            return false;
         else if (symbol_exists(vector_name))
            return false;
         else
            return local_data().vector_store.add(vector_name,v);
      }

      template <typename Allocator>
      inline bool add_vector(const std::string& vector_name, std::deque<T,Allocator>& v)
      {
         if (!valid())
            return false;
         else if (!valid_symbol(vector_name))
            return false;
         else if (symbol_exists(vector_name))
            return false;
         else
            return local_data().vector_store.add(vector_name,v);
      }

      inline bool remove_variable(const std::string& variable_name)
      {
         if (!valid())
            return false;
         else
            return local_data().variable_store.remove(variable_name);
      }

      #ifndef exprtk_disable_string_capabilities
      inline bool remove_stringvar(const std::string& string_name)
      {
         if (!valid())
            return false;
         else
            return local_data().stringvar_store.remove(string_name);
      }
      #endif

      inline bool remove_function(const std::string& function_name)
      {
         if (!valid())
            return false;
         else
            return local_data().function_store.remove(function_name);
      }

      inline bool remove_vararg_function(const std::string& vararg_function_name)
      {
         if (!valid())
            return false;
         else
            return local_data().vararg_function_store.remove(vararg_function_name);
      }

      inline bool remove_vector(const std::string& vector_name)
      {
         if (!valid())
            return false;
         else
            return local_data().vector_store.remove(vector_name);
      }

      inline bool add_constants()
      {
         return add_pi()      &&
                add_epsilon() &&
                add_infinity();
      }

      inline bool add_pi()
      {
         static const T local_pi = T(details::numeric::constant::pi);
         return add_constant("pi",local_pi);
      }

      inline bool add_epsilon()
      {
         static const T local_epsilon = std::numeric_limits<T>::epsilon();
         return add_constant("epsilon",local_epsilon);
      }

      inline bool add_infinity()
      {
         static const T local_infinity = std::numeric_limits<T>::infinity();
         return add_constant("inf",local_infinity);
      }

      template <typename Allocator,
                template <typename, typename> class Sequence>
      inline std::size_t get_variable_list(Sequence<std::pair<std::string,T>,Allocator>& vlist) const
      {
         if (!valid())
            return 0;
         else
            return local_data().variable_store.get_list(vlist);
      }

      template <typename Allocator,
                template <typename, typename> class Sequence>
      inline std::size_t get_variable_list(Sequence<std::string,Allocator>& vlist) const
      {
         if (!valid())
            return 0;
         else
            return local_data().variable_store.get_list(vlist);
      }

      #ifndef exprtk_disable_string_capabilities
      template <typename Allocator,
                template <typename, typename> class Sequence>
      inline std::size_t get_stringvar_list(Sequence<std::pair<std::string,std::string>,Allocator>& svlist) const
      {
         if (!valid())
            return 0;
         else
            return local_data().stringvar_store.get_list(svlist);
      }

      template <typename Allocator,
                template <typename, typename> class Sequence>
      inline std::size_t get_stringvar_list(Sequence<std::string,Allocator>& svlist) const
      {
         if (!valid())
            return 0;
         else
            return local_data().stringvar_store.get_list(svlist);
      }
      #endif

      template <typename Allocator,
                template <typename, typename> class Sequence>
      inline std::size_t get_vector_list(Sequence<std::string,Allocator>& vlist) const
      {
         if (!valid())
            return 0;
         else
            return local_data().vector_store.get_list(vlist);
      }

      inline bool symbol_exists(const std::string& symbol_name) const
      {
         /*
            Will return true if symbol_name exists as either a reserved symbol,
            variable, stringvar or function name in any of the type stores.
         */
         if (!valid())
            return false;
         else if (local_data().variable_store.symbol_exists(symbol_name))
            return true;
         #ifndef exprtk_disable_string_capabilities
         else if (local_data().stringvar_store.symbol_exists(symbol_name))
            return true;
         #endif
         else if (local_data().function_store.symbol_exists(symbol_name))
            return true;
         else if (local_data().is_reserved_symbol(symbol_name))
            return true;
         else
            return false;
      }

      inline bool is_variable(const std::string& variable_name) const
      {
         if (!valid())
            return false;
         else
            return local_data().variable_store.symbol_exists(variable_name);
      }

      #ifndef exprtk_disable_string_capabilities
      inline bool is_stringvar(const std::string& stringvar_name) const
      {
         if (!valid())
            return false;
         else
            return local_data().stringvar_store.symbol_exists(stringvar_name);
      }

      inline bool is_conststr_stringvar(const std::string& symbol_name) const
      {
         if (!valid())
            return false;
         else if (!valid_symbol(symbol_name))
            return false;
         else if (!local_data().stringvar_store.symbol_exists(symbol_name))
            return false;
         return (
                  local_data().stringvar_store.symbol_exists(symbol_name)
                  ||
                  local_data().stringvar_store.is_constant(symbol_name)
                );
      }
      #endif

      inline bool is_function(const std::string& function_name) const
      {
         if (!valid())
            return false;
         else
            return local_data().function_store.symbol_exists(function_name);
      }

      inline bool is_vararg_function(const std::string& vararg_function_name) const
      {
         if (!valid())
            return false;
         else
            return local_data().vararg_function_store.symbol_exists(vararg_function_name);
      }

      inline bool valid() const
      {
         //symbol table sanity check.
         return holder_ && holder_->data_;
      }

   private:

      inline bool valid_symbol(const std::string& symbol) const
      {
         if (symbol.empty())
            return false;
         if (!details::is_letter(symbol[0]))
            return false;
         else if (symbol.size() > 1)
         {
            for (std::size_t i = 1; i < symbol.size(); ++i)
            {
               if (
                   (!details::is_letter(symbol[i])) &&
                   (!details:: is_digit(symbol[i])) &&
                   ('_' != symbol[i])
                  )
                  return false;
            }
         }

         return (!local_data().is_reserved_symbol(symbol));
      }

      inline typename st_holder::st_data& local_data()
      {
         return *(holder_->data_);
      }

      inline const typename st_holder::st_data& local_data() const
      {
         return *(holder_->data_);
      }

      st_holder* holder_;

   };

   template <typename T> class parser;
   template <typename T> class expression_helper;

   template <typename T>
   class expression
   {
   private:

      typedef details::expression_node<T>* expression_ptr;

      struct expression_holder
      {
         expression_holder()
         : ref_count(0),
           expr(0)
         {}

         expression_holder(expression_ptr e)
         : ref_count(1),
           expr(e)
         {}

        ~expression_holder()
         {
            if (expr && !is_variable_node(expr))
            {
               delete expr;
            }
         }

         std::size_t ref_count;
         expression_ptr expr;
      };

   public:

      expression()
      : expression_holder_(0)
      {
         set_expression(new details::null_node<T>());
      }

      expression(const expression<T>& e)
      : expression_holder_(e.expression_holder_),
        symbol_table_(e.symbol_table_)
      {
         expression_holder_->ref_count++;
      }

      inline expression<T>& operator=(const expression<T>& e)
      {
         if (this != &e)
         {
            if (expression_holder_)
            {
               if (0 == --expression_holder_->ref_count)
               {
                  delete expression_holder_;
               }
               expression_holder_ = 0;
            }
            expression_holder_ = e.expression_holder_;
            expression_holder_->ref_count++;
            symbol_table_ = e.symbol_table_;
         }
         return *this;
      }

      inline bool operator==(const expression<T>& e)
      {
         return (this == &e);
      }

      inline bool operator!() const
      {
         return ((0 == expression_holder_) || (0 == expression_holder_->expr));
      }

      inline expression<T>& release()
      {
         if (expression_holder_)
         {
            if (0 == --expression_holder_->ref_count)
            {
               delete expression_holder_;
            }
            expression_holder_ = 0;
         }
         return *this;
      }

     ~expression()
      {
         if (expression_holder_)
         {
            if (0 == --expression_holder_->ref_count)
            {
               delete expression_holder_;
            }
         }
      }

      inline T value() const
      {
         return expression_holder_->expr->value();
      }

      inline T operator()() const
      {
         return value();
      }

      inline operator T() const
      {
         return value();
      }

      inline operator bool() const
      {
         return details::is_true(value());
      }

      inline void register_symbol_table(symbol_table<T>& st)
      {
         symbol_table_ = st;
      }

      inline const symbol_table<T>& get_symbol_table() const
      {
         return symbol_table_;
      }

      inline symbol_table<T>& get_symbol_table()
      {
         return symbol_table_;
      }

   private:

      inline void set_expression(const expression_ptr expr)
      {
         if (expr)
         {
            if (expression_holder_)
            {
               if (0 == --expression_holder_->ref_count)
               {
                  delete expression_holder_;
               }
            }
            expression_holder_ = new expression_holder(expr);
         }
      }

      expression_holder* expression_holder_;
      symbol_table<T> symbol_table_;

      friend class parser<T>;
      friend class expression_helper<T>;
   };

   template <typename T>
   class expression_helper
   {
   public:

      static inline bool is_head_constant(const expression<T>& expr)
      {
         return details::is_constant_node(expr.expression_holder_->expr);
      }

      static inline bool is_head_variable(const expression<T>& expr)
      {
         return details::is_variable_node(expr.expression_holder_->expr);
      }

      static inline bool is_head_unary(const expression<T>& expr)
      {
         return details::is_unary_node(expr.expression_holder_->expr);
      }

      static inline bool is_head_binary(const expression<T>& expr)
      {
         return details::is_binary_node(expr.expression_holder_->expr);
      }

      static inline bool is_head_function(const expression<T>& expr)
      {
         return details::is_function(expr.expression_holder_->expr);
      }
   };

   namespace parser_error
   {
      enum error_mode
      {
         e_unknown = 0,
         e_syntax  = 1,
         e_token   = 2,
         e_numeric = 4,
         e_symtab  = 5,
         e_lexer   = 6,
         e_helper  = 7
      };

      struct type
      {
         lexer::token token;
         error_mode mode;
         std::string diagnostic;
      };

      inline type make_error(error_mode mode, const std::string& diagnostic = "")
      {
         type t;
         t.mode       = mode;
         t.token.type = lexer::token::e_error;
         t.diagnostic = diagnostic;
         return t;
      }

      inline type make_error(error_mode mode, const lexer::token tk, const std::string& diagnostic = "")
      {
         type t;
         t.mode       = mode;
         t.token      = tk;
         t.diagnostic = diagnostic;
         return t;
      }

      inline std::string to_str(error_mode mode)
      {
         switch (mode)
         {
            case e_unknown : return std::string("Unknown Error");
            case e_syntax  : return std::string("Syntax Error");
            case e_token   : return std::string("Token Error");
            case e_numeric : return std::string("Numeric Error");
            case e_symtab  : return std::string("Symbol Error");
            case e_lexer   : return std::string("Lexer Error");
            case e_helper  : return std::string("Helper Error");
            default        : return std::string("Unknown Error");
         }
      }

      inline void dump_error(const type& error)
      {
         printf("Position: %02d   Type: [%s]   Msg: %s\n",
                static_cast<unsigned int>(error.token.position),
                exprtk::parser_error::to_str(error.mode).c_str(),
                error.diagnostic.c_str());
      }

   }

   template <typename T>
   class parser
   {
   private:

      enum precedence_level
      {
         e_level00,
         e_level01,
         e_level02,
         e_level03,
         e_level04,
         e_level05,
         e_level06,
         e_level07,
         e_level08,
         e_level09,
         e_level10,
         e_level11,
         e_level12,
         e_level13,
         e_level14
      };

      struct state_t;
      struct range_pack;

      typedef const T&                                               cref_t;
      typedef const T                                               const_t;
      typedef ifunction                <T>                                F;
      typedef ivararg_function         <T>                              VAF;
      typedef ifunction                <T>                      ifunction_t;
      typedef ivararg_function         <T>               ivararg_function_t;
      typedef details::expression_node <T>                expression_node_t;
      typedef details::literal_node    <T>                   literal_node_t;
      typedef details::unary_node      <T>                     unary_node_t;
      typedef details::binary_node     <T>                    binary_node_t;
      typedef details::trinary_node    <T>                   trinary_node_t;
      typedef details::quaternary_node <T>                quaternary_node_t;
      typedef details::quinary_node    <T>                   quinary_node_t;
      typedef details::senary_node     <T>                    senary_node_t;
      typedef details::conditional_node<T>               conditional_node_t;
      typedef details::while_loop_node <T>                while_loop_node_t;
      typedef details::repeat_until_loop_node<T>   repeat_until_loop_node_t;
      typedef details::switch_node<T>                         switch_node_t;
      typedef details::variable_node<T>                     variable_node_t;
      #ifndef exprtk_disable_string_capabilities
      typedef details::stringvar_node  <T>                 stringvar_node_t;
      typedef details::string_literal_node<T>         string_literal_node_t;
      typedef details::string_range_node<T,range_pack>  string_range_node_t;
      typedef details::const_string_range_node<T,range_pack>  const_string_range_node_t;
      #endif
      typedef details::assignment_node <T>                assignment_node_t;
      typedef details::scand_node <T>                          scand_node_t;
      typedef details::scor_node <T>                            scor_node_t;
      typedef lexer::token                                          token_t;
      typedef expression_node_t*                        expression_node_ptr;

      typedef typename details::functor_t<T> functor_t;
      typedef typename functor_t::qfunc_t quaternary_functor_t;
      typedef typename functor_t::tfunc_t trinary_functor_t;
      typedef typename functor_t::bfunc_t binary_functor_t;
      typedef typename functor_t::ufunc_t unary_functor_t;
      typedef std::map<details::operator_type,unary_functor_t> unary_op_map_t;
      typedef std::map<details::operator_type,binary_functor_t> binary_op_map_t;
      typedef std::map<details::operator_type,trinary_functor_t> trinary_op_map_t;
      typedef std::map<std::string,std::pair<trinary_functor_t,details::operator_type> > sf3_map_t;
      typedef std::map<std::string,std::pair<quaternary_functor_t,details::operator_type> > sf4_map_t;
      typedef std::map<binary_functor_t,details::operator_type> inv_binary_op_map_t;
      typedef std::multimap<std::string,details::base_operation_t,details::ilesscompare> base_ops_map_t;

      typedef details::T0oT1_define<T, cref_t, cref_t> vov_t;
      typedef details::T0oT1_define<T,const_t, cref_t> cov_t;
      typedef details::T0oT1_define<T, cref_t,const_t> voc_t;

      typedef details::T0oT1oT2_define<T, cref_t, cref_t, cref_t> vovov_t;
      typedef details::T0oT1oT2_define<T, cref_t, cref_t,const_t> vovoc_t;
      typedef details::T0oT1oT2_define<T, cref_t,const_t, cref_t> vocov_t;
      typedef details::T0oT1oT2_define<T,const_t, cref_t, cref_t> covov_t;
      typedef details::T0oT1oT2_define<T,const_t, cref_t,const_t> covoc_t;
      typedef details::T0oT1oT2_define<T,const_t,const_t, cref_t> cocov_t;
      typedef details::T0oT1oT2_define<T,cref_t,const_t, const_t> vococ_t;

      typedef details::T0oT1oT2oT3_define<T, cref_t, cref_t, cref_t, cref_t> vovovov_t;
      typedef details::T0oT1oT2oT3_define<T, cref_t, cref_t, cref_t,const_t> vovovoc_t;
      typedef details::T0oT1oT2oT3_define<T, cref_t, cref_t,const_t, cref_t> vovocov_t;
      typedef details::T0oT1oT2oT3_define<T, cref_t,const_t, cref_t, cref_t> vocovov_t;
      typedef details::T0oT1oT2oT3_define<T,const_t, cref_t, cref_t, cref_t> covovov_t;

      typedef details::T0oT1oT2oT3_define<T,const_t, cref_t,const_t, cref_t> covocov_t;
      typedef details::T0oT1oT2oT3_define<T, cref_t,const_t, cref_t,const_t> vocovoc_t;
      typedef details::T0oT1oT2oT3_define<T,const_t, cref_t, cref_t,const_t> covovoc_t;
      typedef details::T0oT1oT2oT3_define<T, cref_t,const_t,const_t, cref_t> vococov_t;

   public:

      enum precompilation_step
      {
         e_unknown            =  0,
         e_replacer           =  1,
         e_joiner             =  2,
         e_numeric_check      =  4,
         e_bracket_check      =  8,
         e_sequence_check     = 16,
         e_commutative_check  = 32,
         e_strength_reduction = 64
      };

      struct unknown_symbol_resolver
      {

         enum symbol_type
         {
            e_variable_type = 0,
            e_constant_type = 1
         };

         virtual ~unknown_symbol_resolver()
         {}

         virtual bool process(const std::string& /*unknown_symbol*/, symbol_type& st, T& default_value, std::string& error_message)
         {
            st = e_variable_type;
            default_value = T(0);
            error_message = "";
            return true;
         }
      };

      static const std::size_t compile_all_opts = e_replacer          +
                                                  e_joiner            +
                                                  e_numeric_check     +
                                                  e_bracket_check     +
                                                  e_sequence_check    +
                                                  e_commutative_check +
                                                  e_strength_reduction;

      parser(const std::size_t compile_options = compile_all_opts)
      : symbol_name_caching_(false),
        compile_options_(compile_options),
        resolve_unknown_symbol_(false),
        unknown_symbol_resolver_(reinterpret_cast<unknown_symbol_resolver*>(0)),
        operator_joiner_2_(2),
        operator_joiner_3_(3)
      {
         init_precompilation();
         load_operations_map(base_ops_map_);
         load_unary_operations_map(unary_op_map_);
         load_binary_operations_map(binary_op_map_);
         load_inv_binary_operations_map(inv_binary_op_map_);
         load_sf3_map(sf3_map_);
         load_sf4_map(sf4_map_);
         expression_generator_.init_synthesize_map();
         expression_generator_.set_uom(unary_op_map_);
         expression_generator_.set_bom(binary_op_map_);
         expression_generator_.set_ibom(inv_binary_op_map_);
         expression_generator_.set_sf3m(sf3_map_);
         expression_generator_.set_sf4m(sf4_map_);
         expression_generator_.set_strength_reduction_state(strength_reduction_enabled());
      }

      inline void init_precompilation()
      {
         if (replacer_enabled())
         {
            symbol_replacer_.clear();
            symbol_replacer_.add_replace("true" ,"1",lexer::token::e_number);
            symbol_replacer_.add_replace("false","0",lexer::token::e_number);
            helper_assembly_.token_modifier_list.clear();
            helper_assembly_.register_modifier(&symbol_replacer_);
         }

         if (commutative_check_enabled())
         {
            for (std::size_t i = 0; i < details::reserved_words_size; ++i)
            {
               commutative_inserter_.ignore_symbol(details::reserved_words[i]);
            }

            helper_assembly_.token_inserter_list.clear();
            helper_assembly_.register_inserter(&commutative_inserter_);
         }

         if (joiner_enabled())
         {
            helper_assembly_.token_joiner_list.clear();
            helper_assembly_.register_joiner(&operator_joiner_2_);
            helper_assembly_.register_joiner(&operator_joiner_3_);
         }

         if (
               numeric_check_enabled() ||
               bracket_check_enabled() ||
               sequence_check_enabled()
            )
         {
            helper_assembly_.token_scanner_list.clear();

            if (numeric_check_enabled())
            {
               helper_assembly_.register_scanner(&numeric_checker_);
            }

            if (bracket_check_enabled())
            {
               helper_assembly_.register_scanner(&bracket_checker_);
            }

            if (sequence_check_enabled())
            {
               helper_assembly_.register_scanner(&sequence_validator_);
            }
         }
      }

      inline bool compile(const std::string& expression_string, expression<T>& expr)
      {
         error_list_.clear();
         expression_generator_.set_allocator(node_allocator_);

         if (expression_string.empty())
         {
            set_error(
               make_error(parser_error::e_syntax,
                          "ERR00 - Empty expression!"));
            return false;
         }

         if (!lexer_.process(expression_string))
         {
            process_lexer_errors();
            return false;
         }

         if (lexer_.empty())
         {
            set_error(
               make_error(parser_error::e_syntax,
                          "ERR01 - Empty expression!"));
            return false;
         }

         if (!run_assemblies())
         {
            return false;
         }

         symbol_table_ = expr.get_symbol_table();
         symbol_name_cache_.clear();

         lexer_.begin();
         next_token();

         expression_node_ptr e = parse_corpus();

         if ((0 != e) && (token_t::e_eof == current_token_.type))
         {
            expr.set_expression(e);
            return !(!expr);
         }
         else
         {
            set_error(
               make_error(parser_error::e_syntax,
                          "ERR02 - Incomplete expression!"));
            symbol_name_cache_.clear();
            if (0 != e)
            {
               delete e;
            }
            return false;
         }
      }

      void process_lexer_errors()
      {
         for (std::size_t i = 0; i < lexer_.size(); ++i)
         {
            if (lexer_[i].is_error())
            {
               std::string diagnostic = "ERR03 - ";
               switch (lexer_[i].type)
               {
                  case lexer::token::e_error      : diagnostic += "General token error";
                                                    break;
                  case lexer::token::e_err_symbol : diagnostic += "Symbol error";
                                                    break;
                  case lexer::token::e_err_number : diagnostic += "Invalid numeric token";
                                                    break;
                  case lexer::token::e_err_string : diagnostic += "Invalid string token";
                                                    break;
                  case lexer::token::e_err_sfunc  : diagnostic += "Invalid special function token";
                                                    break;
                  default                         : diagnostic += "Unknown compiler error";
                                                    break;
               }

               set_error(
                  make_error(parser_error::e_lexer,
                             lexer_[i],
                             diagnostic + ": " + lexer_[i].value));
            }
         }
      }

      inline bool replacer_enabled() const
      {
         return ((compile_options_ & e_replacer) == e_replacer);
      }

      inline bool commutative_check_enabled() const
      {
         return ((compile_options_ & e_commutative_check) == e_commutative_check);
      }

      inline bool joiner_enabled() const
      {
         return ((compile_options_ & e_joiner) == e_joiner);
      }

      inline bool numeric_check_enabled() const
      {
         return ((compile_options_ & e_numeric_check) == e_numeric_check);
      }

      inline bool bracket_check_enabled() const
      {
         return ((compile_options_ & e_bracket_check) == e_bracket_check);
      }

      inline bool sequence_check_enabled() const
      {
         return ((compile_options_ & e_sequence_check) == e_sequence_check);
      }

      inline bool strength_reduction_enabled() const
      {
         return ((compile_options_ & e_strength_reduction) == e_strength_reduction);
      }

      inline bool run_assemblies()
      {
         if (commutative_check_enabled())
         {
            helper_assembly_.run_inserters(lexer_);
         }

         if (joiner_enabled())
         {
            helper_assembly_.run_joiners(lexer_);
         }

         if (replacer_enabled())
         {
            helper_assembly_.run_modifiers(lexer_);
         }

         if (
              numeric_check_enabled () ||
              bracket_check_enabled () ||
              sequence_check_enabled()
            )
         {
            if (!helper_assembly_.run_scanners(lexer_))
            {
               if (helper_assembly_.error_token_scanner)
               {
                  lexer::helper::bracket_checker*    bracket_checker_ptr    = 0;
                  lexer::helper::numeric_checker*    numeric_checker_ptr    = 0;
                  lexer::helper::sequence_validator* sequence_validator_ptr = 0;

                  if (0 != (bracket_checker_ptr = dynamic_cast<lexer::helper::bracket_checker*>(helper_assembly_.error_token_scanner)))
                  {
                     set_error(
                        make_error(parser_error::e_token,
                                   bracket_checker_ptr->error_token(),
                                   "ERR04 - Mismatched brackets: '" + bracket_checker_ptr->error_token().value + "'"));
                  }
                  else if (0 != (numeric_checker_ptr = dynamic_cast<lexer::helper::numeric_checker*>(helper_assembly_.error_token_scanner)))
                  {
                     for (std::size_t i = 0; i < numeric_checker_ptr->error_count(); ++i)
                     {
                        lexer::token error_token = lexer_[numeric_checker_ptr->error_index(i)];
                        set_error(
                           make_error(parser_error::e_token,
                                      error_token,
                                      "ERR05 - Invalid numeric token: '" + error_token.value + "'"));
                     }
                  }
                  else if (0 != (sequence_validator_ptr = dynamic_cast<lexer::helper::sequence_validator*>(helper_assembly_.error_token_scanner)))
                  {
                     for (std::size_t i = 0; i < sequence_validator_ptr->error_count(); ++i)
                     {
                        std::pair<lexer::token,lexer::token> error_token = sequence_validator_ptr->error(i);
                        set_error(
                           make_error(parser_error::e_token,
                                      error_token.first,
                                      "ERR06 - Invalid token sequence: '" +
                                      error_token.first.value  + "' and '" +
                                      error_token.second.value + "'"));
                     }
                  }
               }
               return false;
            }
         }

         return true;
      }

      inline parser_error::type get_error(const std::size_t& index)
      {
         if (index < error_list_.size())
            return error_list_[index];
         else
            throw std::invalid_argument("parser::get_error() - Invalid error index specificed.");
      }

      inline std::string error() const
      {
         if (!error_list_.empty())
         {
            return error_list_[0].diagnostic;
         }
         else
            return std::string("No Error");
      }

      inline std::size_t error_count() const
      {
         return error_list_.size();
      }

      inline bool& cache_symbols()
      {
         return symbol_name_caching_;
      }

      template <typename Allocator,
                template <typename,typename> class Sequence>
      inline std::size_t expression_symbols(Sequence<std::string,Allocator>& symbols_list)
      {
         if (!symbol_name_caching_)
            return 0;
         else if (symbol_name_cache_.empty())
            return 0;
         std::sort(symbol_name_cache_.begin(),symbol_name_cache_.end());
         std::unique_copy(symbol_name_cache_.begin(),
                          symbol_name_cache_.end(),
                          std::back_inserter(symbols_list));
         return symbol_name_cache_.size();
      }

      inline bool replace_symbol(const std::string& old_symbol, const std::string& new_symbol)
      {
         if (!replacer_enabled())
            return false;
         else if (details::is_reserved_word(old_symbol))
            return false;
         else
            return symbol_replacer_.add_replace(old_symbol,new_symbol,lexer::token::e_symbol);
      }

      inline bool remove_replace_symbol(const std::string& symbol)
      {
         if (!replacer_enabled())
            return false;
         else if (details::is_reserved_word(symbol))
            return false;
         else
            return symbol_replacer_.remove(symbol);
      }

      inline void enable_unknown_symbol_resolver(unknown_symbol_resolver* usr = reinterpret_cast<unknown_symbol_resolver*>(0))
      {
         resolve_unknown_symbol_ = true;
         if (usr)
            unknown_symbol_resolver_ = usr;
         else
            unknown_symbol_resolver_ = &default_usr_;
      }

      inline void disable_unknown_symbol_resolver()
      {
         resolve_unknown_symbol_ = false;
         unknown_symbol_resolver_ = &default_usr_;
      }

   private:

      inline bool valid_base_operation(const std::string& symbol)
      {
         const std::size_t length = symbol.size();
         if (
              (length < 3) || //Shortest base op symbol length
              (length > 9)    //Longest base op symbol length
            )
            return false;
         else
            return (base_ops_map_.end() != base_ops_map_.find(symbol));
      }

      inline bool valid_vararg_operation(const std::string& symbol)
      {
         static const std::string s_sum     = "sum" ;
         static const std::string s_mul     = "mul" ;
         static const std::string s_avg     = "avg" ;
         static const std::string s_min     = "min" ;
         static const std::string s_max     = "max" ;
         static const std::string s_mand    = "mand";
         static const std::string s_mor     = "mor" ;
         static const std::string s_multi   = "~"   ;
         static const std::string s_mswitch = "[*]" ;
         return
               (
                  details::imatch(symbol,s_sum    ) ||
                  details::imatch(symbol,s_mul    ) ||
                  details::imatch(symbol,s_avg    ) ||
                  details::imatch(symbol,s_min    ) ||
                  details::imatch(symbol,s_max    ) ||
                  details::imatch(symbol,s_mand   ) ||
                  details::imatch(symbol,s_mor    ) ||
                  details::imatch(symbol,s_multi  ) ||
                  details::imatch(symbol,s_mswitch)
               );
      }

      inline void store_token()
      {
         lexer_.store();
         store_current_token_ = current_token_;
      }

      inline void restore_token()
      {
         lexer_.restore();
         current_token_ = store_current_token_;
      }

      inline void next_token()
      {
         current_token_ = lexer_.next_token();
      }

      inline expression_node_ptr parse_corpus()
      {
         std::deque<expression_node_ptr> arg_list;
         expression_node_ptr result = error_node();

         scoped_deq_delete<expression_node_t> sdd(*this,arg_list);

         do
         {
            expression_node_ptr arg = parse_expression();
            if (0 == arg)
               return error_node();
            else
               arg_list.push_back(arg);

            if (!token_is(token_t::e_eof))
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR07 - Expected ';' at end of sub-expression."));
               return error_node();
            }
         }
         while (!lexer_.finished());

         result = simplify(arg_list);

         sdd.delete_ptr = (0 == result);
         return result;
      }

      static const precedence_level default_precedence = e_level00;

      struct state_t
      {
         inline void set(const precedence_level& l,
                         const precedence_level& r,
                         const details::operator_type& o)
         {
            left  = l;
            right = r;
            operation = o;
         }

         inline void reset()
         {
            left  = e_level00;
            right = e_level00;
         }

         precedence_level left;
         precedence_level right;
         details::operator_type operation;
      };

      inline expression_node_ptr parse_expression(precedence_level precedence = e_level00)
      {
         expression_node_ptr expr = parse_branch(precedence);

         if (0 == expr)
         {
            return expr;
         }

         bool break_loop = false;

         state_t current_state;

         for ( ; ; )
         {
            current_state.reset();

            switch (current_token_.type)
            {
               case token_t::e_assign : current_state.set(e_level00,e_level00,details::e_assign); break;
               case token_t::e_lt     : current_state.set(e_level05,e_level06,details::    e_lt); break;
               case token_t::e_lte    : current_state.set(e_level05,e_level06,details::   e_lte); break;
               case token_t::e_eq     : current_state.set(e_level05,e_level06,details::    e_eq); break;
               case token_t::e_ne     : current_state.set(e_level05,e_level06,details::    e_ne); break;
               case token_t::e_gte    : current_state.set(e_level05,e_level06,details::   e_gte); break;
               case token_t::e_gt     : current_state.set(e_level05,e_level06,details::    e_gt); break;
               case token_t::e_add    : current_state.set(e_level07,e_level08,details::   e_add); break;
               case token_t::e_sub    : current_state.set(e_level07,e_level08,details::   e_sub); break;
               case token_t::e_div    : current_state.set(e_level10,e_level11,details::   e_div); break;
               case token_t::e_mul    : current_state.set(e_level10,e_level11,details::   e_mul); break;
               case token_t::e_mod    : current_state.set(e_level10,e_level11,details::   e_mod); break;
               case token_t::e_pow    : current_state.set(e_level12,e_level12,details::   e_pow); break;
               default                : if (token_t::e_symbol == current_token_.type)
                                        {
                                           static const std::string s_and   =   "and";
                                           static const std::string s_nand  =  "nand";
                                           static const std::string s_or    =    "or";
                                           static const std::string s_nor   =   "nor";
                                           static const std::string s_xor   =   "xor";
                                           static const std::string s_xnor  =  "xnor";
                                           static const std::string s_in    =    "in";
                                           static const std::string s_like  =  "like";
                                           static const std::string s_ilike = "ilike";
                                           static const std::string s_and1  =     "&";
                                           static const std::string s_or1   =     "|";

                                           if (details::imatch(current_token_.value,s_and))
                                           {
                                              current_state.set(e_level01,e_level02,details::e_and);
                                              break;
                                           }
                                           else if (details::imatch(current_token_.value,s_and1))
                                           {
                                              #ifndef exprtk_disable_sc_andor
                                              current_state.set(e_level01,e_level02,details::e_scand);
                                              #else
                                              current_state.set(e_level01,e_level02,details::e_and);
                                              #endif
                                              break;
                                           }
                                           else if (details::imatch(current_token_.value,s_nand))
                                           {
                                              current_state.set(e_level01,e_level02,details::e_nand);
                                              break;
                                           }
                                           else if (details::imatch(current_token_.value,s_or))
                                           {
                                              current_state.set(e_level03,e_level04,details::e_or);
                                              break;
                                           }
                                           else if (details::imatch(current_token_.value,s_or1))
                                           {
                                              #ifndef exprtk_disable_sc_andor
                                              current_state.set(e_level03,e_level04,details::e_scor);
                                              #else
                                              current_state.set(e_level03,e_level04,details::e_or);
                                              #endif
                                              break;
                                           }
                                           else if (details::imatch(current_token_.value,s_nor))
                                           {
                                              current_state.set(e_level03,e_level04,details::e_nor);
                                              break;
                                           }
                                           else if (details::imatch(current_token_.value,s_xor))
                                           {
                                              current_state.set(e_level03,e_level04,details::e_xor);
                                              break;
                                           }
                                           else if (details::imatch(current_token_.value,s_xnor))
                                           {
                                              current_state.set(e_level03,e_level04,details::e_xnor);
                                              break;
                                           }
                                           else if (details::imatch(current_token_.value,s_in))
                                           {
                                              current_state.set(e_level03,e_level04,details::e_in);
                                              break;
                                           }
                                           else if (details::imatch(current_token_.value,s_like))
                                           {
                                              current_state.set(e_level03,e_level04,details::e_like);
                                              break;
                                           }
                                           else if (details::imatch(current_token_.value,s_ilike))
                                           {
                                              current_state.set(e_level03,e_level04,details::e_ilike);
                                              break;
                                           }
                                        }

                                        break_loop = true;
            }

            if (break_loop)
               break;
            else if (current_state.left < precedence)
               break;

            next_token();

            expr = expression_generator_(current_state.operation,expr,parse_expression(current_state.right));

            if (0 == expr)
            {
               return expr;
            }
         }
         return expr;
      }

      static inline expression_node_ptr error_node()
      {
         return reinterpret_cast<expression_node_ptr>(0);
      }

      template <typename Type, std::size_t N>
      struct scoped_delete
      {
         typedef Type* ptr_t;

         scoped_delete(parser<T>& pr, ptr_t& p)
         : delete_ptr(true),
           parser_(pr),
           p_(&p)
         {}

         scoped_delete(parser<T>& pr, ptr_t (&p)[N])
         : delete_ptr(true),
           parser_(pr),
           p_(&p[0])
         {}

        ~scoped_delete()
         {
            if (delete_ptr)
            {
               for (std::size_t i = 0; i < N; ++i)
               {
                  if (p_[i] && !is_variable_node(p_[i]))
                  {
                     parser_.node_allocator_.free(p_[i]);
                     p_[i] = 0;
                  }
               }
            }
         }

         bool delete_ptr;
         parser<T>& parser_;
         ptr_t* p_;

      private:

         scoped_delete<Type,N>& operator=(const scoped_delete<Type,N>&);
      };

      template <typename Type>
      struct scoped_deq_delete
      {
         typedef Type* ptr_t;

         scoped_deq_delete(parser<T>& pr, std::deque<ptr_t>& deq)
         : delete_ptr(true),
           parser_(pr),
           deq_(deq)
         {}

        ~scoped_deq_delete()
         {
            if (delete_ptr)
            {
               for (std::size_t i = 0; i < deq_.size(); ++i)
               {
                  free_node(parser_.node_allocator_,deq_[i]);
               }
               deq_.clear();
            }
         }

         bool delete_ptr;
         parser<T>& parser_;
         std::deque<ptr_t>& deq_;

      private:

         scoped_deq_delete<Type>& operator=(const scoped_deq_delete<Type>&);
      };

      template <typename Type>
      struct scoped_vec_delete
      {
         typedef Type* ptr_t;

         scoped_vec_delete(parser<T>& pr, std::vector<ptr_t>& vec)
         : delete_ptr(true),
           parser_(pr),
           vec_(vec)
         {}

        ~scoped_vec_delete()
         {
            if (delete_ptr)
            {
               for (std::size_t i = 0; i < vec_.size(); ++i)
               {
                  free_node(parser_.node_allocator_,vec_[i]);
               }
               vec_.clear();
            }
         }

         bool delete_ptr;
         parser<T>& parser_;
         std::vector<ptr_t>& vec_;

      private:

         scoped_vec_delete<Type>& operator=(const scoped_vec_delete<Type>&);
      };

      template <std::size_t NumberofParameters>
      inline expression_node_ptr parse_function_call(const details::operator_type& opt_type, bool& internal_error)
      {
         expression_node_ptr branch[NumberofParameters];
         expression_node_ptr result  = 0;

         std::fill_n(branch,NumberofParameters,reinterpret_cast<expression_node_ptr>(0));
         scoped_delete<expression_node_t,NumberofParameters> sd(*this,branch);

         next_token();

         if (!token_is(token_t::e_lbracket))
         {
            return error_node();
         }

         for (int i = 0; i < static_cast<int>(NumberofParameters); ++i)
         {
            branch[i] = parse_expression();
            if (0 == branch[i])
            {
               internal_error = true;
               return error_node();
            }
            else if (i < static_cast<int>(NumberofParameters - 1))
            {
               if (!token_is(token_t::e_comma))
               {
                  return error_node();
               }
            }
         }

         if (!token_is(token_t::e_rbracket))
         {
            return error_node();
         }
         else
            result = expression_generator_(opt_type,branch);
         sd.delete_ptr = false;
         return result;
      }

      template <std::size_t NumberofParameters>
      inline expression_node_ptr parse_function_call(ifunction<T>* function, const std::string& function_name)
      {
         expression_node_ptr branch[NumberofParameters];
         expression_node_ptr result  = error_node();
         std::fill_n(branch,NumberofParameters,reinterpret_cast<expression_node_ptr>(0));
         scoped_delete<expression_node_t,NumberofParameters> sd(*this,branch);
         next_token();
         if (!token_is(token_t::e_lbracket))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR08 - Expecting argument list for function: '" + function_name + "'"));
            return error_node();
         }

         for (int i = 0; i < static_cast<int>(NumberofParameters); ++i)
         {
            branch[i] = parse_expression();
            if (0 == branch[i])
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR09 - Failed to parse argument " + details::to_str(i) + " for function: '" + function_name + "'"));
               return error_node();
            }
            else if (i < static_cast<int>(NumberofParameters - 1))
            {
               if (!token_is(token_t::e_comma))
               {
                  set_error(
                     make_error(parser_error::e_syntax,
                                current_token_,
                                "ERR10 - Invalid number of arguments for function: '" + function_name + "'"));
                  return error_node();
               }
            }
         }

         if (!token_is(token_t::e_rbracket))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR11 - Invalid number of arguments for function: '" + function_name + "'"));
            return error_node();
         }
         else
            result = expression_generator_.function(function,branch);

         sd.delete_ptr = false;
         return result;
      }

      inline expression_node_ptr parse_function_call_0(ifunction<T>* function, const std::string& function_name)
      {
         expression_node_ptr result = expression_generator_.function(function);
         next_token();
         if (token_is(token_t::e_lbracket) && (!token_is(token_t::e_rbracket)))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR12 - Expecting '()' to proceed: '" + function_name + "'"));
            return error_node();
         }
         else
            return result;
      }

      inline expression_node_ptr parse_base_operation()
      {
         typedef std::pair<base_ops_map_t::iterator,base_ops_map_t::iterator> map_range_t;
         const std::string operation_name = current_token_.value;
         map_range_t itr_range = base_ops_map_.equal_range(operation_name);
         if (0 == std::distance(itr_range.first,itr_range.second))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR13 - No entries found for base operation: " + operation_name));
            return error_node();
         }

         for (base_ops_map_t::iterator itr = itr_range.first; itr != itr_range.second; ++itr)
         {
            store_token();
            expression_node_ptr branch = reinterpret_cast<expression_node_ptr>(0);
            details::base_operation_t& operation = itr->second;
            bool internal_error = false; // Any error other than incorrect number of params.
            switch (operation.num_params)
            {
               case 1  : branch = parse_function_call<1>(operation.type,internal_error); break;
               case 2  : branch = parse_function_call<2>(operation.type,internal_error); break;
               case 3  : branch = parse_function_call<3>(operation.type,internal_error); break;
               case 4  : branch = parse_function_call<4>(operation.type,internal_error); break;
               case 5  : branch = parse_function_call<5>(operation.type,internal_error); break;
               case 6  : branch = parse_function_call<6>(operation.type,internal_error); break;
               default :
                        {
                           set_error(
                              make_error(parser_error::e_syntax,
                                         current_token_,
                                         "ERR14 - Impossible argument count for base function: " + operation_name));

                           return error_node();
                        }
            }

            if (branch)
            {
               return branch;
            }
            else if (internal_error)
            {
               return error_node();
            }
            remove_last_error();
            restore_token();
         }

         set_error(
            make_error(parser_error::e_syntax,
                       current_token_,
                       "ERR15 - Invalid parameter count for function: " + operation_name));
         return error_node();
      }

      inline expression_node_ptr parse_conditional_statement()
      {
         // Parse: [if][(][condition][,][consequent][,][alternative][)]
         expression_node_ptr condition   = error_node();
         expression_node_ptr consequent  = error_node();
         expression_node_ptr alternative = error_node();

         next_token();

         if (!token_is(token_t::e_lbracket))
            return error_node();
         else if (0 == (condition = parse_expression()))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR16 - Failed to parse condition for if-statement"));
            return error_node();
         }
         else if (!token_is(token_t::e_comma))
            return error_node();
         else if (0 == (consequent = parse_expression()))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR17 - Failed to parse consequent for if-statement"));
            return error_node();
         }
         else if (!token_is(token_t::e_comma))
            return error_node();
         else if (0 == (alternative = parse_expression()))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR18 - Failed to parse alternative for if-statement"));
            return error_node();
         }
         else if (!token_is(token_t::e_rbracket))
            return error_node();
         else
            return expression_generator_.conditional(condition,consequent,alternative);
      }

      inline expression_node_ptr parse_while_loop()
      {
         // Parse: [while][(][test expr][)][{][expression][}]
         expression_node_ptr condition = error_node();
         expression_node_ptr branch    = error_node();
         next_token();
         if (!token_is(token_t::e_lbracket))
            return error_node();
         else if (0 == (condition = parse_expression()))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR19 - Failed to parse condition for while-loop"));
            return error_node();
         }
         else if (!token_is(token_t::e_rbracket))
            return error_node();

         if (0 == (branch = parse_multi_sequence("while-loop")))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR20 - Failed to parse body of while-loop"));
            return error_node();
         }

         expression_node_ptr result;
         if (0 == (result = expression_generator_.while_loop(condition,branch)))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR21 - Failed to synthesize while-loop"));
            return error_node();
         }
         else
            return result;
      }

      inline expression_node_ptr parse_repeat_until_loop()
      {
         // Parse: [repeat][{][expression][}][until][(][test expr][)]
         expression_node_ptr condition = error_node();
         expression_node_ptr branch    = error_node();
         next_token();

         std::deque<expression_node_ptr> arg_list;
         scoped_deq_delete<expression_node_t> sdd(*this,arg_list);

         {
            token_t::token_type seperator = token_t::e_eof;

            for (;;)
            {
               expression_node_ptr arg = parse_expression();
               if (0 == arg)
                  return error_node();
               else
                  arg_list.push_back(arg);

               if (details::imatch(current_token_.value,"until"))
               {
                  next_token();
                  break;
               }
               else if (!token_is(seperator))
               {
                  set_error(
                     make_error(parser_error::e_syntax,
                                current_token_,
                                "ERR22 - Expected '" + token_t::to_str(seperator) +"' for body of repeat until loop"));
                  return error_node();
               }

               if (details::imatch(current_token_.value,"until"))
               {
                  next_token();
                  break;
               }
            }

            branch = simplify(arg_list);

            sdd.delete_ptr = (0 == branch);

            if (0 == branch)
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR23 - Failed to parse body of repeat until loop."));
               return error_node();
            }
         }

         if (!token_is(token_t::e_lbracket))
            return error_node();
         else if (0 == (condition = parse_expression()))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR24 - Failed to parse condition for repeat until loop."));
            return error_node();
         }
         else if (!token_is(token_t::e_rbracket))
            return error_node();

         expression_node_ptr result;
         if (0 == (result = expression_generator_.repeat_until_loop(condition,branch)))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR25 - Failed to synthesize repeat until loop."));
            return error_node();
         }
         else
            return result;
      }

      inline expression_node_ptr parse_switch_statement()
      {
         std::vector<expression_node_ptr> arg_list;
         expression_node_ptr result = error_node();

         const std::string symbol = current_token_.value;

         if (!details::imatch(current_token_.value,"switch"))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR26 - Expected keyword 'switch'"));
            return error_node();
         }

         scoped_vec_delete<expression_node_t> sdd(*this,arg_list);

         next_token();

         if (!token_is(token_t::e_lcrlbracket))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR27 - Expected '{' for call to switch statement."));
            return error_node();
         }

         for ( ; ; )
         {
            if (!details::imatch("case",current_token_.value))
               return error_node();

            next_token();

            expression_node_ptr condition = parse_expression();
            if (0 == condition)
               return error_node();

            if (!token_is(token_t::e_colon))
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR28 - Expected ':' for case of switch statement."));
               return error_node();
            }

            expression_node_ptr consequent = parse_expression();
            if (0 == consequent)
               return error_node();

            if (!token_is(token_t::e_eof))
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR29 - Expected ';' at end of case for switch statement."));
               return error_node();
            }

            // Can we optimize away the case statement?
            if (is_constant_node(condition) && is_false(condition))
            {
               free_node(node_allocator_,condition);
               free_node(node_allocator_,consequent);
               condition  = 0;
               consequent = 0;
            }
            else
            {
               arg_list.push_back(condition);
               arg_list.push_back(consequent);
            }

            if (details::imatch("default",current_token_.value))
            {
               next_token();
               if (!token_is(token_t::e_colon))
               {
                  set_error(
                     make_error(parser_error::e_syntax,
                                current_token_,
                                "ERR30 - Expected ':' for default of switch statement."));
                  return error_node();
               }

               expression_node_ptr default_statement = parse_expression();
               if (0 == default_statement)
                  return error_node();
               else if (!token_is(token_t::e_eof))
               {
                  set_error(make_error(parser_error::e_syntax,
                            current_token_,
                            "ERR31 - Expected ';' at end of default for switch statement."));
                  return error_node();
               }

               arg_list.push_back(default_statement);
               break;
            }
         }

         if (!token_is(token_t::e_rcrlbracket))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR32 - Expected '}' at end of switch statement."));
            return error_node();
         }

         result = expression_generator_.switch_statement(arg_list);

         sdd.delete_ptr = (0 == result);
         return result;
      }

      inline expression_node_ptr parse_multi_switch_statement()
      {
         std::vector<expression_node_ptr> arg_list;
         expression_node_ptr result = error_node();

         const std::string symbol = current_token_.value;

         if (!details::imatch(current_token_.value,"[*]"))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR33 - Expected token '[*]'"));
            return error_node();
         }

         scoped_vec_delete<expression_node_t> sdd(*this,arg_list);

         next_token();

         if (!token_is(token_t::e_lcrlbracket))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR34 - Expected '{' for call to [*] statement."));
            return error_node();
         }

         for ( ; ; )
         {
            if (!details::imatch("case",current_token_.value))
               return error_node();

            next_token();

            expression_node_ptr condition = parse_expression();
            if (0 == condition)
               return error_node();

            if (!token_is(token_t::e_colon))
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR35 - Expected ':' for case of [*] statement."));
               return error_node();
            }

            expression_node_ptr consequent = parse_expression();
            if (0 == consequent)
               return error_node();

            if (!token_is(token_t::e_eof))
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR36 - Expected ';' at end of case for [*] statement."));
               return error_node();
            }

            // Can we optimize away the case statement?
            if (is_constant_node(condition) && is_false(condition))
            {
               free_node(node_allocator_,condition);
               free_node(node_allocator_,consequent);
               condition  = 0;
               consequent = 0;
            }
            else
            {
               arg_list.push_back(condition);
               arg_list.push_back(consequent);
            }

            if (token_is(token_t::e_rcrlbracket,false))
            {
               break;
            }
         }

         if (!token_is(token_t::e_rcrlbracket))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR37 - Expected '}' at end of [*] statement."));
            return error_node();
         }

         result = expression_generator_.multi_switch_statement(arg_list);

         sdd.delete_ptr = (0 == result);
         return result;
      }

      inline expression_node_ptr parse_vararg_function()
      {
         std::deque<expression_node_ptr> arg_list;
         expression_node_ptr result = error_node();

         details::operator_type opt_type = details::e_default;
         const std::string symbol = current_token_.value;

         if (details::imatch(symbol,"~"))
         {
            next_token();
            return parse_multi_sequence();
         }
         else if (details::imatch(symbol,"[*]"))
         {
            return parse_multi_switch_statement();
         }
         else if (details::imatch(symbol,"sum" )) opt_type = details::e_sum;
         else if (details::imatch(symbol,"mul" )) opt_type = details::e_prod;
         else if (details::imatch(symbol,"avg" )) opt_type = details::e_avg;
         else if (details::imatch(symbol,"min" )) opt_type = details::e_min;
         else if (details::imatch(symbol,"max" )) opt_type = details::e_max;
         else if (details::imatch(symbol,"mand")) opt_type = details::e_mand;
         else if (details::imatch(symbol,"mor" )) opt_type = details::e_mor;
         else
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR38 - Unsupported vararg function: " + symbol));
            return error_node();
         }

         scoped_deq_delete<expression_node_t> sdd(*this,arg_list);

         next_token();
         if (!token_is(token_t::e_lbracket))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR39 - Expected '(' for call to vararg function: " + symbol));
            return error_node();
         }

         for ( ; ; )
         {
            expression_node_ptr arg = parse_expression();
            if (0 == arg)
               return error_node();
            else
               arg_list.push_back(arg);

            if (token_is(token_t::e_rbracket))
               break;
            else if (!token_is(token_t::e_comma))
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR40 - Expected ',' for call to vararg function: " + symbol));
               return error_node();
            }
         }

         result = expression_generator_.vararg_function(opt_type,arg_list);

         sdd.delete_ptr = (0 == result);
         return result;
      }

      template <typename Allocator,
                template <typename,typename> class Sequence>
      inline expression_node_ptr simplify(Sequence<expression_node_ptr,Allocator>& expression_list)
      {
         if (expression_list.empty())
            return error_node();
         else if (1 == expression_list.size())
            return expression_list[0];

         Sequence<expression_node_ptr,Allocator> tmp_expression_list;

         for (std::size_t i = 0; i < (expression_list.size() - 1); ++i)
         {
            if (is_variable_node(expression_list[i]))
               continue;
            else if (is_constant_node(expression_list[i]))
            {
               free_node(node_allocator_,expression_list[i]);
               continue;
            }
            else
               tmp_expression_list.push_back(expression_list[i]);
         }

         tmp_expression_list.push_back(expression_list.back());
         expression_list.swap(tmp_expression_list);
         return expression_generator_.vararg_function(details::e_multi,expression_list);
      }

      inline expression_node_ptr parse_multi_sequence(const std::string& source = "")
      {
         token_t::token_type close_bracket = token_t::e_rcrlbracket;
         token_t::token_type seperator     = token_t::e_eof;

         if (!token_is(token_t::e_lcrlbracket))
         {
            if (token_is(token_t::e_lbracket))
            {
               close_bracket = token_t::e_rbracket;
               seperator     = token_t::e_comma;
            }
            else
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR41 - Expected '"+ details::to_str(close_bracket) + "' for call to multi-sequence" +
                             ((!source.empty()) ? std::string(" section of " + source): "")));
               return error_node();
            }
         }

         std::deque<expression_node_ptr> arg_list;
         expression_node_ptr result = error_node();

         scoped_deq_delete<expression_node_t> sdd(*this,arg_list);

         for (;;)
         {
            expression_node_ptr arg = parse_expression();
            if (0 == arg)
               return error_node();
            else
               arg_list.push_back(arg);
            if (token_is(close_bracket))
               break;

            bool is_next_close = peek_token_is(close_bracket);

            if (!token_is(seperator) && is_next_close)
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR42 - Expected '"+ details::to_str(seperator) +"' for call to multi-sequence section of " + source));
               return error_node();
            }

            if (token_is(close_bracket))
               break;
         }

         result = simplify(arg_list);

         sdd.delete_ptr = (0 == result);
         return result;
      }

      struct range_pack
      {
         range_pack()
         : n0_e(std::make_pair(false,expression_node_ptr(0))),
           n1_e(std::make_pair(false,expression_node_ptr(0))),
           n0_c(std::make_pair(false,0)),
           n1_c(std::make_pair(false,0))
         {}

         void clear()
         {
            n0_e = std::make_pair(false,expression_node_ptr(0));
            n1_e = std::make_pair(false,expression_node_ptr(0));
            n0_c = std::make_pair(false,0);
            n1_c = std::make_pair(false,0);
         }

         void free()
         {
            if (n0_e.first && n0_e.second)
            {
               n0_e.first = false;
               if (!details::is_variable_node(n0_e.second))
               {
                  delete n0_e.second;
               }
            }
            if (n1_e.first && n1_e.second)
            {
               n1_e.first = false;
               if (!details::is_variable_node(n1_e.second))
               {
                  delete n1_e.second;
               }
            }
         }

         bool const_range()
         {
           return ( n0_c.first &&  n1_c.first) &&
                  (!n0_e.first && !n1_e.first);
         }

         bool var_range()
         {
           return ( n0_e.first &&  n1_e.first) &&
                  (!n0_c.first && !n1_c.first);
         }

         bool operator()(std::size_t& r0, std::size_t& r1, const std::size_t& size = std::numeric_limits<std::size_t>::max()) const
         {
            if (n0_c.first)
               r0 = n0_c.second;
            else if (n0_e.first)
            {
               T r0_value = n0_e.second->value();
               if (r0_value < 0)
                  return false;
               else
                  r0 = static_cast<std::size_t>(r0_value);
            }
            else
               return false;

            if (n1_c.first)
               r1 = n1_c.second;
            else if (n1_e.first)
            {
               T r1_value = n1_e.second->value();
               if (r1_value < 0)
                  return false;
               else
                  r1 = static_cast<std::size_t>(r1_value);
            }
            else
               return false;

            if (
                (std::numeric_limits<std::size_t>::max() != size) &&
                (std::numeric_limits<std::size_t>::max() == r1  )
               )
               r1 = size;

            return (r0 <= r1);
         }

         std::pair<bool,expression_node_ptr> n0_e;
         std::pair<bool,expression_node_ptr> n1_e;
         std::pair<bool,std::size_t        > n0_c;
         std::pair<bool,std::size_t        > n1_c;
      };

      inline bool parse_range(range_pack& rp)
      {
         //Examples of valid ranges:
         // 1. [1:5]     -> 1..5
         // 2. [ :5]     -> 0..5
         // 3. [1: ]     -> 1..end
         // 4. [x:y]     -> x..y where x <= y
         // 5. [x+1:y/2] -> x+1..y/2 where x+1 <= y/2
         // 6. [ :y]     -> 0..y where 0 <= y
         // 7. [x: ]     -> x..end where x <= end

         rp.clear();

         if (!token_is(token_t::e_lsqrbracket))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR43 - Expected '[' for start of range."));
            return false;
         }

         if (token_is(token_t::e_colon))
         {
            rp.n0_c.first  = true;
            rp.n0_c.second = 0;
         }
         else
         {
            expression_node_ptr r0 = parse_expression();

            if (0 == r0)
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR44 - Failed parse begin section of range."));
               return false;

            }
            else if (is_constant_node(r0))
            {
               T r0_value = r0->value();
               if (r0_value >= T(0))
               {
                  rp.n0_c.first  = true;
                  rp.n0_c.second = static_cast<std::size_t>(r0_value);
               }
               free_node(node_allocator_,r0);
               if (r0_value < T(0))
               {
                  set_error(
                     make_error(parser_error::e_syntax,
                                current_token_,
                                "ERR45 - Range lower bound less than zero! Constraint: r0 >= 0"));
                  return false;
               }
            }
            else
            {
               rp.n0_e.first  = true;
               rp.n0_e.second = r0;
            }

            if (!token_is(token_t::e_colon))
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR46 - Expected ':' for break  in range."));
               rp.free();
               return false;
            }
         }

         if (token_is(token_t::e_rsqrbracket))
         {
            rp.n1_c.first  = true;
            rp.n1_c.second = std::numeric_limits<std::size_t>::max();
         }
         else
         {
            expression_node_ptr r1 = parse_expression();

            if (0 == r1)
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR47 - Failed parse end section of range."));
               rp.free();
               return false;

            }
            else if (is_constant_node(r1))
            {
               T r1_value = r1->value();
               if (r1_value >= T(0))
               {
                  rp.n1_c.first  = true;
                  rp.n1_c.second = static_cast<std::size_t>(r1_value);
               }
               free_node(node_allocator_,r1);
               if (r1_value < T(0))
               {
                  set_error(
                     make_error(parser_error::e_syntax,
                                current_token_,
                                "ERR48 - Range upper bound less than zero! Constraint: r1 >= 0"));
                  return false;
               }
            }
            else
            {
               rp.n1_e.first  = true;
               rp.n1_e.second = r1;
            }

            if (!token_is(token_t::e_rsqrbracket))
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR49 - Expected ']' for start of range."));
               rp.free();
               return false;
            }
         }

         if (rp.const_range())
         {
            std::size_t r0 = 0;
            std::size_t r1 = 0;
            bool rp_result = rp(r0,r1);
            if (!rp_result || (r0 > r1))
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR50 - Invalid range, Constraint: r0 <= r1"));
               return false;
            }
         }

         return true;
      }

      inline void cache_symbol(const std::string& symbol)
      {
         if (symbol_name_caching_)
         {
            symbol_name_cache_.push_back(symbol);
         }
      }

      inline expression_node_ptr parse_string()
      {
         const std::string symbol = current_token_.value;

         if (!symbol_table_.is_conststr_stringvar(symbol))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR51 - Unknown string symbol"));
            return error_node();
         }

         expression_node_ptr result = symbol_table_.get_stringvar(symbol);

         if (symbol_table_.is_constant_node(symbol))
         {
            result = expression_generator_(dynamic_cast<details::string_literal_node<T>*>(result)->str());
         }

         cache_symbol(symbol);

         if (peek_token_is(token_t::e_lsqrbracket))
         {
            next_token();
            range_pack rp;
            if (!parse_range(rp))
            {
               free_node(node_allocator_,result);
               return error_node();
            }
            result = expression_generator_(dynamic_cast<details::stringvar_node<T>*>(result)->ref(),rp);
            if (result)
               rp.clear();
         }
         else
            next_token();

         return result;
      }

      inline expression_node_ptr parse_const_string()
      {
         const std::string const_str = current_token_.value;
         expression_node_ptr result = expression_generator_(const_str);
         if (peek_token_is(token_t::e_lsqrbracket))
         {
            next_token();
            range_pack rp;
            if (!parse_range(rp))
            {
               free_node(node_allocator_,result);
               return error_node();
            }

            free_node(node_allocator_,result);

            if (rp.n1_c.first && (rp.n1_c.second == std::numeric_limits<std::size_t>::max()))
            {
               rp.n1_c.second = const_str.size() - 1;
            }

            if (
                (rp.n0_c.first && rp.n0_c.second >= const_str.size()) ||
                (rp.n1_c.first && rp.n1_c.second >= const_str.size())
               )
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR52 - Overflow in range for string: '" + const_str + "'[" +
                             (rp.n0_c.first ? details::to_str(rp.n0_c.second) : "?") + ":" +
                             (rp.n1_c.first ? details::to_str(rp.n1_c.second) : "?") + "]"));
               return error_node();
            }
            result = expression_generator_(const_str,rp);
            if (result)
               rp.clear();
         }
         else
            next_token();
         return result;
      }

      inline expression_node_ptr parse_vararg_function_call(ivararg_function<T>* vararg_function, const std::string& vararg_function_name)
      {
         std::vector<expression_node_ptr> arg_list;
         expression_node_ptr result = error_node();

         const std::string symbol = current_token_.value;

         scoped_vec_delete<expression_node_t> sdd(*this,arg_list);

         next_token();
         if (!token_is(token_t::e_lbracket))
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR53 - Expected '(' for call to vararg function: " + vararg_function_name));
            return error_node();
         }

         for ( ; ; )
         {
            expression_node_ptr arg = parse_expression();
            if (0 == arg)
               return error_node();
            else
               arg_list.push_back(arg);

            if (token_is(token_t::e_rbracket))
               break;
            else if (!token_is(token_t::e_comma))
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR54 - Expected ',' for call to vararg function: " + vararg_function_name));
               return error_node();
            }
         }

         result = expression_generator_.vararg_function_call(vararg_function,arg_list);

         sdd.delete_ptr = (0 == result);

         return result;
      }

      template <typename Type, std::size_t NumberOfParameters>
      struct parse_special_function_impl
      {
         static inline expression_node_ptr process(parser<Type>& p,const details::operator_type opt_type)
         {
            expression_node_ptr branch[NumberOfParameters];
            expression_node_ptr result  = error_node();
            std::fill_n(branch,NumberOfParameters,reinterpret_cast<expression_node_ptr>(0));
            scoped_delete<expression_node_t,NumberOfParameters> sd(p,branch);

            p.next_token();
            if (!p.token_is(token_t::e_lbracket))
            {
               return error_node();
            }

            for (std::size_t i = 0; i < NumberOfParameters; ++i)
            {
               branch[i] = p.parse_expression();
               if (0 == branch[i])
               {
                  return p.error_node();
               }
               else if (i < (NumberOfParameters - 1))
               {
                  if (!p.token_is(token_t::e_comma))
                  {
                     return p.error_node();
                  }
               }
            }

            if (!p.token_is(token_t::e_rbracket))
               return p.error_node();
            else
               result = p.expression_generator_.special_function(opt_type,branch);
            sd.delete_ptr = (0 == result);
            return result;
         }
      };

      inline expression_node_ptr parse_special_function()
      {
         // Expect: $fDD(expr0,expr1,expr2) or $fDD(expr0,expr1,expr2,expr3)
         if (
             !details::is_digit(current_token_.value[2]) ||
             !details::is_digit(current_token_.value[3])
            )
         {
            set_error(
               make_error(parser_error::e_token,
                          current_token_,
                          "ERR55 - Invalid special function[1]: " + current_token_.value));
            return error_node();
         }

         const unsigned int id = (current_token_.value[2] - '0') * 10 + (current_token_.value[3] - '0');

         if (id >= details::e_sffinal)
         {
            set_error(
               make_error(parser_error::e_token,
                          current_token_,
                          "ERR56 - Invalid special function[2]: " + current_token_.value));
            return error_node();
         }

         const std::size_t sf_3_to_4 = details::e_sf48;
         const details::operator_type opt_type = details::operator_type(id + 1000);
         const std::size_t NumberOfParameters = (id < (sf_3_to_4 - 1000)) ? 3 : 4;

         switch (NumberOfParameters)
         {
            case 3  : return parse_special_function_impl<T,3>::process(*this,opt_type);
            case 4  : return parse_special_function_impl<T,4>::process(*this,opt_type);
            default : return error_node();
         }
      }

      inline expression_node_ptr parse_null_statement()
      {
         next_token();
         return node_allocator_.allocate<details::null_node<T> >();
      }

      inline expression_node_ptr parse_symtab_symbol()
      {
         const std::string symbol = current_token_.value;
         // Are we dealing with a variable or a special constant?
         expression_node_ptr variable = symbol_table_.get_variable(symbol);
         if (variable)
         {
            cache_symbol(symbol);
            if (symbol_table_.is_constant_node(symbol))
            {
               variable = expression_generator_(variable->value());
            }
            next_token();
            return variable;
         }

         #ifndef exprtk_disable_string_capabilities
         // Are we dealing with a string variable?
         if (symbol_table_.is_stringvar(symbol))
         {
            return parse_string();
         }
         #endif

         // Are we dealing with a function?
         ifunction<T>* function = symbol_table_.get_function(symbol);

         if (function)
         {
            expression_node_ptr func_node = reinterpret_cast<expression_node_ptr>(0);
            switch (function->param_count)
            {
               case  0 : func_node = parse_function_call_0  (function,symbol); break;
               case  1 : func_node = parse_function_call< 1>(function,symbol); break;
               case  2 : func_node = parse_function_call< 2>(function,symbol); break;
               case  3 : func_node = parse_function_call< 3>(function,symbol); break;
               case  4 : func_node = parse_function_call< 4>(function,symbol); break;
               case  5 : func_node = parse_function_call< 5>(function,symbol); break;
               case  6 : func_node = parse_function_call< 6>(function,symbol); break;
               case  7 : func_node = parse_function_call< 7>(function,symbol); break;
               case  8 : func_node = parse_function_call< 8>(function,symbol); break;
               case  9 : func_node = parse_function_call< 9>(function,symbol); break;
               case 10 : func_node = parse_function_call<10>(function,symbol); break;
               case 11 : func_node = parse_function_call<11>(function,symbol); break;
               case 12 : func_node = parse_function_call<12>(function,symbol); break;
               case 13 : func_node = parse_function_call<13>(function,symbol); break;
               case 14 : func_node = parse_function_call<14>(function,symbol); break;
               case 15 : func_node = parse_function_call<15>(function,symbol); break;
               case 16 : func_node = parse_function_call<16>(function,symbol); break;
               case 17 : func_node = parse_function_call<17>(function,symbol); break;
               case 18 : func_node = parse_function_call<18>(function,symbol); break;
               case 19 : func_node = parse_function_call<19>(function,symbol); break;
               case 20 : func_node = parse_function_call<20>(function,symbol); break;
               default : {
                           set_error(
                              make_error(parser_error::e_syntax,
                                         current_token_,
                                         "ERR57 - Invalid number of parameters for function: " + symbol));
                           return error_node();
                         }
            }

            if (func_node)
               return func_node;
            else
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR58 - Failed to generate node for function: '" + symbol + "'"));
               return error_node();
            }
         }

         // Are we dealing with a vararg function?
         ivararg_function<T>* vararg_function = symbol_table_.get_vararg_function(symbol);

         if (vararg_function)
         {
            expression_node_ptr vararg_func_node = error_node();
            vararg_func_node = parse_vararg_function_call(vararg_function,symbol);

            if (vararg_func_node)
               return vararg_func_node;
            else
            {
               set_error(
                  make_error(parser_error::e_syntax,
                             current_token_,
                             "ERR59 - Failed to generate node for vararg function: '" + symbol + "'"));
               return error_node();
            }
         }

         // Should we handle unknown symbols?
         if (resolve_unknown_symbol_ && unknown_symbol_resolver_)
         {
            T default_value = T(0);
            std::string error_message;
            typename unknown_symbol_resolver::symbol_type symbol_type;

            if (unknown_symbol_resolver_->process(symbol,symbol_type,default_value,error_message))
            {
               bool create_result = false;
               switch (symbol_type)
               {
                  case unknown_symbol_resolver::e_variable_type : create_result = symbol_table_.create_variable(symbol,default_value);
                                                                  break;

                  case unknown_symbol_resolver::e_constant_type : create_result = symbol_table_.add_constant(symbol,default_value);
                                                                  break;

                  default                                       : create_result = false;
               }

               if (create_result)
               {
                  expression_node_ptr variable = symbol_table_.get_variable(symbol);
                  if (variable)
                  {
                     cache_symbol(symbol);
                     if (symbol_table_.is_constant_node(symbol))
                     {
                        variable = expression_generator_(variable->value());
                     }
                     next_token();
                     return variable;
                  }
               }

               set_error(
                  make_error(parser_error::e_symtab,
                             current_token_,
                             "ERR60 - Failed to create variable: '" + symbol + "'"));

               return error_node();
            }
         }

         set_error(
            make_error(parser_error::e_syntax,
                       current_token_,
                       "ERR61 - Undefined variable or function: '" + symbol + "'"));
         return error_node();
      }

      inline expression_node_ptr parse_symbol()
      {
         static const std::string symbol_if     = "if";
         static const std::string symbol_while  = "while";
         static const std::string symbol_repeat = "repeat";
         static const std::string symbol_switch = "switch";
         static const std::string symbol_null   = "null";

         if (valid_vararg_operation(current_token_.value))
         {
            return parse_vararg_function();
         }
         else if (valid_base_operation(current_token_.value))
         {
            return parse_base_operation();
         }
         else if (details::imatch(current_token_.value,symbol_if))
         {
            return parse_conditional_statement();
         }
         else if (details::imatch(current_token_.value,symbol_while))
         {
            return parse_while_loop();
         }
         else if (details::imatch(current_token_.value,symbol_repeat))
         {
            return parse_repeat_until_loop();
         }
         else if (details::imatch(current_token_.value,symbol_switch))
         {
            return parse_switch_statement();
         }
         else if (details::is_valid_sf_symbol(current_token_.value))
         {
            return parse_special_function();
         }
         else if (details::imatch(current_token_.value,symbol_null))
         {
            return parse_null_statement();
         }
         else if (symbol_table_.valid())
         {
            return parse_symtab_symbol();
         }
         else
         {
            set_error(
               make_error(parser_error::e_symtab,
                          current_token_,
                          "ERR62 - Variable or function detected, yet symbol-table is invalid, Symbol: " + current_token_.value));
            return error_node();
         }
      }

      inline expression_node_ptr parse_branch(precedence_level precedence = e_level00)
      {
         if (token_t::e_number == current_token_.type)
         {
            T numeric_value = T(0);

            if (details::string_to_real(current_token_.value,numeric_value))
            {
               expression_node_ptr literal_exp = expression_generator_(numeric_value);
               next_token();
               return literal_exp;
            }
            else
               return error_node();
         }
         else if (token_t::e_symbol == current_token_.type)
         {
            return parse_symbol();
         }
         #ifndef exprtk_disable_string_capabilities
         else if (token_t::e_string == current_token_.type)
         {
            return parse_const_string();
         }
         #endif
         else if (token_t::e_lbracket == current_token_.type)
         {
            next_token();
            expression_node_ptr branch = parse_expression();
            if (token_is(token_t::e_rbracket))
               return branch;
            else
               return error_node();
         }
         else if (token_t::e_lsqrbracket == current_token_.type)
         {
            next_token();
            expression_node_ptr branch = parse_expression();
            if (token_is(token_t::e_rsqrbracket))
               return branch;
            else
               return error_node();
         }
         else if (token_t::e_lcrlbracket == current_token_.type)
         {
            next_token();
            expression_node_ptr branch = parse_expression();

            if (token_is(token_t::e_rcrlbracket))
               return branch;
            else
               return error_node();
         }
         else if (token_t::e_sub == current_token_.type)
         {
            next_token();
            return expression_generator_(details::e_neg,
                                         // Was the previous operation exponentiation?
                                         (e_level12 == precedence) ?
                                         parse_branch    (e_level09) :
                                         parse_expression(e_level09));
         }
         else if (token_t::e_add == current_token_.type)
         {
            next_token();
            return parse_expression(e_level13);
         }
         else if (token_t::e_eof == current_token_.type)
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR63 - Premature end of expression.[1]"));
            return error_node();
         }
         else
         {
            set_error(
               make_error(parser_error::e_syntax,
                          current_token_,
                          "ERR64 - Premature end of expression.[2]"));
            return error_node();
         }
      }

      inline bool token_is(const typename token_t::token_type& ttype, const bool advance_token = true)
      {
         if (current_token_.type != ttype)
         {
            return false;
         }

         if (advance_token)
         {
            next_token();
         }

         return true;
      }

      inline bool peek_token_is(const typename token_t::token_type& ttype)
      {
         return (lexer_.peek_next_token().type == ttype);
      }

      template <typename Type>
      class expression_generator
      {
      public:

         typedef details::expression_node<Type>* expression_node_ptr;
         typedef expression_node_ptr (*synthesize_functor_t)(expression_generator<T>&, const details::operator_type& operation, expression_node_ptr (&branch)[2]);
         typedef std::map<std::string,synthesize_functor_t> synthesize_map_t;
         typedef const Type& vtype;
         typedef const Type  ctype;

         inline void init_synthesize_map()
         {
            synthesize_map_["(v)o(v)"] = synthesize_vov_expression::process;
            synthesize_map_["(c)o(v)"] = synthesize_cov_expression::process;
            synthesize_map_["(v)o(c)"] = synthesize_voc_expression::process;

            #define register_synthezier(S)                      \
            synthesize_map_[S ::node_type::id()] = S ::process; \

            register_synthezier(synthesize_vovov_expression0)
            register_synthezier(synthesize_vovov_expression1)
            register_synthezier(synthesize_vovoc_expression0)
            register_synthezier(synthesize_vovoc_expression1)
            register_synthezier(synthesize_vocov_expression0)
            register_synthezier(synthesize_vocov_expression1)
            register_synthezier(synthesize_covov_expression0)
            register_synthezier(synthesize_covov_expression1)
            register_synthezier(synthesize_covoc_expression0)
            register_synthezier(synthesize_covoc_expression1)
            register_synthezier(synthesize_cocov_expression1)
            register_synthezier(synthesize_vococ_expression0)

            register_synthezier(synthesize_vovovov_expression0)
            register_synthezier(synthesize_vovovoc_expression0)
            register_synthezier(synthesize_vovocov_expression0)
            register_synthezier(synthesize_vocovov_expression0)
            register_synthezier(synthesize_covovov_expression0)
            register_synthezier(synthesize_covocov_expression0)
            register_synthezier(synthesize_vocovoc_expression0)
            register_synthezier(synthesize_covovoc_expression0)
            register_synthezier(synthesize_vococov_expression0)

            register_synthezier(synthesize_vovovov_expression1)
            register_synthezier(synthesize_vovovoc_expression1)
            register_synthezier(synthesize_vovocov_expression1)
            register_synthezier(synthesize_vocovov_expression1)
            register_synthezier(synthesize_covovov_expression1)
            register_synthezier(synthesize_covocov_expression1)
            register_synthezier(synthesize_vocovoc_expression1)
            register_synthezier(synthesize_covovoc_expression1)
            register_synthezier(synthesize_vococov_expression1)

            register_synthezier(synthesize_vovovov_expression2)
            register_synthezier(synthesize_vovovoc_expression2)
            register_synthezier(synthesize_vovocov_expression2)
            register_synthezier(synthesize_vocovov_expression2)
            register_synthezier(synthesize_covovov_expression2)
            register_synthezier(synthesize_covocov_expression2)
            register_synthezier(synthesize_vocovoc_expression2)
            register_synthezier(synthesize_covovoc_expression2)

            register_synthezier(synthesize_vovovov_expression3)
            register_synthezier(synthesize_vovovoc_expression3)
            register_synthezier(synthesize_vovocov_expression3)
            register_synthezier(synthesize_vocovov_expression3)
            register_synthezier(synthesize_covovov_expression3)
            register_synthezier(synthesize_covocov_expression3)
            register_synthezier(synthesize_vocovoc_expression3)
            register_synthezier(synthesize_covovoc_expression3)
            register_synthezier(synthesize_vococov_expression3)

            register_synthezier(synthesize_vovovov_expression4)
            register_synthezier(synthesize_vovovoc_expression4)
            register_synthezier(synthesize_vovocov_expression4)
            register_synthezier(synthesize_vocovov_expression4)
            register_synthezier(synthesize_covovov_expression4)
            register_synthezier(synthesize_covocov_expression4)
            register_synthezier(synthesize_vocovoc_expression4)
            register_synthezier(synthesize_covovoc_expression4)
         }

         inline void set_uom(unary_op_map_t& unary_op_map)
         {
            unary_op_map_ = &unary_op_map;
         }

         inline void set_bom(binary_op_map_t& binary_op_map)
         {
            binary_op_map_ = &binary_op_map;
         }

         inline void set_ibom(inv_binary_op_map_t& inv_binary_op_map)
         {
            inv_binary_op_map_ = &inv_binary_op_map;
         }

         inline void set_sf3m(sf3_map_t& sf3_map)
         {
            sf3_map_ = &sf3_map;
         }

         inline void set_sf4m(sf4_map_t& sf4_map)
         {
            sf4_map_ = &sf4_map;
         }

         inline void set_allocator(details::node_allocator& na)
         {
            node_allocator_ = &na;
         }

         inline void set_strength_reduction_state(const bool strength_reduction_enabled)
         {
            strength_reduction_enabled_ = strength_reduction_enabled;
         }

         inline bool strength_reduction_enabled() const
         {
            return strength_reduction_enabled_;
         }

         inline bool valid_operator(const details::operator_type& operation, binary_functor_t& bop)
         {
            typename binary_op_map_t::iterator bop_itr = binary_op_map_->find(operation);
            if ((*binary_op_map_).end() == bop_itr)
               return false;
            bop = bop_itr->second;
            return true;
         }

         inline bool valid_operator(const details::operator_type& operation, unary_functor_t& uop)
         {
            typename unary_op_map_t::iterator uop_itr = unary_op_map_->find(operation);
            if ((*unary_op_map_).end() == uop_itr)
               return false;
            uop = uop_itr->second;
            return true;
         }

         inline details::operator_type get_operator(const binary_functor_t& bop)
         {
            return (*inv_binary_op_map_).find(bop)->second;
         }

         inline expression_node_ptr operator()(const Type& v) const
         {
            return node_allocator_->allocate<literal_node_t>(v);
         }

         inline expression_node_ptr operator()(const std::string& s) const
         {
            return node_allocator_->allocate<string_literal_node_t>(s);
         }

         inline expression_node_ptr operator()(std::string& s, range_pack& rp) const
         {
            return node_allocator_->allocate_rr<string_range_node_t>(s,rp);
         }

         inline expression_node_ptr operator()(const std::string& s, range_pack& rp) const
         {
            return node_allocator_->allocate_tt<const_string_range_node_t>(s,rp);
         }

         inline bool unary_optimizable(const details::operator_type& operation) const
         {
            return (details::e_abs   == operation) || (details::e_acos  == operation) ||
                   (details::e_acosh == operation) || (details::e_asin  == operation) ||
                   (details::e_asinh == operation) || (details::e_atan  == operation) ||
                   (details::e_atanh == operation) || (details::e_ceil  == operation) ||
                   (details::e_cos   == operation) || (details::e_cosh  == operation) ||
                   (details::e_exp   == operation) || (details::e_expm1 == operation) ||
                   (details::e_floor == operation) || (details::e_log   == operation) ||
                   (details::e_log10 == operation) || (details::e_log2  == operation) ||
                   (details::e_log1p == operation) || (details::e_neg   == operation) ||
                   (details::e_pos   == operation) || (details::e_round == operation) ||
                   (details::e_sin   == operation) || (details::e_sinh  == operation) ||
                   (details::e_sqrt  == operation) || (details::e_tan   == operation) ||
                   (details::e_tanh  == operation) || (details::e_cot   == operation) ||
                   (details::e_sec   == operation) || (details::e_csc   == operation) ||
                   (details::e_r2d   == operation) || (details::e_d2r   == operation) ||
                   (details::e_d2g   == operation) || (details::e_g2d   == operation) ||
                   (details::e_notl  == operation) || (details::e_sgn   == operation) ||
                   (details::e_erf   == operation) || (details::e_erfc  == operation) ||
                   (details::e_frac  == operation) || (details::e_trunc == operation);
         }

         inline bool sf3_optimizable(const std::string& sf3id, trinary_functor_t& tfunc)
         {
            typename sf3_map_t::iterator itr = sf3_map_->find(sf3id);
            if (sf3_map_->end() == itr)
               return false;
            else
               tfunc = itr->second.first;
            return true;
         }

         inline bool sf4_optimizable(const std::string& sf4id, quaternary_functor_t& qfunc)
         {
            typename sf4_map_t::iterator itr = sf4_map_->find(sf4id);
            if (sf4_map_->end() == itr)
               return false;
            else
               qfunc = itr->second.first;
            return true;
         }

         inline bool sf3_optimizable(const std::string& sf3id, details::operator_type& operation)
         {
            typename sf3_map_t::iterator itr = sf3_map_->find(sf3id);
            if (sf3_map_->end() == itr)
               return false;
            else
               operation = itr->second.second;
            return true;
         }

         inline bool sf4_optimizable(const std::string& sf4id, details::operator_type& operation)
         {
            typename sf4_map_t::iterator itr = sf4_map_->find(sf4id);
            if (sf4_map_->end() == itr)
               return false;
            else
               operation = itr->second.second;
            return true;
         }

         inline expression_node_ptr operator()(const details::operator_type& operation, expression_node_ptr (&branch)[1])
         {
            if (0 == branch[0])
               return error_node();
            else if (details::is_null_node(branch[0]))
               return branch[0];
            else if (details::is_constant_node(branch[0]))
               return synthesize_expression<unary_node_t,1>(operation,branch);
            else if (unary_optimizable(operation) && details::is_variable_node(branch[0]))
               return synthesize_uv_expression(operation,branch);
            else
               return synthesize_unary_expression(operation,branch);
         }

         #ifndef exprtk_disable_string_capabilities
         inline bool valid_string_operation(const details::operator_type& operation) const
         {
            return (details::e_add   == operation) ||
                   (details::e_lt    == operation) ||
                   (details::e_lte   == operation) ||
                   (details::e_gt    == operation) ||
                   (details::e_gte   == operation) ||
                   (details::e_eq    == operation) ||
                   (details::e_ne    == operation) ||
                   (details::e_in    == operation) ||
                   (details::e_like  == operation) ||
                   (details::e_ilike == operation);
         }
         #else
         inline bool valid_string_operation(const details::operator_type&) const
         {
            return false;
         }
         #endif

         inline std::string to_str(const details::operator_type& operation) const
         {
            switch (operation)
            {
               case details::e_add  : return "+";
               case details::e_sub  : return "-";
               case details::e_mul  : return "*";
               case details::e_div  : return "/";
               case details::e_mod  : return "%";
               case details::e_pow  : return "^";
               case details::e_lt   : return "<";
               case details::e_lte  : return "<=";
               case details::e_gt   : return ">";
               case details::e_gte  : return ">=";
               case details::e_eq   : return "==";
               case details::e_ne   : return "!=";
               case details::e_and  : return "and";
               case details::e_nand : return "nand";
               case details::e_or   : return "or";
               case details::e_nor  : return "nor";
               case details::e_xor  : return "xor";
               case details::e_xnor : return "xnor";
               default              : return "UNKNOWN";
            }
         }

         inline bool operation_optimizable(const details::operator_type& operation) const
         {
            return (details::e_add  == operation) ||
                   (details::e_sub  == operation) ||
                   (details::e_mul  == operation) ||
                   (details::e_div  == operation) ||
                   (details::e_mod  == operation) ||
                   (details::e_pow  == operation) ||
                   (details::e_lt   == operation) ||
                   (details::e_lte  == operation) ||
                   (details::e_gt   == operation) ||
                   (details::e_gte  == operation) ||
                   (details::e_eq   == operation) ||
                   (details::e_ne   == operation) ||
                   (details::e_and  == operation) ||
                   (details::e_nand == operation) ||
                   (details::e_or   == operation) ||
                   (details::e_nor  == operation) ||
                   (details::e_xor  == operation) ||
                   (details::e_xnor == operation) ||
                   false;
         }

         inline std::string branch_to_id(expression_node_ptr branch)
         {
            static const std::string null_str   ("(null)" );
            static const std::string const_str  ("(c)"    );
            static const std::string var_str    ("(v)"    );
            static const std::string vov_str    ("(vov)"  );
            static const std::string cov_str    ("(cov)"  );
            static const std::string voc_str    ("(voc)"  );
            static const std::string str_str    ("(s)"    );
            static const std::string strrng_str ("(rngs)" );
            static const std::string cs_str     ("(cs)"   );
            static const std::string cstrrng_str("(crngs)");

            if (details::is_null_node(branch))
               return null_str;
            else if (details::is_constant_node(branch))
               return const_str;
            else if (details::is_variable_node(branch))
               return var_str;
            else if (details::is_vov_node(branch))
               return vov_str;
            else if (details::is_cov_node(branch))
               return cov_str;
            else if (details::is_voc_node(branch))
               return voc_str;
            else if (details::is_string_node(branch))
               return str_str;
            else if (details::is_const_string_node(branch))
               return cs_str;
            else if (details::is_string_range_node(branch))
               return strrng_str;
            else if (details::is_const_string_range_node(branch))
               return cstrrng_str;
            else if (details::is_t0ot1ot2_node(branch))
               return "(" + dynamic_cast<details::T0oT1oT2_base_node<T>*>(branch)->type_id() + ")";
            else if (details::is_t0ot1ot2ot3_node(branch))
               return "(" + dynamic_cast<details::T0oT1oT2oT3_base_node<T>*>(branch)->type_id() + ")";
            else
               return "ERROR";
         }

         inline std::string branch_to_id(expression_node_ptr (&branch)[2])
         {
            return branch_to_id(branch[0]) + std::string("o") + branch_to_id(branch[1]);
         }

         inline bool cov_optimizable(const details::operator_type& operation, expression_node_ptr (&branch)[2]) const
         {
            if (!operation_optimizable(operation))
               return false;
            else
               return (details::is_constant_node(branch[0]) && details::is_variable_node(branch[1]));
         }

         inline bool voc_optimizable(const details::operator_type& operation, expression_node_ptr (&branch)[2]) const
         {
            if (!operation_optimizable(operation))
               return false;
            else
               return (details::is_variable_node(branch[0]) && details::is_constant_node(branch[1]));
         }

         inline bool vov_optimizable(const details::operator_type& operation, expression_node_ptr (&branch)[2]) const
         {
            if (!operation_optimizable(operation))
               return false;
            else
               return (details::is_variable_node(branch[0]) && details::is_variable_node(branch[1]));
         }

         inline bool cob_optimizable(const details::operator_type& operation, expression_node_ptr (&branch)[2]) const
         {
            if (!operation_optimizable(operation))
               return false;
            else
               return (details::is_constant_node(branch[0]) && !details::is_constant_node(branch[1]));
         }

         inline bool boc_optimizable(const details::operator_type& operation, expression_node_ptr (&branch)[2]) const
         {
            if (!operation_optimizable(operation))
               return false;
            else
               return (!details::is_constant_node(branch[0]) && details::is_constant_node(branch[1]));
         }

         inline bool uvouv_optimizable(const details::operator_type& operation, expression_node_ptr (&branch)[2]) const
         {
            if (!operation_optimizable(operation))
               return false;
            else
               return (details::is_uv_node(branch[0]) && details::is_uv_node(branch[1]));
         }

         inline bool vob_optimizable(const details::operator_type& operation, expression_node_ptr (&branch)[2]) const
         {
            if (!operation_optimizable(operation))
               return false;
            else
               return (details::is_variable_node(branch[0]) && !details::is_variable_node(branch[1]));
         }

         inline bool bov_optimizable(const details::operator_type& operation, expression_node_ptr (&branch)[2]) const
         {
            if (!operation_optimizable(operation))
               return false;
            else
               return (!details::is_variable_node(branch[0]) && details::is_variable_node(branch[1]));
         }

         inline bool binext_optimizable(const details::operator_type& operation, expression_node_ptr (&branch)[2]) const
         {
            if (!operation_optimizable(operation))
               return false;
            else
               return (!details::is_constant_node(branch[0]) || !details::is_constant_node(branch[1]));
         }

         inline bool is_invalid_string_op(const details::operator_type& operation, expression_node_ptr (&branch)[2])
         {
            const bool b0_string = is_generally_string_node(branch[0]);
            const bool b1_string = is_generally_string_node(branch[1]);
            if ((b0_string || b1_string) && !(b0_string && b1_string))
               return true;
            if (!valid_string_operation(operation) && b0_string && b1_string)
               return true;
            else
               return false;
         }

         inline bool is_invalid_string_op(const details::operator_type& operation, expression_node_ptr (&branch)[3])
         {
            const bool b0_string = is_generally_string_node(branch[0]);
            const bool b1_string = is_generally_string_node(branch[1]);
            const bool b2_string = is_generally_string_node(branch[2]);
            if ((b0_string || b1_string || b2_string) && !(b0_string && b1_string && b2_string))
               return true;
            if ((details::e_inrange != operation) && b0_string && b1_string && b2_string)
               return true;
            else
               return false;
         }

         inline bool is_string_operation(const details::operator_type& operation, expression_node_ptr (&branch)[2])
         {
            const bool b0_string = is_generally_string_node(branch[0]);
            const bool b1_string = is_generally_string_node(branch[1]);
            return (b0_string && b1_string && valid_string_operation(operation));
         }

         inline bool is_string_operation(const details::operator_type& operation, expression_node_ptr (&branch)[3])
         {
            const bool b0_string = is_generally_string_node(branch[0]);
            const bool b1_string = is_generally_string_node(branch[1]);
            const bool b2_string = is_generally_string_node(branch[2]);
            return (b0_string && b1_string && b2_string && (details::e_inrange == operation));
         }

         #ifndef exprtk_disable_sc_andor
         inline bool is_shortcircuit_expression(const details::operator_type& operation)
         {
            return ((details::e_scand == operation) || (details::e_scor == operation));
         }
         #else
         inline bool is_shortcircuit_expression(const details::operator_type&)
         {
            return false;
         }
         #endif

         inline bool is_null_present(expression_node_ptr (&branch)[2])
         {
            return details::is_null_node(branch[0]) ||
                   details::is_null_node(branch[1]);
         }

         inline expression_node_ptr operator()(const details::operator_type& operation, expression_node_ptr (&branch)[2])
         {
            if ((0 == branch[0]) || (0 == branch[1]))
            {
               details::free_all_nodes(*node_allocator_,branch);
               return error_node();
            }
            else if (is_invalid_string_op(operation,branch))
               return error_node();
            else if (details::e_assign == operation)
               return synthesize_assignment_expression(operation,branch);
            else if (is_shortcircuit_expression(operation))
               return synthesize_shortcircuit_expression(operation,branch);
            else if (is_string_operation(operation,branch))
               return synthesize_string_expression(operation,branch);
            else if (is_null_present(branch))
               return synthesize_null_expression(operation,branch);

            expression_node_ptr result = error_node();

            if (synthesize_expression(operation,branch,result))
               return result;
            else if (uvouv_optimizable(operation,branch))
               return synthesize_uvouv_expression(operation,branch);
            else if (vob_optimizable(operation,branch))
               return synthesize_vob_expression::process(*this,operation,branch);
            else if (bov_optimizable(operation,branch))
               return synthesize_bov_expression::process(*this,operation,branch);
            else if (cob_optimizable(operation,branch))
               return synthesize_cob_expression::process(*this,operation,branch);
            else if (boc_optimizable(operation,branch))
               return synthesize_boc_expression::process(*this,operation,branch);
            else if (cov_optimizable(operation,branch))
               return synthesize_cov_expression::process(*this,operation,branch);
            else if (binext_optimizable(operation,branch))
               return synthesize_binary_ext_expression::process(*this,operation,branch);
            else
               return synthesize_expression<binary_node_t,2>(operation,branch);
         }

         inline expression_node_ptr operator()(const details::operator_type& operation, expression_node_ptr (&branch)[3])
         {
            if ((0 == branch[0]) || (0 == branch[1]) || (0 == branch[2]))
            {
               details::free_all_nodes(*node_allocator_,branch);
               return error_node();
            }
            else if (is_invalid_string_op(operation,branch))
               return error_node();
            else if (is_string_operation(operation,branch))
               return synthesize_string_expression(operation,branch);
            else
               return synthesize_expression<trinary_node_t,3>(operation,branch);
         }

         inline expression_node_ptr operator()(const details::operator_type& operation, expression_node_ptr (&branch)[4])
         {
            return synthesize_expression<quaternary_node_t,4>(operation,branch);
         }

         inline expression_node_ptr operator()(const details::operator_type& operation, expression_node_ptr (&branch)[5])
         {
            return synthesize_expression<quinary_node_t,5>(operation,branch);
         }

         inline expression_node_ptr operator()(const details::operator_type& operation, expression_node_ptr (&branch)[6])
         {
            return synthesize_expression<senary_node_t,6>(operation,branch);
         }

         inline expression_node_ptr operator()(const details::operator_type& operation, expression_node_ptr b0)
         {
            expression_node_ptr branch[1] = { b0 };
            return synthesize_expression<unary_node_t,1>(operation,branch);
         }

         inline expression_node_ptr operator()(const details::operator_type& operation, expression_node_ptr b0, expression_node_ptr b1)
         {
            if ((0 == b0) || (0 == b1))
            {
               details::free_node(*node_allocator_,b0);
               details::free_node(*node_allocator_,b1);
               return error_node();
            }
            else
            {
               expression_node_ptr branch[2] = { b0, b1 };
               return expression_generator<Type>::operator()(operation,branch);
            }
         }

         inline expression_node_ptr conditional(expression_node_ptr condition,
                                                expression_node_ptr consequent,
                                                expression_node_ptr alternative) const
         {
            if ((0 == condition) || (0 == consequent) || (0 == alternative))
            {
               free_node(*node_allocator_,condition);
               free_node(*node_allocator_,consequent);
               free_node(*node_allocator_,alternative);
               return error_node();
            }
            // Can the condition be immediately evaluated? if so optimize.
            else if (details::is_constant_node(condition))
            {
               // True branch
               if (details::is_true(condition))
               {
                  free_node(*node_allocator_,condition);
                  free_node(*node_allocator_,alternative);
                  return consequent;
               }
               // False branch
               else
               {
                  free_node(*node_allocator_,condition);
                  free_node(*node_allocator_,consequent);
                  return alternative;
               }
            }
            else
               return node_allocator_->allocate<conditional_node_t>(condition,consequent,alternative);
         }

         inline expression_node_ptr while_loop(expression_node_ptr condition,
                                               expression_node_ptr branch) const
         {
            if (details::is_constant_node(condition))
            {
               expression_node_ptr result = error_node();
               if (details::is_true(condition))
                  //Infinite loops are not allowed.
                  result = error_node();
               else
                  result = node_allocator_->allocate<details::null_node<Type> >();
               free_node(*node_allocator_,condition);
               free_node(*node_allocator_,branch);
               return result;
            }
            else if (details::is_null_node(condition))
            {
               free_node(*node_allocator_,condition);
               return branch;
            }
            else
               return node_allocator_->allocate<while_loop_node_t>(condition,branch);
         }

         inline expression_node_ptr repeat_until_loop(expression_node_ptr condition,
                                                      expression_node_ptr branch) const
         {
            if (details::is_constant_node(condition))
            {
               if (details::is_true(condition) && details::is_constant_node(branch))
               {
                  free_node(*node_allocator_,condition);
                  return branch;
               }
               expression_node_ptr result = error_node();
               free_node(*node_allocator_,condition);
               free_node(*node_allocator_,branch);
               return result;
            }
            else if (details::is_null_node(condition))
            {
               free_node(*node_allocator_,condition);
               return branch;
            }
            else
               return node_allocator_->allocate<repeat_until_loop_node_t>(condition,branch);
         }

         template <typename Allocator,
                   template <typename,typename> class Sequence>
         inline expression_node_ptr const_optimize_switch(Sequence<expression_node_ptr,Allocator>& arglist)
         {
            expression_node_ptr result = error_node();

            for (std::size_t i = 0; i < (arglist.size() / 2); ++i)
            {
               expression_node_ptr condition  = arglist[(2 * i)    ];
               expression_node_ptr consequent = arglist[(2 * i) + 1];

               if ((0 == result) && details::is_true(condition))
               {
                  result = consequent;
                  break;
               }
            }

            if (0 == result)
            {
               result = arglist.back();
            }

            for (std::size_t i = 0; i < arglist.size(); ++i)
            {
               expression_node_ptr current_expr = arglist[i];

               if (current_expr && (current_expr != result))
               {
                  free_node(*node_allocator_,current_expr);
               }
            }

            return result;
         }

         template <typename Allocator,
                   template <typename,typename> class Sequence>
         inline expression_node_ptr const_optimize_mswitch(Sequence<expression_node_ptr,Allocator>& arglist)
         {
            expression_node_ptr result = error_node();

            for (std::size_t i = 0; i < (arglist.size() / 2); ++i)
            {
               expression_node_ptr condition  = arglist[(2 * i)    ];
               expression_node_ptr consequent = arglist[(2 * i) + 1];

               if (details::is_true(condition))
               {
                  result = consequent;
               }
            }

            if (0 == result)
            {
               T zero = T(0);
               result = node_allocator_->allocate<literal_node_t>(zero);
            }

            for (std::size_t i = 0; i < arglist.size(); ++i)
            {
               expression_node_ptr current_expr = arglist[i];

               if (current_expr && (current_expr != result))
               {
                  free_node(*node_allocator_,current_expr);
               }
            }

            return result;
         }

         template <typename Allocator,
                   template <typename,typename> class Sequence>
         inline expression_node_ptr switch_statement(Sequence<expression_node_ptr,Allocator>& arglist)
         {
            if (!all_nodes_valid(arglist))
            {
               details::free_all_nodes(*node_allocator_,arglist);
               return error_node();
            }
            else if (is_constant_foldable(arglist))
               return const_optimize_switch(arglist);
            else
               return node_allocator_->allocate<details::switch_node<Type> >(arglist);
         }

         template <typename Allocator,
                   template <typename,typename> class Sequence>
         inline expression_node_ptr multi_switch_statement(Sequence<expression_node_ptr,Allocator>& arglist)
         {
            if (!all_nodes_valid(arglist))
            {
               details::free_all_nodes(*node_allocator_,arglist);
               return error_node();
            }
            else if (is_constant_foldable(arglist))
               return const_optimize_mswitch(arglist);
            else
               return node_allocator_->allocate<details::multi_switch_node<Type> >(arglist);
         }

         #define unary_opr_switch_statements           \
         case_stmt(details::  e_abs,details::  abs_op) \
         case_stmt(details:: e_acos,details:: acos_op) \
         case_stmt(details::e_acosh,details::acosh_op) \
         case_stmt(details:: e_asin,details:: asin_op) \
         case_stmt(details::e_asinh,details::asinh_op) \
         case_stmt(details:: e_atan,details:: atan_op) \
         case_stmt(details::e_atanh,details::atanh_op) \
         case_stmt(details:: e_ceil,details:: ceil_op) \
         case_stmt(details::  e_cos,details::  cos_op) \
         case_stmt(details:: e_cosh,details:: cosh_op) \
         case_stmt(details::  e_exp,details::  exp_op) \
         case_stmt(details::e_expm1,details::expm1_op) \
         case_stmt(details::e_floor,details::floor_op) \
         case_stmt(details::  e_log,details::  log_op) \
         case_stmt(details::e_log10,details::log10_op) \
         case_stmt(details:: e_log2,details:: log2_op) \
         case_stmt(details::e_log1p,details::log1p_op) \
         case_stmt(details::  e_neg,details::  neg_op) \
         case_stmt(details::  e_pos,details::  pos_op) \
         case_stmt(details::e_round,details::round_op) \
         case_stmt(details::  e_sin,details::  sin_op) \
         case_stmt(details:: e_sinh,details:: sinh_op) \
         case_stmt(details:: e_sqrt,details:: sqrt_op) \
         case_stmt(details::  e_tan,details::  tan_op) \
         case_stmt(details:: e_tanh,details:: tanh_op) \
         case_stmt(details::  e_cot,details::  cot_op) \
         case_stmt(details::  e_sec,details::  sec_op) \
         case_stmt(details::  e_csc,details::  csc_op) \
         case_stmt(details::  e_r2d,details::  r2d_op) \
         case_stmt(details::  e_d2r,details::  d2r_op) \
         case_stmt(details::  e_d2g,details::  d2g_op) \
         case_stmt(details::  e_g2d,details::  g2d_op) \
         case_stmt(details:: e_notl,details:: notl_op) \
         case_stmt(details::  e_sgn,details::  sgn_op) \
         case_stmt(details::  e_erf,details::  erf_op) \
         case_stmt(details:: e_erfc,details:: erfc_op) \
         case_stmt(details:: e_frac,details:: frac_op) \
         case_stmt(details::e_trunc,details::trunc_op) \

         inline expression_node_ptr synthesize_uv_expression(const details::operator_type& operation, expression_node_ptr (&branch)[1])
         {
            T& v = dynamic_cast<details::variable_node<T>*>(branch[0])->ref();
            switch (operation)
            {
               #define case_stmt(op0,op1)                                                          \
               case op0 : return node_allocator_->                                                 \
                             allocate<typename details::unary_variable_node<Type,op1<Type> > >(v); \

               unary_opr_switch_statements
               #undef case_stmt
               default : return error_node();
            }
         }

         inline expression_node_ptr synthesize_unary_expression(const details::operator_type& operation, expression_node_ptr (&branch)[1])
         {
            switch (operation)
            {
               #define case_stmt(op0,op1)                                                                \
               case op0 : return node_allocator_->                                                       \
                             allocate<typename details::unary_branch_node<Type,op1<Type> > >(branch[0]); \

               unary_opr_switch_statements
               #undef case_stmt
               default : return error_node();
            }
         }

         inline expression_node_ptr const_optimize_sf3(const details::operator_type& operation, expression_node_ptr (&branch)[3])
         {
            expression_node_ptr temp_node = error_node();
            switch (operation)
            {
               #define case_stmt(op0,op1)                                              \
               case op0 : temp_node = node_allocator_->                                \
                                         allocate<details::sf3_node<Type,op1<Type> > > \
                                            (operation,branch);                        \
                          break;                                                       \

               case_stmt(details::e_sf00,details::sf00_op) case_stmt(details::e_sf01,details::sf01_op)
               case_stmt(details::e_sf02,details::sf02_op) case_stmt(details::e_sf03,details::sf03_op)
               case_stmt(details::e_sf04,details::sf04_op) case_stmt(details::e_sf05,details::sf05_op)
               case_stmt(details::e_sf06,details::sf06_op) case_stmt(details::e_sf07,details::sf07_op)
               case_stmt(details::e_sf08,details::sf08_op) case_stmt(details::e_sf09,details::sf09_op)
               case_stmt(details::e_sf10,details::sf10_op) case_stmt(details::e_sf11,details::sf11_op)
               case_stmt(details::e_sf12,details::sf12_op) case_stmt(details::e_sf13,details::sf13_op)
               case_stmt(details::e_sf14,details::sf14_op) case_stmt(details::e_sf15,details::sf15_op)
               case_stmt(details::e_sf16,details::sf16_op) case_stmt(details::e_sf17,details::sf17_op)
               case_stmt(details::e_sf18,details::sf18_op) case_stmt(details::e_sf19,details::sf19_op)
               case_stmt(details::e_sf20,details::sf20_op) case_stmt(details::e_sf21,details::sf21_op)
               case_stmt(details::e_sf22,details::sf22_op) case_stmt(details::e_sf23,details::sf23_op)
               case_stmt(details::e_sf24,details::sf24_op) case_stmt(details::e_sf25,details::sf25_op)
               case_stmt(details::e_sf26,details::sf26_op) case_stmt(details::e_sf27,details::sf27_op)
               case_stmt(details::e_sf28,details::sf28_op) case_stmt(details::e_sf29,details::sf29_op)
               case_stmt(details::e_sf30,details::sf30_op) case_stmt(details::e_sf31,details::sf31_op)
               case_stmt(details::e_sf32,details::sf32_op) case_stmt(details::e_sf33,details::sf33_op)
               case_stmt(details::e_sf34,details::sf34_op) case_stmt(details::e_sf35,details::sf35_op)
               case_stmt(details::e_sf36,details::sf36_op) case_stmt(details::e_sf37,details::sf37_op)
               case_stmt(details::e_sf38,details::sf38_op) case_stmt(details::e_sf39,details::sf39_op)
               case_stmt(details::e_sf40,details::sf40_op) case_stmt(details::e_sf41,details::sf41_op)
               case_stmt(details::e_sf42,details::sf42_op) case_stmt(details::e_sf43,details::sf43_op)
               case_stmt(details::e_sf44,details::sf44_op) case_stmt(details::e_sf45,details::sf45_op)
               case_stmt(details::e_sf46,details::sf46_op) case_stmt(details::e_sf47,details::sf47_op)
               #undef case_stmt
               default : return error_node();
            }

            T v = temp_node->value();
            node_allocator_->free(temp_node);
            details::free_node(*node_allocator_,temp_node);

            return node_allocator_->allocate<literal_node_t>(v);
         }

         inline expression_node_ptr varnode_optimize_sf3(const details::operator_type& operation, expression_node_ptr (&branch)[3])
         {
            const Type& v0 = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
            const Type& v1 = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
            const Type& v2 = dynamic_cast<details::variable_node<Type>*>(branch[2])->ref();

            switch (operation)
            {
               #define case_stmt(op0,op1)                                                 \
               case op0 : return node_allocator_->                                        \
                                    allocate_rrr<details::sf3_var_node<Type,op1<Type> > > \
                                       (v0,v1,v2);                                        \

               case_stmt(details::e_sf00,details::sf00_op) case_stmt(details::e_sf01,details::sf01_op)
               case_stmt(details::e_sf02,details::sf02_op) case_stmt(details::e_sf03,details::sf03_op)
               case_stmt(details::e_sf04,details::sf04_op) case_stmt(details::e_sf05,details::sf05_op)
               case_stmt(details::e_sf06,details::sf06_op) case_stmt(details::e_sf07,details::sf07_op)
               case_stmt(details::e_sf08,details::sf08_op) case_stmt(details::e_sf09,details::sf09_op)
               case_stmt(details::e_sf10,details::sf10_op) case_stmt(details::e_sf11,details::sf11_op)
               case_stmt(details::e_sf12,details::sf12_op) case_stmt(details::e_sf13,details::sf13_op)
               case_stmt(details::e_sf14,details::sf14_op) case_stmt(details::e_sf15,details::sf15_op)
               case_stmt(details::e_sf16,details::sf16_op) case_stmt(details::e_sf17,details::sf17_op)
               case_stmt(details::e_sf18,details::sf18_op) case_stmt(details::e_sf19,details::sf19_op)
               case_stmt(details::e_sf20,details::sf20_op) case_stmt(details::e_sf21,details::sf21_op)
               case_stmt(details::e_sf22,details::sf22_op) case_stmt(details::e_sf23,details::sf23_op)
               case_stmt(details::e_sf24,details::sf24_op) case_stmt(details::e_sf25,details::sf25_op)
               case_stmt(details::e_sf26,details::sf26_op) case_stmt(details::e_sf27,details::sf27_op)
               case_stmt(details::e_sf28,details::sf28_op) case_stmt(details::e_sf29,details::sf29_op)
               case_stmt(details::e_sf30,details::sf30_op) case_stmt(details::e_sf31,details::sf31_op)
               case_stmt(details::e_sf32,details::sf32_op) case_stmt(details::e_sf33,details::sf33_op)
               case_stmt(details::e_sf34,details::sf34_op) case_stmt(details::e_sf35,details::sf35_op)
               case_stmt(details::e_sf36,details::sf36_op) case_stmt(details::e_sf37,details::sf37_op)
               case_stmt(details::e_sf38,details::sf38_op) case_stmt(details::e_sf39,details::sf39_op)
               case_stmt(details::e_sf40,details::sf40_op) case_stmt(details::e_sf41,details::sf41_op)
               case_stmt(details::e_sf42,details::sf42_op) case_stmt(details::e_sf43,details::sf43_op)
               case_stmt(details::e_sf44,details::sf44_op) case_stmt(details::e_sf45,details::sf45_op)
               case_stmt(details::e_sf46,details::sf46_op) case_stmt(details::e_sf47,details::sf47_op)
               #undef case_stmt
               default : return error_node();
            }
         }

         inline expression_node_ptr special_function(const details::operator_type& operation, expression_node_ptr (&branch)[3])
         {
            if (!all_nodes_valid(branch))
               return error_node();
            else if (is_constant_foldable(branch))
               return const_optimize_sf3(operation,branch);
            else if (all_nodes_variables(branch))
               return varnode_optimize_sf3(operation,branch);
            else
            {
               switch (operation)
               {
                  #define case_stmt(op0,op1)                                                     \
                  case op0 : return node_allocator_->                                            \
                                allocate<details::sf3_node<Type,op1<Type> > >(operation,branch); \

                  case_stmt(details::e_sf00,details::sf00_op) case_stmt(details::e_sf01,details::sf01_op)
                  case_stmt(details::e_sf02,details::sf02_op) case_stmt(details::e_sf03,details::sf03_op)
                  case_stmt(details::e_sf04,details::sf04_op) case_stmt(details::e_sf05,details::sf05_op)
                  case_stmt(details::e_sf06,details::sf06_op) case_stmt(details::e_sf07,details::sf07_op)
                  case_stmt(details::e_sf08,details::sf08_op) case_stmt(details::e_sf09,details::sf09_op)
                  case_stmt(details::e_sf10,details::sf10_op) case_stmt(details::e_sf11,details::sf11_op)
                  case_stmt(details::e_sf12,details::sf12_op) case_stmt(details::e_sf13,details::sf13_op)
                  case_stmt(details::e_sf14,details::sf14_op) case_stmt(details::e_sf15,details::sf15_op)
                  case_stmt(details::e_sf16,details::sf16_op) case_stmt(details::e_sf17,details::sf17_op)
                  case_stmt(details::e_sf18,details::sf18_op) case_stmt(details::e_sf19,details::sf19_op)
                  case_stmt(details::e_sf20,details::sf20_op) case_stmt(details::e_sf21,details::sf21_op)
                  case_stmt(details::e_sf22,details::sf22_op) case_stmt(details::e_sf23,details::sf23_op)
                  case_stmt(details::e_sf24,details::sf24_op) case_stmt(details::e_sf25,details::sf25_op)
                  case_stmt(details::e_sf26,details::sf26_op) case_stmt(details::e_sf27,details::sf27_op)
                  case_stmt(details::e_sf28,details::sf28_op) case_stmt(details::e_sf29,details::sf29_op)
                  case_stmt(details::e_sf30,details::sf30_op) case_stmt(details::e_sf31,details::sf31_op)
                  case_stmt(details::e_sf32,details::sf32_op) case_stmt(details::e_sf33,details::sf33_op)
                  case_stmt(details::e_sf34,details::sf34_op) case_stmt(details::e_sf35,details::sf35_op)
                  case_stmt(details::e_sf36,details::sf36_op) case_stmt(details::e_sf37,details::sf37_op)
                  case_stmt(details::e_sf38,details::sf38_op) case_stmt(details::e_sf39,details::sf39_op)
                  case_stmt(details::e_sf40,details::sf40_op) case_stmt(details::e_sf41,details::sf41_op)
                  case_stmt(details::e_sf42,details::sf42_op) case_stmt(details::e_sf43,details::sf43_op)
                  case_stmt(details::e_sf44,details::sf44_op) case_stmt(details::e_sf45,details::sf45_op)
                  case_stmt(details::e_sf46,details::sf46_op) case_stmt(details::e_sf47,details::sf47_op)
                  #undef case_stmt
                  default : return error_node();
               }
            }
         }

         inline expression_node_ptr const_optimize_sf4(const details::operator_type& operation, expression_node_ptr (&branch)[4])
         {
            expression_node_ptr temp_node = error_node();
            switch (operation)
            {
               #define case_stmt(op0,op1)                                                                 \
               case op0 : temp_node = node_allocator_->                                                   \
                                         allocate<details::sf4_node<Type,op1<Type> > >(operation,branch); \
                          break;                                                                          \

               case_stmt(details::e_sf48,details::sf48_op) case_stmt(details::e_sf49,details::sf49_op)
               case_stmt(details::e_sf50,details::sf50_op) case_stmt(details::e_sf51,details::sf51_op)
               case_stmt(details::e_sf52,details::sf52_op) case_stmt(details::e_sf53,details::sf53_op)
               case_stmt(details::e_sf54,details::sf54_op) case_stmt(details::e_sf55,details::sf55_op)
               case_stmt(details::e_sf56,details::sf56_op) case_stmt(details::e_sf57,details::sf57_op)
               case_stmt(details::e_sf58,details::sf58_op) case_stmt(details::e_sf59,details::sf59_op)
               case_stmt(details::e_sf60,details::sf60_op) case_stmt(details::e_sf61,details::sf61_op)
               case_stmt(details::e_sf62,details::sf62_op) case_stmt(details::e_sf63,details::sf63_op)
               case_stmt(details::e_sf64,details::sf64_op) case_stmt(details::e_sf65,details::sf65_op)
               case_stmt(details::e_sf66,details::sf66_op) case_stmt(details::e_sf67,details::sf67_op)
               case_stmt(details::e_sf68,details::sf68_op) case_stmt(details::e_sf69,details::sf69_op)
               case_stmt(details::e_sf70,details::sf70_op) case_stmt(details::e_sf71,details::sf71_op)
               case_stmt(details::e_sf72,details::sf72_op) case_stmt(details::e_sf73,details::sf73_op)
               case_stmt(details::e_sf74,details::sf74_op) case_stmt(details::e_sf75,details::sf75_op)
               case_stmt(details::e_sf76,details::sf76_op) case_stmt(details::e_sf77,details::sf77_op)
               case_stmt(details::e_sf78,details::sf78_op) case_stmt(details::e_sf79,details::sf79_op)
               case_stmt(details::e_sf80,details::sf80_op) case_stmt(details::e_sf81,details::sf81_op)
               case_stmt(details::e_sf82,details::sf82_op) case_stmt(details::e_sf83,details::sf83_op)
               case_stmt(details::e_sf84,details::sf84_op) case_stmt(details::e_sf85,details::sf85_op)
               case_stmt(details::e_sf86,details::sf86_op) case_stmt(details::e_sf87,details::sf87_op)
               case_stmt(details::e_sf88,details::sf88_op) case_stmt(details::e_sf89,details::sf89_op)
               case_stmt(details::e_sf90,details::sf90_op) case_stmt(details::e_sf91,details::sf91_op)
               case_stmt(details::e_sf92,details::sf92_op) case_stmt(details::e_sf93,details::sf93_op)
               case_stmt(details::e_sf94,details::sf94_op) case_stmt(details::e_sf95,details::sf95_op)
               case_stmt(details::e_sf96,details::sf96_op) case_stmt(details::e_sf97,details::sf97_op)
               case_stmt(details::e_sf98,details::sf98_op) case_stmt(details::e_sf99,details::sf99_op)
               #undef case_stmt
               default : return error_node();
            }
            T v = temp_node->value();
            details::free_node(*node_allocator_,temp_node);
            return node_allocator_->allocate<literal_node_t>(v);
         }

         inline expression_node_ptr varnode_optimize_sf4(const details::operator_type& operation, expression_node_ptr (&branch)[4])
         {
            const Type& v0 = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
            const Type& v1 = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
            const Type& v2 = dynamic_cast<details::variable_node<Type>*>(branch[2])->ref();
            const Type& v3 = dynamic_cast<details::variable_node<Type>*>(branch[3])->ref();

            switch (operation)
            {
               #define case_stmt(op0,op1)                                                         \
               case op0 : return node_allocator_->                                                \
                             allocate_rrrr<details::sf4_var_node<Type,op1<Type> > >(v0,v1,v2,v3); \

               case_stmt(details::e_sf48,details::sf48_op) case_stmt(details::e_sf49,details::sf49_op)
               case_stmt(details::e_sf50,details::sf50_op) case_stmt(details::e_sf51,details::sf51_op)
               case_stmt(details::e_sf52,details::sf52_op) case_stmt(details::e_sf53,details::sf53_op)
               case_stmt(details::e_sf54,details::sf54_op) case_stmt(details::e_sf55,details::sf55_op)
               case_stmt(details::e_sf56,details::sf56_op) case_stmt(details::e_sf57,details::sf57_op)
               case_stmt(details::e_sf58,details::sf58_op) case_stmt(details::e_sf59,details::sf59_op)
               case_stmt(details::e_sf60,details::sf60_op) case_stmt(details::e_sf61,details::sf61_op)
               case_stmt(details::e_sf62,details::sf62_op) case_stmt(details::e_sf63,details::sf63_op)
               case_stmt(details::e_sf64,details::sf64_op) case_stmt(details::e_sf65,details::sf65_op)
               case_stmt(details::e_sf66,details::sf66_op) case_stmt(details::e_sf67,details::sf67_op)
               case_stmt(details::e_sf68,details::sf68_op) case_stmt(details::e_sf69,details::sf69_op)
               case_stmt(details::e_sf70,details::sf70_op) case_stmt(details::e_sf71,details::sf71_op)
               case_stmt(details::e_sf72,details::sf72_op) case_stmt(details::e_sf73,details::sf73_op)
               case_stmt(details::e_sf74,details::sf74_op) case_stmt(details::e_sf75,details::sf75_op)
               case_stmt(details::e_sf76,details::sf76_op) case_stmt(details::e_sf77,details::sf77_op)
               case_stmt(details::e_sf78,details::sf78_op) case_stmt(details::e_sf79,details::sf79_op)
               case_stmt(details::e_sf80,details::sf80_op) case_stmt(details::e_sf81,details::sf81_op)
               case_stmt(details::e_sf82,details::sf82_op) case_stmt(details::e_sf83,details::sf83_op)
               case_stmt(details::e_sf84,details::sf84_op) case_stmt(details::e_sf85,details::sf85_op)
               case_stmt(details::e_sf86,details::sf86_op) case_stmt(details::e_sf87,details::sf87_op)
               case_stmt(details::e_sf88,details::sf88_op) case_stmt(details::e_sf89,details::sf89_op)
               case_stmt(details::e_sf90,details::sf90_op) case_stmt(details::e_sf91,details::sf91_op)
               case_stmt(details::e_sf92,details::sf92_op) case_stmt(details::e_sf93,details::sf93_op)
               case_stmt(details::e_sf94,details::sf94_op) case_stmt(details::e_sf95,details::sf95_op)
               case_stmt(details::e_sf96,details::sf96_op) case_stmt(details::e_sf97,details::sf97_op)
               case_stmt(details::e_sf98,details::sf98_op) case_stmt(details::e_sf99,details::sf99_op)
               #undef case_stmt
               default : return error_node();
            }
         }

         inline expression_node_ptr special_function(const details::operator_type& operation, expression_node_ptr (&branch)[4])
         {
            if (!all_nodes_valid(branch))
               return error_node();
            else if (is_constant_foldable(branch))
               return const_optimize_sf4(operation,branch);
            else if (all_nodes_variables(branch))
               return varnode_optimize_sf4(operation,branch);
            switch (operation)
            {
               #define case_stmt(op0,op1)                                                     \
               case op0 : return node_allocator_->                                            \
                             allocate<details::sf4_node<Type,op1<Type> > >(operation,branch); \

               case_stmt(details::e_sf48,details::sf48_op) case_stmt(details::e_sf49,details::sf49_op)
               case_stmt(details::e_sf50,details::sf50_op) case_stmt(details::e_sf51,details::sf51_op)
               case_stmt(details::e_sf52,details::sf52_op) case_stmt(details::e_sf53,details::sf53_op)
               case_stmt(details::e_sf54,details::sf54_op) case_stmt(details::e_sf55,details::sf55_op)
               case_stmt(details::e_sf56,details::sf56_op) case_stmt(details::e_sf57,details::sf57_op)
               case_stmt(details::e_sf58,details::sf58_op) case_stmt(details::e_sf59,details::sf59_op)
               case_stmt(details::e_sf60,details::sf60_op) case_stmt(details::e_sf61,details::sf61_op)
               case_stmt(details::e_sf62,details::sf62_op) case_stmt(details::e_sf63,details::sf63_op)
               case_stmt(details::e_sf64,details::sf64_op) case_stmt(details::e_sf65,details::sf65_op)
               case_stmt(details::e_sf66,details::sf66_op) case_stmt(details::e_sf67,details::sf67_op)
               case_stmt(details::e_sf68,details::sf68_op) case_stmt(details::e_sf69,details::sf69_op)
               case_stmt(details::e_sf70,details::sf70_op) case_stmt(details::e_sf71,details::sf71_op)
               case_stmt(details::e_sf72,details::sf72_op) case_stmt(details::e_sf73,details::sf73_op)
               case_stmt(details::e_sf74,details::sf74_op) case_stmt(details::e_sf75,details::sf75_op)
               case_stmt(details::e_sf76,details::sf76_op) case_stmt(details::e_sf77,details::sf77_op)
               case_stmt(details::e_sf78,details::sf78_op) case_stmt(details::e_sf79,details::sf79_op)
               case_stmt(details::e_sf80,details::sf80_op) case_stmt(details::e_sf81,details::sf81_op)
               case_stmt(details::e_sf82,details::sf82_op) case_stmt(details::e_sf83,details::sf83_op)
               case_stmt(details::e_sf84,details::sf84_op) case_stmt(details::e_sf85,details::sf85_op)
               case_stmt(details::e_sf86,details::sf86_op) case_stmt(details::e_sf87,details::sf87_op)
               case_stmt(details::e_sf88,details::sf88_op) case_stmt(details::e_sf89,details::sf89_op)
               case_stmt(details::e_sf90,details::sf90_op) case_stmt(details::e_sf91,details::sf91_op)
               case_stmt(details::e_sf92,details::sf92_op) case_stmt(details::e_sf93,details::sf93_op)
               case_stmt(details::e_sf94,details::sf94_op) case_stmt(details::e_sf95,details::sf95_op)
               case_stmt(details::e_sf96,details::sf96_op) case_stmt(details::e_sf97,details::sf97_op)
               case_stmt(details::e_sf98,details::sf98_op) case_stmt(details::e_sf99,details::sf99_op)
               #undef case_stmt
               default : return error_node();
            }
         }

         template <typename Allocator,
                   template <typename,typename> class Sequence>
         inline expression_node_ptr const_optimize_varargfunc(const details::operator_type& operation, Sequence<expression_node_ptr,Allocator>& arglist)
         {
            expression_node_ptr temp_node = error_node();
            switch (operation)
            {
               #define case_stmt(op0,op1)                                                 \
               case op0 : temp_node = node_allocator_->                                   \
                                         allocate<details::vararg_node<Type,op1<Type> > > \
                                            (arglist);                                    \
                          break;                                                          \

               case_stmt(details::e_sum,  details::vararg_add_op  )
               case_stmt(details::e_prod, details::vararg_mul_op  )
               case_stmt(details::e_avg,  details::vararg_avg_op  )
               case_stmt(details::e_min,  details::vararg_min_op  )
               case_stmt(details::e_max,  details::vararg_max_op  )
               case_stmt(details::e_mand, details::vararg_mand_op )
               case_stmt(details::e_mor,  details::vararg_mor_op  )
               case_stmt(details::e_multi,details::vararg_multi_op)
               #undef case_stmt
               default : return error_node();
            }
            T v = temp_node->value();
            details::free_node(*node_allocator_,temp_node);
            return node_allocator_->allocate<literal_node_t>(v);
         }

         template <typename Allocator,
                   template <typename,typename> class Sequence>
         inline expression_node_ptr varnode_optimize_varargfunc(const details::operator_type& operation, Sequence<expression_node_ptr,Allocator>& arglist)
         {
            switch (operation)
            {
               #define case_stmt(op0,op1)                                                  \
               case op0 : return node_allocator_->                                         \
                             allocate<details::vararg_varnode<Type,op1<Type> > >(arglist); \

               case_stmt(details::e_sum,  details::vararg_add_op  )
               case_stmt(details::e_prod, details::vararg_mul_op  )
               case_stmt(details::e_avg,  details::vararg_avg_op  )
               case_stmt(details::e_min,  details::vararg_min_op  )
               case_stmt(details::e_max,  details::vararg_max_op  )
               case_stmt(details::e_mand, details::vararg_mand_op )
               case_stmt(details::e_mor,  details::vararg_mor_op  )
               case_stmt(details::e_multi,details::vararg_multi_op)
               #undef case_stmt
               default : return error_node();
            }
         }

         template <typename Allocator,
                   template <typename,typename> class Sequence>
         inline expression_node_ptr vararg_function(const details::operator_type& operation, Sequence<expression_node_ptr,Allocator>& arglist)
         {
            if (!all_nodes_valid(arglist))
            {
               details::free_all_nodes(*node_allocator_,arglist);
               return error_node();
            }
            else if (is_constant_foldable(arglist))
               return const_optimize_varargfunc(operation,arglist);
            else if (all_nodes_variables(arglist))
               return varnode_optimize_varargfunc(operation,arglist);

            switch (operation)
            {
               #define case_stmt(op0,op1)                                               \
               case op0 : return node_allocator_->                                      \
                             allocate<details::vararg_node<Type,op1<Type> > >(arglist); \

               case_stmt(details::e_sum,  details::vararg_add_op  )
               case_stmt(details::e_prod, details::vararg_mul_op  )
               case_stmt(details::e_avg,  details::vararg_avg_op  )
               case_stmt(details::e_min,  details::vararg_min_op  )
               case_stmt(details::e_max,  details::vararg_max_op  )
               case_stmt(details::e_mand, details::vararg_mand_op )
               case_stmt(details::e_mor,  details::vararg_mor_op  )
               case_stmt(details::e_multi,details::vararg_multi_op)
               #undef case_stmt
               default : return error_node();
            }
         }

         template <std::size_t N>
         inline expression_node_ptr function(ifunction_t* f, expression_node_ptr (&b)[N])
         {
            typedef typename details::function_N_node<T,ifunction_t,N> function_N_node_t;
            expression_node_ptr result = synthesize_expression<function_N_node_t,N>(f,b);
            if (0 == result)
               return error_node();
            else
            {
               // Can the function call be completely optimized?
               if (details::is_constant_node(result))
                  return result;
               else if (!all_nodes_valid(b))
                  return error_node();
               else if (N != f->param_count)
               {
                  details::free_all_nodes(*node_allocator_,b);
                  return error_node();
               }

               function_N_node_t* func_node_ptr = dynamic_cast<function_N_node_t*>(result);

               if (func_node_ptr)
               {
                  if (func_node_ptr->init_branches(b))
                     return result;
                  else
                  {
                     details::free_all_nodes(*node_allocator_,b);
                     return error_node();
                  }
               }
               else
               {
                  details::free_all_nodes(*node_allocator_,b);
                  return error_node();
               }
            }
         }

         inline expression_node_ptr function(ifunction_t* f)
         {
            typedef typename details::function_N_node<T,ifunction_t,0> function_N_node_t;
            return node_allocator_->allocate<function_N_node_t>(f);
         }

         inline expression_node_ptr vararg_function_call(ivararg_function_t* vaf,
                                                         std::vector<expression_node_ptr>& arglist)
         {
            if (!all_nodes_valid(arglist))
            {
               details::free_all_nodes(*node_allocator_,arglist);
               return error_node();
            }

            expression_node_ptr result = node_allocator_->allocate<details::vararg_function_node<Type,ivararg_function<T> > >(vaf,arglist);

            if (is_constant_foldable(arglist))
            {
               T v = result->value();
               details::free_node(*node_allocator_,result);
               result = node_allocator_->allocate<literal_node_t>(v);
            }

            return result;
         }

      private:

         template <std::size_t N, typename NodePtr>
         inline bool is_constant_foldable(NodePtr (&b)[N]) const
         {
            for (std::size_t i = 0; i < N; ++i)
            {
               if (0 == b[i])
                  return false;
               else if (!details::is_constant_node(b[i]))
                  return false;
            }

            return true;
         }

         template <typename NodePtr,
                   typename Allocator,
                   template <typename,typename> class Sequence>
         inline bool is_constant_foldable(const Sequence<NodePtr,Allocator>& b) const
         {
            for (std::size_t i = 0; i < b.size(); ++i)
            {
               if (0 == b[i])
                  return false;
               else if (!details::is_constant_node(b[i]))
                  return false;
            }

            return true;
         }

         inline expression_node_ptr synthesize_assignment_expression(const details::operator_type& operation, expression_node_ptr (&branch)[2])
         {
            if (details::is_variable_node(branch[0]))
               return synthesize_expression<assignment_node_t,2>(operation,branch);
            else
               return error_node();
         }

         #ifndef exprtk_disable_sc_andor
         inline expression_node_ptr synthesize_shortcircuit_expression(const details::operator_type& operation, expression_node_ptr (&branch)[2])
         {
            expression_node_ptr result = error_node();
            if (details::is_constant_node(branch[0]))
            {
               if ((details::e_scand == operation) && (T(0) == branch[0]->value()))
                  result = node_allocator_->allocate_c<literal_node_t>(T(0));
               else if ((details::e_scor == operation) && (T(0) != branch[0]->value()))
                  result = node_allocator_->allocate_c<literal_node_t>(T(1));
            }

            if (details::is_constant_node(branch[1]) && (0 == result))
            {
               if ((details::e_scand == operation) && (T(0) == branch[1]->value()))
                  result = node_allocator_->allocate_c<literal_node_t>(T(0));
               else if ((details::e_scor == operation) && (T(0) != branch[1]->value()))
                  result = node_allocator_->allocate_c<literal_node_t>(T(1));
            }

            if (result)
            {
               free_node(*node_allocator_,branch[0]);
               free_node(*node_allocator_,branch[1]);
               return result;
            }
            else if (details::e_scand == operation)
               return synthesize_expression<scand_node_t,2>(operation,branch);
            else if (details::e_scor == operation)
               return synthesize_expression<scor_node_t,2>(operation,branch);
            else
               return error_node();
         }
         #else
         inline expression_node_ptr synthesize_shortcircuit_expression(const details::operator_type&, expression_node_ptr (&)[2])
         {
            return error_node();
         }
         #endif

         #define basic_opr_switch_statements         \
         case_stmt(details:: e_add,details:: add_op) \
         case_stmt(details:: e_sub,details:: sub_op) \
         case_stmt(details:: e_mul,details:: mul_op) \
         case_stmt(details:: e_div,details:: div_op) \
         case_stmt(details:: e_mod,details:: mod_op) \
         case_stmt(details:: e_pow,details:: pow_op) \

         #define extended_opr_switch_statements      \
         case_stmt(details::  e_lt,details::  lt_op) \
         case_stmt(details:: e_lte,details:: lte_op) \
         case_stmt(details::  e_gt,details::  gt_op) \
         case_stmt(details:: e_gte,details:: gte_op) \
         case_stmt(details::  e_eq,details::  eq_op) \
         case_stmt(details::  e_ne,details::  ne_op) \
         case_stmt(details:: e_and,details:: and_op) \
         case_stmt(details::e_nand,details::nand_op) \
         case_stmt(details::  e_or,details::  or_op) \
         case_stmt(details:: e_nor,details:: nor_op) \
         case_stmt(details:: e_xor,details:: xor_op) \
         case_stmt(details::e_xnor,details::xnor_op) \

         #ifndef exprtk_disable_cardinal_pow_optimisation
         template <template <typename,typename> class IPowNode>
         inline expression_node_ptr cardinal_pow_optimization_impl(const T& v, const unsigned int& p)
         {
            switch (p)
            {
               #define case_stmt(cp)                                                     \
               case cp : return node_allocator_->                                        \
                            allocate<IPowNode<T,details::numeric::fast_exp<T,cp> > >(v); \

               case_stmt( 1) case_stmt( 2) case_stmt( 3) case_stmt( 4)
               case_stmt( 5) case_stmt( 6) case_stmt( 7) case_stmt( 8)
               case_stmt( 9) case_stmt(10) case_stmt(11) case_stmt(12)
               case_stmt(13) case_stmt(14) case_stmt(15) case_stmt(16)
               case_stmt(17) case_stmt(18) case_stmt(19) case_stmt(20)
               case_stmt(21) case_stmt(22) case_stmt(23) case_stmt(24)
               case_stmt(25) case_stmt(26) case_stmt(27) case_stmt(28)
               case_stmt(29) case_stmt(30) case_stmt(31) case_stmt(32)
               case_stmt(33) case_stmt(34) case_stmt(35) case_stmt(36)
               case_stmt(37) case_stmt(38) case_stmt(39) case_stmt(40)
               case_stmt(41) case_stmt(42) case_stmt(43) case_stmt(44)
               case_stmt(45) case_stmt(46) case_stmt(47) case_stmt(48)
               case_stmt(49) case_stmt(50) case_stmt(51) case_stmt(52)
               case_stmt(53) case_stmt(54) case_stmt(55) case_stmt(56)
               case_stmt(57) case_stmt(58) case_stmt(59) case_stmt(60)
               #undef case_stmt
               default : return error_node();
            }
         }

         inline expression_node_ptr cardinal_pow_optimization(const T& v, const T& c)
         {
            const bool not_recipricol = (c >= T(0));
            const unsigned int p = static_cast<unsigned int>(std::abs(c));
            if (0 == p)
               return node_allocator_->allocate_c<literal_node_t>(T(1));
            else if (T(2) == c)
            {
               return node_allocator_->
                  template allocate_rr<typename details::vov_node<Type,details::mul_op<Type> > >(v,v);
            }
            else
            {
               if (not_recipricol)
                  return cardinal_pow_optimization_impl<details::ipow_node>(v,p);
               else
                  return cardinal_pow_optimization_impl<details::ipowinv_node>(v,p);
            }
         }

         inline bool cardinal_pow_optimizable(const details::operator_type& operation, const T& c)
         {
            return (details::e_pow == operation) && (std::abs(c) <= T(60)) && details::numeric::is_integer(c);
         }
         #else
         inline expression_node_ptr cardinal_pow_optimization(T&, const T&)
         {
            return error_node();
         }

         inline bool cardinal_pow_optimizable(const details::operator_type&, const T&)
         {
            return false;
         }
         #endif

         struct synthesize_binary_ext_expression
         {
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               switch (operation)
               {
                  #define case_stmt(op0,op1)                                                           \
                  case op0 : return expr_gen.node_allocator_->                                         \
                                template allocate<typename details::binary_ext_node<Type,op1<Type> > > \
                                   (branch[0],branch[1]);                                              \

                  basic_opr_switch_statements
                  extended_opr_switch_statements
                  #undef case_stmt
                  default : return error_node();
               }
            }
         };

         struct synthesize_vob_expression
         {
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               const Type& v = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();

               if (details::is_sf3ext_node(branch[1]))
               {
                  expression_node_ptr result = error_node();
                  if (synthesize_sf4ext_expression::template compile_right<vtype>(expr_gen,v,operation,branch[1],result))
                  {
                     free_node(*expr_gen.node_allocator_,branch[1]);
                     return result;
                  }
               }

               switch (operation)
               {
                  #define case_stmt(op0,op1)                                                       \
                  case op0 : return expr_gen.node_allocator_->                                     \
                                template allocate_rc<typename details::vob_node<Type,op1<Type> > > \
                                   (v,branch[1]);                                                  \

                  basic_opr_switch_statements
                  extended_opr_switch_statements
                  #undef case_stmt
                  default : return error_node();
               }
            }
         };

         struct synthesize_bov_expression
         {
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               const Type& v = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();

               if (details::is_sf3ext_node(branch[0]))
               {
                  expression_node_ptr result = error_node();
                  if (synthesize_sf4ext_expression::template compile_left<vtype>(expr_gen,v,operation,branch[0],result))
                  {
                     free_node(*expr_gen.node_allocator_,branch[0]);
                     return result;
                  }
               }

               switch (operation)
               {
                  #define case_stmt(op0,op1)                                                       \
                  case op0 : return expr_gen.node_allocator_->                                     \
                                template allocate_cr<typename details::bov_node<Type,op1<Type> > > \
                                   (branch[0],v);                                                  \

                  basic_opr_switch_statements
                  extended_opr_switch_statements
                  #undef case_stmt
                  default : return error_node();
               }
            }
         };

         struct synthesize_cob_expression
         {
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               const Type c = dynamic_cast<details::literal_node<Type>*>(branch[0])->value();
               free_node(*expr_gen.node_allocator_,branch[0]);
               if (details::is_cob_node(branch[1]))
               {
                  // Simplify expressions of the form:
                  // 1. (1 * (2 * (3 * (4 * (5 * (6 * (7 * (8 * (9 + x)))))))))
                  // 2. (1 + (2 + (3 + (4 + (5 + (6 + (7 + (8 + (9 + x)))))))))
                  if (
                      (operation == details::e_mul) ||
                      (operation == details::e_add)
                     )
                  {
                     details::cob_base_node<Type>* cobnode = dynamic_cast<details::cob_base_node<Type>*>(branch[1]);
                     if (operation == cobnode->operation())
                     {
                        switch (operation)
                        {
                           case details::e_add : cobnode->set_c(c + cobnode->c()); break;
                           case details::e_mul : cobnode->set_c(c * cobnode->c()); break;
                           default             : return error_node();
                        }
                        return cobnode;
                     }
                  }

                  if (operation == details::e_mul)
                  {
                     details::cob_base_node<Type>* cobnode = dynamic_cast<details::cob_base_node<Type>*>(branch[1]);
                     details::operator_type cob_opr = cobnode->operation();

                     if (
                          (details::e_div == cob_opr) ||
                          (details::e_mul == cob_opr)
                        )
                     {
                        switch (cob_opr)
                        {
                           case details::e_div : cobnode->set_c(c * cobnode->c()); break;
                           case details::e_mul : cobnode->set_c(cobnode->c() / c); break;
                           default             : return error_node();
                        }
                        return cobnode;
                     }
                  }
                  else if (operation == details::e_div)
                  {
                     details::cob_base_node<Type>* cobnode = dynamic_cast<details::cob_base_node<Type>*>(branch[1]);
                     details::operator_type cob_opr = cobnode->operation();

                     if (
                          (details::e_div == cob_opr) ||
                          (details::e_mul == cob_opr)
                        )
                     {
                        details::expression_node<Type>* new_cobnode = error_node();
                        switch (cob_opr)
                        {
                           case details::e_div : new_cobnode = expr_gen.node_allocator_->
                                                                  template allocate_tt<typename details::cob_node<Type,details::mul_op<Type> > >
                                                                     (c / cobnode->c(),cobnode->move_branch(0));
                                                 break;

                           case details::e_mul : new_cobnode = expr_gen.node_allocator_->
                                                                  template allocate_tt<typename details::cob_node<Type,details::div_op<Type> > >
                                                                     (c / cobnode->c(),cobnode->move_branch(0));
                                                 break;

                           default             : return error_node();
                        }

                        free_node(*expr_gen.node_allocator_,branch[1]);
                        return new_cobnode;
                     }
                  }
               }
               else if (details::is_sf3ext_node(branch[1]))
               {
                  expression_node_ptr result = error_node();
                  if (synthesize_sf4ext_expression::template compile_right<ctype>(expr_gen,c,operation,branch[1],result))
                  {
                     free_node(*expr_gen.node_allocator_,branch[1]);
                     return result;
                  }
               }

               switch (operation)
               {
                  #define case_stmt(op0,op1)                                                       \
                  case op0 : return expr_gen.node_allocator_->                                     \
                                template allocate_tt<typename details::cob_node<Type,op1<Type> > > \
                                   (c,branch[1]);                                                  \

                  basic_opr_switch_statements
                  extended_opr_switch_statements
                  #undef case_stmt
                  default : return error_node();
               }
            }
         };

         struct synthesize_boc_expression
         {
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               const Type c = dynamic_cast<details::literal_node<Type>*>(branch[1])->value();
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               if (details::is_boc_node(branch[0]))
               {
                  // Simplify expressions of the form:
                  // 1. (((((((((x + 9) * 8) * 7) * 6) * 5) * 4) * 3) * 2) * 1)
                  // 2. (((((((((x + 9) + 8) + 7) + 6) + 5) + 4) + 3) + 2) + 1)
                  if (
                      (operation == details::e_mul) ||
                      (operation == details::e_add)
                     )
                  {
                     details::boc_base_node<Type>* bocnode = dynamic_cast<details::boc_base_node<Type>*>(branch[0]);
                     if (operation == bocnode->operation())
                     {
                        switch (operation)
                        {
                           case details::e_add : bocnode->set_c(c + bocnode->c()); break;
                           case details::e_mul : bocnode->set_c(c * bocnode->c()); break;
                           default             : return error_node();
                        }
                        return bocnode;
                     }
                  }
                  else if (operation == details::e_div)
                  {
                     details::boc_base_node<Type>* bocnode = dynamic_cast<details::boc_base_node<Type>*>(branch[0]);
                     details::operator_type boc_opr = bocnode->operation();

                     if (
                          (details::e_div == boc_opr) ||
                          (details::e_mul == boc_opr)
                        )
                     {
                        switch (boc_opr)
                        {
                           case details::e_div : bocnode->set_c(c * bocnode->c()); break;
                           case details::e_mul : bocnode->set_c(bocnode->c() / c); break;
                           default             : return error_node();
                        }
                        return bocnode;
                     }
                  }
               }

               if (details::is_sf3ext_node(branch[0]))
               {
                  expression_node_ptr result = error_node();
                  if (synthesize_sf4ext_expression::template compile_left<ctype>(expr_gen,c,operation,branch[0],result))
                  {
                     free_node(*expr_gen.node_allocator_,branch[0]);
                     return result;
                  }
               }

               switch (operation)
               {
                  #define case_stmt(op0,op1)                                                    \
                  case op0 : return expr_gen.node_allocator_->                                  \
                             template allocate_cr<typename details::boc_node<Type,op1<Type> > > \
                                (branch[0],c);                                                  \

                  basic_opr_switch_statements
                  extended_opr_switch_statements
                  #undef case_stmt
                  default : return error_node();
               }
            }
         };

         inline bool synthesize_expression(const details::operator_type& operation, expression_node_ptr (&branch)[2], expression_node_ptr& result)
         {
            result = error_node();
            if (!operation_optimizable(operation))
               return false;
            const std::string node_id = branch_to_id(branch);
            typename synthesize_map_t::iterator itr = synthesize_map_.find(node_id);
            if (synthesize_map_.end() != itr)
            {
               result = itr->second(*this,operation,branch);
               return true;
            }
            else
               return false;
         }

         struct synthesize_vov_expression
         {
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               const Type& v1 = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
               const Type& v2 = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
               switch (operation)
               {
                  #define case_stmt(op0,op1)                                                       \
                  case op0 : return expr_gen.node_allocator_->                                     \
                                template allocate_rr<typename details::vov_node<Type,op1<Type> > > \
                                   (v1,v2);                                                        \

                  basic_opr_switch_statements
                  extended_opr_switch_statements
                  #undef case_stmt
                  default : return error_node();
               }
            }
         };

         struct synthesize_cov_expression
         {
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               const Type  c = dynamic_cast<details::literal_node<Type>*> (branch[0])->value();
               const Type& v = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               switch (operation)
               {
                  #define case_stmt(op0,op1)                                                       \
                  case op0 : return expr_gen.node_allocator_->                                     \
                                template allocate_cr<typename details::cov_node<Type,op1<Type> > > \
                                   (c,v);                                                          \

                  basic_opr_switch_statements
                  extended_opr_switch_statements
                  #undef case_stmt
                  default : return error_node();
               }
            }
         };

         struct synthesize_voc_expression
         {
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               const Type& v = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
               const Type  c = dynamic_cast<details::literal_node<Type>*> (branch[1])->value();
               details::free_node(*(expr_gen.node_allocator_),branch[1]);

               if (expr_gen.cardinal_pow_optimizable(operation,c))
               {
                  if (T(1) == c)
                     return branch[0];
                  else
                     return expr_gen.cardinal_pow_optimization(v,c);
               }

               switch (operation)
               {
                  #define case_stmt(op0,op1)                                                       \
                  case op0 : return expr_gen.node_allocator_->                                     \
                                template allocate_rc<typename details::voc_node<Type,op1<Type> > > \
                                   (v,c);                                                          \

                  basic_opr_switch_statements
                  extended_opr_switch_statements
                  #undef case_stmt
                  default : return error_node();
               }
            }
         };

         struct synthesize_sf3ext_expression
         {
            template <typename T0, typename T1, typename T2>
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& sf3opr,
                                                      T0 t0, T1 t1, T2 t2)
            {
               switch (sf3opr)
               {
                  #define case_stmt(op0,op1)                                           \
                  case op0 : return details::T0oT1oT2_sf3ext<T,T0,T1,T2,op1<Type> >::  \
                                       allocate(*(expr_gen.node_allocator_),t0,t1,t2); \

                  case_stmt(details::e_sf00,details::sf00_op) case_stmt(details::e_sf01,details::sf01_op)
                  case_stmt(details::e_sf02,details::sf02_op) case_stmt(details::e_sf03,details::sf03_op)
                  case_stmt(details::e_sf04,details::sf04_op) case_stmt(details::e_sf05,details::sf05_op)
                  case_stmt(details::e_sf06,details::sf06_op) case_stmt(details::e_sf07,details::sf07_op)
                  case_stmt(details::e_sf08,details::sf08_op) case_stmt(details::e_sf09,details::sf09_op)
                  case_stmt(details::e_sf10,details::sf10_op) case_stmt(details::e_sf11,details::sf11_op)
                  case_stmt(details::e_sf12,details::sf12_op) case_stmt(details::e_sf13,details::sf13_op)
                  case_stmt(details::e_sf14,details::sf14_op) case_stmt(details::e_sf15,details::sf15_op)
                  case_stmt(details::e_sf16,details::sf16_op) case_stmt(details::e_sf17,details::sf17_op)
                  case_stmt(details::e_sf18,details::sf18_op) case_stmt(details::e_sf19,details::sf19_op)
                  case_stmt(details::e_sf20,details::sf20_op) case_stmt(details::e_sf21,details::sf21_op)
                  case_stmt(details::e_sf22,details::sf22_op) case_stmt(details::e_sf23,details::sf23_op)
                  case_stmt(details::e_sf24,details::sf24_op) case_stmt(details::e_sf25,details::sf25_op)
                  case_stmt(details::e_sf26,details::sf26_op) case_stmt(details::e_sf27,details::sf27_op)
                  case_stmt(details::e_sf28,details::sf28_op) case_stmt(details::e_sf29,details::sf29_op)
                  case_stmt(details::e_sf30,details::sf30_op)
                  #undef case_stmt
                  default : return error_node();
               }
            }

            template <typename T0, typename T1, typename T2>
            static inline bool compile(expression_generator<Type>& expr_gen, const std::string& id,
                                       T0 t0, T1 t1, T2 t2,
                                       expression_node_ptr& result)
            {
               details::operator_type sf3opr;
               if (!expr_gen.sf3_optimizable(id,sf3opr))
                  return false;
               else
                  result = synthesize_sf3ext_expression::template process<T0,T1,T2>(expr_gen,sf3opr,t0,t1,t2);
               return true;
            }

         };

         struct synthesize_sf4ext_expression
         {
            template <typename T0, typename T1, typename T2, typename T3>
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& sf4opr,
                                                      T0 t0, T1 t1, T2 t2, T3 t3)
            {
               switch (sf4opr)
               {
                  #define case_stmt(op0,op1)                                                   \
                  case op0 : return details::T0oT1oT2oT3_sf4ext<Type,T0,T1,T2,T3,op1<Type> >:: \
                                       allocate(*(expr_gen.node_allocator_),t0,t1,t2,t3);      \

                  case_stmt(details::e_sf48,details::sf48_op) case_stmt(details::e_sf49,details::sf49_op)
                  case_stmt(details::e_sf50,details::sf50_op) case_stmt(details::e_sf51,details::sf51_op)
                  case_stmt(details::e_sf52,details::sf52_op) case_stmt(details::e_sf53,details::sf53_op)
                  case_stmt(details::e_sf54,details::sf54_op) case_stmt(details::e_sf55,details::sf55_op)
                  case_stmt(details::e_sf56,details::sf56_op) case_stmt(details::e_sf57,details::sf57_op)
                  case_stmt(details::e_sf58,details::sf58_op) case_stmt(details::e_sf59,details::sf59_op)
                  case_stmt(details::e_sf60,details::sf60_op) case_stmt(details::e_sf61,details::sf61_op)
                  case_stmt(details::e_sf62,details::sf62_op) case_stmt(details::e_sf63,details::sf63_op)
                  case_stmt(details::e_sf64,details::sf64_op) case_stmt(details::e_sf65,details::sf65_op)
                  case_stmt(details::e_sf66,details::sf66_op) case_stmt(details::e_sf67,details::sf67_op)
                  case_stmt(details::e_sf68,details::sf68_op) case_stmt(details::e_sf69,details::sf69_op)
                  case_stmt(details::e_sf70,details::sf70_op) case_stmt(details::e_sf71,details::sf71_op)
                  case_stmt(details::e_sf72,details::sf72_op) case_stmt(details::e_sf73,details::sf73_op)
                  case_stmt(details::e_sf74,details::sf74_op) case_stmt(details::e_sf75,details::sf75_op)
                  case_stmt(details::e_sf76,details::sf76_op) case_stmt(details::e_sf77,details::sf77_op)
                  case_stmt(details::e_sf78,details::sf78_op) case_stmt(details::e_sf79,details::sf79_op)
                  case_stmt(details::e_sf80,details::sf80_op) case_stmt(details::e_sf81,details::sf81_op)
                  case_stmt(details::e_sf82,details::sf82_op) case_stmt(details::e_sf83,details::sf83_op)
                  case_stmt(details::e_sf4ext00,details::sfext00_op) case_stmt(details::e_sf4ext01,details::sfext01_op)
                  case_stmt(details::e_sf4ext02,details::sfext02_op) case_stmt(details::e_sf4ext03,details::sfext03_op)
                  case_stmt(details::e_sf4ext04,details::sfext04_op) case_stmt(details::e_sf4ext05,details::sfext05_op)
                  case_stmt(details::e_sf4ext06,details::sfext06_op) case_stmt(details::e_sf4ext07,details::sfext07_op)
                  case_stmt(details::e_sf4ext08,details::sfext08_op) case_stmt(details::e_sf4ext09,details::sfext09_op)
                  case_stmt(details::e_sf4ext10,details::sfext10_op) case_stmt(details::e_sf4ext11,details::sfext11_op)
                  case_stmt(details::e_sf4ext12,details::sfext12_op) case_stmt(details::e_sf4ext13,details::sfext13_op)
                  case_stmt(details::e_sf4ext14,details::sfext14_op) case_stmt(details::e_sf4ext15,details::sfext15_op)
                  case_stmt(details::e_sf4ext16,details::sfext16_op) case_stmt(details::e_sf4ext17,details::sfext17_op)
                  case_stmt(details::e_sf4ext18,details::sfext18_op) case_stmt(details::e_sf4ext19,details::sfext19_op)
                  case_stmt(details::e_sf4ext20,details::sfext20_op) case_stmt(details::e_sf4ext21,details::sfext21_op)
                  case_stmt(details::e_sf4ext22,details::sfext22_op) case_stmt(details::e_sf4ext23,details::sfext23_op)
                  case_stmt(details::e_sf4ext24,details::sfext24_op) case_stmt(details::e_sf4ext25,details::sfext25_op)
                  case_stmt(details::e_sf4ext26,details::sfext26_op) case_stmt(details::e_sf4ext27,details::sfext27_op)
                  case_stmt(details::e_sf4ext28,details::sfext28_op) case_stmt(details::e_sf4ext29,details::sfext29_op)
                  case_stmt(details::e_sf4ext30,details::sfext30_op) case_stmt(details::e_sf4ext31,details::sfext31_op)
                  case_stmt(details::e_sf4ext32,details::sfext32_op) case_stmt(details::e_sf4ext33,details::sfext33_op)
                  case_stmt(details::e_sf4ext34,details::sfext34_op) case_stmt(details::e_sf4ext35,details::sfext35_op)
                  case_stmt(details::e_sf4ext36,details::sfext36_op) case_stmt(details::e_sf4ext37,details::sfext37_op)
                  case_stmt(details::e_sf4ext38,details::sfext38_op) case_stmt(details::e_sf4ext39,details::sfext39_op)
                  case_stmt(details::e_sf4ext40,details::sfext40_op) case_stmt(details::e_sf4ext41,details::sfext41_op)
                  case_stmt(details::e_sf4ext42,details::sfext42_op) case_stmt(details::e_sf4ext43,details::sfext43_op)
                  case_stmt(details::e_sf4ext44,details::sfext44_op) case_stmt(details::e_sf4ext45,details::sfext45_op)
                  #undef case_stmt
                  default : return error_node();
               }
            }

            template <typename T0, typename T1, typename T2, typename T3>
            static inline bool compile(expression_generator<Type>& expr_gen, const std::string& id,
                                       T0 t0, T1 t1, T2 t2, T3 t3,
                                       expression_node_ptr& result)
            {
               details::operator_type sf4opr;
               if (!expr_gen.sf4_optimizable(id,sf4opr))
                  return false;
               else
                  result = synthesize_sf4ext_expression::template process<T0,T1,T2,T3>(expr_gen,sf4opr,t0,t1,t2,t3);
               return true;
            }

            // T o (sf3ext)
            template <typename ExternalType>
            static inline bool compile_right(expression_generator<Type>& expr_gen,
                                             ExternalType t,
                                             const details::operator_type& operation,
                                             expression_node_ptr& sf3node,
                                             expression_node_ptr& result)
            {
               if (!details::is_sf3ext_node(sf3node))
                  return false;

               typedef details::T0oT1oT2_base_node<Type>* sf3ext_base_ptr;
               sf3ext_base_ptr n = dynamic_cast<sf3ext_base_ptr>(sf3node);
               std::string id = "t" + expr_gen.to_str(operation) + "(" + n->type_id() + ")";

               switch (n->type())
               {
                  case details::expression_node<Type>::e_covoc : return compile_right_impl
                                                                        <typename covoc_t::sf3_type_node,ExternalType,ctype,vtype,ctype>(expr_gen,id,t,sf3node,result);

                  case details::expression_node<Type>::e_covov : return compile_right_impl
                                                                        <typename covov_t::sf3_type_node,ExternalType,ctype,vtype,vtype>(expr_gen,id,t,sf3node,result);

                  case details::expression_node<Type>::e_vocov : return compile_right_impl
                                                                        <typename vocov_t::sf3_type_node,ExternalType,vtype,ctype,vtype>(expr_gen,id,t,sf3node,result);

                  case details::expression_node<Type>::e_vovoc : return compile_right_impl
                                                                        <typename vovoc_t::sf3_type_node,ExternalType,vtype,vtype,ctype>(expr_gen,id,t,sf3node,result);

                  case details::expression_node<Type>::e_vovov : return compile_right_impl
                                                                        <typename vovov_t::sf3_type_node,ExternalType,vtype,vtype,vtype>(expr_gen,id,t,sf3node,result);

                  default                                      : return false;
               }
            }

            // (sf3ext) o T
            template <typename ExternalType>
            static inline bool compile_left(expression_generator<Type>& expr_gen,
                                            ExternalType t,
                                            const details::operator_type& operation,
                                            expression_node_ptr& sf3node,
                                            expression_node_ptr& result)
            {
               if (!details::is_sf3ext_node(sf3node))
                  return false;

               typedef details::T0oT1oT2_base_node<Type>* sf3ext_base_ptr;
               sf3ext_base_ptr n = dynamic_cast<sf3ext_base_ptr>(sf3node);
               std::string id = "(" + n->type_id() + ")" + expr_gen.to_str(operation) + "t";
               switch (n->type())
               {
                  case details::expression_node<Type>::e_covoc : return compile_left_impl
                                                                        <typename covoc_t::sf3_type_node,ExternalType,ctype,vtype,ctype>(expr_gen,id,t,sf3node,result);

                  case details::expression_node<Type>::e_covov : return compile_left_impl
                                                                        <typename covov_t::sf3_type_node,ExternalType,ctype,vtype,vtype>(expr_gen,id,t,sf3node,result);

                  case details::expression_node<Type>::e_vocov : return compile_left_impl
                                                                        <typename vocov_t::sf3_type_node,ExternalType,vtype,ctype,vtype>(expr_gen,id,t,sf3node,result);

                  case details::expression_node<Type>::e_vovoc : return compile_left_impl
                                                                        <typename vovoc_t::sf3_type_node,ExternalType,vtype,vtype,ctype>(expr_gen,id,t,sf3node,result);

                  case details::expression_node<Type>::e_vovov : return compile_left_impl
                                                                        <typename vovov_t::sf3_type_node,ExternalType,vtype,vtype,vtype>(expr_gen,id,t,sf3node,result);

                  default                                      : return false;
               }
            }

            template <typename SF3TypeNode, typename ExternalType, typename T0, typename T1, typename T2>
            static inline bool compile_right_impl(expression_generator<Type>& expr_gen,
                                                  const std::string& id,
                                                  ExternalType t,
                                                  expression_node_ptr& node,
                                                  expression_node_ptr& result)
            {
               SF3TypeNode* n = dynamic_cast<SF3TypeNode*>(node);
               if (n)
               {
                  T0 t0 = n->t0();
                  T1 t1 = n->t1();
                  T2 t2 = n->t2();
                  return synthesize_sf4ext_expression::
                           template compile<ExternalType,T0,T1,T2>(expr_gen,id,t,t0,t1,t2,result);
               }
               else
                  return false;
            }

            template <typename SF3TypeNode, typename ExternalType, typename T0, typename T1, typename T2>
            static inline bool compile_left_impl(expression_generator<Type>& expr_gen,
                                                 const std::string& id,
                                                 ExternalType t,
                                                 expression_node_ptr& node,
                                                 expression_node_ptr& result)
            {
               SF3TypeNode* n = dynamic_cast<SF3TypeNode*>(node);
               if (n)
               {
                  T0 t0 = n->t0();
                  T1 t1 = n->t1();
                  T2 t2 = n->t2();
                  return synthesize_sf4ext_expression::
                           template compile<T0,T1,T2,ExternalType>(expr_gen,id,t0,t1,t2,t,result);
               }
               else
                  return false;
            }
         };

         struct synthesize_vovov_expression0
         {
            typedef typename vovov_t::type0 node_type;
            typedef typename vovov_t::sf3_type sf3_type;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (v0 o0 v1) o1 (v2)
               const details::vov_base_node<Type>* vov = dynamic_cast<details::vov_base_node<Type>*>(branch[0]);
               const Type& v0 = vov->v0();
               const Type& v1 = vov->v1();
               const Type& v2 = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
               const details::operator_type o0 = vov->operation();
               const details::operator_type o1 = operation;
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               expression_node_ptr result = error_node();

               if (expr_gen.strength_reduction_enabled())
               {
                  // (v0 / v1) / v2 --> (vovov) v0 / (v1 * v2)
                  if ((details::e_div == o0) && (details::e_div == o1))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<vtype,vtype,vtype>(expr_gen,"t/(t*t)",v0,v1,v2,result);
                     return (synthesis_result) ? result : error_node();
                  }
               }

               if (synthesize_sf3ext_expression::template compile<vtype,vtype,vtype>(expr_gen,id(expr_gen,o0,o1),v0,v1,v2,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,v2,f0,f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t");
            }
         };

         struct synthesize_vovov_expression1
         {
            typedef typename vovov_t::type1 node_type;
            typedef typename vovov_t::sf3_type sf3_type;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (v0) o0 (v1 o1 v2)
               const details::vov_base_node<Type>* vov = dynamic_cast<details::vov_base_node<Type>*>(branch[1]);
               const Type& v0 = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
               const Type& v1 = vov->v0();
               const Type& v2 = vov->v1();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = vov->operation();
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();

               if (expr_gen.strength_reduction_enabled())
               {
                  // v0 / (v1 / v2) --> (vovov) (v0 * v2) / v1
                  if ((details::e_div == o0) && (details::e_div == o1))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<vtype,vtype,vtype>(expr_gen,"(t*t)/t",v0,v2,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
               }

               if (synthesize_sf3ext_expression::template compile<vtype,vtype,vtype>(expr_gen,id(expr_gen,o0,o1),v0,v1,v2,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,v2,f0,f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)");
            }
         };

         struct synthesize_vovoc_expression0
         {
            typedef typename vovoc_t::type0 node_type;
            typedef typename vovoc_t::sf3_type sf3_type;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (v0 o0 v1) o1 (c)
               const details::vov_base_node<Type>* vov = dynamic_cast<details::vov_base_node<Type>*>(branch[0]);
               const Type& v0 = vov->v0();
               const Type& v1 = vov->v1();
               const Type   c = dynamic_cast<details::literal_node<Type>*>(branch[1])->value();
               const details::operator_type o0 = vov->operation();
               const details::operator_type o1 = operation;
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();

               if (expr_gen.strength_reduction_enabled())
               {
                  // (v0 / v1) / c --> (vovoc) v0 / (v1 * c)
                  if ((details::e_div == o0) && (details::e_div == o1))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<vtype,vtype,ctype>(expr_gen,"t/(t*t)",v0,v1,c,result);
                     return (synthesis_result) ? result : error_node();
                  }
               }

               if (synthesize_sf3ext_expression::template compile<vtype,vtype,ctype>(expr_gen,id(expr_gen,o0,o1),v0,v1,c,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,c,f0,f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t");
            }
         };

         struct synthesize_vovoc_expression1
         {
            typedef typename vovoc_t::type1 node_type;
            typedef typename vovoc_t::sf3_type sf3_type;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (v0) o0 (v1 o1 c)
               const details::voc_base_node<Type>* voc = dynamic_cast<const details::voc_base_node<Type>*>(branch[1]);
               const Type& v0 = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
               const Type& v1 = voc->v();
               const Type   c = voc->c();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = voc->operation();
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();

               if (expr_gen.strength_reduction_enabled())
               {
                  // v0 / (v1 / c) --> (vocov) (v0 * c) / v1
                  if ((details::e_div == o0) && (details::e_div == o1))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<vtype,ctype,vtype>(expr_gen,"(t*t)/t",v0,c,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
               }

               if (synthesize_sf3ext_expression::template compile<vtype,vtype,ctype>(expr_gen,id(expr_gen,o0,o1),v0,v1,c,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,c,f0,f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)");
            }
         };

         struct synthesize_vocov_expression0
         {
            typedef typename vocov_t::type0 node_type;
            typedef typename vocov_t::sf3_type sf3_type;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (v0 o0 c) o1 (v1)
               const details::voc_base_node<Type>* voc = dynamic_cast<details::voc_base_node<Type>*>(branch[0]);
               const Type& v0 = voc->v();
               const Type   c = voc->c();
               const Type& v1 = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
               const details::operator_type o0 = voc->operation();
               const details::operator_type o1 = operation;
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               expression_node_ptr result = error_node();

               if (expr_gen.strength_reduction_enabled())
               {
                  // (v0 / c) / v1 --> (vovoc) v0 / (v1 * c)
                  if ((details::e_div == o0) && (details::e_div == o1))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<vtype,vtype,ctype>(expr_gen,"t/(t*t)",v0,v1,c,result);
                     return (synthesis_result) ? result : error_node();
                  }
               }

               if (synthesize_sf3ext_expression::template compile<vtype,ctype,vtype>(expr_gen,id(expr_gen,o0,o1),v0,c,v1,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,c,v1,f0,f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t");
            }
         };

         struct synthesize_vocov_expression1
         {
            typedef typename vocov_t::type1 node_type;
            typedef typename vocov_t::sf3_type sf3_type;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (v0) o0 (c o1 v1)
               const details::cov_base_node<Type>* cov = dynamic_cast<details::cov_base_node<Type>*>(branch[1]);
               const Type& v0 = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
               const Type   c = cov->c();
               const Type& v1 = cov->v();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = cov->operation();
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();

               if (expr_gen.strength_reduction_enabled())
               {
                  // v0 / (c / v1) --> (vovoc) (v0 * v1) / c
                  if ((details::e_div == o0) && (details::e_div == o1))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<vtype,vtype,ctype>(expr_gen,"(t*t)/t",v0,v1,c,result);
                     return (synthesis_result) ? result : error_node();
                  }
               }

               if (synthesize_sf3ext_expression::template compile<vtype,ctype,vtype>(expr_gen,id(expr_gen,o0,o1),v0,c,v1,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,c,v1,f0,f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)");
            }
         };

         struct synthesize_covov_expression0
         {
            typedef typename covov_t::type0 node_type;
            typedef typename covov_t::sf3_type sf3_type;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (c o0 v0) o1 (v1)
               const details::cov_base_node<Type>* cov = dynamic_cast<details::cov_base_node<Type>*>(branch[0]);
               const Type   c = cov->c();
               const Type& v0 = cov->v();
               const Type& v1 = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
               const details::operator_type o0 = cov->operation();
               const details::operator_type o1 = operation;
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               expression_node_ptr result = error_node();

               if (expr_gen.strength_reduction_enabled())
               {
                  // (c / v0) / v1 --> (covov) c / (v0 * v1)
                  if ((details::e_div == o0) && (details::e_div == o1))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"t/(t*t)",c,v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
               }

               if (synthesize_sf3ext_expression::template compile<ctype,vtype,vtype>(expr_gen,id(expr_gen,o0,o1),c,v0,v1,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c,v0,v1,f0,f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t");
            }
         };

         struct synthesize_covov_expression1
         {
            typedef typename covov_t::type1 node_type;
            typedef typename covov_t::sf3_type sf3_type;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (c) o0 (v0 o1 v1)
               const details::vov_base_node<Type>* vov = dynamic_cast<details::vov_base_node<Type>*>(branch[1]);
               const Type   c = dynamic_cast<details::literal_node<Type>*>(branch[0])->value();
               const Type& v0 = vov->v0();
               const Type& v1 = vov->v1();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = vov->operation();
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();

               if (expr_gen.strength_reduction_enabled())
               {
                  // c / (v0 / v1) --> (covov) (c * v1) / v0
                  if ((details::e_div == o0) && (details::e_div == o1))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t*t)/t",c,v1,v0,result);
                     return (synthesis_result) ? result : error_node();
                  }
               }

               if (synthesize_sf3ext_expression::template compile<ctype,vtype,vtype>(expr_gen,id(expr_gen,o0,o1),c,v0,v1,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c,v0,v1,f0,f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen, const details::operator_type o0, const details::operator_type o1)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)");
            }
         };

         struct synthesize_covoc_expression0
         {
            typedef typename covoc_t::type0 node_type;
            typedef typename covoc_t::sf3_type sf3_type;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (c0 o0 v) o1 (c1)
               const details::cov_base_node<Type>* cov = dynamic_cast<details::cov_base_node<Type>*>(branch[0]);
               const Type  c0 = cov->c();
               const Type&  v = cov->v();
               const Type  c1 = dynamic_cast<details::literal_node<Type>*>(branch[1])->value();
               const details::operator_type o0 = cov->operation();
               const details::operator_type o1 = operation;
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();

               if (expr_gen.strength_reduction_enabled())
               {
                  // (c0 + v) + c1 --> (cov) (c0 + c1) + v
                  if ((details::e_add == o0) && (details::e_add == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::add_op<Type> > >(c0 + c1,v);
                  }
                  // (c0 + v) - c1 --> (cov) (c0 - c1) + v
                  else if ((details::e_add == o0) && (details::e_sub == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::add_op<Type> > >(c0 - c1,v);
                  }
                  // (c0 - v) + c1 --> (cov) (c0 + c1) - v
                  else if ((details::e_sub == o0) && (details::e_add == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::sub_op<Type> > >(c0 + c1,v);
                  }
                  // (c0 - v) - c1 --> (cov) (c0 - c1) - v
                  else if ((details::e_sub == o0) && (details::e_sub == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::sub_op<Type> > >(c0 - c1,v);
                  }
                  // (c0 * v) * c1 --> (cov) (c0 * c1) * v
                  else if ((details::e_mul == o0) && (details::e_mul == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::mul_op<Type> > >(c0 * c1,v);
                  }
                  // (c0 * v) / c1 --> (cov) (c0 / c1) * v
                  else if ((details::e_mul == o0) && (details::e_div == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::mul_op<Type> > >(c0 / c1,v);
                  }
                  // (c0 / v) * c1 --> (cov) (c0 * c1) / v
                  else if ((details::e_div == o0) && (details::e_mul == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::div_op<Type> > >(c0 * c1,v);
                  }
                  // (c0 / v) / c1 --> (cov) (c0 / c1) / v
                  else if ((details::e_div == o0) && (details::e_div == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::div_op<Type> > >(c0 / c1,v);
                  }
               }

               if (synthesize_sf3ext_expression::template compile<ctype,vtype,ctype>(expr_gen,id(expr_gen,o0,o1),c0,v,c1,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c0,v,c1,f0,f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t");
            }
         };

         struct synthesize_covoc_expression1
         {
            typedef typename covoc_t::type1 node_type;
            typedef typename covoc_t::sf3_type sf3_type;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (c0) o0 (v o1 c1)
               const details::voc_base_node<Type>* voc = dynamic_cast<details::voc_base_node<Type>*>(branch[1]);
               const Type  c0 = dynamic_cast<details::literal_node<Type>*>(branch[0])->value();
               const Type&  v = voc->v();
               const Type  c1 = voc->c();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = voc->operation();
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();

               if (expr_gen.strength_reduction_enabled())
               {
                  // (c0) + (v + c1) --> (cov) (c0 + c1) + v
                  if ((details::e_add == o0) && (details::e_add == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::add_op<Type> > >(c0 + c1,v);
                  }
                  // (c0) + (v - c1) --> (cov) (c0 - c1) + v
                  else if ((details::e_add == o0) && (details::e_sub == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::add_op<Type> > >(c0 - c1,v);
                  }
                  // (c0) - (v + c1) --> (cov) (c0 - c1) - v
                  else if ((details::e_sub == o0) && (details::e_add == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::sub_op<Type> > >(c0 - c1,v);
                  }
                  // (c0) - (v - c1) --> (cov) (c0 + c1) - v
                  else if ((details::e_sub == o0) && (details::e_sub == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::sub_op<Type> > >(c0 + c1,v);
                  }
                  // (c0) * (v * c1) --> (voc) v * (c0 * c1)
                  else if ((details::e_mul == o0) && (details::e_mul == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::mul_op<Type> > >(c0 * c1,v);
                  }
                  // (c0) * (v / c1) --> (cov) (c0 / c1) * v
                  else if ((details::e_mul == o0) && (details::e_div == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::mul_op<Type> > >(c0 / c1,v);
                  }
                  // (c0) / (v * c1) --> (cov) (c0 / c1) / v
                  else if ((details::e_div == o0) && (details::e_mul == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::div_op<Type> > >(c0 / c1,v);
                  }
                  // (c0) / (v / c1) --> (cov) (c0 * c1) / v
                  else if ((details::e_div == o0) && (details::e_div == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::div_op<Type> > >(c0 * c1,v);
                  }
               }

               if (synthesize_sf3ext_expression::template compile<ctype,vtype,ctype>(expr_gen,id(expr_gen,o0,o1),c0,v,c1,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c0,v,c1,f0,f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)");
            }
         };

         struct synthesize_cocov_expression0
         {
            typedef typename cocov_t::type0 node_type;
            static inline expression_node_ptr process(expression_generator<Type>&, const details::operator_type&, expression_node_ptr (&)[2])
            {
               // (c0 o0 c1) o1 (v) - Not possible.
               return error_node();
            }
         };

         struct synthesize_cocov_expression1
         {
            typedef typename cocov_t::type1 node_type;
            typedef typename cocov_t::sf3_type sf3_type;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (c0) o0 (c1 o1 v)
               const details::cov_base_node<Type>* cov = dynamic_cast<details::cov_base_node<Type>*>(branch[1]);
               const Type  c0 = dynamic_cast<details::literal_node<Type>*>(branch[0])->value();
               const Type  c1 = cov->c();
               const Type&  v = cov->v();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = cov->operation();
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();

               if (expr_gen.strength_reduction_enabled())
               {
                  // (c0) + (c1 + v) --> (cov) (c0 + c1) + v
                  if ((details::e_add == o0) && (details::e_add == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::add_op<Type> > >(c0 + c1,v);
                  }
                  // (c0) + (c1 - v) --> (cov) (c0 + c1) - v
                  else if ((details::e_add == o0) && (details::e_sub == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::sub_op<Type> > >(c0 + c1,v);
                  }
                  // (c0) - (c1 + v) --> (cov) (c0 - c1) - v
                  else if ((details::e_sub == o0) && (details::e_add == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::sub_op<Type> > >(c0 - c1,v);
                  }
                  // (c0) - (c1 - v) --> (cov) (c0 - c1) + v
                  else if ((details::e_sub == o0) && (details::e_sub == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::add_op<Type> > >(c0 - c1,v);
                  }
                  // (c0) * (c1 * v) --> (cov) (c0 * c1) * v
                  else if ((details::e_mul == o0) && (details::e_mul == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::mul_op<Type> > >(c0 * c1,v);
                  }
                  // (c0) * (c1 / v) --> (cov) (c0 * c1) / v
                  else if ((details::e_mul == o0) && (details::e_div == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::div_op<Type> > >(c0 * c1,v);
                  }
                  // (c0) / (c1 * v) --> (cov) (c0 / c1) / v
                  else if ((details::e_div == o0) && (details::e_mul == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::div_op<Type> > >(c0 / c1,v);
                  }
                  // (c0) / (c1 / v) --> (cov) (c0 / c1) * v
                  else if ((details::e_div == o0) && (details::e_div == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_cr<typename details::cov_node<Type,details::mul_op<Type> > >(c0 / c1,v);
                  }
               }

               if (synthesize_sf3ext_expression::template compile<ctype,ctype,vtype>(expr_gen,id(expr_gen,o0,o1),c0,c1,v,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c0,c1,v,f0,f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen, const details::operator_type o0, const details::operator_type o1)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)");
            }
         };

         struct synthesize_vococ_expression0
         {
            typedef typename vococ_t::type0 node_type;
            typedef typename vococ_t::sf3_type sf3_type;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (v o0 c0) o1 (c1)
               const details::voc_base_node<Type>* voc = dynamic_cast<details::voc_base_node<Type>*>(branch[0]);
               const Type&  v = voc->v();
               const Type& c0 = voc->c();
               const Type& c1 = dynamic_cast<details::literal_node<Type>*>(branch[1])->value();
               const details::operator_type o0 = voc->operation();
               const details::operator_type o1 = operation;
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();

               if (expr_gen.strength_reduction_enabled())
               {
                  // (v + c0) + c1 --> (voc) v + (c0 + c1)
                  if ((details::e_add == o0) && (details::e_add == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_rc<typename details::voc_node<Type,details::add_op<Type> > >(v,c0 + c1);
                  }
                  // (v + c0) - c1 --> (voc) v + (c0 - c1)
                  else if ((details::e_add == o0) && (details::e_sub == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_rc<typename details::voc_node<Type,details::add_op<Type> > >(v,c0 - c1);
                  }
                  // (v - c0) + c1 --> (voc) v - (c0 + c1)
                  else if ((details::e_sub == o0) && (details::e_add == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_rc<typename details::voc_node<Type,details::add_op<Type> > >(v,c1 - c0);
                  }
                  // (v - c0) - c1 --> (voc) v - (c0 + c1)
                  else if ((details::e_sub == o0) && (details::e_sub == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_rc<typename details::voc_node<Type,details::sub_op<Type> > >(v,c0 + c1);
                  }
                  // (v * c0) * c1 --> (voc) v * (c0 * c1)
                  else if ((details::e_mul == o0) && (details::e_mul == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_rc<typename details::voc_node<Type,details::mul_op<Type> > >(v,c0 * c1);
                  }
                  // (v * c0) / c1 --> (voc) v * (c0 / c1)
                  else if ((details::e_mul == o0) && (details::e_div == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_rc<typename details::voc_node<Type,details::mul_op<Type> > >(v,c0 / c1);
                  }
                  // (v / c0) * c1 --> (voc) v * (c1 / c0)
                  else if ((details::e_div == o0) && (details::e_mul == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_rc<typename details::voc_node<Type,details::mul_op<Type> > >(v,c1 / c0);
                  }
                  // (v / c0) / c1 --> (voc) v / (c0 * c1)
                  else if ((details::e_div == o0) && (details::e_div == o1))
                  {
                     return expr_gen.node_allocator_->
                               template allocate_rc<typename details::voc_node<Type,details::div_op<Type> > >(v,c0 * c1);
                  }
               }

               if (synthesize_sf3ext_expression::template compile<vtype,ctype,ctype>(expr_gen,id(expr_gen,o0,o1),v,c0,c1,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v,c0,c1,f0,f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t");
            }
         };

         struct synthesize_vococ_expression1
         {
            typedef typename vococ_t::type0 node_type;
            static inline expression_node_ptr process(expression_generator<Type>&, const details::operator_type&, expression_node_ptr (&)[2])
            {
               // (v) o0 (c0 o1 c1) - Not possible.
               return error_node();
            }
         };

         struct synthesize_vovovov_expression0
         {
            typedef typename vovovov_t::type0 node_type;
            typedef typename vovovov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;

            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (v0 o0 v1) o1 (v2 o2 v3)
               const details::vov_base_node<Type>* vov0 = dynamic_cast<details::vov_base_node<Type>*>(branch[0]);
               const details::vov_base_node<Type>* vov1 = dynamic_cast<details::vov_base_node<Type>*>(branch[1]);
               const Type& v0 = vov0->v0();
               const Type& v1 = vov0->v1();
               const Type& v2 = vov1->v0();
               const Type& v3 = vov1->v1();
               const details::operator_type o0 = vov0->operation();
               const details::operator_type o1 = operation;
               const details::operator_type o2 = vov1->operation();
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();

               if (expr_gen.strength_reduction_enabled())
               {
                  // (v0 / v1) * (v2 / v3) --> (vovovov) (v0 * v2) / (v1 * v3)
                  if ((details::e_div == o0) && (details::e_mul == o1) && (details::e_div == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf4ext_expression::
                           template compile<vtype,vtype,vtype,vtype>(expr_gen,"(t*t)/(t*t)",v0,v2,v1,v3,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (v0 / v1) / (v2 / v3) --> (vovovov) (v0 * v3) / (v1 * v2)
                  if ((details::e_div == o0) && (details::e_div == o1) && (details::e_div == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf4ext_expression::
                           template compile<vtype,vtype,vtype,vtype>(expr_gen,"(t*t)/(t*t)",v0,v3,v1,v2,result);
                     return (synthesis_result) ? result : error_node();
                  }
               }

               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,v1,v2,v3,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,v2,v3,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "t)"<< expr_gen.to_str(o1) << "(t" << expr_gen.to_str(o2) << "t)");
            }
         };
         struct synthesize_vovovoc_expression0
         {
            typedef typename vovovoc_t::type0 node_type;
            typedef typename vovovoc_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (v0 o0 v1) o1 (v2 o2 c)
               const details::vov_base_node<Type>* vov = dynamic_cast<details::vov_base_node<Type>*>(branch[0]);
               const details::voc_base_node<Type>* voc = dynamic_cast<details::voc_base_node<Type>*>(branch[1]);
               const Type& v0 = vov->v0();
               const Type& v1 = vov->v1();
               const Type& v2 = voc->v ();
               const Type   c = voc->c ();
               const details::operator_type o0 = vov->operation();
               const details::operator_type o1 = operation;
               const details::operator_type o2 = voc->operation();
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,v1,v2,c,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,v2,c,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "t)"<< expr_gen.to_str(o1) << "(t" << expr_gen.to_str(o2) << "t)");
            }
         };

         struct synthesize_vovocov_expression0
         {
            typedef typename vovocov_t::type0 node_type;
            typedef typename vovocov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (v0 o0 v1) o1 (c o2 v2)
               const details::vov_base_node<Type>* vov = dynamic_cast<details::vov_base_node<Type>*>(branch[0]);
               const details::cov_base_node<Type>* cov = dynamic_cast<details::cov_base_node<Type>*>(branch[1]);
               const Type& v0 = vov->v0();
               const Type& v1 = vov->v1();
               const Type& v2 = cov->v ();
               const Type   c = cov->c ();
               const details::operator_type o0 = vov->operation();
               const details::operator_type o1 = operation;
               const details::operator_type o2 = cov->operation();
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,v1,c,v2,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,c,v2,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "t)"<< expr_gen.to_str(o1) << "(t" << expr_gen.to_str(o2) << "t)");
            }
         };

         struct synthesize_vocovov_expression0
         {
            typedef typename vocovov_t::type0 node_type;
            typedef typename vocovov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (v0 o0 c) o1 (v1 o2 v2)
               const details::voc_base_node<Type>* voc = dynamic_cast<details::voc_base_node<Type>*>(branch[0]);
               const details::vov_base_node<Type>* vov = dynamic_cast<details::vov_base_node<Type>*>(branch[1]);
               const Type   c = voc->c ();
               const Type& v0 = voc->v ();
               const Type& v1 = vov->v0();
               const Type& v2 = vov->v1();
               const details::operator_type o0 = voc->operation();
               const details::operator_type o1 = operation;
               const details::operator_type o2 = vov->operation();
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,c,v1,v2,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,c,v1,v2,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "t)"<< expr_gen.to_str(o1) << "(t" << expr_gen.to_str(o2) << "t)");
            }
         };

         struct synthesize_covovov_expression0
         {
            typedef typename covovov_t::type0 node_type;
            typedef typename covovov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (c o0 v0) o1 (v1 o2 v2)
               const details::cov_base_node<Type>* cov = dynamic_cast<details::cov_base_node<Type>*>(branch[0]);
               const details::vov_base_node<Type>* vov = dynamic_cast<details::vov_base_node<Type>*>(branch[1]);
               const Type   c = cov->c ();
               const Type& v0 = cov->v ();
               const Type& v1 = vov->v0();
               const Type& v2 = vov->v1();
               const details::operator_type o0 = cov->operation();
               const details::operator_type o1 = operation;
               const details::operator_type o2 = vov->operation();
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),c,v0,v1,v2,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c,v0,v1,v2,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "t)"<< expr_gen.to_str(o1) << "(t" << expr_gen.to_str(o2) << "t)");
            }
         };

         struct synthesize_covocov_expression0
         {
            typedef typename covocov_t::type0 node_type;
            typedef typename covocov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (c0 o0 v0) o1 (c1 o2 v1)
               const details::cov_base_node<Type>* cov0 = dynamic_cast<details::cov_base_node<Type>*>(branch[0]);
               const details::cov_base_node<Type>* cov1 = dynamic_cast<details::cov_base_node<Type>*>(branch[1]);
               const Type  c0 = cov0->c();
               const Type& v0 = cov0->v();
               const Type  c1 = cov1->c();
               const Type& v1 = cov1->v();
               const details::operator_type o0 = cov0->operation();
               const details::operator_type o1 = operation;
               const details::operator_type o2 = cov1->operation();
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();

               if (expr_gen.strength_reduction_enabled())
               {
                  // (c0 + v0) + (c1 + v1) --> (covov) (c0 + c1) + v0 + v1
                  if ((details::e_add == o0) && (details::e_add == o1) && (details::e_add == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t+t)+t",(c0 + c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (c0 + v0) - (c1 + v1) --> (covov) (c0 - c1) + v0 - v1
                  else if ((details::e_add == o0) && (details::e_sub == o1) && (details::e_add == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t+t)-t",(c0 - c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (c0 - v0) - (c1 - v1) --> (covov) (c0 - c1) - v0 + v1
                  else if ((details::e_sub == o0) && (details::e_sub == o1) && (details::e_sub == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t-t)+t",(c0 - c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (c0 * v0) * (c1 * v1) --> (covov) (c0 * c1) * v0 * v1
                  else if ((details::e_mul == o0) && (details::e_mul == o1) && (details::e_mul == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t*t)*t",(c0 * c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (c0 * v0) / (c1 * v1) --> (covov) (c0 / c1) * (v0 / v1)
                  else if ((details::e_mul == o0) && (details::e_div == o1) && (details::e_mul == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t*t)/t",(c0 / c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (c0 / v0) * (c1 / v1) --> (covov) (c0 * c1) / (v0 * v1)
                  else if ((details::e_div == o0) && (details::e_mul == o1) && (details::e_div == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"t/(t*t)",(c0 * c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (c0 * v0) / (c1 / v1) --> (covov) (c0 / c1) * (v0 * v1)
                  else if ((details::e_mul == o0) && (details::e_div == o1) && (details::e_div == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"t*(t*t)",(c0 / c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (c0 / v0) / (c1 * v1) --> (covov) (c0 / c1) / (v0 * v1)
                  else if ((details::e_div == o0) && (details::e_div == o1) && (details::e_mul == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"t/(t*t)",(c0 / c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (c * v0) +/- (c * v1) --> (covov) c * (v0 +/- v1)
                  else if (
                            (details::e_mul == o0) && (details::e_mul == o2) && (c0 == c1) &&
                            ((details::e_add == o1) || (details::e_sub == o1))
                          )
                  {
                     std::string specfunc;
                     switch (o1)
                     {
                        case details::e_add : specfunc = "t*(t+t)"; break;
                        case details::e_sub : specfunc = "t*(t-t)"; break;
                        default             : return error_node();
                     }

                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,specfunc,c0,v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
               }

               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),c0,v0,c1,v1,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c0,v0,c1,v1,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "t)"<< expr_gen.to_str(o1) << "(t" << expr_gen.to_str(o2) << "t)");
            }
         };

         struct synthesize_vocovoc_expression0
         {
            typedef typename vocovoc_t::type0 node_type;
            typedef typename vocovoc_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (v0 o0 c0) o1 (v1 o2 c1)
               const details::voc_base_node<Type>* voc0 = dynamic_cast<details::voc_base_node<Type>*>(branch[0]);
               const details::voc_base_node<Type>* voc1 = dynamic_cast<details::voc_base_node<Type>*>(branch[1]);
               const Type  c0 = voc0->c();
               const Type& v0 = voc0->v();
               const Type  c1 = voc1->c();
               const Type& v1 = voc1->v();
               const details::operator_type o0 = voc0->operation();
               const details::operator_type o1 = operation;
               const details::operator_type o2 = voc1->operation();
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();

               if (expr_gen.strength_reduction_enabled())
               {
                  // (v0 + c0) + (v1 + c1) --> (covov) (c0 + c1) + v0 + v1
                  if ((details::e_add == o0) && (details::e_add == o1) && (details::e_add == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t+t)+t",(c0 + c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (v0 + c0) - (v1 + c1) --> (covov) (c0 - c1) + v0 - v1
                  else if ((details::e_add == o0) && (details::e_sub == o1) && (details::e_add == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t+t)-t",(c0 - c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (v0 - c0) - (v1 - c1) --> (covov) (c1 - c0) + v0 - v1
                  else if ((details::e_sub == o0) && (details::e_sub == o1) && (details::e_sub == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t+t)-t",(c1 - c0),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (v0 * c0) * (v1 * c1) --> (covov) (c0 * c1) * v0 * v1
                  else if ((details::e_mul == o0) && (details::e_mul == o1) && (details::e_mul == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t*t)*t",(c0 * c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (v0 * c0) / (v1 * c1) --> (covov) (c0 / c1) * (v0 / v1)
                  else if ((details::e_mul == o0) && (details::e_div == o1) && (details::e_mul == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t*t)/t",(c0 / c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (v0 / c0) * (v1 / c1) --> (covov) (1 / (c0 * c1)) * v0 * v1
                  else if ((details::e_div == o0) && (details::e_mul == o1) && (details::e_div == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t*t)*t",Type(1) / (c0 * c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (v0 * c0) / (v1 / c1) --> (covov) (c0 * c1) * (v0 / v1)
                  else if ((details::e_mul == o0) && (details::e_div == o1) && (details::e_mul == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"t*(t/t)",(c0 * c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (v0 / c0) / (v1 * c1) --> (covov) (1 / (c0 * c1)) * v0 / v1
                  else if ((details::e_div == o0) && (details::e_div == o1) && (details::e_mul == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"t*(t/t)",Type(1) / (c0 * c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (v0 * c) +/- (v1 * c) --> (covov) c * (v0 +/- v1)
                  else if (
                            (details::e_mul == o0) && (details::e_mul == o2) && (c0 == c1) &&
                            ((details::e_add == o1) || (details::e_sub == o1))
                          )
                  {
                     std::string specfunc;
                     switch (o1)
                     {
                        case details::e_add : specfunc = "t*(t+t)"; break;
                        case details::e_sub : specfunc = "t*(t-t)"; break;
                        default             : return error_node();
                     }

                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,specfunc,c0,v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
               }

               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,c0,v1,c1,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,c0,v1,c1,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "t)"<< expr_gen.to_str(o1) << "(t" << expr_gen.to_str(o2) << "t)");
            }
         };

         struct synthesize_covovoc_expression0
         {
            typedef typename covovoc_t::type0 node_type;
            typedef typename covovoc_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (c0 o0 v0) o1 (v1 o2 c1)
               const details::cov_base_node<Type>* cov = dynamic_cast<details::cov_base_node<Type>*>(branch[0]);
               const details::voc_base_node<Type>* voc = dynamic_cast<details::voc_base_node<Type>*>(branch[1]);
               const Type  c0 = cov->c();
               const Type& v0 = cov->v();
               const Type  c1 = voc->c();
               const Type& v1 = voc->v();
               const details::operator_type o0 = cov->operation();
               const details::operator_type o1 = operation;
               const details::operator_type o2 = voc->operation();
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();

               if (expr_gen.strength_reduction_enabled())
               {
                  // (c0 + v0) + (v1 + c1) --> (covov) (c0 + c1) + v0 + v1
                  if ((details::e_add == o0) && (details::e_add == o1) && (details::e_add == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t+t)+t",(c0 + c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (c0 + v0) - (v1 + c1) --> (covov) (c0 - c1) + v0 - v1
                  else if ((details::e_add == o0) && (details::e_sub == o1) && (details::e_add == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t+t)-t",(c0 - c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (c0 - v0) - (v1 - c1) --> (covov) (c0 + c1) - v0 - v1
                  else if ((details::e_sub == o0) && (details::e_sub == o1) && (details::e_sub == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"t-(t+t)",(c0 + c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (c0 * v0) * (v1 * c1) --> (covov) (c0 * c1) * v0 * v1
                  else if ((details::e_mul == o0) && (details::e_mul == o1) && (details::e_mul == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t*t)*t",(c0 * c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (c0 * v0) / (v1 * c1) --> (covov) (c0 / c1) * (v0 / v1)
                  else if ((details::e_mul == o0) && (details::e_div == o1) && (details::e_mul == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t*t)/t",(c0 / c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (c0 / v0) * (v1 / c1) --> (covov) (c0 / c1) * (v1 / v0)
                  else if ((details::e_div == o0) && (details::e_mul == o1) && (details::e_div == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"t*(t/t)",(c0 / c1),v1,v0,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (c0 * v0) / (v1 / c1) --> (covov) (c0 * c1) * (v0 / v1)
                  else if ((details::e_mul == o0) && (details::e_div == o1) && (details::e_div == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t*t)/t",(c0 * c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (c0 / v0) / (v1 * c1) --> (covov) (c0 / c1) / (v0 * v1)
                  else if ((details::e_div == o0) && (details::e_div == o1) && (details::e_mul == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"t/(t*t)",(c0 / c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (c * v0) +/- (v1 * c) --> (covov) c * (v0 +/- v1)
                  else if (
                            (details::e_mul == o0) && (details::e_mul == o2) && (c0 == c1) &&
                            ((details::e_add == o1) || (details::e_sub == o1))
                          )
                  {
                     std::string specfunc;
                     switch (o1)
                     {
                        case details::e_add : specfunc = "t*(t+t)"; break;
                        case details::e_sub : specfunc = "t*(t-t)"; break;
                        default             : return error_node();
                     }

                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,specfunc,c0,v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
               }

               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),c0,v0,v1,c1,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c0,v0,v1,c1,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "t)"<< expr_gen.to_str(o1) << "(t" << expr_gen.to_str(o2) << "t)");
            }
         };

         struct synthesize_vococov_expression0
         {
            typedef typename vococov_t::type0 node_type;
            typedef typename vococov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (v0 o0 c0) o1 (c1 o2 v1)
               const details::voc_base_node<Type>* voc = dynamic_cast<details::voc_base_node<Type>*>(branch[0]);
               const details::cov_base_node<Type>* cov = dynamic_cast<details::cov_base_node<Type>*>(branch[1]);
               const T  c0 = voc->c();
               const T& v0 = voc->v();
               const T  c1 = cov->c();
               const T& v1 = cov->v();
               const details::operator_type o0 = voc->operation();
               const details::operator_type o1 = operation;
               const details::operator_type o2 = cov->operation();
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();

               if (expr_gen.strength_reduction_enabled())
               {
                  // (v0 + c0) + (c1 + v1) --> (covov) (c0 + c1) + v0 + v1
                  if ((details::e_add == o0) && (details::e_add == o1) && (details::e_add == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t+t)+t",(c0 + c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (v0 + c0) - (c1 + v1) --> (covov) (c0 - c1) + v0 - v1
                  else if ((details::e_add == o0) && (details::e_sub == o1) && (details::e_add == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t+t)-t",(c0 - c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (v0 - c0) - (c1 - v1) --> (vovoc) v0 + v1 - (c1 + c0)
                  else if ((details::e_sub == o0) && (details::e_sub == o1) && (details::e_sub == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<vtype,vtype,ctype>(expr_gen,"(t+t)-t",v0,v1,(c1 + c0),result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (v0 * c0) * (c1 * v1) --> (covov) (c0 * c1) * v0 * v1
                  else if ((details::e_mul == o0) && (details::e_mul == o1) && (details::e_mul == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t*t)*t",(c0 * c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (v0 * c0) / (c1 * v1) --> (covov) (c0 / c1) * (v0 * v1)
                  else if ((details::e_mul == o0) && (details::e_div == o1) && (details::e_mul == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t*t)/t",(c0 / c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (v0 / c0) * (c1 / v1) --> (covov) (c1 / c0) * (v0 / v1)
                  else if ((details::e_div == o0) && (details::e_mul == o1) && (details::e_div == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t*t)/t",(c1 / c0),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (v0 * c0) / (c1 / v1) --> (covov) (c0 / c1) * (v0 * v1)
                  else if ((details::e_mul == o0) && (details::e_mul == o1) && (details::e_mul == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t*t)*t",(c0 / c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (v0 / c0) / (c1 * v1) --> (covov) (1 / (c0 * c1)) * (v0 / v1)
                  else if ((details::e_div == o0) && (details::e_div == o1) && (details::e_mul == o2))
                  {
                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,"(t*t)/t",Type(1) / (c0 * c1),v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
                  // (v0 * c) +/- (c * v1) --> (covov) c * (v0 +/- v1)
                  else if (
                            (details::e_mul == o0) && (details::e_mul == o2) && (c0 == c1) &&
                            ((details::e_add == o1) || (details::e_sub == o1))
                          )
                  {
                     std::string specfunc;
                     switch (o1)
                     {
                        case details::e_add : specfunc = "t*(t+t)"; break;
                        case details::e_sub : specfunc = "t*(t-t)"; break;
                        default             : return error_node();
                     }

                     const bool synthesis_result =
                        synthesize_sf3ext_expression::
                           template compile<ctype,vtype,vtype>(expr_gen,specfunc,c0,v0,v1,result);
                     return (synthesis_result) ? result : error_node();
                  }
               }

               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,c0,c1,v1,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else if (!expr_gen.valid_operator(o1,f1))
                  return error_node();
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,c0,c1,v1,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "t)"<< expr_gen.to_str(o1) << "(t" << expr_gen.to_str(o2) << "t)");
            }
         };

         struct synthesize_vovovov_expression1
         {
            typedef typename vovovov_t::type1 node_type;
            typedef typename vovovov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // v0 o0 (v1 o1 (v2 o2 v3))
               typedef typename synthesize_vovov_expression1::node_type vovov_t;
               const vovov_t* vovov = dynamic_cast<const vovov_t*>(branch[1]);
               const T& v0 = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
               const T& v1 = vovov->t0();
               const T& v2 = vovov->t1();
               const T& v3 = vovov->t2();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = expr_gen.get_operator(vovov->f0());
               const details::operator_type o2 = expr_gen.get_operator(vovov->f1());
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = vovov->f0();
               binary_functor_t f2 = vovov->f1();
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,v1,v2,v3,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,v2,v3,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "(t"<< expr_gen.to_str(o1) << "(t" << expr_gen.to_str(o2) << "t))");
            }
         };

         struct synthesize_vovovoc_expression1
         {
            typedef typename vovovoc_t::type1 node_type;
            typedef typename vovovoc_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // v0 o0 (v1 o1 (v2 o2 c))
               typedef typename synthesize_vovoc_expression1::node_type vovoc_t;
               const vovoc_t* vovoc = dynamic_cast<const vovoc_t*>(branch[1]);
               const T& v0 = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
               const T& v1 = vovoc->t0();
               const T& v2 = vovoc->t1();
               const T   c = vovoc->t2();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = expr_gen.get_operator(vovoc->f0());
               const details::operator_type o2 = expr_gen.get_operator(vovoc->f1());
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = vovoc->f0();
               binary_functor_t f2 = vovoc->f1();
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,v1,v2,c,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,v2,c,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "(t"<< expr_gen.to_str(o1) << "(t" << expr_gen.to_str(o2) << "t))");
            }
         };

         struct synthesize_vovocov_expression1
         {
            typedef typename vovocov_t::type1 node_type;
            typedef typename vovocov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // v0 o0 (v1 o1 (c o2 v2))
               typedef typename synthesize_vocov_expression1::node_type vocov_t;
               const vocov_t* vocov = dynamic_cast<const vocov_t*>(branch[1]);
               const T& v0 = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
               const T& v1 = vocov->t0();
               const T   c = vocov->t1();
               const T& v2 = vocov->t2();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = expr_gen.get_operator(vocov->f0());
               const details::operator_type o2 = expr_gen.get_operator(vocov->f1());
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = vocov->f0();
               binary_functor_t f2 = vocov->f1();
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,v1,c,v2,result))
                  return result;
               if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,c,v2,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "(t"<< expr_gen.to_str(o1) << "(t" << expr_gen.to_str(o2) << "t))");
            }
         };

         struct synthesize_vocovov_expression1
         {
            typedef typename vocovov_t::type1 node_type;
            typedef typename vocovov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // v0 o0 (c o1 (v1 o2 v2))
               typedef typename synthesize_covov_expression1::node_type covov_t;
               const covov_t* covov = dynamic_cast<const covov_t*>(branch[1]);
               const T& v0 = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
               const T   c = covov->t0();
               const T& v1 = covov->t1();
               const T& v2 = covov->t2();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = expr_gen.get_operator(covov->f0());
               const details::operator_type o2 = expr_gen.get_operator(covov->f1());
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = covov->f0();
               binary_functor_t f2 = covov->f1();
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,c,v1,v2,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,c,v1,v2,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "(t"<< expr_gen.to_str(o1) << "(t" << expr_gen.to_str(o2) << "t))");
            }
         };

         struct synthesize_covovov_expression1
         {
            typedef typename covovov_t::type1 node_type;
            typedef typename covovov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // c o0 (v0 o1 (v1 o2 v2))
               typedef typename synthesize_vovov_expression1::node_type vovov_t;
               const vovov_t* vovov = dynamic_cast<const vovov_t*>(branch[1]);
               const T   c = dynamic_cast<details::literal_node<Type>*>(branch[0])->value();
               const T& v0 = vovov->t0();
               const T& v1 = vovov->t1();
               const T& v2 = vovov->t2();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = expr_gen.get_operator(vovov->f0());
               const details::operator_type o2 = expr_gen.get_operator(vovov->f1());
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = vovov->f0();
               binary_functor_t f2 = vovov->f1();
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),c,v0,v1,v2,result))
                  return result;
               if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c,v0,v1,v2,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "(t"<< expr_gen.to_str(o1) << "(t" << expr_gen.to_str(o2) << "t))");
            }
         };

         struct synthesize_covocov_expression1
         {
            typedef typename covocov_t::type1 node_type;
            typedef typename covocov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // c0 o0 (v0 o1 (c1 o2 v1))
               typedef typename synthesize_vocov_expression1::node_type vocov_t;
               const vocov_t* vocov = dynamic_cast<const vocov_t*>(branch[1]);
               const T  c0 = dynamic_cast<details::literal_node<Type>*>(branch[0])->value();
               const T& v0 = vocov->t0();
               const T  c1 = vocov->t1();
               const T& v1 = vocov->t2();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = expr_gen.get_operator(vocov->f0());
               const details::operator_type o2 = expr_gen.get_operator(vocov->f1());
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = vocov->f0();
               binary_functor_t f2 = vocov->f1();
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),c0,v0,c1,v1,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c0,v0,c1,v1,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "(t"<< expr_gen.to_str(o1) << "(t" << expr_gen.to_str(o2) << "t))");
            }
         };

         struct synthesize_vocovoc_expression1
         {
            typedef typename vocovoc_t::type1 node_type;
            typedef typename vocovoc_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // v0 o0 (c0 o1 (v1 o2 c2))
               typedef typename synthesize_covoc_expression1::node_type covoc_t;
               const covoc_t* covoc = dynamic_cast<const covoc_t*>(branch[1]);
               const T& v0 = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
               const T  c0 = covoc->t0();
               const T& v1 = covoc->t1();
               const T  c1 = covoc->t2();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = expr_gen.get_operator(covoc->f0());
               const details::operator_type o2 = expr_gen.get_operator(covoc->f1());
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = covoc->f0();
               binary_functor_t f2 = covoc->f1();
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,c0,v1,c1,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,c0,v1,c1,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "(t"<< expr_gen.to_str(o1) << "(t" << expr_gen.to_str(o2) << "t))");
            }
         };

         struct synthesize_covovoc_expression1
         {
            typedef typename covovoc_t::type1 node_type;
            typedef typename covovoc_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // c0 o0 (v0 o1 (v1 o2 c1))
               typedef typename synthesize_vovoc_expression1::node_type vovoc_t;
               const vovoc_t* vovoc = dynamic_cast<const vovoc_t*>(branch[1]);
               const T  c0 = dynamic_cast<details::literal_node<Type>*>(branch[0])->value();
               const T& v0 = vovoc->t0();
               const T& v1 = vovoc->t1();
               const T  c1 = vovoc->t2();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = expr_gen.get_operator(vovoc->f0());
               const details::operator_type o2 = expr_gen.get_operator(vovoc->f1());
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = vovoc->f0();
               binary_functor_t f2 = vovoc->f1();
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),c0,v0,v1,c1,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c0,v0,v1,c1,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "(t"<< expr_gen.to_str(o1) << "(t" << expr_gen.to_str(o2) << "t))");
            }
         };

         struct synthesize_vococov_expression1
         {
            typedef typename vococov_t::type1 node_type;
            typedef typename vococov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // v0 o0 (c0 o1 (c1 o2 v1))
               typedef typename synthesize_cocov_expression1::node_type cocov_t;
               const cocov_t* cocov = dynamic_cast<const cocov_t*>(branch[1]);
               const T& v0 = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
               const T  c0 = cocov->t0();
               const T  c1 = cocov->t1();
               const T& v1 = cocov->t2();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = expr_gen.get_operator(cocov->f0());
               const details::operator_type o2 = expr_gen.get_operator(cocov->f1());
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = cocov->f0();
               binary_functor_t f2 = cocov->f1();
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,c0,c1,v1,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,c0,c1,v1,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "(t"<< expr_gen.to_str(o1) << "(t" << expr_gen.to_str(o2) << "t))");
            }
         };

         struct synthesize_vovovov_expression2
         {
            typedef typename vovovov_t::type2 node_type;
            typedef typename vovovov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // v0 o0 ((v1 o1 v2) o2 v3)
               typedef typename synthesize_vovov_expression0::node_type vovov_t;
               const vovov_t* vovov = dynamic_cast<const vovov_t*>(branch[1]);
               const T& v0 = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
               const T& v1 = vovov->t0();
               const T& v2 = vovov->t1();
               const T& v3 = vovov->t2();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = expr_gen.get_operator(vovov->f0());
               const details::operator_type o2 = expr_gen.get_operator(vovov->f1());
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = vovov->f0();
               binary_functor_t f2 = vovov->f1();
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,v1,v2,v3,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,v2,v3,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "((t"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t)");
            }
         };

         struct synthesize_vovovoc_expression2
         {
            typedef typename vovovoc_t::type2 node_type;
            typedef typename vovovoc_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // v0 o0 ((v1 o1 v2) o2 c)
               typedef typename synthesize_vovoc_expression0::node_type vovoc_t;
               const vovoc_t* vovoc = dynamic_cast<const vovoc_t*>(branch[1]);
               const T& v0 = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
               const T& v1 = vovoc->t0();
               const T& v2 = vovoc->t1();
               const T   c = vovoc->t2();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = expr_gen.get_operator(vovoc->f0());
               const details::operator_type o2 = expr_gen.get_operator(vovoc->f1());
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = vovoc->f0();
               binary_functor_t f2 = vovoc->f1();
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,v1,v2,c,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,v2,c,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "((t"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t)");
            }
         };

         struct synthesize_vovocov_expression2
         {
            typedef typename vovocov_t::type2 node_type;
            typedef typename vovocov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // v0 o0 ((v1 o1 c) o2 v2)
               typedef typename synthesize_vocov_expression0::node_type vocov_t;
               const vocov_t* vocov = dynamic_cast<const vocov_t*>(branch[1]);
               const T& v0 = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
               const T& v1 = vocov->t0();
               const T   c = vocov->t1();
               const T& v2 = vocov->t2();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = expr_gen.get_operator(vocov->f0());
               const details::operator_type o2 = expr_gen.get_operator(vocov->f1());
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = vocov->f0();
               binary_functor_t f2 = vocov->f1();
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,v1,c,v2,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,c,v2,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "((t"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t)");
            }
         };

         struct synthesize_vocovov_expression2
         {
            typedef typename vocovov_t::type2 node_type;
            typedef typename vocovov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // v0 o0 ((c o1 v1) o2 v2)
               typedef typename synthesize_covov_expression0::node_type covov_t;
               const covov_t* covov = dynamic_cast<const covov_t*>(branch[1]);
               const T& v0 = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
               const T   c = covov->t0();
               const T& v1 = covov->t1();
               const T& v2 = covov->t2();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = expr_gen.get_operator(covov->f0());
               const details::operator_type o2 = expr_gen.get_operator(covov->f1());
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = covov->f0();
               binary_functor_t f2 = covov->f1();
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,c,v1,v2,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,c,v1,v2,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "((t"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t)");
            }
         };

         struct synthesize_covovov_expression2
         {
            typedef typename covovov_t::type2 node_type;
            typedef typename covovov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // c o0 ((v1 o1 v2) o2 v3)
               typedef typename synthesize_vovov_expression0::node_type vovov_t;
               const vovov_t* vovov = dynamic_cast<const vovov_t*>(branch[1]);
               const T   c = dynamic_cast<details::literal_node<Type>*>(branch[0])->value();
               const T& v0 = vovov->t0();
               const T& v1 = vovov->t1();
               const T& v2 = vovov->t2();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = expr_gen.get_operator(vovov->f0());
               const details::operator_type o2 = expr_gen.get_operator(vovov->f1());
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = vovov->f0();
               binary_functor_t f2 = vovov->f1();
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),c,v0,v1,v2,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c,v0,v1,v2,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "((t"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t)");
            }
        };

         struct synthesize_covocov_expression2
         {
            typedef typename covocov_t::type2 node_type;
            typedef typename covocov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // c0 o0 ((v0 o1 c1) o2 v1)
               typedef typename synthesize_vocov_expression0::node_type vocov_t;
               const vocov_t* vocov = dynamic_cast<const vocov_t*>(branch[1]);
               const T  c0 = dynamic_cast<details::literal_node<Type>*>(branch[0])->value();
               const T& v0 = vocov->t0();
               const T  c1 = vocov->t1();
               const T& v1 = vocov->t2();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = expr_gen.get_operator(vocov->f0());
               const details::operator_type o2 = expr_gen.get_operator(vocov->f1());
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = vocov->f0();
               binary_functor_t f2 = vocov->f1();
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),c0,v0,c1,v1,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c0,v0,c1,v1,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "((t"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t)");
            }
         };

         struct synthesize_vocovoc_expression2
         {
            typedef typename vocovoc_t::type2 node_type;
            typedef typename vocovoc_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // v0 o0 ((c0 o1 v1) o2 c1)
               typedef typename synthesize_covoc_expression0::node_type covoc_t;
               const covoc_t* covoc = dynamic_cast<const covoc_t*>(branch[1]);
               const T& v0 = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
               const T  c0 = covoc->t0();
               const T& v1 = covoc->t1();
               const T  c1 = covoc->t2();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = expr_gen.get_operator(covoc->f0());
               const details::operator_type o2 = expr_gen.get_operator(covoc->f1());
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = covoc->f0();
               binary_functor_t f2 = covoc->f1();
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,c0,v1,c1,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,c0,v1,c1,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "((t"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t)");
            }
         };

         struct synthesize_covovoc_expression2
         {
            typedef typename covovoc_t::type2 node_type;
            typedef typename covovoc_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // c0 o0 ((v0 o1 v1) o2 c1)
               typedef typename synthesize_vovoc_expression0::node_type vovoc_t;
               const vovoc_t* vovoc = dynamic_cast<const vovoc_t*>(branch[1]);
               const T  c0 = dynamic_cast<details::literal_node<Type>*>(branch[0])->value();
               const T& v0 = vovoc->t0();
               const T& v1 = vovoc->t1();
               const T  c1 = vovoc->t2();
               const details::operator_type o0 = operation;
               const details::operator_type o1 = expr_gen.get_operator(vovoc->f0());
               const details::operator_type o2 = expr_gen.get_operator(vovoc->f1());
               binary_functor_t f0 = reinterpret_cast<binary_functor_t>(0);
               binary_functor_t f1 = vovoc->f0();
               binary_functor_t f2 = vovoc->f1();
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),c0,v0,v1,c1,result))
                  return result;
               else if (!expr_gen.valid_operator(o0,f0))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c0,v0,v1,c1,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "t" << expr_gen.to_str(o0) << "((t"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t)");
            }
         };

         struct synthesize_vococov_expression2
         {
            typedef typename vococov_t::type2 node_type;
            static inline expression_node_ptr process(expression_generator<Type>&, const details::operator_type&, expression_node_ptr (&)[2])
            {
               // v0 o0 ((c0 o1 c1) o2 v1) - Not possible
               return error_node();
            }

            static inline std::string id(expression_generator<Type>&,
                                         const details::operator_type, const details::operator_type, const details::operator_type)
            {
               return "INVALID";
            }
         };

         struct synthesize_vovovov_expression3
         {
            typedef typename vovovov_t::type3 node_type;
            typedef typename vovovov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // ((v0 o0 v1) o1 v2) o2 v3
               typedef typename synthesize_vovov_expression0::node_type vovov_t;
               const vovov_t* vovov = dynamic_cast<const vovov_t*>(branch[0]);
               const T& v0 = vovov->t0();
               const T& v1 = vovov->t1();
               const T& v2 = vovov->t2();
               const T& v3 = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
               const details::operator_type o0 = expr_gen.get_operator(vovov->f0());
               const details::operator_type o1 = expr_gen.get_operator(vovov->f1());
               const details::operator_type o2 = operation;
               binary_functor_t f0 = vovov->f0();
               binary_functor_t f1 = vovov->f1();
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,v1,v2,v3,result))
                  return result;
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,v2,v3,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "((t" << expr_gen.to_str(o0) << "t)"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t");
            }
         };

         struct synthesize_vovovoc_expression3
         {
            typedef typename vovovoc_t::type3 node_type;
            typedef typename vovovoc_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // ((v0 o0 v1) o1 v2) o2 c
               typedef typename synthesize_vovov_expression0::node_type vovov_t;
               const vovov_t* vovov = dynamic_cast<const vovov_t*>(branch[0]);
               const T& v0 = vovov->t0();
               const T& v1 = vovov->t1();
               const T& v2 = vovov->t2();
               const T   c = dynamic_cast<details::literal_node<Type>*>(branch[1])->value();
               const details::operator_type o0 = expr_gen.get_operator(vovov->f0());
               const details::operator_type o1 = expr_gen.get_operator(vovov->f1());
               const details::operator_type o2 = operation;
               binary_functor_t f0 = vovov->f0();
               binary_functor_t f1 = vovov->f1();
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,v1,v2,c,result))
                  return result;
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,v2,c,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "((t" << expr_gen.to_str(o0) << "t)"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t");
            }
         };

         struct synthesize_vovocov_expression3
         {
            typedef typename vovocov_t::type3 node_type;
            typedef typename vovocov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // ((v0 o0 v1) o1 c) o2 v2
               typedef typename synthesize_vovoc_expression0::node_type vovoc_t;
               const vovoc_t* vovoc = dynamic_cast<const vovoc_t*>(branch[0]);
               const T& v0 = vovoc->t0();
               const T& v1 = vovoc->t1();
               const T   c = vovoc->t2();
               const T& v2 = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
               const details::operator_type o0 = expr_gen.get_operator(vovoc->f0());
               const details::operator_type o1 = expr_gen.get_operator(vovoc->f1());
               const details::operator_type o2 = operation;
               binary_functor_t f0 = vovoc->f0();
               binary_functor_t f1 = vovoc->f1();
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,v1,c,v2,result))
                  return result;
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,c,v2,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "((t" << expr_gen.to_str(o0) << "t)"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t");
            }
         };

         struct synthesize_vocovov_expression3
         {
            typedef typename vocovov_t::type3 node_type;
            typedef typename vocovov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // ((v0 o0 c) o1 v1) o2 v2
               typedef typename synthesize_vocov_expression0::node_type vocov_t;
               const vocov_t* vocov = dynamic_cast<const vocov_t*>(branch[0]);
               const T& v0 = vocov->t0();
               const T   c = vocov->t1();
               const T& v1 = vocov->t2();
               const T& v2 = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
               const details::operator_type o0 = expr_gen.get_operator(vocov->f0());
               const details::operator_type o1 = expr_gen.get_operator(vocov->f1());
               const details::operator_type o2 = operation;
               binary_functor_t f0 = vocov->f0();
               binary_functor_t f1 = vocov->f1();
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,c,v1,v2,result))
                  return result;
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,c,v1,v2,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "((t" << expr_gen.to_str(o0) << "t)"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t");
            }
         };

         struct synthesize_covovov_expression3
         {
            typedef typename covovov_t::type3 node_type;
            typedef typename covovov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // ((c o0 v0) o1 v1) o2 v2
               typedef typename synthesize_covov_expression0::node_type covov_t;
               const covov_t* covov = dynamic_cast<const covov_t*>(branch[0]);
               const T   c = covov->t0();
               const T& v0 = covov->t1();
               const T& v1 = covov->t2();
               const T& v2 = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
               const details::operator_type o0 = expr_gen.get_operator(covov->f0());
               const details::operator_type o1 = expr_gen.get_operator(covov->f1());
               const details::operator_type o2 = operation;
               binary_functor_t f0 = covov->f0();
               binary_functor_t f1 = covov->f1();
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),c,v0,v1,v2,result))
                  return result;
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c,v0,v1,v2,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "((t" << expr_gen.to_str(o0) << "t)"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t");
            }
         };

         struct synthesize_covocov_expression3
         {
            typedef typename covocov_t::type3 node_type;
            typedef typename covocov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // ((c0 o0 v0) o1 c1) o2 v1
               typedef typename synthesize_covoc_expression0::node_type covoc_t;
               const covoc_t* covoc = dynamic_cast<const covoc_t*>(branch[0]);
               const T  c0 = covoc->t0();
               const T& v0 = covoc->t1();
               const T  c1 = covoc->t2();
               const T& v1 = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
               const details::operator_type o0 = expr_gen.get_operator(covoc->f0());
               const details::operator_type o1 = expr_gen.get_operator(covoc->f1());
               const details::operator_type o2 = operation;
               binary_functor_t f0 = covoc->f0();
               binary_functor_t f1 = covoc->f1();
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),c0,v0,c1,v1,result))
                  return result;
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c0,v0,c1,v1,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "((t" << expr_gen.to_str(o0) << "t)"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t");
            }
         };

         struct synthesize_vocovoc_expression3
         {
            typedef typename vocovoc_t::type3 node_type;
            typedef typename vocovoc_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // ((v0 o0 c0) o1 v1) o2 c1
               typedef typename synthesize_vocov_expression0::node_type vocov_t;
               const vocov_t* vocov = dynamic_cast<const vocov_t*>(branch[0]);
               const T& v0 = vocov->t0();
               const T  c0 = vocov->t1();
               const T& v1 = vocov->t2();
               const T  c1 = dynamic_cast<details::literal_node<Type>*>(branch[1])->value();
               const details::operator_type o0 = expr_gen.get_operator(vocov->f0());
               const details::operator_type o1 = expr_gen.get_operator(vocov->f1());
               const details::operator_type o2 = operation;
               binary_functor_t f0 = vocov->f0();
               binary_functor_t f1 = vocov->f1();
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,c0,v1,c1,result))
                  return result;
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,c0,v1,c1,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "((t" << expr_gen.to_str(o0) << "t)"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t");
            }
         };

         struct synthesize_covovoc_expression3
         {
            typedef typename covovoc_t::type3 node_type;
            typedef typename covovoc_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // ((c0 o0 v0) o1 v1) o2 c1
               typedef typename synthesize_covov_expression0::node_type covov_t;
               const covov_t* covov = dynamic_cast<const covov_t*>(branch[0]);
               const T  c0 = covov->t0();
               const T& v0 = covov->t1();
               const T& v1 = covov->t2();
               const T  c1 = dynamic_cast<details::literal_node<Type>*>(branch[1])->value();
               const details::operator_type o0 = expr_gen.get_operator(covov->f0());
               const details::operator_type o1 = expr_gen.get_operator(covov->f1());
               const details::operator_type o2 = operation;
               binary_functor_t f0 = covov->f0();
               binary_functor_t f1 = covov->f1();
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),c0,v0,v1,c1,result))
                  return result;
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c0,v0,v1,c1,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "((t" << expr_gen.to_str(o0) << "t)"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t");
            }
         };

         struct synthesize_vococov_expression3
         {
            typedef typename vococov_t::type3 node_type;
            typedef typename vococov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // ((v0 o0 c0) o1 c1) o2 v1
               typedef typename synthesize_vococ_expression0::node_type vococ_t;
               const vococ_t* vococ = dynamic_cast<const vococ_t*>(branch[0]);
               const T& v0 = vococ->t0();
               const T  c0 = vococ->t1();
               const T  c1 = vococ->t2();
               const T& v1 = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
               const details::operator_type o0 = expr_gen.get_operator(vococ->f0());
               const details::operator_type o1 = expr_gen.get_operator(vococ->f1());
               const details::operator_type o2 = operation;
               binary_functor_t f0 = vococ->f0();
               binary_functor_t f1 = vococ->f1();
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,c0,c1,v1,result))
                  return result;
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,c0,c1,v1,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "((t" << expr_gen.to_str(o0) << "t)"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t");
            }
         };

         struct synthesize_vovovov_expression4
         {
            typedef typename vovovov_t::type4 node_type;
            typedef typename vovovov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // (v0 o0 (v1 o1 v2)) o2 v3
               typedef typename synthesize_vovov_expression1::node_type vovov_t;
               const vovov_t* vovov = dynamic_cast<const vovov_t*>(branch[0]);
               const T& v0 = vovov->t0();
               const T& v1 = vovov->t1();
               const T& v2 = vovov->t2();
               const T& v3 = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
               const details::operator_type o0 = expr_gen.get_operator(vovov->f0());
               const details::operator_type o1 = expr_gen.get_operator(vovov->f1());
               const details::operator_type o2 = operation;
               binary_functor_t f0 = vovov->f0();
               binary_functor_t f1 = vovov->f1();
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,v1,v2,v3,result))
                  return result;
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,v2,v3,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "(t"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t");
            }
         };

         struct synthesize_vovovoc_expression4
         {
            typedef typename vovovoc_t::type4 node_type;
            typedef typename vovovoc_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // ((v0 o0 (v1 o1 v2)) o2 c)
               typedef typename synthesize_vovov_expression1::node_type vovov_t;
               const vovov_t* vovov = dynamic_cast<const vovov_t*>(branch[0]);
               const T& v0 = vovov->t0();
               const T& v1 = vovov->t1();
               const T& v2 = vovov->t2();
               const T   c = dynamic_cast<details::literal_node<Type>*>(branch[1])->value();
               const details::operator_type o0 = expr_gen.get_operator(vovov->f0());
               const details::operator_type o1 = expr_gen.get_operator(vovov->f1());
               const details::operator_type o2 = operation;
               binary_functor_t f0 = vovov->f0();
               binary_functor_t f1 = vovov->f1();
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,v1,v2,c,result))
                  return result;
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,v2,c,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "(t"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t");
            }
         };

         struct synthesize_vovocov_expression4
         {
            typedef typename vovocov_t::type4 node_type;
            typedef typename vovocov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // ((v0 o0 (v1 o1 c)) o2 v1)
               typedef typename synthesize_vovoc_expression1::node_type vovoc_t;
               const vovoc_t* vovoc = dynamic_cast<const vovoc_t*>(branch[0]);
               const T& v0 = vovoc->t0();
               const T& v1 = vovoc->t1();
               const T   c = vovoc->t2();
               const T& v2 = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
               const details::operator_type o0 = expr_gen.get_operator(vovoc->f0());
               const details::operator_type o1 = expr_gen.get_operator(vovoc->f1());
               const details::operator_type o2 = operation;
               binary_functor_t f0 = vovoc->f0();
               binary_functor_t f1 = vovoc->f1();
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,v1,c,v2,result))
                  return result;
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,v1,c,v2,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "(t"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t");
            }
         };

         struct synthesize_vocovov_expression4
         {
            typedef typename vocovov_t::type4 node_type;
            typedef typename vocovov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // ((v0 o0 (c o1 v1)) o2 v2)
               typedef typename synthesize_vocov_expression1::node_type vocov_t;
               const vocov_t* vocov = dynamic_cast<const vocov_t*>(branch[0]);
               const T& v0 = vocov->t0();
               const T   c = vocov->t1();
               const T& v1 = vocov->t2();
               const T& v2 = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
               const details::operator_type o0 = expr_gen.get_operator(vocov->f0());
               const details::operator_type o1 = expr_gen.get_operator(vocov->f1());
               const details::operator_type o2 = operation;
               binary_functor_t f0 = vocov->f0();
               binary_functor_t f1 = vocov->f1();
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,c,v1,v2,result))
                  return result;
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,c,v1,v2,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "(t"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t");
            }
         };

         struct synthesize_covovov_expression4
         {
            typedef typename covovov_t::type4 node_type;
            typedef typename covovov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // ((c o0 (v0 o1 v1)) o2 v2)
               typedef typename synthesize_covov_expression1::node_type covov_t;
               const covov_t* covov = dynamic_cast<const covov_t*>(branch[0]);
               const T   c = covov->t0();
               const T& v0 = covov->t1();
               const T& v1 = covov->t2();
               const T& v2 = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
               const details::operator_type o0 = expr_gen.get_operator(covov->f0());
               const details::operator_type o1 = expr_gen.get_operator(covov->f1());
               const details::operator_type o2 = operation;
               binary_functor_t f0 = covov->f0();
               binary_functor_t f1 = covov->f1();
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),c,v0,v1,v2,result))
                  return result;
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c,v0,v1,v2,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "(t"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t");
            }
         };

         struct synthesize_covocov_expression4
         {
            typedef typename covocov_t::type4 node_type;
            typedef typename covocov_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // ((c0 o0 (v0 o1 c1)) o2 v1)
               typedef typename synthesize_covoc_expression1::node_type covoc_t;
               const covoc_t* covoc = dynamic_cast<const covoc_t*>(branch[0]);
               const T  c0 = covoc->t0();
               const T& v0 = covoc->t1();
               const T  c1 = covoc->t2();
               const T& v1 = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
               const details::operator_type o0 = expr_gen.get_operator(covoc->f0());
               const details::operator_type o1 = expr_gen.get_operator(covoc->f1());
               const details::operator_type o2 = operation;
               binary_functor_t f0 = covoc->f0();
               binary_functor_t f1 = covoc->f1();
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),c0,v0,c1,v1,result))
                  return result;
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c0,v0,c1,v1,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "(t"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t");
            }
         };

         struct synthesize_vocovoc_expression4
         {
            typedef typename vocovoc_t::type4 node_type;
            typedef typename vocovoc_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // ((v0 o0 (c0 o1 v1)) o2 c1)
               typedef typename synthesize_vocov_expression1::node_type vocov_t;
               const vocov_t* vocov = dynamic_cast<const vocov_t*>(branch[0]);
               const T& v0 = vocov->t0();
               const T  c0 = vocov->t1();
               const T& v1 = vocov->t2();
               const T  c1 = dynamic_cast<details::literal_node<Type>*>(branch[1])->value();
               const details::operator_type o0 = expr_gen.get_operator(vocov->f0());
               const details::operator_type o1 = expr_gen.get_operator(vocov->f1());
               const details::operator_type o2 = operation;
               binary_functor_t f0 = vocov->f0();
               binary_functor_t f1 = vocov->f1();
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),v0,c0,v1,c1,result))
                  return result;
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),v0,c0,v1,c1,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "(t"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t");
            }
         };

         struct synthesize_covovoc_expression4
         {
            typedef typename covovoc_t::type4 node_type;
            typedef typename covovoc_t::sf4_type sf4_type;
            typedef typename node_type::T0 T0;
            typedef typename node_type::T1 T1;
            typedef typename node_type::T2 T2;
            typedef typename node_type::T3 T3;
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const details::operator_type& operation,
                                                      expression_node_ptr (&branch)[2])
            {
               // ((c0 o0 (v0 o1 v1)) o2 c1)
               typedef typename synthesize_covov_expression1::node_type covov_t;
               const covov_t* covov = dynamic_cast<const covov_t*>(branch[0]);
               const T  c0 = covov->t0();
               const T& v0 = covov->t1();
               const T& v1 = covov->t2();
               const T  c1 = dynamic_cast<details::literal_node<Type>*>(branch[1])->value();
               const details::operator_type o0 = expr_gen.get_operator(covov->f0());
               const details::operator_type o1 = expr_gen.get_operator(covov->f1());
               const details::operator_type o2 = operation;
               binary_functor_t f0 = covov->f0();
               binary_functor_t f1 = covov->f1();
               binary_functor_t f2 = reinterpret_cast<binary_functor_t>(0);
               details::free_node(*(expr_gen.node_allocator_),branch[0]);
               details::free_node(*(expr_gen.node_allocator_),branch[1]);
               expression_node_ptr result = error_node();
               if (synthesize_sf4ext_expression::template compile<T0,T1,T2,T3>(expr_gen,id(expr_gen,o0,o1,o2),c0,v0,v1,c1,result))
                  return result;
               else if (!expr_gen.valid_operator(o2,f2))
                  return error_node();
               else
                  return node_type::allocate(*(expr_gen.node_allocator_),c0,v0,v1,c1,f0,f1,f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const details::operator_type o0, const details::operator_type o1, const details::operator_type o2)
            {
               return (details::build_string() << "(t" << expr_gen.to_str(o0) << "(t"<< expr_gen.to_str(o1) << "t)" << expr_gen.to_str(o2) << "t");
            }
         };

         struct synthesize_vococov_expression4
         {
            typedef typename vococov_t::type4 node_type;
            static inline expression_node_ptr process(expression_generator<Type>&, const details::operator_type&, expression_node_ptr (&)[2])
            {
               // ((v0 o0 (c0 o1 c1)) o2 v1) - Not possible
               return error_node();
            }

            static inline std::string id(expression_generator<Type>&,
                                         const details::operator_type, const details::operator_type, const details::operator_type)
            {
               return "INVALID";
            }
         };

         inline expression_node_ptr synthesize_uvouv_expression(const details::operator_type& operation, expression_node_ptr (&branch)[2])
         {
            // Definition: uv o uv
            details::operator_type o0 = dynamic_cast<details::uv_base_node<Type>*>(branch[0])->operation();
            details::operator_type o1 = dynamic_cast<details::uv_base_node<Type>*>(branch[1])->operation();
            const Type& v0 = dynamic_cast<details::uv_base_node<Type>*>(branch[0])->v();
            const Type& v1 = dynamic_cast<details::uv_base_node<Type>*>(branch[1])->v();
            unary_functor_t u0 = reinterpret_cast<unary_functor_t>(0);
            unary_functor_t u1 = reinterpret_cast<unary_functor_t>(0);
            binary_functor_t f = reinterpret_cast<binary_functor_t>(0);

            if (!valid_operator(o0,u0))
               return error_node();
            else if (!valid_operator(o1,u1))
               return error_node();
            else if (!valid_operator(operation,f))
               return error_node();

            expression_node_ptr result = error_node();
            result = node_allocator_->allocate_rrrrr<typename details::uvouv_node<Type> >(v0,v1,u0,u1,f);

            details::free_all_nodes(*node_allocator_,branch);
            return result;
         }

         #undef basic_opr_switch_statements
         #undef extended_opr_switch_statements
         #undef unary_opr_switch_statements

         #ifndef exprtk_disable_string_capabilities

         #define string_opr_switch_statements          \
         case_stmt(details::  e_lt ,details::   lt_op) \
         case_stmt(details:: e_lte ,details::  lte_op) \
         case_stmt(details::  e_gt ,details::   gt_op) \
         case_stmt(details:: e_gte ,details::  gte_op) \
         case_stmt(details::  e_eq ,details::   eq_op) \
         case_stmt(details::  e_ne ,details::   ne_op) \
         case_stmt(details::e_in   ,details::   in_op) \
         case_stmt(details::e_like ,details:: like_op) \
         case_stmt(details::e_ilike,details::ilike_op) \

         template <typename T0, typename T1>
         inline expression_node_ptr synthesize_str_xrox_expression_impl(const details::operator_type& opr, T0 s0, T1 s1, range_pack rp0)
         {
            switch (opr)
            {
               #define case_stmt(op0,op1)                                                                                 \
               case op0 : return node_allocator_->                                                                        \
                                    allocate_ttt<typename details::str_xrox_node<Type,T0,T1,range_pack,op1<Type> >,T0,T1> \
                                       (s0,s1,rp0);                                                                       \

               string_opr_switch_statements
               #undef case_stmt
               default : return error_node();
            }
         }

         template <typename T0, typename T1>
         inline expression_node_ptr synthesize_str_xoxr_expression_impl(const details::operator_type& opr, T0 s0, T1 s1, range_pack rp1)
         {
            switch (opr)
            {
               #define case_stmt(op0,op1)                                                                                 \
               case op0 : return node_allocator_->                                                                        \
                                    allocate_ttt<typename details::str_xoxr_node<Type,T0,T1,range_pack,op1<Type> >,T0,T1> \
                                       (s0,s1,rp1);                                                                       \

               string_opr_switch_statements
               #undef case_stmt
               default : return error_node();
            }
         }

         template <typename T0, typename T1>
         inline expression_node_ptr synthesize_str_xroxr_expression_impl(const details::operator_type& opr, T0 s0, T1 s1, range_pack rp0, range_pack rp1)
         {
            switch (opr)
            {
               #define case_stmt(op0,op1)                                                                                   \
               case op0 : return node_allocator_->                                                                          \
                                    allocate_tttt<typename details::str_xroxr_node<Type,T0,T1,range_pack,op1<Type> >,T0,T1> \
                                       (s0,s1,rp0,rp1);                                                                     \

               string_opr_switch_statements
               #undef case_stmt
               default : return error_node();
            }
         }

         template <typename T0, typename T1>
         inline expression_node_ptr synthesize_sos_expression_impl(const details::operator_type& opr, T0 s0, T1 s1)
         {
            switch (opr)
            {
               #define case_stmt(op0,op1)                                                                        \
               case op0 : return node_allocator_->                                                               \
                                    allocate_tt<typename details::sos_node<Type,T0,T1,op1<Type> >,T0,T1>(s0,s1); \

               string_opr_switch_statements
               #undef case_stmt
               default : return error_node();
            }
         }

         inline expression_node_ptr synthesize_sos_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            std::string& s0 = dynamic_cast<details::stringvar_node<Type>*>(branch[0])->ref();
            std::string& s1 = dynamic_cast<details::stringvar_node<Type>*>(branch[1])->ref();
            return synthesize_sos_expression_impl<std::string&,std::string&>(opr,s0,s1);
         }

         inline expression_node_ptr synthesize_sros_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            std::string& s0 = dynamic_cast<details::string_range_node<Type,range_pack>*>(branch[0])->ref();
            std::string& s1 = dynamic_cast<details::stringvar_node<Type>*>(branch[1])->ref();
            range_pack rp0  = dynamic_cast<details::string_range_node<Type,range_pack>*>(branch[0])->range();
            dynamic_cast<details::string_range_node<Type,range_pack>*>(branch[0])->range_ref().clear();
            free_node(*node_allocator_,branch[0]);
            return synthesize_str_xrox_expression_impl<std::string&,std::string&>(opr,s0,s1,rp0);
         }

         inline expression_node_ptr synthesize_sosr_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            std::string& s0 = dynamic_cast<details::stringvar_node<Type>*>(branch[0])->ref();
            std::string& s1 = dynamic_cast<details::string_range_node<Type,range_pack>*>(branch[1])->ref();
            range_pack rp1  = dynamic_cast<details::string_range_node<Type,range_pack>*>(branch[1])->range();
            dynamic_cast<details::string_range_node<Type,range_pack>*>(branch[1])->range_ref().clear();
            free_node(*node_allocator_,branch[1]);
            return synthesize_str_xoxr_expression_impl<std::string&,std::string&>(opr,s0,s1,rp1);
         }


         inline expression_node_ptr synthesize_socsr_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            std::string& s0 = dynamic_cast<details::stringvar_node<Type>*>(branch[0])->ref();
            std::string  s1 = dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[1])->str();
            range_pack rp1  = dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[1])->range();
            dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[1])->range_ref().clear();
            free_node(*node_allocator_,branch[1]);
            return synthesize_str_xoxr_expression_impl<std::string&,const std::string>(opr,s0,s1,rp1);
         }

         inline expression_node_ptr synthesize_srosr_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            std::string& s0 = dynamic_cast<details::string_range_node<Type,range_pack>*>(branch[0])->ref();
            std::string& s1 = dynamic_cast<details::string_range_node<Type,range_pack>*>(branch[1])->ref();
            range_pack rp0  = dynamic_cast<details::string_range_node<Type,range_pack>*>(branch[0])->range();
            range_pack rp1  = dynamic_cast<details::string_range_node<Type,range_pack>*>(branch[1])->range();
            dynamic_cast<details::string_range_node<Type,range_pack>*>(branch[0])->range_ref().clear();
            dynamic_cast<details::string_range_node<Type,range_pack>*>(branch[1])->range_ref().clear();
            details::free_node(*node_allocator_,branch[0]);
            details::free_node(*node_allocator_,branch[1]);
            return synthesize_str_xroxr_expression_impl<std::string&,std::string&>(opr,s0,s1,rp0,rp1);
         }

         inline expression_node_ptr synthesize_socs_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            std::string& s0 = dynamic_cast<     details::stringvar_node<Type>*>(branch[0])->ref();
            std::string  s1 = dynamic_cast<details::string_literal_node<Type>*>(branch[1])->str();
            details::free_node(*node_allocator_,branch[1]);
            return synthesize_sos_expression_impl<std::string&,const std::string>(opr,s0,s1);
         }

         inline expression_node_ptr synthesize_csos_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            std::string  s0 = dynamic_cast<details::string_literal_node<Type>*>(branch[0])->str();
            std::string& s1 = dynamic_cast<     details::stringvar_node<Type>*>(branch[1])->ref();
            details::free_node(*node_allocator_,branch[0]);
            return synthesize_sos_expression_impl<const std::string,std::string&>(opr,s0,s1);
         }

         inline expression_node_ptr synthesize_csosr_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            std::string  s0 = dynamic_cast<details::string_literal_node<Type>*>         (branch[0])->str();
            std::string& s1 = dynamic_cast<details::string_range_node<Type,range_pack>*>(branch[1])->ref();
            range_pack rp1  = dynamic_cast<details::string_range_node<Type,range_pack>*>(branch[1])->range();
            dynamic_cast<details::string_range_node<Type,range_pack>*>(branch[1])->range_ref().clear();
            details::free_node(*node_allocator_,branch[0]);
            details::free_node(*node_allocator_,branch[1]);
            return synthesize_str_xoxr_expression_impl<const std::string,std::string&>(opr,s0,s1,rp1);
         }

         inline expression_node_ptr synthesize_srocs_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            std::string& s0 = dynamic_cast<details::string_range_node<Type,range_pack>*>(branch[0])->ref();
            std::string  s1 = dynamic_cast<details::string_literal_node<Type>*>         (branch[1])->str();
            range_pack rp0  = dynamic_cast<details::string_range_node<Type,range_pack>*>(branch[0])->range();
            dynamic_cast<details::string_range_node<Type,range_pack>*>(branch[0])->range_ref().clear();
            details::free_node(*node_allocator_,branch[0]);
            details::free_node(*node_allocator_,branch[1]);
            return synthesize_str_xrox_expression_impl<std::string&,const std::string>(opr,s0,s1,rp0);
         }

         inline expression_node_ptr synthesize_srocsr_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            std::string& s0 = dynamic_cast<details::string_range_node<Type,range_pack>*>      (branch[0])->ref();
            std::string  s1 = dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[1])->str();
            range_pack rp0  = dynamic_cast<details::string_range_node<Type,range_pack>*>      (branch[0])->range();
            range_pack rp1  = dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[1])->range();
            dynamic_cast<details::string_range_node<Type,range_pack>*>      (branch[0])->range_ref().clear();
            dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[1])->range_ref().clear();
            details::free_node(*node_allocator_,branch[0]);
            details::free_node(*node_allocator_,branch[1]);
            return synthesize_str_xroxr_expression_impl<std::string&,const std::string>(opr,s0,s1,rp0,rp1);
         }

         inline expression_node_ptr synthesize_csocs_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            const std::string s0 = dynamic_cast<details::string_literal_node<Type>*>(branch[0])->str();
            const std::string s1 = dynamic_cast<details::string_literal_node<Type>*>(branch[1])->str();
            expression_node_ptr result = error_node();

            if (details::e_add == opr)
               result = node_allocator_->allocate_c<details::string_literal_node<Type> >(s0 + s1);
            else if (details::e_in == opr)
               result = node_allocator_->allocate_c<details::literal_node<Type> >(details::in_op<Type>::process(s0,s1));
            else if (details::e_like == opr)
               result = node_allocator_->allocate_c<details::literal_node<Type> >(details::like_op<Type>::process(s0,s1));
            else if (details::e_ilike == opr)
               result = node_allocator_->allocate_c<details::literal_node<Type> >(details::ilike_op<Type>::process(s0,s1));
            else
            {
               expression_node_ptr temp = synthesize_sos_expression_impl<const std::string,const std::string>(opr,s0,s1);
               Type v = temp->value();
               details::free_node(*node_allocator_,temp);
               result = node_allocator_->allocate<literal_node_t>(v);
            }
            details::free_all_nodes(*node_allocator_,branch);
            return result;
         }

         inline expression_node_ptr synthesize_csocsr_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            const std::string s0 = dynamic_cast<details::string_literal_node<Type>*>               (branch[0])->str();
                  std::string s1 = dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[1])->str();
            range_pack rp1       = dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[1])->range();
            dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[1])->range_ref().clear();
            free_node(*node_allocator_,branch[0]);
            free_node(*node_allocator_,branch[1]);
            return synthesize_str_xoxr_expression_impl<const std::string,const std::string>(opr,s0,s1,rp1);
         }

         inline expression_node_ptr synthesize_csros_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            std::string  s0 = dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[0])->str();
            std::string& s1 = dynamic_cast<details::stringvar_node<Type>*>                    (branch[1])->ref();
            range_pack rp0  = dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[0])->range();
            dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[0])->range_ref().clear();
            free_node(*node_allocator_,branch[0]);
            return synthesize_str_xrox_expression_impl<const std::string,std::string&>(opr,s0,s1,rp0);
         }

         inline expression_node_ptr synthesize_csrosr_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            const std::string  s0 = dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[0])->str();
                  std::string& s1 = dynamic_cast<details::string_range_node<Type,range_pack>*>      (branch[1])->ref();
            range_pack rp0        = dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[0])->range();
            range_pack rp1        = dynamic_cast<details::string_range_node<Type,range_pack>*>      (branch[1])->range();
            dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[0])->range_ref().clear();
            dynamic_cast<details::string_range_node<Type,range_pack>*>      (branch[1])->range_ref().clear();
            free_node(*node_allocator_,branch[0]);
            free_node(*node_allocator_,branch[1]);
            return synthesize_str_xroxr_expression_impl<const std::string,std::string&>(opr,s0,s1,rp0,rp1);
         }

         inline expression_node_ptr synthesize_csrocs_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            std::string       s0 = dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[0])->str();
            const std::string s1 = dynamic_cast<details::string_literal_node<Type>*>               (branch[1])->str();
            range_pack rp0  = dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[0])->range();
            dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[0])->range_ref().clear();
            details::free_all_nodes(*node_allocator_,branch);
            return synthesize_str_xrox_expression_impl<const std::string,std::string>(opr,s0,s1,rp0);
         }

         inline expression_node_ptr synthesize_csrocsr_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            std::string s0 = dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[0])->str();
            std::string s1 = dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[1])->str();
            range_pack rp0 = dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[0])->range();
            range_pack rp1 = dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[1])->range();
            dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[0])->range_ref().clear();
            dynamic_cast<details::const_string_range_node<Type,range_pack>*>(branch[1])->range_ref().clear();
            details::free_all_nodes(*node_allocator_,branch);
            return synthesize_str_xroxr_expression_impl<const std::string,const std::string>(opr,s0,s1,rp0,rp1);
         }
         #endif

         #ifndef exprtk_disable_string_capabilities
         inline expression_node_ptr synthesize_string_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            if ((0 == branch[0]) || (0 == branch[1]))
            {
               details::free_all_nodes(*node_allocator_,branch);
               return error_node();
            }

            const bool b0_is_s   = details::is_string_node            (branch[0]);
            const bool b0_is_cs  = details::is_const_string_node      (branch[0]);
            const bool b0_is_sr  = details::is_string_range_node      (branch[0]);
            const bool b0_is_csr = details::is_const_string_range_node(branch[0]);
            const bool b1_is_s   = details::is_string_node            (branch[1]);
            const bool b1_is_cs  = details::is_const_string_node      (branch[1]);
            const bool b1_is_sr  = details::is_string_range_node      (branch[1]);
            const bool b1_is_csr = details::is_const_string_range_node(branch[1]);

            if (b0_is_s)
            {
                    if (b1_is_s  ) return synthesize_sos_expression  (opr,branch);
               else if (b1_is_cs ) return synthesize_socs_expression (opr,branch);
               else if (b1_is_sr ) return synthesize_sosr_expression (opr,branch);
               else if (b1_is_csr) return synthesize_socsr_expression(opr,branch);
            }
            else if (b0_is_cs)
            {
                    if (b1_is_s  ) return synthesize_csos_expression  (opr,branch);
               else if (b1_is_cs ) return synthesize_csocs_expression (opr,branch);
               else if (b1_is_sr ) return synthesize_csosr_expression (opr,branch);
               else if (b1_is_csr) return synthesize_csocsr_expression(opr,branch);
            }
            else if (b0_is_sr)
            {
                    if (b1_is_s  ) return synthesize_sros_expression  (opr,branch);
               else if (b1_is_sr ) return synthesize_srosr_expression (opr,branch);
               else if (b1_is_cs ) return synthesize_srocs_expression (opr,branch);
               else if (b1_is_csr) return synthesize_srocsr_expression(opr,branch);
            }
            else if (b0_is_csr)
            {
                    if (b1_is_s  ) return synthesize_csros_expression  (opr,branch);
               else if (b1_is_sr ) return synthesize_csrosr_expression (opr,branch);
               else if (b1_is_cs ) return synthesize_csrocs_expression (opr,branch);
               else if (b1_is_csr) return synthesize_csrocsr_expression(opr,branch);
            }
            return error_node();
         }
         #else
         inline expression_node_ptr synthesize_string_expression(const details::operator_type&, expression_node_ptr (&)[2])
         {
            details::free_all_nodes(*node_allocator_,branch);
            return error_node();
         }
         #endif

         #ifndef exprtk_disable_string_capabilities
         inline expression_node_ptr synthesize_string_expression(const details::operator_type& opr, expression_node_ptr (&branch)[3])
         {
            if (details::e_inrange != opr)
               return error_node();
            else if ((0 == branch[0]) || (0 == branch[1]) || (0 == branch[2]))
            {
               details::free_all_nodes(*node_allocator_,branch);
               return error_node();
            }
            else if (
                      details::is_const_string_node(branch[0]) &&
                      details::is_const_string_node(branch[1]) &&
                      details::is_const_string_node(branch[2])
                    )
            {
               const std::string s0 = dynamic_cast<details::string_literal_node<Type>*>(branch[0])->str();
               const std::string s1 = dynamic_cast<details::string_literal_node<Type>*>(branch[1])->str();
               const std::string s2 = dynamic_cast<details::string_literal_node<Type>*>(branch[2])->str();
               Type v = (((s0 <= s1) && (s1 <= s2)) ? Type(1) : Type(0));
               details::free_all_nodes(*node_allocator_,branch);
               return node_allocator_->allocate_c<details::literal_node<Type> >(v);
            }
            else if (
                      details::is_string_node(branch[0]) &&
                      details::is_string_node(branch[1]) &&
                      details::is_string_node(branch[2])
                    )
            {
               std::string& s0 = dynamic_cast<details::stringvar_node<Type>*>(branch[0])->ref();
               std::string& s1 = dynamic_cast<details::stringvar_node<Type>*>(branch[1])->ref();
               std::string& s2 = dynamic_cast<details::stringvar_node<Type>*>(branch[2])->ref();
               typedef typename details::sosos_node<Type,std::string&,std::string&,std::string&,details::inrange_op<Type> > inrange_t;
               return node_allocator_->allocate_type<inrange_t,std::string&,std::string&,std::string&>(s0,s1,s2);
            }
            else if (
                      details::is_const_string_node(branch[0]) &&
                            details::is_string_node(branch[1]) &&
                      details::is_const_string_node(branch[2])
                    )
            {
               std::string  s0 = dynamic_cast<details::string_literal_node<Type>*>(branch[0])->str();
               std::string& s1 = dynamic_cast<     details::stringvar_node<Type>*>(branch[1])->ref();
               std::string  s2 = dynamic_cast<details::string_literal_node<Type>*>(branch[2])->str();
               typedef typename details::sosos_node<Type,std::string,std::string&,std::string,details::inrange_op<Type> > inrange_t;
               details::free_node(*node_allocator_,branch[0]);
               details::free_node(*node_allocator_,branch[2]);
               return node_allocator_->allocate_type<inrange_t,std::string,std::string&,std::string>(s0,s1,s2);
            }
            else if (
                            details::is_string_node(branch[0]) &&
                      details::is_const_string_node(branch[1]) &&
                            details::is_string_node(branch[2])
                    )
            {
               std::string&  s0 = dynamic_cast<     details::stringvar_node<Type>*>(branch[0])->ref();
               std::string   s1 = dynamic_cast<details::string_literal_node<Type>*>(branch[1])->str();
               std::string&  s2 = dynamic_cast<     details::stringvar_node<Type>*>(branch[2])->ref();
               typedef typename details::sosos_node<Type,std::string&,std::string,std::string&,details::inrange_op<Type> > inrange_t;
               details::free_node(*node_allocator_,branch[1]);
               return node_allocator_->allocate_type<inrange_t,std::string&,std::string,std::string&>(s0,s1,s2);
            }
            else if (
                      details::is_string_node(branch[0]) &&
                      details::is_string_node(branch[1]) &&
                      details::is_const_string_node(branch[2])
                    )
            {
               std::string& s0 = dynamic_cast<     details::stringvar_node<Type>*>(branch[0])->ref();
               std::string& s1 = dynamic_cast<     details::stringvar_node<Type>*>(branch[1])->ref();
               std::string  s2 = dynamic_cast<details::string_literal_node<Type>*>(branch[2])->str();
               typedef typename details::sosos_node<Type,std::string&,std::string&,std::string,details::inrange_op<Type> > inrange_t;
               details::free_node(*node_allocator_,branch[2]);
               return node_allocator_->allocate_type<inrange_t,std::string&,std::string&,std::string>(s0,s1,s2);
            }
            else if (
                      details::is_const_string_node(branch[0]) &&
                      details::      is_string_node(branch[1]) &&
                      details::      is_string_node(branch[2])
                    )
            {
               std::string  s0 = dynamic_cast<details::string_literal_node<Type>*>(branch[0])->str();
               std::string& s1 = dynamic_cast<     details::stringvar_node<Type>*>(branch[1])->ref();
               std::string& s2 = dynamic_cast<     details::stringvar_node<Type>*>(branch[2])->ref();
               typedef typename details::sosos_node<Type,std::string,std::string&,std::string&,details::inrange_op<Type> > inrange_t;
               details::free_node(*node_allocator_,branch[0]);
               return node_allocator_->allocate_type<inrange_t,std::string,std::string&,std::string&>(s0,s1,s2);
            }
            else
               return error_node();
         }
         #else
         inline expression_node_ptr synthesize_string_expression(const details::operator_type&, expression_node_ptr (&)[3])
         {
            details::free_all_nodes(*node_allocator_,branch);
            return error_node();
         }
         #endif

         inline expression_node_ptr synthesize_null_expression(const details::operator_type& operation, expression_node_ptr (&branch)[2])
         {
            bool b0_null = details::is_null_node(branch[0]);
            bool b1_null = details::is_null_node(branch[1]);

            if (b0_null && b1_null)
            {
               details::free_node(*node_allocator_,branch[1]);
               return branch[0];
            }
            else if (b0_null)
            {
               details::free_node(*node_allocator_,branch[0]);
               branch[0] = branch[1];
               branch[1] = error_node();
            }
            else if (b1_null)
            {
               details::free_node(*node_allocator_,branch[1]);
               branch[1] = error_node();
            }

            if (
                 (details::e_add  == operation) || (details::e_sub  == operation) ||
                 (details::e_mul  == operation) || (details::e_div  == operation) ||
                 (details::e_mod  == operation) || (details::e_pow  == operation)
               )
            {
               return branch[0];
            }
            else if (
                      (details::e_lt    == operation) || (details::e_lte  == operation) ||
                      (details::e_gt    == operation) || (details::e_gte  == operation) ||
                      (details::e_eq    == operation) || (details::e_ne   == operation) ||
                      (details::e_and   == operation) || (details::e_nand == operation) ||
                      (details::e_or    == operation) || (details::e_nor  == operation) ||
                      (details::e_xor   == operation) || (details::e_xnor == operation) ||
                      (details::e_in    == operation) || (details::e_like == operation) ||
                      (details::e_ilike == operation)
                    )
            {
               return node_allocator_->allocate_c<literal_node_t>(T(0));
            }
            details::free_node(*node_allocator_,branch[0]);
            return node_allocator_->allocate<details::null_node<Type> >();
         }

         template <typename NodeType, std::size_t N>
         inline expression_node_ptr synthesize_expression(const details::operator_type& operation, expression_node_ptr (&branch)[N])
         {
            if (
                 (details::e_in    == operation) ||
                 (details::e_like  == operation) ||
                 (details::e_ilike == operation)
               )
               return error_node();
            else if (!details::all_nodes_valid<N>(branch))
            {
               free_all_nodes(*node_allocator_,branch);
               return error_node();
            }
            else if ((details::e_default != operation))
            {
               // Attempt simple constant folding optimization.
               expression_node_ptr expression_point = node_allocator_->allocate<NodeType>(operation,branch);
               if (is_constant_foldable<N>(branch))
               {
                  Type v = expression_point->value();
                  details::free_node(*node_allocator_,expression_point);
                  return node_allocator_->allocate<literal_node_t>(v);
               }
               else
                  return expression_point;
            }
            else
               return error_node();
         }

         template <typename NodeType, std::size_t N>
         inline expression_node_ptr synthesize_expression(F* f, expression_node_ptr (&branch)[N])
         {
            if (!details::all_nodes_valid<N>(branch))
            {
               free_all_nodes(*node_allocator_,branch);
               return error_node();
            }
            typedef typename details::function_N_node<T,ifunction_t,N> function_N_node_t;
            // Attempt simple constant folding optimization.
            expression_node_ptr expression_point = node_allocator_->allocate<NodeType>(f);
            dynamic_cast<function_N_node_t*>(expression_point)->init_branches(branch);
            if (is_constant_foldable<N>(branch))
            {
               Type v = expression_point->value();
               details::free_node(*node_allocator_,expression_point);
               return node_allocator_->allocate<literal_node_t>(v);
            }
            else
               return expression_point;
         }

         bool strength_reduction_enabled_;
         details::node_allocator* node_allocator_;
         synthesize_map_t synthesize_map_;
         unary_op_map_t* unary_op_map_;
         binary_op_map_t* binary_op_map_;
         inv_binary_op_map_t* inv_binary_op_map_;
         sf3_map_t* sf3_map_;
         sf4_map_t* sf4_map_;
      };

      inline void set_error(const parser_error::type& error_type)
      {
         error_list_.push_back(error_type);
      }

      inline void remove_last_error()
      {
         if (!error_list_.empty())
         {
            error_list_.pop_back();
         }
      }

      inline void load_unary_operations_map(unary_op_map_t& m)
      {
         #define register_unary_op(Op,UnaryFunctor)             \
         m.insert(std::make_pair(Op,UnaryFunctor<T>::process)); \

         register_unary_op(details::  e_abs,details::  abs_op)
         register_unary_op(details:: e_acos,details:: acos_op)
         register_unary_op(details::e_acosh,details::acosh_op)
         register_unary_op(details:: e_asin,details:: asin_op)
         register_unary_op(details::e_asinh,details::asinh_op)
         register_unary_op(details::e_atanh,details::atanh_op)
         register_unary_op(details:: e_ceil,details:: ceil_op)
         register_unary_op(details::  e_cos,details::  cos_op)
         register_unary_op(details:: e_cosh,details:: cosh_op)
         register_unary_op(details::  e_exp,details::  exp_op)
         register_unary_op(details::e_expm1,details::expm1_op)
         register_unary_op(details::e_floor,details::floor_op)
         register_unary_op(details::  e_log,details::  log_op)
         register_unary_op(details::e_log10,details::log10_op)
         register_unary_op(details:: e_log2,details:: log2_op)
         register_unary_op(details::e_log1p,details::log1p_op)
         register_unary_op(details::  e_neg,details::  neg_op)
         register_unary_op(details::  e_pos,details::  pos_op)
         register_unary_op(details::e_round,details::round_op)
         register_unary_op(details::  e_sin,details::  sin_op)
         register_unary_op(details:: e_sinh,details:: sinh_op)
         register_unary_op(details:: e_sqrt,details:: sqrt_op)
         register_unary_op(details::  e_tan,details::  tan_op)
         register_unary_op(details:: e_tanh,details:: tanh_op)
         register_unary_op(details::  e_cot,details::  cot_op)
         register_unary_op(details::  e_sec,details::  sec_op)
         register_unary_op(details::  e_csc,details::  csc_op)
         register_unary_op(details::  e_r2d,details::  r2d_op)
         register_unary_op(details::  e_d2r,details::  d2r_op)
         register_unary_op(details::  e_d2g,details::  d2g_op)
         register_unary_op(details::  e_g2d,details::  g2d_op)
         register_unary_op(details:: e_notl,details:: notl_op)
         register_unary_op(details::  e_sgn,details::  sgn_op)
         register_unary_op(details::  e_erf,details::  erf_op)
         register_unary_op(details:: e_erfc,details:: erfc_op)
         register_unary_op(details:: e_frac,details:: frac_op)
         register_unary_op(details::e_trunc,details::trunc_op)
         #undef register_unary_op
      }

      inline void load_binary_operations_map(binary_op_map_t& m)
      {
         #define register_binary_op(Op,BinaryFunctor)                                  \
         m.insert(typename binary_op_map_t::value_type(Op,BinaryFunctor<T>::process)); \

         register_binary_op(details:: e_add,details:: add_op)
         register_binary_op(details:: e_sub,details:: sub_op)
         register_binary_op(details:: e_mul,details:: mul_op)
         register_binary_op(details:: e_div,details:: div_op)
         register_binary_op(details:: e_mod,details:: mod_op)
         register_binary_op(details:: e_pow,details:: pow_op)
         register_binary_op(details::  e_lt,details::  lt_op)
         register_binary_op(details:: e_lte,details:: lte_op)
         register_binary_op(details::  e_gt,details::  gt_op)
         register_binary_op(details:: e_gte,details:: gte_op)
         register_binary_op(details::  e_eq,details::  eq_op)
         register_binary_op(details::  e_ne,details::  ne_op)
         register_binary_op(details:: e_and,details:: and_op)
         register_binary_op(details::e_nand,details::nand_op)
         register_binary_op(details::  e_or,details::  or_op)
         register_binary_op(details:: e_nor,details:: nor_op)
         register_binary_op(details:: e_xor,details:: xor_op)
         register_binary_op(details::e_xnor,details::xnor_op)
         #undef register_binary_op
      }

      inline void load_inv_binary_operations_map(inv_binary_op_map_t& m)
      {
         #define register_binary_op(Op,BinaryFunctor)                                      \
         m.insert(typename inv_binary_op_map_t::value_type(BinaryFunctor<T>::process,Op)); \

         register_binary_op(details:: e_add,details:: add_op)
         register_binary_op(details:: e_sub,details:: sub_op)
         register_binary_op(details:: e_mul,details:: mul_op)
         register_binary_op(details:: e_div,details:: div_op)
         register_binary_op(details:: e_mod,details:: mod_op)
         register_binary_op(details:: e_pow,details:: pow_op)
         register_binary_op(details::  e_lt,details::  lt_op)
         register_binary_op(details:: e_lte,details:: lte_op)
         register_binary_op(details::  e_gt,details::  gt_op)
         register_binary_op(details:: e_gte,details:: gte_op)
         register_binary_op(details::  e_eq,details::  eq_op)
         register_binary_op(details::  e_ne,details::  ne_op)
         register_binary_op(details:: e_and,details:: and_op)
         register_binary_op(details::e_nand,details::nand_op)
         register_binary_op(details::  e_or,details::  or_op)
         register_binary_op(details:: e_nor,details:: nor_op)
         register_binary_op(details:: e_xor,details:: xor_op)
         register_binary_op(details::e_xnor,details::xnor_op)
         #undef register_binary_op
      }

      inline void load_sf3_map(sf3_map_t& sf3_map)
      {
         typedef std::pair<trinary_functor_t,details::operator_type> pair_t;

         #define register_sf3(Op)                                                                             \
         sf3_map[details::sf##Op##_op<T>::id()] = pair_t(details::sf##Op##_op<T>::process,details::e_sf##Op); \

         register_sf3(00) register_sf3(01) register_sf3(02) register_sf3(03)
         register_sf3(04) register_sf3(05) register_sf3(06) register_sf3(07)
         register_sf3(08) register_sf3(09) register_sf3(10) register_sf3(11)
         register_sf3(12) register_sf3(13) register_sf3(14) register_sf3(15)
         register_sf3(16) register_sf3(17) register_sf3(18) register_sf3(19)
         register_sf3(20) register_sf3(21) register_sf3(22) register_sf3(23)
         register_sf3(24) register_sf3(25) register_sf3(26) register_sf3(27)
         register_sf3(28) register_sf3(29) register_sf3(30)
         #undef register_sf3
      }

      inline void load_sf4_map(sf4_map_t& sf4_map)
      {
         typedef std::pair<quaternary_functor_t,details::operator_type> pair_t;

         #define register_sf4(Op)                                                                             \
         sf4_map[details::sf##Op##_op<T>::id()] = pair_t(details::sf##Op##_op<T>::process,details::e_sf##Op); \

         register_sf4(48) register_sf4(49) register_sf4(50) register_sf4(51)
         register_sf4(52) register_sf4(53) register_sf4(54) register_sf4(55)
         register_sf4(56) register_sf4(57) register_sf4(58) register_sf4(59)
         register_sf4(60) register_sf4(61) register_sf4(62) register_sf4(63)
         register_sf4(64) register_sf4(65) register_sf4(66) register_sf4(67)
         register_sf4(68) register_sf4(69) register_sf4(70) register_sf4(71)
         register_sf4(72) register_sf4(73) register_sf4(74) register_sf4(75)
         register_sf4(76) register_sf4(77) register_sf4(78) register_sf4(79)
         register_sf4(80) register_sf4(81) register_sf4(82) register_sf4(83)
         #undef register_sf4

         #define register_sf4ext(Op)                                                                                    \
         sf4_map[details::sfext##Op##_op<T>::id()] = pair_t(details::sfext##Op##_op<T>::process,details::e_sf4ext##Op); \

         register_sf4ext(00) register_sf4ext(01) register_sf4ext(02) register_sf4ext(03)
         register_sf4ext(04) register_sf4ext(05) register_sf4ext(06) register_sf4ext(07)
         register_sf4ext(08) register_sf4ext(09) register_sf4ext(10) register_sf4ext(11)
         register_sf4ext(12) register_sf4ext(13) register_sf4ext(14) register_sf4ext(15)
         register_sf4ext(16) register_sf4ext(17) register_sf4ext(18) register_sf4ext(19)
         register_sf4ext(20) register_sf4ext(21) register_sf4ext(22) register_sf4ext(23)
         register_sf4ext(24) register_sf4ext(25) register_sf4ext(26) register_sf4ext(27)
         register_sf4ext(28) register_sf4ext(29) register_sf4ext(30) register_sf4ext(31)
         register_sf4ext(32) register_sf4ext(33) register_sf4ext(34) register_sf4ext(35)
         register_sf4ext(36) register_sf4ext(36) register_sf4ext(38) register_sf4ext(39)
         register_sf4ext(40) register_sf4ext(41) register_sf4ext(42) register_sf4ext(43)
         register_sf4ext(44) register_sf4ext(45)
         #undef register_sf4ext
      }

   private:

      parser(const parser<T>&);
      parser<T>& operator=(const parser<T>&);

      lexer::generator lexer_;
      lexer::token current_token_;
      lexer::token store_current_token_;
      expression_generator<T> expression_generator_;
      details::node_allocator node_allocator_;
      symbol_table<T> symbol_table_;
      bool symbol_name_caching_;
      std::size_t compile_options_;
      std::deque<std::string> symbol_name_cache_;
      std::deque<parser_error::type> error_list_;
      bool resolve_unknown_symbol_;
      unknown_symbol_resolver* unknown_symbol_resolver_;
      unknown_symbol_resolver default_usr_;
      base_ops_map_t base_ops_map_;
      unary_op_map_t unary_op_map_;
      binary_op_map_t binary_op_map_;
      inv_binary_op_map_t inv_binary_op_map_;
      sf3_map_t sf3_map_;
      sf4_map_t sf4_map_;

      lexer::helper::helper_assembly helper_assembly_;

      lexer::helper::commutative_inserter commutative_inserter_;
      lexer::helper::operator_joiner      operator_joiner_2_;
      lexer::helper::operator_joiner      operator_joiner_3_;
      lexer::helper::symbol_replacer      symbol_replacer_;
      lexer::helper::bracket_checker      bracket_checker_;
      lexer::helper::numeric_checker      numeric_checker_;
      lexer::helper::sequence_validator   sequence_validator_;
   };

   template <typename T>
   inline T integrate(expression<T>& e,
                      T& x,
                      const T& r0, const T& r1,
                      const std::size_t number_of_intervals = 1000000)
   {
      if (r0 > r1) return T(0);
      T h = (r1 - r0) / (T(2) * number_of_intervals);
      T total_area = T(0);
      for (std::size_t i = 0; i < number_of_intervals; ++i)
      {
         x = r0 + T(2) * i * h;
         T y0 = e.value(); x += h;
         T y1 = e.value(); x += h;
         T y2 = e.value(); x += h;
         total_area += h * (y0 + T(4) * y1 + y2) / T(3);
      }
      return total_area;
   }

   template <typename T>
   inline T integrate(expression<T>& e,
                      const std::string& variable_name,
                      const T& r0, const T& r1,
                      const std::size_t number_of_intervals = 1000000)
   {
      symbol_table<T>& sym_table = e.get_symbol_table();
      if (!sym_table.valid())
         return std::numeric_limits<T>::quiet_NaN();
      details::variable_node<T>* var = sym_table.get_variable(variable_name);
      if (var)
      {
         T& x = var->ref();
         T  x_original = x;
         T result = integrate(e,x,r0,r1,number_of_intervals);
         x = x_original;
         return result;
      }
      else
         return std::numeric_limits<T>::quiet_NaN();
   }

   template <typename T>
   inline T derivative(expression<T>& e,
                       T& x,
                       const T& h = T(0.00000001))
   {
      T x_init = x;
      x = x_init + T(2) * h;
      T y0 = e.value();
      x = x_init + h;
      T y1 = e.value();
      x = x_init - h;
      T y2 = e.value();
      x = x_init - T(2) * h;
      T y3 = e.value();
      x = x_init;
      return (-y0 + T(8) * (y1 - y2) + y3) / (T(12) * h);
   }

   template <typename T>
   inline T second_derivative(expression<T>& e,
                              T& x,
                              const T& h = T(0.00001))
   {
      T y = e.value();
      T x_init = x;
      x = x_init + T(2) * h;
      T y0 = e.value();
      x = x_init + h;
      T y1 = e.value();
      x = x_init - h;
      T y2 = e.value();
      x = x_init - T(2) * h;
      T y3 = e.value();
      x = x_init;
      return (-y0 + T(16) * (y1 + y2) - T(30) * y - y3) / (T(12) * h * h);
   }

   template <typename T>
   inline T third_derivative(expression<T>& e,
                             T& x,
                             const T& h = T(0.0001))
   {
      T x_init = x;
      x = x_init + T(2) * h;
      T y0 = e.value();
      x = x_init + h;
      T y1 = e.value();
      x = x_init - h;
      T y2 = e.value();
      x = x_init - T(2) * h;
      T y3 = e.value();
      x = x_init;
      return (y0 + T(2) * (y2 - y1) - y3) / (T(2) * h * h * h);
   }

   template <typename T>
   inline T derivative(expression<T>& e,
                       const std::string& variable_name,
                       const T& h = T(0.00000001))
   {
      symbol_table<T>& sym_table = e.get_symbol_table();
      if (!sym_table.valid())
         return std::numeric_limits<T>::quiet_NaN();
      details::variable_node<T>* var = sym_table.get_variable(variable_name);
      if (var)
      {
         T& x = var->ref();
         T x_original = x;
         T result = derivative(e,x,h);
         x = x_original;
         return result;
      }
      else
         return std::numeric_limits<T>::quiet_NaN();
   }

   template <typename T>
   inline T second_derivative(expression<T>& e,
                              const std::string& variable_name,
                              const T& h = T(0.00001))
   {
      symbol_table<T>& sym_table = e.get_symbol_table();
      if (!sym_table.valid())
         return std::numeric_limits<T>::quiet_NaN();
      details::variable_node<T>* var = sym_table.get_variable(variable_name);
      if (var)
      {
         T& x = var->ref();
         T x_original = x;
         T result = second_derivative(e,x,h);
         x = x_original;
         return result;
      }
      else
         return std::numeric_limits<T>::quiet_NaN();
   }

   template <typename T>
   inline T third_derivative(expression<T>& e,
                             const std::string& variable_name,
                             const T& h = T(0.0001))
   {
      symbol_table<T>& sym_table = e.get_symbol_table();
      if (!sym_table.valid())
         return std::numeric_limits<T>::quiet_NaN();
      details::variable_node<T>* var = sym_table.get_variable(variable_name);
      if (var)
      {
         T& x = var->ref();
         T x_original = x;
         T result = third_derivative(e,x,h);
         x = x_original;
         return result;
      }
      else
         return std::numeric_limits<T>::quiet_NaN();
   }

   /*
      Note: The following 'compute' routines are very simple helpers,
      for quickly setting up the required pieces of code in order to
      evaluate an expression. By virtue of how they operate there will
      be an overhead with regards to their setup and teardown and hence
      should not be used in time critical sections of code.
      Furthermore they only assume a small sub set of variables - no
      string variables or user defined functions.
   */
   template <typename T>
   inline bool compute(const std::string& expression_string, T& result)
   {
      // No variables
      symbol_table<T> symbol_table;
      symbol_table.add_constants();

      expression<T> expression;
      parser<T> parser;

      if (parser.compile(expression_string,expression))
      {
         result = expression.value();
         return true;
      }
      else
         return false;
   }

   template <typename T>
   inline bool compute(const std::string& expression_string,
                       const T& x,
                       T& result)
   {
      // Only 'x'
      static const std::string x_var("x");
      symbol_table<T> symbol_table;
      symbol_table.add_constants();
      symbol_table.add_variable(x_var,x);
      expression<T> expression;
      parser<T> parser;
      if (parser.compile(expression_string,expression))
      {
         result = expression.value();
         return true;
      }
      else
         return false;
   }

   template <typename T>
   inline bool compute(const std::string& expression_string,
                       const T&x, const T& y,
                       T& result)
   {
      // Only 'x' and 'y'
      static const std::string x_var("x");
      static const std::string y_var("y");
      symbol_table<T> symbol_table;
      symbol_table.add_constants();
      symbol_table.add_variable(x_var,x);
      symbol_table.add_variable(y_var,y);
      expression<T> expression;
      parser<T> parser;
      if (parser.compile(expression_string,expression))
      {
         result = expression.value();
         return true;
      }
      else
         return false;
   }

   template <typename T>
   inline bool compute(const std::string& expression_string,
                       const T& x, const T& y, const T& z,
                       T& result)
   {
      // Only 'x', 'y' or 'z'
      static const std::string x_var("x");
      static const std::string y_var("y");
      static const std::string z_var("z");
      symbol_table<T> symbol_table;
      symbol_table.add_constants();
      symbol_table.add_variable(x_var,x);
      symbol_table.add_variable(y_var,y);
      symbol_table.add_variable(z_var,z);
      expression<T> expression;
      parser<T> parser;
      if (parser.compile(expression_string,expression))
      {
         result = expression.value();
         return true;
      }
      else
         return false;
   }

   template <typename T, std::size_t N>
   class polynomial : public ifunction<T>
   {
   private:

      template <typename Type, std::size_t NumberOfCoefficients>
      struct poly_impl { };

      template <typename Type>
      struct poly_impl <Type,12>
      {
         static inline T evaluate(const Type x,
                                  const Type c12, const Type c11, const Type c10, const Type c9, const Type c8,
                                  const Type  c7, const Type  c6, const Type  c5, const Type c4, const Type c3,
                                  const Type  c2, const Type  c1, const Type  c0)
         {
            // p(x) = c_12x^12 + c_11x^11 + c_10x^10 + c_9x^9 + c_8x^8 + c_7x^7 + c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return ((((((((((((c12 * x + c11) * x + c10) * x + c9) * x + c8) * x + c7) * x + c6) * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,11>
      {
         static inline T evaluate(const Type x,
                                  const Type c11, const Type c10, const Type c9, const Type c8, const Type c7,
                                  const Type c6,  const Type  c5, const Type c4, const Type c3, const Type c2,
                                  const Type c1,  const Type  c0)
         {
            // p(x) = c_11x^11 + c_10x^10 + c_9x^9 + c_8x^8 + c_7x^7 + c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return (((((((((((c11 * x + c10) * x + c9) * x + c8) * x + c7) * x + c6) * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,10>
      {
         static inline T evaluate(const Type x,
                                  const Type c10, const Type c9, const Type c8, const Type c7, const Type c6,
                                  const Type c5,  const Type c4, const Type c3, const Type c2, const Type c1,
                                  const Type c0)
         {
            // p(x) = c_10x^10 + c_9x^9 + c_8x^8 + c_7x^7 + c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return ((((((((((c10 * x + c9) * x + c8) * x + c7) * x + c6) * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,9>
      {
         static inline T evaluate(const Type x,
                                  const Type c9, const Type c8, const Type c7, const Type c6, const Type c5,
                                  const Type c4, const Type c3, const Type c2, const Type c1, const Type c0)
         {
            // p(x) = c_9x^9 + c_8x^8 + c_7x^7 + c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return (((((((((c9 * x + c8) * x + c7) * x + c6) * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,8>
      {
         static inline T evaluate(const Type x,
                                  const Type c8, const Type c7, const Type c6, const Type c5, const Type c4,
                                  const Type c3, const Type c2, const Type c1, const Type c0)
         {
            // p(x) = c_8x^8 + c_7x^7 + c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return ((((((((c8 * x + c7) * x + c6) * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,7>
      {
         static inline T evaluate(const Type x,
                                  const Type c7, const Type c6, const Type c5, const Type c4, const Type c3,
                                  const Type c2, const Type c1, const Type c0)
         {
            // p(x) = c_7x^7 + c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return (((((((c7 * x + c6) * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,6>
      {
         static inline T evaluate(const Type x,
                                  const Type c6, const Type c5, const Type c4, const Type c3, const Type c2,
                                  const Type c1, const Type c0)
         {
            // p(x) = c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return ((((((c6 * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,5>
      {
         static inline T evaluate(const Type x,
                                  const Type c5, const Type c4, const Type c3, const Type c2,
                                  const Type c1, const Type c0)
         {
            // p(x) = c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return (((((c5 * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,4>
      {
         static inline T evaluate(const Type x, const Type c4, const Type c3, const Type c2, const Type c1, const Type c0)
         {
            // p(x) = c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return ((((c4 * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,3>
      {
         static inline T evaluate(const Type x, const Type c3, const Type c2, const Type c1, const Type c0)
         {
            // p(x) = c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return (((c3 * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,2>
      {
         static inline T evaluate(const Type x, const Type c2, const Type c1, const Type c0)
         {
            // p(x) = c_2x^2 + c_1x^1 + c_0x^0
            return ((c2 * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,1>
      {
         static inline T evaluate(const Type x, const Type c1, const Type c0)
         {
            // p(x) = c_1x^1 + c_0x^0
            return (c1 * x + c0);
         }
      };

   public:

      polynomial() : exprtk::ifunction<T>((N+2 <= 20) ? (N + 2) : std::numeric_limits<std::size_t>::max()) {}

      inline virtual T operator()(const T& x, const T& c1, const T& c0)
      {
         return ((1 == N) ? poly_impl<T,1>::evaluate(x,c1,c0) : std::numeric_limits<T>::quiet_NaN());
      }

      inline virtual T operator()(const T& x, const T& c2, const T& c1, const T& c0)
      {
         return ((2 == N) ? poly_impl<T,2>::evaluate(x,c2,c1,c0) : std::numeric_limits<T>::quiet_NaN());
      }

      inline virtual T operator()(const T& x, const T& c3, const T& c2, const T& c1, const T& c0)
      {
         return ((3 == N) ? poly_impl<T,3>::evaluate(x,c3,c2,c1,c0) : std::numeric_limits<T>::quiet_NaN());
      }

      inline virtual T operator()(const T& x, const T& c4, const T& c3, const T& c2, const T& c1, const T& c0)
      {
         return ((4 == N) ? poly_impl<T,4>::evaluate(x,c4,c3,c2,c1,c0) : std::numeric_limits<T>::quiet_NaN());
      }

      inline virtual T operator()(const T& x, const T& c5, const T& c4, const T& c3, const T& c2, const T& c1, const T& c0)
      {
         return ((5 == N) ? poly_impl<T,5>::evaluate(x,c5,c4,c3,c2,c1,c0) : std::numeric_limits<T>::quiet_NaN());
      }

      inline virtual T operator()(const T& x, const T& c6, const T& c5, const T& c4, const T& c3, const T& c2, const T& c1, const T& c0)
      {
         return ((6 == N) ? poly_impl<T,6>::evaluate(x,c6,c5,c4,c3,c2,c1,c0) : std::numeric_limits<T>::quiet_NaN());
      }

      inline virtual T operator()(const T& x, const T& c7, const T& c6, const T& c5, const T& c4, const T& c3, const T& c2, const T& c1, const T& c0)
      {
         return ((7 == N) ? poly_impl<T,7>::evaluate(x,c7,c6,c5,c4,c3,c2,c1,c0) : std::numeric_limits<T>::quiet_NaN());
      }

      inline virtual T operator()(const T& x, const T& c8, const T& c7, const T& c6, const T& c5, const T& c4, const T& c3, const T& c2, const T& c1, const T& c0)
      {
         return ((8 == N) ? poly_impl<T,8>::evaluate(x,c8,c7,c6,c5,c4,c3,c2,c1,c0) : std::numeric_limits<T>::quiet_NaN());
      }

      inline virtual T operator()(const T& x, const T& c9, const T& c8, const T& c7, const T& c6, const T& c5, const T& c4, const T& c3, const T& c2, const T& c1, const T& c0)
      {
         return ((9 == N) ? poly_impl<T,9>::evaluate(x,c9,c8,c7,c6,c5,c4,c3,c2,c1,c0) : std::numeric_limits<T>::quiet_NaN());
      }

      inline virtual T operator()(const T& x, const T& c10, const T& c9, const T& c8, const T& c7, const T& c6, const T& c5, const T& c4, const T& c3, const T& c2, const T& c1, const T& c0)
      {
         return ((10 == N) ? poly_impl<T,10>::evaluate(x,c10,c9,c8,c7,c6,c5,c4,c3,c2,c1,c0) : std::numeric_limits<T>::quiet_NaN());
      }

      inline virtual T operator()(const T& x, const T& c11, const T& c10, const T& c9, const T& c8, const T& c7, const T& c6, const T& c5, const T& c4, const T& c3, const T& c2, const T& c1, const T& c0)
      {
         return ((11 == N) ? poly_impl<T,11>::evaluate(x,c11,c10,c9,c8,c7,c6,c5,c4,c3,c2,c1,c0) : std::numeric_limits<T>::quiet_NaN());
      }

      inline virtual T operator()(const T& x, const T& c12, const T& c11, const T& c10, const T& c9, const T& c8, const T& c7, const T& c6, const T& c5, const T& c4, const T& c3, const T& c2, const T& c1, const T& c0)
      {
         return ((12 == N) ? poly_impl<T,12>::evaluate(x,c12,c11,c10,c9,c8,c7,c6,c5,c4,c3,c2,c1,c0) : std::numeric_limits<T>::quiet_NaN());
      }

      inline virtual T operator()()
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator()(const T&, const T&)
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

   };

   template <typename T>
   class function_compositor
   {
   public:

      typedef exprtk::expression<T>   expression_t;
      typedef exprtk::symbol_table<T> symbol_table_t;
      typedef exprtk::parser<T>       parser_t;

      struct function
      {
         function(const std::string& n)
         : name_(n)
         {}

         inline function& name(const std::string& n)
         {
            name_ = n;
            return (*this);
         }

         inline function& expression(const std::string& e)
         {
            expression_ = e;
            return (*this);
         }

         inline function& var(const std::string& v)
         {
            v_.push_back(v);
            return (*this);
         }

         std::string name_;
         std::string expression_;
         std::deque<std::string> v_;
      };

   private:

      struct base_func : public exprtk::ifunction<T>
      {
         typedef const T& type;
         typedef exprtk::ifunction<T> function_t;
         typedef std::vector<T*> varref_t;
         typedef std::vector<T>  var_t;

         base_func(const std::size_t& param_count = 0)
         : exprtk::ifunction<T>(param_count),
           stack_depth(0)
         {
            v.resize(param_count);
         }

         inline void update(const T& v0)
         {
            (*v[0]) = v0;
         }

         inline void update(const T& v0, const T& v1)
         {
            (*v[0]) = v0; (*v[1]) = v1;
         }

         inline void update(const T& v0, const T& v1, const T& v2)
         {
            (*v[0]) = v0; (*v[1]) = v1;
            (*v[2]) = v2;
         }

         inline void update(const T& v0, const T& v1, const T& v2, const T& v3)
         {
            (*v[0]) = v0; (*v[1]) = v1;
            (*v[2]) = v2; (*v[3]) = v3;
         }

         inline void update(const T& v0, const T& v1, const T& v2, const T& v3, const T& v4)
         {
            (*v[0]) = v0; (*v[1]) = v1;
            (*v[2]) = v2; (*v[3]) = v3;
            (*v[4]) = v4;
         }

         inline void update(const T& v0, const T& v1, const T& v2, const T& v3, const T& v4, const T& v5)
         {
            (*v[0]) = v0; (*v[1]) = v1;
            (*v[2]) = v2; (*v[3]) = v3;
            (*v[4]) = v4; (*v[5]) = v5;
         }

         inline function_t& setup(expression_t& expr, T& v0, T& v1, T& v2, T& v3, T& v4, T& v5)
         {
            expression = expr;
            v[0] = &v0; v[1] = &v1;
            v[2] = &v2; v[3] = &v3;
            v[4] = &v4; v[5] = &v5;
            clear_stack();
            return (*this);
         }

         inline function_t& setup(expression_t& expr, T& v0, T& v1, T& v2, T& v3, T& v4)
         {
            expression = expr;
            v[0] = &v0; v[1] = &v1;
            v[2] = &v2; v[3] = &v3;
            v[4] = &v4;
            clear_stack();
            return (*this);
         }

         inline function_t& setup(expression_t& expr, T& v0, T& v1, T& v2, T& v3)
         {
            expression = expr;
            v[0] = &v0; v[1] = &v1;
            v[2] = &v2; v[3] = &v3;
            clear_stack();
            return (*this);
         }

         inline function_t& setup(expression_t& expr, T& v0, T& v1, T& v2)
         {
            expression = expr;
            v[0] = &v0; v[1] = &v1;
            v[2] = &v2;
            clear_stack();
            return (*this);
         }

         inline function_t& setup(expression_t& expr, T& v0, T& v1)
         {
            expression = expr;
            v[0] = &v0; v[1] = &v1;
            clear_stack();
            return (*this);
         }

         inline function_t& setup(expression_t& expr, T& v0)
         {
            expression = expr;
            v[0] = &v0;
            clear_stack();
            return (*this);
         }

         inline function_t& setup(expression_t& expr)
         {
            expression = expr;
            return (*this);
         }

         inline void pre()
         {
            if (stack_depth++)
            {
               var_t var_stack(v.size(),T(0));
               copy(v,var_stack);
               stack.push_back(var_stack);
            }
         }

         inline void post()
         {
            if (--stack_depth)
            {
               copy(stack.back(),v);
               stack.pop_back();
            }
         }

         void copy(const varref_t& src_v, var_t& dest_v)
         {
            for (std::size_t i = 0; i < src_v.size(); ++i)
            {
               dest_v[i] = (*src_v[i]);
            }
         }

         void copy(const var_t& src_v, varref_t& dest_v)
         {
            for (std::size_t i = 0; i < src_v.size(); ++i)
            {
               (*dest_v[i]) = src_v[i];
            }
         }

         inline void clear_stack()
         {
            for (std::size_t i = 0; i < v.size(); ++i)
            {
               (*v[i]) = 0;
            }
         }

         expression_t expression;
         varref_t v;
         std::size_t stack_depth;
         std::deque<var_t> stack;
      };

      struct func_0param : public base_func
      {
         func_0param() : base_func(0) {}

         inline T operator()()
         {
            return base_func::expression.value();
         }
      };

      typedef const T& type;

      struct func_1param : public base_func
      {
         func_1param() : base_func(1) {}

         inline T operator()(type v0)
         {
            base_func::pre();
            base_func::update(v0);
            T result = base_func::expression.value();
            base_func::post();
            return result;
         }
      };

      struct func_2param : public base_func
      {
         func_2param() : base_func(2) {}

         inline T operator()(type v0, type v1)
         {
            base_func::pre();
            base_func::update(v0,v1);
            T result = base_func::expression.value();
            base_func::post();
            return result;
         }
      };

      struct func_3param : public base_func
      {
         func_3param() : base_func(3) {}

         inline T operator()(type v0, type v1, type v2)
         {
            base_func::pre();
            base_func::update(v0,v1,v2);
            T result = base_func::expression.value();
            base_func::post();
            return result;
         }
      };

      struct func_4param : public base_func
      {
         func_4param() : base_func(4) {}

         inline T operator()(type v0, type v1, type v2, type v3)
         {
            base_func::pre();
            base_func::update(v0,v1,v2,v3);
            T result = base_func::expression.value();
            base_func::post();
            return result;
         }
      };

      struct func_5param : public base_func
      {
         func_5param() : base_func(5) {}

         inline T operator()(type v0, type v1, type v2, type v3, type v4)
         {
            base_func::pre();
            base_func::update(v0,v1,v2,v3,v4);
            T result = base_func::expression.value();
            base_func::post();
            return result;
         }
      };

      struct func_6param : public base_func
      {
         func_6param() : base_func(6) {}

         inline T operator()(type v0, type v1, type v2, type v3, type v4, type v5)
         {
            base_func::pre();
            base_func::update(v0,v1,v2,v3,v4,v5);
            T result = base_func::expression.value();
            base_func::post();
            return result;
         }
      };

   public:

      function_compositor()
      : suffix_index_(1),
        id_(get_id())
      {}

      function_compositor(const symbol_table_t& st)
      : symbol_table_(st),
        suffix_index_(1),
        id_(get_id())
      {}

      inline symbol_table_t& symbol_table()
      {
         return symbol_table_;
      }

      void clear()
      {
         symbol_table_.clear();
         expr_map_.clear();
         f0p_map_.clear();
         f1p_map_.clear();
         f2p_map_.clear();
         f3p_map_.clear();
         f4p_map_.clear();
         f5p_map_.clear();
         f6p_map_.clear();
         suffix_index_ = 1;
      }

      inline bool add(const function& f)
      {
         switch (f.v_.size())
         {
            case 0  : return add(f.name_,f.expression_);
            case 1  : return add(f.name_,f.expression_,f.v_[0]);
            case 2  : return add(f.name_,f.expression_,f.v_[0],f.v_[1]);
            case 3  : return add(f.name_,f.expression_,f.v_[0],f.v_[1],f.v_[2]);
            case 4  : return add(f.name_,f.expression_,f.v_[0],f.v_[1],f.v_[2],f.v_[3]);
            case 5  : return add(f.name_,f.expression_,f.v_[0],f.v_[1],f.v_[2],f.v_[3],f.v_[4]);
            case 6  : return add(f.name_,f.expression_,f.v_[0],f.v_[1],f.v_[2],f.v_[3],f.v_[4],f.v_[5]);
            default : return false;
         }
      }

      inline bool add(const std::string& name,
                      const std::string& expression)
      {
         if (expr_map_.end() != expr_map_.find(name))
            return false;
         else if (!forward(name,0))
            return false;
         std::vector<std::pair<std::string,std::string> > var_transform_list;
         if (compile_expression(name,expression,var_transform_list))
         {
            f0p_map_[name].setup(expr_map_[name]);
            return true;
         }
         else
         {
            remove(name,0);
            return false;
         }
      }

      inline bool add(const std::string& name,
                      const std::string& expression,
                      const std::string& v0)
      {
         const std::size_t n = 1;
         T* v[n] = { 0 };
         std::string sv[n];
         if (expr_map_.end() != expr_map_.find(name))
            return false;
         else if (!forward(name,1))
            return false;
         else if (!add_variable(v0,v[0],sv[0]))
            return false;
         std::vector<std::pair<std::string,std::string> > var_transform_list;
         var_transform_list.push_back(std::make_pair(v0,sv[0]));
         if (compile_expression(name,expression,var_transform_list))
         {
            f1p_map_[name].setup(expr_map_[name],(*v[0]));
            return true;
         }
         else
         {
            remove(name,sv);
            return false;
         }
      }

      inline bool add(const std::string& name,
                      const std::string& expression,
                      const std::string& v0, const std::string& v1)
      {
         const std::size_t n = 2;
         T* v[n] = { 0 };
         std::string sv[n];
         if (expr_map_.end() != expr_map_.find(name))
            return false;
         else if (!forward(name,2))
            return false;
         else if (!add_variable(v0,v[0],sv[0])) return false;
         else if (!add_variable(v1,v[1],sv[1])) return false;
         std::vector<std::pair<std::string,std::string> > var_transform_list;
         var_transform_list.push_back(std::make_pair(v0,sv[0]));
         var_transform_list.push_back(std::make_pair(v1,sv[1]));
         if (compile_expression(name,expression,var_transform_list))
         {
            f2p_map_[name].setup(expr_map_[name],(*v[0]),(*v[1]));
            return true;
         }
         else
         {
            remove(name,sv);
            return false;
         }
      }

      inline bool add(const std::string& name,
                      const std::string& expression,
                      const std::string& v0, const std::string& v1, const std::string& v2)
      {
         const std::size_t n = 3;
         T* v[n] = { 0 };
         std::string sv[n];
         if (expr_map_.end() != expr_map_.find(name))
            return false;
         else if (!forward(name,3))
            return false;
         else if (!add_variable(v0,v[0],sv[0])) return false;
         else if (!add_variable(v1,v[1],sv[1])) return false;
         else if (!add_variable(v2,v[2],sv[2])) return false;
         std::vector<std::pair<std::string,std::string> > var_transform_list;
         var_transform_list.push_back(std::make_pair(v0,sv[0]));
         var_transform_list.push_back(std::make_pair(v1,sv[1]));
         var_transform_list.push_back(std::make_pair(v2,sv[2]));
         if (compile_expression(name,expression,var_transform_list))
         {
            f3p_map_[name].setup(expr_map_[name],(*v[0]),(*v[1]),(*v[2]));
            return true;
         }
         else
         {
            remove(name,sv);
            return false;
         }
      }

      inline bool add(const std::string& name,
                      const std::string& expression,
                      const std::string& v0, const std::string& v1, const std::string& v2,
                      const std::string& v3)
      {
         const std::size_t n = 4;
         T* v[n] = { 0 };
         std::string sv[n];
         if (expr_map_.end() != expr_map_.find(name))
            return false;
         else if (!forward(name,4))
            return false;
         else if (!add_variable(v0,v[0],sv[0])) return false;
         else if (!add_variable(v1,v[1],sv[1])) return false;
         else if (!add_variable(v2,v[2],sv[2])) return false;
         else if (!add_variable(v3,v[3],sv[3])) return false;
         std::vector<std::pair<std::string,std::string> > var_transform_list;
         var_transform_list.push_back(std::make_pair(v0,sv[0]));
         var_transform_list.push_back(std::make_pair(v1,sv[1]));
         var_transform_list.push_back(std::make_pair(v2,sv[2]));
         var_transform_list.push_back(std::make_pair(v3,sv[3]));
         if (compile_expression(name,expression,var_transform_list))
         {
            f4p_map_[name].setup(expr_map_[name],(*v[0]),(*v[1]),(*v[2]),(*v[3]));
            return true;
         }
         else
         {
            remove(name,sv);
            return false;
         }
      }

      inline bool add(const std::string& name,
                      const std::string& expression,
                      const std::string& v0, const std::string& v1, const std::string& v2,
                      const std::string& v3, const std::string& v4)
      {
         const std::size_t n = 5;
         T* v[n] = { 0 };
         std::string sv[n];
         if (expr_map_.end() != expr_map_.find(name))
            return false;
         else if (!forward(name,5))
            return false;
         else if (!add_variable(v0,v[0],sv[0])) return false;
         else if (!add_variable(v1,v[1],sv[1])) return false;
         else if (!add_variable(v2,v[2],sv[2])) return false;
         else if (!add_variable(v3,v[3],sv[3])) return false;
         else if (!add_variable(v3,v[4],sv[4])) return false;
         std::vector<std::pair<std::string,std::string> > var_transform_list;
         var_transform_list.push_back(std::make_pair(v0,sv[0]));
         var_transform_list.push_back(std::make_pair(v1,sv[1]));
         var_transform_list.push_back(std::make_pair(v2,sv[2]));
         var_transform_list.push_back(std::make_pair(v3,sv[3]));
         var_transform_list.push_back(std::make_pair(v4,sv[4]));
         if (compile_expression(name,expression,var_transform_list))
         {
            f5p_map_[name].setup(expr_map_[name],(*v[0]),(*v[1]),(*v[2]),(*v[3]),(*v[4]));
            return true;
         }
         else
         {
            remove(name,sv);
            return false;
         }
      }

      inline bool add(const std::string& name,
                      const std::string& expression,
                      const std::string& v0, const std::string& v1, const std::string& v2,
                      const std::string& v3, const std::string& v4, const std::string& v5)
      {
         const std::size_t n = 6;
         T* v[n] = { 0 };
         std::string sv[n];
         if (expr_map_.end() != expr_map_.find(name))
            return false;
         else if (!forward(name,6))
            return false;
         else if (!add_variable(v0,v[0],sv[0])) return false;
         else if (!add_variable(v1,v[1],sv[1])) return false;
         else if (!add_variable(v2,v[2],sv[2])) return false;
         else if (!add_variable(v3,v[3],sv[3])) return false;
         else if (!add_variable(v4,v[4],sv[4])) return false;
         else if (!add_variable(v5,v[5],sv[5])) return false;
         std::vector<std::pair<std::string,std::string> > var_transform_list;
         var_transform_list.push_back(std::make_pair(v0,sv[0]));
         var_transform_list.push_back(std::make_pair(v1,sv[1]));
         var_transform_list.push_back(std::make_pair(v2,sv[2]));
         var_transform_list.push_back(std::make_pair(v3,sv[3]));
         var_transform_list.push_back(std::make_pair(v4,sv[4]));
         var_transform_list.push_back(std::make_pair(v5,sv[5]));
         if (compile_expression(name,expression,var_transform_list))
         {
            f6p_map_[name].setup(expr_map_[name],(*v[0]),(*v[1]),(*v[2]),(*v[3]),(*v[4]),(*v[5]));
            return true;
         }
         else
         {
            remove(name,sv);
            return false;
         }
      }

   private:

      template <typename Allocator,
                template <typename,typename> class Sequence>
      bool compile_expression(const std::string& name,
                              const std::string& expression,
                              const Sequence<std::pair<std::string,std::string>,Allocator>& var_transform_list)
      {
         expression_t compiled_expression;
         compiled_expression.register_symbol_table(symbol_table_);

         for (std::size_t i = 0; i < var_transform_list.size(); ++i)
         {
            parser_.remove_replace_symbol(var_transform_list[i].first);
            if (!parser_.replace_symbol(var_transform_list[i].first,var_transform_list[i].second))
               return false;
         }

         if (!parser_.compile(expression,compiled_expression))
         {
            return false;
         }

         for (std::size_t i = 0; i < var_transform_list.size(); ++i)
         {
            parser_.remove_replace_symbol(var_transform_list[i].first);
         }

         expr_map_[name] = compiled_expression;
         return true;
      }

      bool add_variable(const std::string& v, T*& t, std::string& new_var)
      {
         static const unsigned int max_suffix_index = 1000000000;
         while (suffix_index_ < max_suffix_index)
         {
            new_var = generate_name(v);
            if (!symbol_used(new_var))
            {
               symbol_table_.create_variable(new_var,T(0));
               t = 0;
               t = &symbol_table_.get_variable(new_var)->ref();
               return (0 != t);
            }
            else
               ++suffix_index_;
         }
         return false;
      }

      std::string generate_name(const std::string v)
      {
         //eg: x --> function_compositor_1__x_123
         return std::string("function_compositor") + exprtk::details::to_str(id_) + "__" +
                v + "_" +
                exprtk::details::to_str(static_cast<int>(suffix_index_));
      }

      unsigned int get_id()
      {
         static unsigned int base_id = 1;
         return ++base_id;
      }

      inline bool symbol_used(const std::string& symbol)
      {
         return symbol_table_.is_variable       (symbol) ||
                symbol_table_.is_stringvar      (symbol) ||
                symbol_table_.is_function       (symbol) ||
                symbol_table_.is_vararg_function(symbol);
      }

      inline bool forward(const std::string& name, const std::size_t& arg_count)
      {
         if (symbol_used(name))
            return false;
         else
         {
            switch (arg_count)
            {
               case 0 : { if (f0p_map_.end() != f0p_map_.find(name)) return false; } break;
               case 1 : { if (f1p_map_.end() != f1p_map_.find(name)) return false; } break;
               case 2 : { if (f2p_map_.end() != f2p_map_.find(name)) return false; } break;
               case 3 : { if (f3p_map_.end() != f3p_map_.find(name)) return false; } break;
               case 4 : { if (f4p_map_.end() != f4p_map_.find(name)) return false; } break;
               case 5 : { if (f5p_map_.end() != f5p_map_.find(name)) return false; } break;
               case 6 : { if (f6p_map_.end() != f6p_map_.find(name)) return false; } break;
            }

            switch (arg_count)
            {
               case 0  : { f0p_map_[name] = func_0param(); return symbol_table_.add_function(name,f0p_map_[name]); }
               case 1  : { f1p_map_[name] = func_1param(); return symbol_table_.add_function(name,f1p_map_[name]); }
               case 2  : { f2p_map_[name] = func_2param(); return symbol_table_.add_function(name,f2p_map_[name]); }
               case 3  : { f3p_map_[name] = func_3param(); return symbol_table_.add_function(name,f3p_map_[name]); }
               case 4  : { f4p_map_[name] = func_4param(); return symbol_table_.add_function(name,f4p_map_[name]); }
               case 5  : { f5p_map_[name] = func_5param(); return symbol_table_.add_function(name,f5p_map_[name]); }
               case 6  : { f6p_map_[name] = func_6param(); return symbol_table_.add_function(name,f6p_map_[name]); }
               default : return false;
            }
         }
      }

      template <std::size_t N>
      inline void remove(const std::string& name, const std::string (&v)[N])
      {
         symbol_table_.remove_function(name);
         for (std::size_t i = 0; i < N; ++i)
         {
            symbol_table_.remove_variable(v[i]);
         }
         remove(name,N);
      }

      inline void remove(const std::string& name, const std::size_t& arg_count)
      {
         typename std::map<std::string,expression_t>::iterator em_itr = expr_map_.find(name);
         if (expr_map_.end() != em_itr)
         {
            expr_map_.erase(em_itr);
         }

         switch (arg_count)
         {
            case 0 : { if (f0p_map_.end() == f0p_map_.find(name)) return; }
            case 1 : { if (f1p_map_.end() == f1p_map_.find(name)) return; }
            case 2 : { if (f2p_map_.end() == f2p_map_.find(name)) return; }
            case 3 : { if (f3p_map_.end() == f3p_map_.find(name)) return; }
            case 4 : { if (f4p_map_.end() == f4p_map_.find(name)) return; }
            case 5 : { if (f5p_map_.end() == f5p_map_.find(name)) return; }
            case 6 : { if (f6p_map_.end() == f6p_map_.find(name)) return; }
         }

         switch (arg_count)
         {
            case 0 : { f0p_map_.erase(f0p_map_.find(name)); return; }
            case 1 : { f1p_map_.erase(f1p_map_.find(name)); return; }
            case 2 : { f2p_map_.erase(f2p_map_.find(name)); return; }
            case 3 : { f3p_map_.erase(f3p_map_.find(name)); return; }
            case 4 : { f4p_map_.erase(f4p_map_.find(name)); return; }
            case 5 : { f5p_map_.erase(f5p_map_.find(name)); return; }
            case 6 : { f6p_map_.erase(f6p_map_.find(name)); return; }
         }
      }

   private:

      symbol_table_t symbol_table_;
      parser_t parser_;
      std::map<std::string,expression_t> expr_map_;
      std::map<std::string,func_0param>  f0p_map_;
      std::map<std::string,func_1param>  f1p_map_;
      std::map<std::string,func_2param>  f2p_map_;
      std::map<std::string,func_3param>  f3p_map_;
      std::map<std::string,func_4param>  f4p_map_;
      std::map<std::string,func_5param>  f5p_map_;
      std::map<std::string,func_6param>  f6p_map_;
      unsigned int suffix_index_;
      unsigned int id_;
   };

   template <typename T>
   inline bool pgo_primer()
   {
      static const std::string expression_list[]
                                        = {
                                             "(y + x)",
                                             "2 * (y + x)",
                                             "(2 * y + 2 * x)",
                                             "(y + x / y) * (x - y / x)",
                                             "x / ((x + y) * (x - y)) / y",
                                             "1 - ((x * y) + (y / x)) - 3",
                                             "sin(2 * x) + cos(pi / y)",
                                             "1 - sin(2 * x) + cos(pi / y)",
                                             "sqrt(1 - sin(2 * x) + cos(pi / y) / 3)",
                                             "(x^2 / sin(2 * pi / y)) -x / 2",
                                             "x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y",
                                             "clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)",
                                             "iclamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)",
                                             "max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))",
                                             "if(avg(x,y) <= x + y, x - y, x * y) + 2 * pi / x",
                                             "1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^4 - 5.5x^5 + 6.6y^6 - 7.7x^27 + 8.8y^55",
                                             "(yy + xx)",
                                             "2 * (yy + xx)",
                                             "(2 * yy + 2 * xx)",
                                             "(yy + xx / yy) * (xx - yy / xx)",
                                             "xx / ((xx + yy) * (xx - yy)) / yy",
                                             "1 - ((xx * yy) + (yy / xx)) - 3",
                                             "sin(2 * xx) + cos(pi / yy)",
                                             "1 - sin(2 * xx) + cos(pi / yy)",
                                             "sqrt(1 - sin(2 * xx) + cos(pi / yy) / 3)",
                                             "(xx^2 / sin(2 * pi / yy)) -xx / 2",
                                             "xx + (cos(yy - sin(2 / xx * pi)) - sin(xx - cos(2 * yy / pi))) - yy",
                                             "clamp(-1.0, sin(2 * pi * xx) + cos(yy / 2 * pi), +1.0)",
                                             "max(3.33, min(sqrt(1 - sin(2 * xx) + cos(pi / yy) / 3), 1.11))",
                                             "if(avg(xx,yy) <= xx + yy, xx - yy, xx * yy) + 2 * pi / xx",
                                             "1.1xx^1 + 2.2yy^2 - 3.3xx^3 + 4.4yy^4 - 5.5xx^5 + 6.6yy^6 - 7.7xx^27 + 8.8yy^55",
                                             "(1.1*(2.2*(3.3*(4.4*(5.5*(6.6*(7.7*(8.8*(9.9+x)))))))))",
                                             "(((((((((x+9.9)*8.8)*7.7)*6.6)*5.5)*4.4)*3.3)*2.2)*1.1)",
                                             "(x + y) * z", "x + (y * z)", "(x + y) * 7", "x + (y * 7)",
                                             "(x + 7) * y", "x + (7 * y)", "(7 + x) * y", "7 + (x * y)",
                                             "(2 + x) * 3", "2 + (x * 3)", "(2 + 3) * x", "2 + (3 * x)",
                                             "(x + 2) * 3", "x + (2 * 3)",
                                             "(x + y) * (z / w)", "(x + y) * (z / 7)", "(x + y) * (7 / z)", "(x + 7) * (y / z)",
                                             "(7 + x) * (y / z)", "(2 + x) * (y / z)", "(x + 2) * (y / 3)", "(2 + x) * (y / 3)",
                                             "(x + 2) * (3 / y)", "x + (y * (z / w))", "x + (y * (z / 7))", "x + (y * (7 / z))",
                                             "x + (7 * (y / z))", "7 + (x * (y / z))", "2 + (x * (3 / y))", "x + (2 * (y / 4))",
                                             "2 + (x * (y / 3))", "x + (2 * (3 / y))",
                                             "x + ((y * z) / w)", "x + ((y * z) / 7)", "x + ((y * 7) / z)", "x + ((7 * y) / z)",
                                             "7 + ((y * z) / w)", "2 + ((x * 3) / y)", "x + ((2 * y) / 3)", "2 + ((x * y) / 3)",
                                             "x + ((2 * 3) / y)", "(((x + y) * z) / w)",
                                             "(((x + y) * z) / 7)", "(((x + y) * 7) / z)", "(((x + 7) * y) / z)", "(((7 + x) * y) / z)",
                                             "(((2 + x) * 3) / y)", "(((x + 2) * y) / 3)", "(((2 + x) * y) / 3)", "(((x + 2) * 3) / y)",
                                             "((x + (y * z)) / w)", "((x + (y * z)) / 7)", "((x + (y * 7)) / y)", "((x + (7 * y)) / z)",
                                             "((7 + (x * y)) / z)", "((2 + (x * 3)) / y)", "((x + (2 * y)) / 3)", "((2 + (x * y)) / 3)",
                                             "((x + (2 * 3)) / y)",
                                             "(xx + yy) * zz", "xx + (yy * zz)",
                                             "(xx + yy) * 7", "xx + (yy * 7)",
                                             "(xx + 7) * yy", "xx + (7 * yy)",
                                             "(7 + xx) * yy", "7 + (xx * yy)",
                                             "(2 + x) * 3", "2 + (x * 3)",
                                             "(2 + 3) * x", "2 + (3 * x)",
                                             "(x + 2) * 3", "x + (2 * 3)",
                                             "(xx + yy) * (zz / ww)", "(xx + yy) * (zz / 7)",
                                             "(xx + yy) * (7 / zz)", "(xx + 7) * (yy / zz)",
                                             "(7 + xx) * (yy / zz)", "(2 + xx) * (yy / zz)",
                                             "(xx + 2) * (yy / 3)", "(2 + xx) * (yy / 3)",
                                             "(xx + 2) * (3 / yy)", "xx + (yy * (zz / ww))",
                                             "xx + (yy * (zz / 7))", "xx + (yy * (7 / zz))",
                                             "xx + (7 * (yy / zz))", "7 + (xx * (yy / zz))",
                                             "2 + (xx * (3 / yy))", "xx + (2 * (yy / 4))",
                                             "2 + (xx * (yy / 3))", "xx + (2 * (3 / yy))",
                                             "xx + ((yy * zz) / ww)", "xx + ((yy * zz) / 7)",
                                             "xx + ((yy * 7) / zz)", "xx + ((7 * yy) / zz)",
                                             "7 + ((yy * zz) / ww)", "2 + ((xx * 3) / yy)",
                                             "xx + ((2 * yy) / 3)", "2 + ((xx * yy) / 3)",
                                             "xx + ((2 * 3) / yy)", "(((xx + yy) * zz) / ww)",
                                             "(((xx + yy) * zz) / 7)", "(((xx + yy) * 7) / zz)",
                                             "(((xx + 7) * yy) / zz)", "(((7 + xx) * yy) / zz)",
                                             "(((2 + xx) * 3) / yy)", "(((xx + 2) * yy) / 3)",
                                             "(((2 + xx) * yy) / 3)", "(((xx + 2) * 3) / yy)",
                                             "((xx + (yy * zz)) / ww)", "((xx + (yy * zz)) / 7)",
                                             "((xx + (yy * 7)) / yy)", "((xx + (7 * yy)) / zz)",
                                             "((7 + (xx * yy)) / zz)", "((2 + (xx * 3)) / yy)",
                                             "((xx + (2 * yy)) / 3)", "((2 + (xx * yy)) / 3)",
                                             "((xx + (2 * 3)) / yy)"
                                          };
      static const std::size_t expression_list_size = sizeof(expression_list) / sizeof(std::string);

      T  x = T(0);
      T  y = T(0);
      T  z = T(0);
      T  w = T(0);
      T xx = T(0);
      T yy = T(0);
      T zz = T(0);
      T ww = T(0);

      exprtk::symbol_table<T> symbol_table;
      symbol_table.add_constants();
      symbol_table.add_variable( "x", x);
      symbol_table.add_variable( "y", y);
      symbol_table.add_variable( "z", z);
      symbol_table.add_variable( "w", w);
      symbol_table.add_variable("xx",xx);
      symbol_table.add_variable("yy",yy);
      symbol_table.add_variable("zz",zz);
      symbol_table.add_variable("ww",ww);

      typedef typename std::deque<exprtk::expression<T> > expr_list_t;
      expr_list_t expr_list;

      const std::size_t rounds = 50;

      {
         for (std::size_t r = 0; r < rounds; ++r)
         {
            expr_list.clear();
            exprtk::parser<T> parser;
            for (std::size_t i = 0; i < expression_list_size; ++i)
            {
               exprtk::expression<T> expression;
               expression.register_symbol_table(symbol_table);
               if (!parser.compile(expression_list[i],expression))
               {
                  return false;
               }
               expr_list.push_back(expression);
            }
         }
      }

      struct execute
      {
         static inline T process(T& x, T& y, expression<T>& expression)
         {
            static const T lower_bound = T(-20);
            static const T upper_bound = T(+20);
            T delta = T(0.1);
            T total = T(0);
            for (x = lower_bound; x <= upper_bound; x += delta)
            {
               for (y = lower_bound; y <= upper_bound; y += delta)
               {
                  total += expression.value();
               }
            }
            return total;
         }
      };

      for (std::size_t i = 0; i < expr_list.size(); ++i)
      {
         execute::process( x, y,  expr_list[i]);
         execute::process(xx,yy,  expr_list[i]);
      }

      {
         for (std::size_t i = 0; i < 10000; ++i)
         {
            T v = T(123.456 + i);
                 if (details::numeric::nequal(details::numeric::fast_exp<T, 1>::result(v),std::pow(v,T( 1)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T, 2>::result(v),std::pow(v,T( 2)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T, 3>::result(v),std::pow(v,T( 3)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T, 4>::result(v),std::pow(v,T( 4)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T, 5>::result(v),std::pow(v,T( 5)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T, 6>::result(v),std::pow(v,T( 6)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T, 7>::result(v),std::pow(v,T( 7)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T, 8>::result(v),std::pow(v,T( 8)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T, 9>::result(v),std::pow(v,T( 9)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,10>::result(v),std::pow(v,T(10)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,11>::result(v),std::pow(v,T(11)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,12>::result(v),std::pow(v,T(12)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,13>::result(v),std::pow(v,T(13)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,14>::result(v),std::pow(v,T(14)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,15>::result(v),std::pow(v,T(15)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,16>::result(v),std::pow(v,T(16)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,17>::result(v),std::pow(v,T(17)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,18>::result(v),std::pow(v,T(18)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,19>::result(v),std::pow(v,T(19)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,20>::result(v),std::pow(v,T(20)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,21>::result(v),std::pow(v,T(21)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,22>::result(v),std::pow(v,T(22)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,23>::result(v),std::pow(v,T(23)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,24>::result(v),std::pow(v,T(24)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,25>::result(v),std::pow(v,T(25)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,26>::result(v),std::pow(v,T(26)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,27>::result(v),std::pow(v,T(27)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,28>::result(v),std::pow(v,T(28)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,29>::result(v),std::pow(v,T(29)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,30>::result(v),std::pow(v,T(30)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,31>::result(v),std::pow(v,T(31)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,32>::result(v),std::pow(v,T(32)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,33>::result(v),std::pow(v,T(33)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,34>::result(v),std::pow(v,T(34)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,35>::result(v),std::pow(v,T(35)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,36>::result(v),std::pow(v,T(36)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,37>::result(v),std::pow(v,T(37)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,38>::result(v),std::pow(v,T(38)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,39>::result(v),std::pow(v,T(39)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,40>::result(v),std::pow(v,T(40)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,41>::result(v),std::pow(v,T(41)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,42>::result(v),std::pow(v,T(42)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,43>::result(v),std::pow(v,T(43)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,44>::result(v),std::pow(v,T(44)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,45>::result(v),std::pow(v,T(45)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,46>::result(v),std::pow(v,T(46)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,47>::result(v),std::pow(v,T(47)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,48>::result(v),std::pow(v,T(48)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,49>::result(v),std::pow(v,T(49)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,50>::result(v),std::pow(v,T(50)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,51>::result(v),std::pow(v,T(51)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,52>::result(v),std::pow(v,T(52)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,53>::result(v),std::pow(v,T(53)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,54>::result(v),std::pow(v,T(54)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,55>::result(v),std::pow(v,T(55)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,56>::result(v),std::pow(v,T(56)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,57>::result(v),std::pow(v,T(57)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,58>::result(v),std::pow(v,T(58)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,59>::result(v),std::pow(v,T(59)))) return false;
            else if (details::numeric::nequal(details::numeric::fast_exp<T,60>::result(v),std::pow(v,T(60)))) return false;
            else
               return true;
         }
      }
      return true;
   }

}

#ifdef WIN32
#   ifndef NOMINMAX
#      define NOMINMAX
#   endif
#   ifndef WIN32_LEAN_AND_MEAN
#      define WIN32_LEAN_AND_MEAN
#   endif
#   include <windows.h>
#else
#   include <sys/time.h>
#   include <sys/types.h>
#endif

namespace exprtk
{

   class timer
   {
   public:

      #ifdef WIN32
         timer()
         : in_use_(false)
         {
            QueryPerformanceFrequency(&clock_frequency_);
         }

         inline void start()
         {
            in_use_ = true;
            QueryPerformanceCounter(&start_time_);
         }

         inline void stop()
         {
            QueryPerformanceCounter(&stop_time_);
            in_use_ = false;
         }

         inline double time() const
         {
            return (1.0 * (stop_time_.QuadPart - start_time_.QuadPart)) / (1.0 * clock_frequency_.QuadPart);
         }

      #else

         timer()
         : in_use_(false)
         {
            start_time_.tv_sec  = 0;
            start_time_.tv_usec = 0;
            stop_time_.tv_sec   = 0;
            stop_time_.tv_usec  = 0;
         }

         inline void start()
         {
            in_use_ = true;
            gettimeofday(&start_time_,0);
         }

         inline void stop()
         {
            gettimeofday(&stop_time_, 0);
            in_use_ = false;
         }

         inline unsigned long long int usec_time() const
         {
            if (!in_use_)
            {
               if (stop_time_.tv_sec >= start_time_.tv_sec)
               {
                  return 1000000 * (stop_time_.tv_sec  - start_time_.tv_sec ) +
                                   (stop_time_.tv_usec - start_time_.tv_usec);
               }
               else
                  return std::numeric_limits<unsigned long long int>::max();
            }
            else
               return std::numeric_limits<unsigned long long int>::max();
         }

         inline double time() const
         {
            return usec_time() * 0.000001;
         }

      #endif

         inline bool in_use() const
         {
            return in_use_;
         }

   private:

         bool in_use_;

      #ifdef WIN32
         LARGE_INTEGER start_time_;
         LARGE_INTEGER stop_time_;
         LARGE_INTEGER clock_frequency_;
      #else
         struct timeval start_time_;
         struct timeval stop_time_;
      #endif
   };

   namespace information
   {
      static const char* library = "Mathematical Expression Toolkit";
      static const char* version = "2.718281828459045235360287471352662497";
      static const char* date    = "20140104";

      static inline std::string data()
      {
         static const std::string info_str = std::string(library) +
                                             std::string(" v") + std::string(version) +
                                             std::string(" (") + date + std::string(")");
         return info_str;
      }

   } // namespace information

} // namespace exprtk

#endif
