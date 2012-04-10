/*
 ****************************************************************
 *          C++ Mathematical Expression Toolkit Library         *
 *                                                              *
 * Author: Arash Partow (1999-2012)                             *
 * URL: http://www.partow.net/programming/exprtk/index.html     *
 *                                                              *
 * Copyright notice:                                            *
 * Free use of the C++ Mathematical Expression Toolkit Library  *
 * is permitted under the guidelines and in accordance with the *
 * most current version of the Common Public License.           *
 * http://www.opensource.org/licenses/cpl1.0.php                *
 *                                                              *
 * Example expressions:                                         *
 * (00) (y+x/y)*(x-y/x)                                         *
 * (01) (x^2/sin(2*pi/y))-x/2                                   *
 * (02) sqrt(1-(x^2))                                           *
 * (03) 1-sin(2*x)+cos(pi/y)                                    *
 * (04) a*exp(2*t)+c                                            *
 * (05) if(((x+2)==3)and((y+5)<=9),1+w,2/z)                     *
 * (06) if(avg(x,y)<=x+y,x-y,x*y)+2*pi/x                        *
 * (07) z:=x+sin(2*pi/y)                                        *
 * (08) u<-2*(pi*z)/(w:=x+cos(y/pi))                            *
 * (09) clamp(-1,sin(2*pi*x)+cos(y/2*pi),+1)                    *
 * (10) inrange(-2,m,+2)==if(({-2<=m}and[m<=+2]),1,0)           *
 * (11) (12.34sin(x)cos(2y)7+1)==(12.34*sin(x)*cos(2*y)*7+1)    *
 *                                                              *
 ****************************************************************
*/


#ifndef INCLUDE_EXPRTK_HPP
#define INCLUDE_EXPRTK_HPP

#include <cctype>
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <limits>
#include <deque>
#include <list>
#include <map>
#include <stack>


namespace exprtk
{
   namespace details
   {
      inline bool is_whitespace(const char& c)
      {
         return (' '  == c) ||
                ('\n' == c) ||
                ('\r' == c) ||
                ('\t' == c) ||
                ('\b' == c);
      }

      inline bool is_operator_char(const char& c)
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
                ('?' == c);
      }

      inline bool is_letter(const char c)
      {
         return (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z'));
      }

      inline bool is_digit(const char c)
      {
         return ('0' <= c) && (c <= '9');
      }

      inline bool is_left_bracket(const char c)
      {
         return ('(' == c) || ('[' == c) || ('{' == c);
      }

      inline bool is_right_bracket(const char c)
      {
         return (')' == c) || (']' == c) || ('}' == c);
      }

      inline bool is_sign(const char c)
      {
         return ('+' == c) || ('-' == c);
      }

      inline bool is_invalid(const char& c)
      {
         return !is_whitespace(c)    &&
                !is_operator_char(c) &&
                !is_letter(c)        &&
                !is_digit(c)         &&
                ('.' != c)           &&
                ('_' != c)           &&
                ('$' != c)           &&
                ('\'' != c);
      }

      inline bool imatch(const char& c1, const char& c2)
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

      static const std::string reserved_words[] =
                                  {
                                     "and", "for", "if", "ilike", "in", "like", "nand", "nor", "not", "or", "while", "xor"
                                  };
      static const std::size_t reserved_words_size = sizeof(reserved_words) / sizeof(std::string);

      static const std::string reserved_symbols[] =
                                  {
                                     "abs", "acos", "and", "asin", "atan", "atan2", "avg", "ceil", "clamp",
                                     "cos", "cosh", "cot", "csc", "deg2grad", "deg2rad", "equal", "exp",
                                     "floor", "for", "grad2deg", "hyp", "if", "ilike", "in", "inrange",
                                     "like", "log", "log10", "logn", "max", "min", "mod", "mul", "nand",
                                     "nor", "not", "not_equal", "or", "rad2deg", "root", "round", "roundn",
                                     "sec", "shl", "shr", "sin", "sinh", "sqrt", "sum", "tan", "tanh",
                                     "while", "xor"
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
         }

         namespace details
         {
            struct unknown_type_tag {};
            struct real_type_tag    {};
            struct int_type_tag     {};

            template <typename T>
            struct number_type { typedef unknown_type_tag type; };

            #define exprtk_register_real_type_tag(T)\
            template<> struct number_type<T> { typedef real_type_tag type; };

            #define exprtk_register_int_type_tag(T)\
            template<> struct number_type<T> { typedef int_type_tag type; };

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
            inline T equal_impl(const T& v0, const T& v1, real_type_tag)
            {
               static const T epsilon = T(0.0000000001);
               return (std::abs(v0 - v1) <= (std::max(T(1),std::max(std::abs(v0),std::abs(v1))) * epsilon)) ? T(1) : T(0);
            }

            template <typename T>
            inline T equal_impl(const T& v0, const T& v1, int_type_tag)
            {
               return (v0 == v1) ? 1 : 0;
            }

            template <typename T>
            inline T nequal_impl(const T& v0, const T& v1, real_type_tag)
            {
               static const T epsilon = T(0.0000000001);
               return (std::abs(v0 - v1) > (std::max(T(1),std::max(std::abs(v0),std::abs(v1))) * epsilon)) ? T(1) : T(0);
            }

            template <typename T>
            inline T nequal_impl(const T& v0, const T& v1, int_type_tag)
            {
               return (v0 != v1) ? 1 : 0;
            }

            template <typename T>
            inline T modulus_impl(const T& v0, const T& v1, real_type_tag)
            {
               return std::fmod(v0,v1);
            }

            template <typename T>
            inline T modulus_impl(const T& v0, const T& v1, int_type_tag)
            {
               return v0 % v1;
            }

            template <typename T>
            inline T pow_impl(const T& v0, const T& v1, real_type_tag)
            {
               return std::pow(v0,v1);
            }

            template <typename T>
            inline T pow_impl(const T& v0, const T& v1, int_type_tag)
            {
               return std::pow(static_cast<double>(v0),static_cast<double>(v1));
            }

            template <typename T>
            inline T logn_impl(const T& v0, const T& v1, real_type_tag)
            {
               return std::log(v0) / std::log(v1);
            }

            template <typename T>
            inline T logn_impl(const T& v0, const T& v1, int_type_tag)
            {
               return static_cast<T>(logn_impl<double>(static_cast<double>(v0),static_cast<double>(v1),real_type_tag()));
            }

            template <typename T>
            inline T root_impl(const T& v0, const T& v1, real_type_tag)
            {
               return std::pow(v0,T(1)/v1);
            }

            template <typename T>
            inline T root_impl(const T& v0, const T& v1, int_type_tag)
            {
               return root_impl<double>(static_cast<double>(v0),static_cast<double>(v1),real_type_tag());
            }

            template <typename T>
            inline T roundn_impl(const T& v0, const T& v1, real_type_tag)
            {
               return std::floor((v0 * pow10[(int)std::floor(v1)]) + T(0.5)) / T(pow10[(int)std::floor(v1)]);
            }

            template <typename T>
            inline T roundn_impl(const T& v0, const T&, int_type_tag)
            {
               return v0;
            }

            template <typename T>
            inline T hyp_impl(const T& v0, const T& v1, real_type_tag)
            {
               return std::sqrt((v0 * v0) + (v1 * v1));
            }

            template <typename T>
            inline T hyp_impl(const T& v0, const T& v1, int_type_tag)
            {
               return static_cast<T>(std::sqrt(static_cast<double>((v0 * v0) + (v1 * v1))));
            }

            template <typename T>
            inline T atan2_impl(const T& v0, const T& v1, real_type_tag)
            {
               return std::atan2(v0,v1);
            }

            template <typename T>
            inline T atan2_impl(const T&, const T&, int_type_tag)
            {
               return 0;
            }

            template <typename T>
            inline T shr_impl(const T& v0, const T& v1, real_type_tag)
            {
               return v0 * (T(1) / std::pow(T(2),static_cast<double>(static_cast<int>(v1))));
            }

            template <typename T>
            inline T shr_impl(const T& v0, const T& v1, int_type_tag)
            {
               return v0 >> v1;
            }

            template <typename T>
            inline T shl_impl(const T& v0, const T& v1, real_type_tag)
            {
               return v0 * std::pow(T(2),static_cast<double>(static_cast<int>(v1)));
            }

            template <typename T>
            inline T shl_impl(const T& v0, const T& v1, int_type_tag)
            {
               return v0 << v1;
            }

            template <typename T>
            inline T xor_impl(const T& v0, const T& v1, real_type_tag)
            {
               return v0 != v1;
            }

            template <typename T>
            inline T xor_impl(const T& v0, const T& v1, int_type_tag)
            {
               return v0 ^ v1;
            }

         }

         template <typename Type>
         struct numeric_info { enum { length = 0, size = 32, bound_length = 0, min_exp = 0, max_exp = 0 }; };

         template<> struct numeric_info<int>         { enum { length = 10, size = 16, bound_length =  9}; };
         template<> struct numeric_info<float>       { enum { min_exp =  -38, max_exp =  +38}; };
         template<> struct numeric_info<double>      { enum { min_exp = -308, max_exp = +308}; };
         template<> struct numeric_info<long double> { enum { min_exp = -308, max_exp = +308}; };

         template <typename T>
         inline T equal(const T& v0, const T& v1)
         {
            typename details::number_type<T>::type num_type;
            return details::equal_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T nequal(const T& v0, const T& v1)
         {
            typename details::number_type<T>::type num_type;
            return details::nequal_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T modulus(const T& v0, const T& v1)
         {
            typename details::number_type<T>::type num_type;
            return details::modulus_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T pow(const T& v0, const T& v1)
         {
            typename details::number_type<T>::type num_type;
            return details::pow_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T logn(const T& v0, const T& v1)
         {
            typename details::number_type<T>::type num_type;
            return details::logn_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T root(const T& v0, const T& v1)
         {
            typename details::number_type<T>::type num_type;
            return details::root_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T roundn(const T& v0, const T& v1)
         {
            typename details::number_type<T>::type num_type;
            return details::roundn_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T hyp(const T& v0, const T& v1)
         {
            typename details::number_type<T>::type num_type;
            return details::hyp_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T atan2(const T& v0, const T& v1)
         {
            typename details::number_type<T>::type num_type;
            return details::atan2_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T shr(const T& v0, const T& v1)
         {
            typename details::number_type<T>::type num_type;
            return details::shr_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T shl(const T& v0, const T& v1)
         {
            typename details::number_type<T>::type num_type;
            return details::shl_impl(v0,v1,num_type);
         }

         template <typename T>
         inline T xor_opr(const T& v0, const T& v1)
         {
            typename details::number_type<T>::type num_type;
            return details::xor_impl(v0,v1,num_type);
         }

      }

      template <typename T>
      struct token
      {

         enum token_type
         {
            none        =   0,
            error       =   1,
            eof         =   2,
            number      =   3,
            symbol      =   4,
            string      =   5,
            assign      =   6,
            shr         =   7,
            shl         =   8,
            lte         =   9,
            ne          =  10,
            gte         =  11,
            lt          = '<',
            gt          = '>',
            eq          = '=',
            rbracket    = ')',
            lbracket    = '(',
            rsqrbracket = ']',
            lsqrbracket = '[',
            rcrlbracket = '}',
            lcrlbracket = '{',
            comma       = ',',
            add         = '+',
            sub         = '-',
            div         = '/',
            mul         = '*',
            mod         = '%',
            pow         = '^'
         };

         token() {}

         explicit token(token_type ttype)
         : type(ttype)
         {}

         token(token_type ttype,
               const char* begin, const char* end)
         : type(ttype),
           value(std::string(begin,end))
         {}

         token(token_type ttype, const std::string& v)
         : type(ttype),
           value(v)
         {}

         token(token_type ttype, const T& num_val)
         : type(ttype),
           numeric_value(num_val)
         {}

         token_type type;
         std::string value;
         T numeric_value;
      };

      template <typename T>
      class lexer
      {
      public:

         typedef token<T> token_t;

         inline bool process(const std::string& str)
         {
            error_description_ = "";
            s_itr = str.data();
            s_end = str.data() + str.size();
            eof_token_ = token_t(token_t::eof,s_end,s_end);
            token_list_.clear();
            while (s_end != s_itr)
            {
               scan_token();
               if (!error_description_.empty())
               {
                  return false;
               }
            }
            process_commutative_symbols();
            token_itr_ = token_list_.begin();
            store_token_itr_ = token_list_.begin();
            return true;
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

         inline std::string error() const
         {
            return error_description_;
         }

      private:

         inline void skip_whitespace()
         {
            while ((s_end != s_itr) && is_whitespace(*s_itr))
            {
               ++s_itr;
            }
         }

         inline void scan_token()
         {
            skip_whitespace();

            if (s_end == s_itr)
            {
               return;
            }
            else if (is_operator_char(*s_itr))
            {
               scan_operator();
               return;
            }
            else if (is_letter(*s_itr))
            {
               scan_symbol();
               return;
            }
            else if (is_digit((*s_itr)) || ('.' == (*s_itr)))
            {
               scan_number();
               return;
            }
            else if ('$' == (*s_itr))
            {
               scan_special_function();
               return;
            }
            else if ('\'' == (*s_itr))
            {
               scan_string();
               return;
            }
            else
            {
               set_error(std::string("scan_token() - error invalid token: ") + std::string(s_itr,s_itr + 2));
               token_list_.push_back(error(s_itr,s_itr + 1));
               ++s_itr;
            }
         }

         inline void scan_operator()
         {
            if ((s_itr + 1) != s_end)
            {
               typename token_t::token_type ttype = token_t::none;
               char c0 = s_itr[0];
               char c1 = s_itr[1];
                    if ((c0 == '<') && (c1 == '=')) ttype = token_t::lte;
               else if ((c0 == '>') && (c1 == '=')) ttype = token_t::gte;
               else if ((c0 == '<') && (c1 == '>')) ttype = token_t::ne;
               else if ((c0 == '!') && (c1 == '=')) ttype = token_t::ne;
               else if ((c0 == '=') && (c1 == '=')) ttype = token_t::eq;
               else if ((c0 == ':') && (c1 == '=')) ttype = token_t::assign;
               else if ((c0 == '<') && (c1 == '-')) ttype = token_t::assign;
               else if ((c0 == '<') && (c1 == '<')) ttype = token_t::shl;
               else if ((c0 == '>') && (c1 == '>')) ttype = token_t::shr;
               if (token_t::none != ttype)
               {
                  token_list_.push_back(token_t(ttype));
                  s_itr += 2;
                  return;
               }
            }
                 if ('<' == *s_itr) token_list_.push_back(token_t(token_t::lt));
            else if ('>' == *s_itr) token_list_.push_back(token_t(token_t::gt));
            else
               token_list_.push_back(token_t(typename token_t::token_type((*s_itr))));
            ++s_itr;
         }

         inline void scan_symbol()
         {
            const char* begin = s_itr;
            while ((s_end != s_itr) &&
                   (is_letter((*s_itr)) || is_digit ((*s_itr)) || ((*s_itr) == '_')))
            {
               ++s_itr;
            }
            token_list_.push_back(token_t(token_t::symbol,begin,s_itr));
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
            const char* begin = s_itr;
            bool dot_found = false;
            bool e_found = false;
            bool post_e_sign_found = false;
            while (s_end != s_itr)
            {
              if ('.' == (*s_itr))
              {
                 if (dot_found)
                 {
                    set_error(std::string("scan_number() - error invalid numeric token[1]: ") + std::string(begin,s_itr));
                    token_list_.push_back(error(begin,s_itr));
                    return;
                 }
                 dot_found = true;
                 ++s_itr;
                 continue;
              }
              else if (imatch('e',(*s_itr)))
              {
                 const char& c = *(s_itr + 1);
                 if (s_end == (s_itr + 1))
                 {
                    set_error(std::string("scan_number() - error invalid numeric token[2]: ") + std::string(begin,s_itr));
                    token_list_.push_back(error(begin,s_itr));
                    return;
                 }
                 else if (('+' != c) && ('-' != c) && !is_digit(c))
                 {
                    set_error(std::string("scan_number() - error invalid numeric token[3]: ") + std::string(begin,s_itr));
                    token_list_.push_back(error(begin,s_itr));
                    return;
                 }
                 e_found = true;
                 ++s_itr;
                 continue;
              }
              else if (e_found && is_sign(*s_itr))
              {
                 if (post_e_sign_found)
                 {
                    set_error(std::string("scan_number() - error invalid numeric token[4]: ") + std::string(begin,s_itr));
                    token_list_.push_back(error(begin,s_itr));
                    return;
                 }
                 post_e_sign_found = true;
                 ++s_itr;
                 continue;
              }
              else if (('.' != (*s_itr)) && !is_digit(*s_itr))
                  break;
               else
                  ++s_itr;
            }
            T value = T(0.0);
            if (string_to_real(begin,s_itr,value))
               token_list_.push_back(token_t(token_t::number,value));
            else
            {
               set_error(std::string("scan_number() - error failed to parse token to real type. ") + std::string(begin,s_itr));
               token_list_.push_back(error(begin,s_itr));
            }
            return;
         }

         inline void scan_special_function()
         {
            const char* begin = s_itr;
            //$fdd(x,x,x) = 11 chars
            if (std::distance(s_itr,s_end) < 11)
            {
               set_error(std::string("scan_special_function() - error invalid special function [1]: ") + std::string(begin,s_itr));
               token_list_.push_back(error(begin,s_itr));
               return;
            }

            if (!(('$' == *s_itr)            &&
                  (imatch('f',*(s_itr + 1))) &&
                  ('(' == *(s_itr + 4))      &&
                  (is_digit(*(s_itr + 2)))   &&
                  (is_digit(*(s_itr + 3)))))
            {
               set_error(std::string("scan_special_function() - error invalid special function [2]: ") + std::string(begin,s_itr));
               token_list_.push_back(error(begin,s_itr));
               return;
            }
            s_itr += 4;
            token_list_.push_back(token_t(token_t::symbol,begin,s_itr));
            return;
         }

         inline void scan_string()
         {
            const char* begin = s_itr + 1;
            if (std::distance(s_itr,s_end) < 2)
            {
               set_error(std::string("scan_string() - error invalid string [1]: ") + std::string(begin,s_itr));
               token_list_.push_back(error(begin,s_itr));
               return;
            }
            ++s_itr;
            bool escaped = false;
            std::string result_string;
            while (s_end != s_itr)
            {
               if ('\\' == *s_itr)
               {
                  escaped = true;
                  ++s_itr;
                  continue;
               }
               else if (!escaped)
               {
                  if ('\'' == *s_itr)
                     break;
               }
               else if (escaped)
                  escaped = false;
               result_string += *s_itr;
               ++s_itr;
            }
            if (s_end == s_itr)
            {
               set_error(std::string("scan_string() - error string has not been terminated: ") + std::string(begin,s_itr));
               token_list_.push_back(error(begin,s_itr));
               return;
            }
            token_list_.push_back(token_t(token_t::string,result_string));
            ++s_itr;
            return;
         }

         inline void process_commutative_symbols()
         {
            if (token_list_.size() < 2)
               return;
            typename std::deque<token_t>::iterator itr = token_list_.begin() + 1;
            typename std::deque<token_t>::iterator prev_itr = token_list_.begin();
            while (token_list_.end() != itr)
            {
               token_t& curr_token = *itr;
               token_t& prev_token = *prev_itr;
               bool curr_token_not_reserved = !is_reserved_word(curr_token.value);
               if (
                    //3x -> 3*x
                    ((token_t::symbol == curr_token.type) && (token_t::number == prev_token.type) && curr_token_not_reserved) ||
                    //3(x+1) -> 3*(x+1)
                    (is_left_bracket(curr_token.type)     && (token_t::number == prev_token.type)) ||
                    //(x+1)3 -> (x+1)*3
                    (is_right_bracket(prev_token.type)    && (token_t::number == curr_token.type)) ||
                    //(x+1)y -> (x+1)*y
                    (is_right_bracket(prev_token.type)    && (token_t::symbol == curr_token.type) && curr_token_not_reserved)
                  )
               {
                  prev_itr = itr = token_list_.insert(itr,token_t(token_t::mul));
                  ++itr;
                  continue;
               }
               ++itr;
               ++prev_itr;
            }
         }

         inline void set_error(const std::string& s)
         {
            if (error_description_.empty())
            {
               error_description_ = s;
            }
         }

         inline token_t error(const char* begin, const char* end) const
         {
            return token_t(token_t::error,begin,end);
         }

      private:

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

         template <typename Iterator>
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

         template <typename Iterator>
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

         template <typename Iterator>
         inline bool string_to_real(Iterator& itr_external, const Iterator end, T& t)
         {
            if (end == itr_external) return false;
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

         std::string error_description_;
         std::deque<token_t> token_list_;
         typename std::deque<token_t>::iterator token_itr_;
         typename std::deque<token_t>::iterator store_token_itr_;
         token_t eof_token_;
         const char* s_itr;
         const char* s_end;
      };

      enum operator_type
      {
         e_default,
         e_add    ,
         e_sub    ,
         e_mul    ,
         e_div    ,
         e_mod    ,
         e_pow    ,
         e_atan2  ,
         e_min    ,
         e_max    ,
         e_avg    ,
         e_sum    ,
         e_prod   ,
         e_lt     ,
         e_lte    ,
         e_eq     ,
         e_equal  ,
         e_ne     ,
         e_nequal ,
         e_gte    ,
         e_gt     ,
         e_and    ,
         e_nand   ,
         e_or     ,
         e_nor    ,
         e_xor    ,
         e_shr    ,
         e_shl    ,
         e_abs    ,
         e_acos   ,
         e_asin   ,
         e_atan   ,
         e_ceil   ,
         e_cos    ,
         e_cosh   ,
         e_exp    ,
         e_floor  ,
         e_log    ,
         e_log10  ,
         e_logn   ,
         e_neg    ,
         e_pos    ,
         e_round  ,
         e_roundn ,
         e_root   ,
         e_sqrt   ,
         e_sin    ,
         e_sinh   ,
         e_sec    ,
         e_csc    ,
         e_tan    ,
         e_tanh   ,
         e_cot    ,
         e_clamp  ,
         e_inrange,
         e_r2d    ,
         e_d2r    ,
         e_d2g    ,
         e_g2d    ,
         e_hyp    ,
         e_not    ,
         e_assign ,
         e_in     ,
         e_like   ,
         e_ilike  ,

         // Do not add new functions/operators after this point.
         e_sf00 = 1000,
         e_sf01 = 1001,
         e_sf02 = 1002,
         e_sf03 = 1003,
         e_sf04 = 1004,
         e_sf05 = 1005,
         e_sf06 = 1006,
         e_sf07 = 1007,
         e_sf08 = 1008,
         e_sf09 = 1009,
         e_sf10 = 1010,
         e_sf11 = 1011,
         e_sf12 = 1012,
         e_sf13 = 1013,
         e_sf14 = 1014,
         e_sf15 = 1015,
         e_sf16 = 1016,
         e_sf17 = 1017,
         e_sf18 = 1018,
         e_sf19 = 1019,
         e_sf20 = 1020,
         e_sf21 = 1021,
         e_sf22 = 1022,
         e_sf23 = 1023,
         e_sf24 = 1024,
         e_sf25 = 1025,
         e_sf26 = 1026,
         e_sf27 = 1027,
         e_sf28 = 1028,
         e_sf29 = 1029,
         e_sf30 = 1030,
         e_sf31 = 1031,
         e_sf32 = 1032,
         e_sf33 = 1033,
         e_sf34 = 1034,
         e_sf35 = 1035,
         e_sf36 = 1036,
         e_sf37 = 1037,
         e_sf38 = 1038,
         e_sf39 = 1039,
         e_sf40 = 1040,
         e_sf41 = 1041,
         e_sf42 = 1042
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
                  case e_abs   : return std::abs  (arg);
                  case e_acos  : return std::acos (arg);
                  case e_asin  : return std::asin (arg);
                  case e_atan  : return std::atan (arg);
                  case e_ceil  : return std::ceil (arg);
                  case e_cos   : return std::cos  (arg);
                  case e_cosh  : return std::cosh (arg);
                  case e_exp   : return std::exp  (arg);
                  case e_floor : return std::floor(arg);
                  case e_log   : return std::log  (arg);
                  case e_log10 : return std::log10(arg);
                  case e_neg   : return -arg;
                  case e_pos   : return +arg;
                  case e_round : return std::floor(arg + T(0.5));
                  case e_sin   : return std::sin  (arg);
                  case e_sinh  : return std::sinh (arg);
                  case e_sqrt  : return std::sqrt (arg);
                  case e_tan   : return std::tan  (arg);
                  case e_tanh  : return std::tanh (arg);
                  case e_cot   : return T(1) / std::tan(arg);
                  case e_sec   : return T(1) / std::cos(arg);
                  case e_csc   : return T(1) / std::sin(arg);
                  case e_r2d   : return (arg * T(numeric::constant::_180_pi));
                  case e_d2r   : return (arg * T(numeric::constant::pi_180));
                  case e_d2g   : return (arg * T(20/9));
                  case e_g2d   : return (arg * T(9/20));
                  case e_not   : return (arg != T(0) ? T(0) : T(1));
                  default      : return std::numeric_limits<T>::quiet_NaN();
               }
            }

            template <typename T>
            inline T process_impl(const operator_type operation, const T& arg, int_type_tag)
            {
               switch (operation)
               {
                  case e_abs   : return std::abs  (arg);
                  case e_exp   : return std::exp  (arg);
                  case e_log   : return std::log  (arg);
                  case e_log10 : return std::log10(arg);
                  case e_neg   : return -arg;
                  case e_pos   : return +arg;
                  case e_sqrt  : return std::sqrt (arg);
                  case e_not   : return !arg;
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
                  case e_and    : return ((arg0 != T(0)) && (arg1 != T(0))) ? T(1) : T(0);
                  case e_nand   : return ((arg0 != T(0)) && (arg1 != T(0))) ? T(0) : T(1);
                  case e_or     : return ((arg0 != T(0)) || (arg1 != T(0))) ? T(1) : T(0);
                  case e_nor    : return ((arg0 != T(0)) || (arg1 != T(0))) ? T(0) : T(1);
                  case e_xor    : return (arg0 != arg1) ? T(1) : T(0);
                  case e_root   : return root<T>(arg0,arg1);
                  case e_roundn : return roundn<T>(arg0,arg1);
                  case e_equal  : return equal<T>(arg0,arg1);
                  case e_nequal : return nequal<T>(arg0,arg1);
                  case e_hyp    : return hyp<T>(arg0,arg1);
                  case e_avg    : return (arg0 + arg1)/T(2);
                  case e_sum    : return (arg0 + arg1);
                  case e_prod   : return (arg0 * arg1);
                  case e_shr    : return shr<T>(arg0,arg1);
                  case e_shl    : return shl<T>(arg0,arg1);
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
                  case e_root   : return root<T>(arg0,arg1);
                  case e_equal  : return arg0 == arg1;
                  case e_nequal : return arg0 != arg1;
                  case e_hyp    : return hyp<T>(arg0,arg1);
                  case e_avg    : return (arg0 + arg1) >> 1;
                  case e_sum    : return (arg0 + arg1);
                  case e_prod   : return (arg0 * arg1);
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
            e_none        ,
            e_constant    ,
            e_unary       ,
            e_binary      ,
            e_trinary     ,
            e_quaternary  ,
            e_quinary     ,
            e_senary      ,
            e_conditional ,
            e_while       ,
            e_variable    ,
            e_stringvar   ,
            e_stringconst ,
            e_function    ,
            e_add         ,
            e_sub         ,
            e_mul         ,
            e_div         ,
            e_mod         ,
            e_pow         ,
            e_lt          ,
            e_lte         ,
            e_gt          ,
            e_gte         ,
            e_eq          ,
            e_ne          ,
            e_and         ,
            e_nand        ,
            e_or          ,
            e_nor         ,
            e_xor         ,
            e_in          ,
            e_like        ,
            e_ilike       ,
            e_vov
         };

         typedef T value_type;
         typedef expression_node<T>* expression_ptr;

         virtual ~expression_node()
         {}

         virtual inline T value() const
         {
            return std::numeric_limits<T>::quiet_NaN();
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
      inline bool is_function(const expression_node<T>* node)
      {
         return (details::expression_node<T>::e_function == node->type());
      }

      template <typename T>
      inline bool branch_deletable(expression_node<T>* node)
      {
         return !is_variable_node(node);
      }

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

         inline expression_node<T>* branch(const std::size_t& index = 0) const
         {
            return reinterpret_cast<expression_node<T>*>(index * 0);
         }

      private:

         T value_;
      };

      template <typename T>
      class string_literal_node : public expression_node<T>
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

         inline expression_node<T>* branch(const std::size_t& index = 0) const
         {
            return reinterpret_cast<expression_node<T>*>(index * 0);
         }

         inline std::string str()
         {
            return value_;
         }

      private:

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
         bool           branch_deletable_;
      };

      template <std::size_t N, typename T>
      inline void init_branches(std::pair< expression_node<T>*,bool> (&branch)[N],
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
         typedef expression_node<T> Node;
         //Needed for incompetent and broken msvc compiler versions
         #ifdef _MSC_VER
          #pragma warning(push)
          #pragma warning(disable: 4127)
         #endif
         if (b0 && (N > 0)) { branch[0] = std::make_pair(b0,branch_deletable(b0)); }
         if (b1 && (N > 1)) { branch[1] = std::make_pair(b1,branch_deletable(b1)); }
         if (b2 && (N > 2)) { branch[2] = std::make_pair(b2,branch_deletable(b2)); }
         if (b3 && (N > 3)) { branch[3] = std::make_pair(b3,branch_deletable(b3)); }
         if (b4 && (N > 4)) { branch[4] = std::make_pair(b4,branch_deletable(b4)); }
         if (b5 && (N > 5)) { branch[5] = std::make_pair(b5,branch_deletable(b5)); }
         if (b6 && (N > 6)) { branch[6] = std::make_pair(b6,branch_deletable(b6)); }
         if (b7 && (N > 7)) { branch[7] = std::make_pair(b7,branch_deletable(b7)); }
         if (b8 && (N > 8)) { branch[8] = std::make_pair(b8,branch_deletable(b8)); }
         if (b9 && (N > 9)) { branch[9] = std::make_pair(b9,branch_deletable(b9)); }
         #ifdef _MSC_VER
          #pragma warning(pop)
         #endif
      }

      template <std::size_t N, typename T>
      inline void cleanup_branches(std::pair<expression_node<T>*,bool> (&branch)[N])
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
            cleanup_branches<2>(branch_);
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

         inline operator_type operation() const
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
            cleanup_branches<3>(branch_);
         }

         inline T value() const
         {
            const T arg0 = branch_[0].first->value();
            const T arg1 = branch_[1].first->value();
            const T arg2 = branch_[2].first->value();
            switch (operation_)
            {
               case e_clamp   : return (arg1 < arg0) ? arg0 : (arg1 > arg2 ? arg2 : arg1);
               case e_inrange : return (arg1 < arg0) ? T(0) : ((arg1 > arg2) ? T(0) : T(1));
               case e_min     : return std::min<T>(std::min<T>(arg0,arg1),arg2);
               case e_max     : return std::max<T>(std::max<T>(arg0,arg1),arg2);
               case e_avg     : return (arg0 + arg1 + arg2) / T(3.0);
               case e_sum     : return (arg0 + arg1 + arg2);
               case e_prod    : return (arg0 * arg1 * arg2);
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
            cleanup_branches<4>(branch_);
         }

         inline T value() const
         {
            const T arg0 = branch_[0].first->value();
            const T arg1 = branch_[1].first->value();
            const T arg2 = branch_[2].first->value();
            const T arg3 = branch_[3].first->value();
            switch (operation_)
            {
               case e_min  : return std::min<T>(std::min<T>(arg0,arg1),std::min<T>(arg2,arg3));
               case e_max  : return std::max<T>(std::max<T>(arg0,arg1),std::max<T>(arg2,arg3));
               case e_avg  : return (arg0 + arg1 + arg2 + arg3) / T(4.0);
               case e_sum  : return (arg0 + arg1 + arg2 + arg3);
               case e_prod : return (arg0 * arg1 * arg2 * arg3);
               default     : return std::numeric_limits<T>::quiet_NaN();
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
            cleanup_branches<5>(branch_);
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
               case e_avg  : return (arg0 + arg1 + arg2 + arg3 + arg4) / T(5.0);
               case e_sum  : return (arg0 + arg1 + arg2 + arg3 + arg4);
               case e_prod : return (arg0 * arg1 * arg2 * arg3 * arg4);
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
            cleanup_branches<6>(branch_);
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
               case e_avg     : return (arg0 + arg1 + arg2 + arg3 + arg4 + arg5) / T(6.0);
               case e_sum     : return (arg0 + arg1 + arg2 + arg3 + arg4 + arg5);
               case e_prod    : return (arg0 * arg1 * arg2 * arg3 * arg4 * arg5);
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
            if (test_->value() != 0)
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

         while_loop_node(expression_ptr test,
                         expression_ptr branch)
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
            while (test_->value() != T(0))
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

      template <typename T>
      class stringvar_node : public expression_node<T>
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

      template <typename T> struct sf00_op { static inline T process(const T& x, const T& y, const T& z) { return (x + y) / z; } };
      template <typename T> struct sf01_op { static inline T process(const T& x, const T& y, const T& z) { return (x + y) * z; } };
      template <typename T> struct sf02_op { static inline T process(const T& x, const T& y, const T& z) { return (x - y) / z; } };
      template <typename T> struct sf03_op { static inline T process(const T& x, const T& y, const T& z) { return (x - y) * z; } };
      template <typename T> struct sf04_op { static inline T process(const T& x, const T& y, const T& z) { return (x * y) + z; } };
      template <typename T> struct sf05_op { static inline T process(const T& x, const T& y, const T& z) { return (x * y) - z; } };
      template <typename T> struct sf06_op { static inline T process(const T& x, const T& y, const T& z) { return (x * y) / z; } };
      template <typename T> struct sf07_op { static inline T process(const T& x, const T& y, const T& z) { return (x * y) * z; } };
      template <typename T> struct sf08_op { static inline T process(const T& x, const T& y, const T& z) { return (x / y) + z; } };
      template <typename T> struct sf09_op { static inline T process(const T& x, const T& y, const T& z) { return (x / y) - z; } };
      template <typename T> struct sf10_op { static inline T process(const T& x, const T& y, const T& z) { return (x / y) / z; } };
      template <typename T> struct sf11_op { static inline T process(const T& x, const T& y, const T& z) { return (x / y) * z; } };
      template <typename T> struct sf12_op { static inline T process(const T& x, const T& y, const T& z) { return z / (x + y); } };
      template <typename T> struct sf13_op { static inline T process(const T& x, const T& y, const T& z) { return z / (x - y); } };
      template <typename T> struct sf14_op { static inline T process(const T& x, const T& y, const T& z) { return z / (x * y); } };
      template <typename T> struct sf15_op { static inline T process(const T& x, const T& y, const T& z) { return z / (x / y); } };
      template <typename T> struct sf16_op { static inline T process(const T& x, const T& y, const T& z) { return z - (x / y); } };
      template <typename T> struct sf17_op { static inline T process(const T& x, const T& y, const T& z) { return z - (x / y); } };
      template <typename T> struct sf18_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return w + ((x + y) / z); } };
      template <typename T> struct sf19_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return w + ((x + y) * z); } };
      template <typename T> struct sf20_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return w + ((x - y) / z); } };
      template <typename T> struct sf21_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return w + ((x - y) * z); } };
      template <typename T> struct sf22_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return w + ((x * y) / z); } };
      template <typename T> struct sf23_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return w + ((x * y) * z); } };
      template <typename T> struct sf24_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return w + ((x / y) + z); } };
      template <typename T> struct sf25_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return w + ((x / y) / z); } };
      template <typename T> struct sf26_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return w + ((x / y) * z); } };
      template <typename T> struct sf27_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return w - ((x + y) / z); } };
      template <typename T> struct sf28_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return w - ((x + y) * z); } };
      template <typename T> struct sf29_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return w - ((x - y) / z); } };
      template <typename T> struct sf30_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return w - ((x - y) * z); } };
      template <typename T> struct sf31_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return w - ((x * y) / z); } };
      template <typename T> struct sf32_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return w - ((x * y) * z); } };
      template <typename T> struct sf33_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return w - ((x / y) / z); } };
      template <typename T> struct sf34_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return w - ((x / y) * z); } };
      template <typename T> struct sf35_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return ((x + y) * z) - w; } };
      template <typename T> struct sf36_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return ((x - y) * z) - w; } };
      template <typename T> struct sf37_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return ((x * y) * z) - w; } };
      template <typename T> struct sf38_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return ((x / y) * z) - w; } };
      template <typename T> struct sf39_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return ((x + y) / z) - w; } };
      template <typename T> struct sf40_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return ((x - y) / z) - w; } };
      template <typename T> struct sf41_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return ((x * y) / z) - w; } };
      template <typename T> struct sf42_op { static inline T process(const T& x, const T& y, const T& z, const T& w) { return ((x / y) / z) - w; } };

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

      template <typename T>
      class assignment_node : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         assignment_node(const operator_type& operation,
                         expression_ptr branch0,
                         expression_ptr branch1)
         : binary_node<T>(operation,branch0,branch1)
         {}

         inline T value() const
         {
            if (is_variable_node(binary_node<T>::branch_[0].first))
            {
               variable_node<T>* var_node_ptr = dynamic_cast<variable_node<T>*>(binary_node<T>::branch_[0].first);
               if (var_node_ptr)
               {
                  T& result = var_node_ptr->ref();
                  result = binary_node<T>::branch_[1].first->value();
                  return result;
               }
            }
            return std::numeric_limits<T>::quiet_NaN();
         }
      };

      template <typename T, typename IFunction>
      class function_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;
         typedef IFunction ifunction;

         static const std::size_t N = 10;

         function_node(ifunction* func)
         : function(func)
         {}

        ~function_node()
         {
            cleanup_branches<N>(branch_);
         }

         template <std::size_t NumBranches>
         bool init_branches(expression_ptr (&b)[NumBranches])
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

         inline bool operator <(const function_node<T,IFunction>& fn) const
         {
            return this < (&fn);
         }

         inline T value() const
         {
            T v[N];
            if (function->param_count)
            {
               for (std::size_t i = 0; i < function->param_count; ++i)
               {
                  v[i] = branch_[i].first->value();
               }
               switch (function->param_count)
               {
                  case  1 : return (*function)(v[0]);
                  case  2 : return (*function)(v[0],v[1]);
                  case  3 : return (*function)(v[0],v[1],v[2]);
                  case  4 : return (*function)(v[0],v[1],v[2],v[3]);
                  case  5 : return (*function)(v[0],v[1],v[2],v[3],v[4]);
                  case  6 : return (*function)(v[0],v[1],v[2],v[3],v[4],v[5]);
                  case  7 : return (*function)(v[0],v[1],v[2],v[3],v[4],v[5],v[6]);
                  case  8 : return (*function)(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7]);
                  case  9 : return (*function)(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8]);
                  case 10 : return (*function)(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9]);
                  default : return std::numeric_limits<T>::quiet_NaN();
               }
            }
            else
               return (*function)();
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_function;
         }

      private:

         ifunction* function;
         branch_t branch_[N];
      };

      template <typename T>
      struct add_op
      {
         static inline T process(const T& t1, const T& t2) { return t1 + t2; }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_add; }
         static inline details::operator_type operation() { return details::e_add; }
      };

      template <typename T>
      struct mul_op
      {
         static inline T process(const T& t1, const T& t2) { return t1 * t2; }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_mul; }
         static inline details::operator_type operation() { return details::e_mul; }
      };

      template <typename T>
      struct sub_op
      {
         static inline T process(const T& t1, const T& t2) { return t1 - t2; }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_sub; }
         static inline details::operator_type operation() { return details::e_sub; }
      };

      template <typename T>
      struct div_op
      {
         static inline T process(const T& t1, const T& t2) { return t1 / t2; }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_div; }
         static inline details::operator_type operation() { return details::e_div; }
      };

      template <typename T>
      struct mod_op
      {
         static inline T process(const T& t1, const T& t2) { return numeric::modulus<T>(t1,t2); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_mod; }
         static inline details::operator_type operation() { return details::e_mod; }
      };

      template <typename T>
      struct pow_op
      {
         static inline T process(const T& t1, const T& t2) { return numeric::pow<T>(t1,t2); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_pow; }
         static inline details::operator_type operation() { return details::e_pow; }
      };

      template <typename T>
      struct lt_op
      {
         static inline T process(const T& t1, const T& t2) { return ((t1 < t2) ? T(1) : T(0)); }
         static inline T process(const std::string& t1, const std::string& t2) { return ((t1 < t2) ? T(1) : T(0)); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_lt; }
         static inline details::operator_type operation() { return details::e_lt; }
      };

      template <typename T>
      struct lte_op
      {
         static inline T process(const T& t1, const T& t2) { return ((t1 <= t2) ? T(1) : T(0)); }
         static inline T process(const std::string& t1, const std::string& t2) { return ((t1 <= t2) ? T(1) : T(0)); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_lte; }
         static inline details::operator_type operation() { return details::e_lte; }
      };

      template <typename T>
      struct gt_op
      {
         static inline T process(const T& t1, const T& t2) { return ((t1 > t2) ? T(1) : T(0)); }
         static inline T process(const std::string& t1, const std::string& t2) { return ((t1 > t2) ? T(1) : T(0)); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_gt; }
         static inline details::operator_type operation() { return details::e_gt; }
      };

      template <typename T>
      struct gte_op
      {
         static inline T process(const T& t1, const T& t2) { return ((t1 >= t2) ? T(1) : T(0)); }
         static inline T process(const std::string& t1, const std::string& t2) { return ((t1 >= t2) ? T(1) : T(0)); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_gte; }
         static inline details::operator_type operation() { return details::e_gte; }
      };

      template <typename T>
      struct eq_op
      {
         static inline T process(const T& t1, const T& t2) { return ((t1 == t2) ? T(1) : T(0)); }
         static inline T process(const std::string& t1, const std::string& t2) { return ((t1 == t2) ? T(1) : T(0)); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_eq; }
         static inline details::operator_type operation() { return details::e_eq; }
      };

      template <typename T>
      struct ne_op
      {
         static inline T process(const T& t1, const T& t2) { return ((t1 != t2) ? T(1) : T(0)); }
         static inline T process(const std::string& t1, const std::string& t2) { return ((t1 != t2) ? T(1) : T(0)); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_ne; }
         static inline details::operator_type operation() { return details::e_ne; }
      };

      template <typename T>
      struct and_op
      {
         static inline T process(const T& t1, const T& t2) { return ((t1 != T(0)) && (t2 != T(0))) ? T(1) : T(0); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_and; }
         static inline details::operator_type operation() { return details::e_and; }
      };

      template <typename T>
      struct nand_op
      {
         static inline T process(const T& t1, const T& t2) { return ((t1 != T(0)) && (t2 != T(0))) ? T(0) : T(1); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_nand; }
         static inline details::operator_type operation() { return details::e_nand; }
      };

      template <typename T>
      struct or_op
      {
         static inline T process(const T& t1, const T& t2) { return ((t1 != T(0)) || (t2 != T(0))) ? T(1) : T(0); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_or; }
         static inline details::operator_type operation() { return details::e_or; }
      };

      template <typename T>
      struct nor_op
      {
         static inline T process(const T& t1, const T& t2) { return ((t1 != T(0)) || (t2 != T(0))) ? T(0) : T(1); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_nor; }
         static inline details::operator_type operation() { return details::e_nor; }
      };

      template <typename T>
      struct xor_op
      {
         static inline T process(const T& t1, const T& t2) { return numeric::xor_opr<T>(t1,t2); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_nor; }
         static inline details::operator_type operation() { return details::e_xor; }
      };

      template <typename T>
      struct in_op
      {
         static inline T process(const T&, const T&) { return std::numeric_limits<T>::quiet_NaN(); }
         static inline T process(const std::string& t1, const std::string& t2) { return ((std::string::npos != t2.find(t1)) ? T(1) : T(0)); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_in; }
         static inline details::operator_type operation() { return details::e_in; }
      };

      template <typename T>
      struct like_op
      {
         static inline T process(const T&, const T&) { return std::numeric_limits<T>::quiet_NaN(); }
         static inline T process(const std::string& t1, const std::string& t2) { return (details::wc_match(t2,t1) ? T(1) : T(0)); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_like; }
         static inline details::operator_type operation() { return details::e_like; }
      };

      template <typename T>
      struct ilike_op
      {
         static inline T process(const T&, const T&) { return std::numeric_limits<T>::quiet_NaN(); }
         static inline T process(const std::string& t1, const std::string& t2) { return (details::wc_imatch(t2,t1) ? T(1) : T(0)); }
         static inline typename expression_node<T>::node_type type() { return expression_node<T>::e_ilike; }
         static inline details::operator_type operation() { return details::e_ilike; }
      };

      template <typename T>
      class vov_base_node : public expression_node<T>
      {
      public:

         inline virtual operator_type operation() const
         {
            return details::e_default;
         }
      };

      template <typename T>
      class cov_base_node : public expression_node<T>
      {
      public:

         inline virtual operator_type operation() const
         {
            return details::e_default;
         }
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

      template <typename T, typename Operation>
      class vov_node : public vov_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         //variable op variable node
         explicit vov_node(T& v0, T& v1)
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

         inline T& v0()
         {
            return v0_;
         }

         inline T& v1()
         {
            return v1_;
         }

      protected:

         T& v0_;
         T& v1_;
      };

      template <typename T, typename Type0, typename Type1, typename Operation>
      class vov_nodex : public vov_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         //variable op variable node
         explicit vov_nodex(Type0 v0, Type1 v1)
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

         inline Type0 v0()
         {
            return v0_;
         }

         inline Type1 v1()
         {
            return v1_;
         }

      protected:

         Type0 v0_;
         Type1 v1_;
      };

      template <typename T, typename Operation>
      class cov_node : public cov_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         //constant op variable node
         explicit cov_node(const T& c, T& v)
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

         inline T c()
         {
            return c_;
         }

         inline T& v()
         {
            return v_;
         }

      protected:

         T  c_;
         T& v_;
      };

      template <typename T, typename Operation>
      class voc_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         //variable op constant node
         explicit voc_node(T& v, const T& c)
         : v_(v),
           c_(c)
         {}

         inline T value() const
         {
            return Operation::process(v_,c_);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return Operation::type();
         }

      protected:

         T& v_;
         T  c_;
      };

      template <typename T, typename Operation1, typename Operation2>
      class vovov1_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         //variable0 op1 (variable1 op2 variable2) node
         explicit vovov1_node(T& v0, vov_node<T,Operation2>& vn)
         : v0_(v0),
           v1_(vn.v0()),
           v2_(vn.v1())
         {}

         inline T value() const
         {
            return Operation1::process(v0_,Operation2::process(v1_,v2_));
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_none;
         }

         inline typename expression_node<T>::node_type type1() const
         {
            return Operation1::type();
         }

         inline typename expression_node<T>::node_type type2() const
         {
            return Operation2::type();
         }

      protected:

         T& v0_;
         T& v1_;
         T& v2_;
      };

      template <typename T, typename Operation1, typename Operation2>
      class vovov2_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         //(variable0 op1 variable1) op2 variable2 node
         explicit vovov2_node(vov_node<T,Operation1>& vn, T& v2)
         : v0_(vn.v0()),
           v1_(vn.v1()),
           v2_(v2)
         {}

         inline T value() const
         {
            return Operation2::process(Operation1::process(v0_,v1_),v2_);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_none;
         }

         inline typename expression_node<T>::node_type type1() const
         {
            return Operation1::type();
         }

         inline typename expression_node<T>::node_type type2() const
         {
            return Operation2::type();
         }

      protected:

         T& v0_;
         T& v1_;
         T& v2_;
      };

      template <typename T, typename Operation1, typename Operation2>
      class covov1_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         //constant op1 (variable0 op2 variable1) node
         explicit covov1_node(const T& c, vov_node<T,Operation2>& vn)
         : c_(c),
           v0_(vn.v0()),
           v1_(vn.v1())
         {}

         inline T value() const
         {
            return Operation1::process(c_,Operation2::process(v0_,v1_));
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_none;
         }

         inline typename expression_node<T>::node_type type1() const
         {
            return Operation1::type();
         }

         inline typename expression_node<T>::node_type type2() const
         {
            return Operation2::type();
         }

      protected:

         T   c_;
         T& v0_;
         T& v1_;
      };

      template <typename T, typename Operation1, typename Operation2>
      class covov2_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         //(constant op1 variable0) op2 variable1 node
         explicit covov2_node(cov_node<T,Operation1>& cvn, T& v1)
         : c_ (cvn.c()),
           v0_(cvn.v()),
           v1_(v1)
         {}

         inline T value() const
         {
            return Operation2::process(Operation1::process(c_,v0_),v1_);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_none;
         }

         inline typename expression_node<T>::node_type type1() const
         {
            return Operation1::type();
         }

         inline typename expression_node<T>::node_type type2() const
         {
            return Operation2::type();
         }

      protected:

         T   c_;
         T& v0_;
         T& v1_;
      };

      template <typename T, typename Operation1, typename Operation2, typename Operation3>
      class vovovov_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         //(variable0 op1 variable1) op2 (variable2 op3 variable3) node
         explicit vovovov_node(vov_node<T,Operation1>& vn0, vov_node<T,Operation3>& vn1)
         : v0_(vn0.v0()),
           v1_(vn0.v1()),
           v2_(vn1.v0()),
           v3_(vn1.v1())
         {}

         inline T value() const
         {
            return Operation2::process(Operation1::process(v0_,v1_),Operation3::process(v2_,v3_));
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_none;
         }

         inline typename expression_node<T>::node_type type1() const
         {
            return Operation1::type();
         }

         inline typename expression_node<T>::node_type type2() const
         {
            return Operation2::type();
         }

      protected:

         T& v0_;
         T& v1_;
         T& v2_;
         T& v3_;
      };

      template <typename T, typename SType0, typename SType1, typename Operation>
      class sos_node : public sos_base_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef Operation operation_t;

         //variable op variable node
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
      inline bool is_string_node(const expression_node<T>* node)
      {
         return (expression_node<T>::e_stringvar == node->type());
      }

      template <typename T>
      inline bool is_const_string_node(const expression_node<T>* node)
      {
         return (expression_node<T>::e_stringconst == node->type());
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
         inline expression_node<typename node_type::value_type>* allocate_rrr(T1& t1, T2& t2, T3& t3) const
         {
            return new node_type(t1,t2,t3);
         }

         template <typename node_type,
                   typename T1, typename T2,
                   typename T3>
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

         template <typename T>
         void inline free(expression_node<T>*& e) const
         {
            delete e;
            e = 0;
         }
      };

      struct operation_t
      {
         operation_t(const std::string& n, const operator_type t, const unsigned int& np)
         : name(n),
           type(t),
           num_params(np)
         {}

         std::string name;
         operator_type type;
         unsigned int num_params;
      };

      static const operation_t operation_list[] =
                                 {
                                    operation_t(      "abs" , e_abs    , 1),
                                    operation_t(     "acos" , e_acos   , 1),
                                    operation_t(     "asin" , e_asin   , 1),
                                    operation_t(     "atan" , e_atan   , 1),
                                    operation_t(     "ceil" , e_ceil   , 1),
                                    operation_t(      "cos" , e_cos    , 1),
                                    operation_t(     "cosh" , e_cosh   , 1),
                                    operation_t(      "exp" , e_exp    , 1),
                                    operation_t(    "floor" , e_floor  , 1),
                                    operation_t(      "log" , e_log    , 1),
                                    operation_t(    "log10" , e_log10  , 1),
                                    operation_t(    "round" , e_round  , 1),
                                    operation_t(      "sin" , e_sin    , 1),
                                    operation_t(     "sinh" , e_sinh   , 1),
                                    operation_t(      "sec" , e_sec    , 1),
                                    operation_t(      "csc" , e_csc    , 1),
                                    operation_t(     "sqrt" , e_sqrt   , 1),
                                    operation_t(      "tan" , e_tan    , 1),
                                    operation_t(     "tanh" , e_tanh   , 1),
                                    operation_t(      "cot" , e_cot    , 1),
                                    operation_t(  "rad2deg" , e_r2d    , 1),
                                    operation_t(  "deg2rad" , e_d2r    , 1),
                                    operation_t( "deg2grad" , e_d2g    , 1),
                                    operation_t( "grad2deg" , e_g2d    , 1),
                                    operation_t(      "not" , e_not    , 1),
                                    operation_t(    "atan2", e_atan2   , 2),
                                    operation_t(      "min", e_min     , 2),
                                    operation_t(      "max", e_max     , 2),
                                    operation_t(      "avg", e_avg     , 2),
                                    operation_t(      "sum", e_sum     , 2),
                                    operation_t(      "mul", e_prod    , 2),
                                    operation_t(      "mod", e_mod     , 2),
                                    operation_t(     "logn", e_logn    , 2),
                                    operation_t(     "root", e_root    , 2),
                                    operation_t(   "roundn", e_roundn  , 2),
                                    operation_t(    "equal", e_equal   , 2),
                                    operation_t("not_equal", e_nequal  , 2),
                                    operation_t(      "hyp", e_hyp     , 2),
                                    operation_t(      "shr", e_shr     , 2),
                                    operation_t(      "shl", e_shl     , 2),
                                    operation_t(    "clamp", e_clamp   , 3),
                                    operation_t(  "inrange", e_inrange , 3),
                                    operation_t(      "min", e_min     , 3),
                                    operation_t(      "max", e_max     , 3),
                                    operation_t(      "avg", e_avg     , 3),
                                    operation_t(      "sum", e_sum     , 3),
                                    operation_t(      "mul", e_prod    , 3),
                                    operation_t(      "min", e_min     , 4),
                                    operation_t(      "max", e_max     , 4),
                                    operation_t(      "avg", e_avg     , 4),
                                    operation_t(      "sum", e_sum     , 4),
                                    operation_t(      "mul", e_prod    , 4),
                                    operation_t(      "min", e_min     , 5),
                                    operation_t(      "max", e_max     , 5),
                                    operation_t(      "avg", e_avg     , 5),
                                    operation_t(      "sum", e_sum     , 5),
                                    operation_t(      "mul", e_prod    , 5),
                                    operation_t(      "min", e_min     , 6),
                                    operation_t(      "max", e_max     , 6),
                                    operation_t(      "avg", e_avg     , 6),
                                    operation_t(      "sum", e_sum     , 6),
                                    operation_t(      "mul", e_prod    , 6),
                                 };

      static const std::size_t operation_list_size = sizeof(operation_list) / sizeof(operation_t);

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
   };

   template <typename T>
   class symbol_table
   {
   private:

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

      typedef typename details::variable_node<T> variable_t;
      typedef variable_t* variable_ptr;
      typedef std::pair<bool,variable_ptr> variable_pair_t;
      typedef std::map<std::string,variable_pair_t,ilesscompare> variable_map_t;
      typedef typename variable_map_t::iterator vm_itr_t;
      typedef typename variable_map_t::const_iterator vm_const_itr_t;

      typedef typename details::stringvar_node<T> stringvar_t;
      typedef stringvar_t* stringvar_ptr;
      typedef std::pair<bool,stringvar_ptr> stringvar_pair_t;
      typedef std::map<std::string,stringvar_pair_t,ilesscompare> stringvar_map_t;
      typedef typename stringvar_map_t::iterator svm_itr_t;
      typedef typename stringvar_map_t::const_iterator svm_const_itr_t;

      typedef ifunction<T> function_t;
      typedef function_t* function_ptr;
      typedef std::map<std::string,function_ptr,ilesscompare> function_map_t;
      typedef typename function_map_t::iterator fm_itr_t;
      typedef typename function_map_t::const_iterator fm_const_itr_t;

      static const std::size_t lut_size = 256;

   public:

      symbol_table()
      : variable_count_(0),
        function_count_(0)
      {
         clear_short_symbol_luts();
      }

     ~symbol_table()
      {
         clear();
      }

      inline void clear()
      {
         if (!variable_map_.empty())
         {
            vm_itr_t itr = variable_map_.begin();
            vm_itr_t end = variable_map_.end();
            while (end != itr)
            {
               delete (*itr).second.second;
               ++itr;
            }
            variable_map_.clear();
         }
         if (!stringvar_map_.empty())
         {
            svm_itr_t itr = stringvar_map_.begin();
            svm_itr_t end = stringvar_map_.end();
            while (end != itr)
            {
               delete (*itr).second.second;
               ++itr;
            }
            stringvar_map_.clear();
         }
         if (!function_map_.empty())
         {
            function_map_.clear();
         }
         for (std::size_t i = 0; i < lut_size; ++i)
         {
            if (short_variable_lut_[i].second) delete short_variable_lut_[i].second;
            if (short_stringvar_lut_[i].second) delete short_stringvar_lut_[i].second;
            if (short_function_lut_[i]) delete short_function_lut_[i];
         }
         clear_short_symbol_luts();
         local_symbol_list_.clear();
         local_stringvar_list_.clear();
      }

      inline std::size_t variable_count() const
      {
         return variable_count_;
      }

      inline std::size_t function_count() const
      {
         return function_count_;
      }

      inline variable_ptr get_variable(const std::string& variable_name)
      {
         if (!valid_symbol(variable_name))
            return reinterpret_cast<variable_ptr>(0);
         else if (1 == variable_name.size())
         {
            variable_pair_t& vp = short_variable_lut_[static_cast<std::size_t>(variable_name[0])];
            if (vp.second)
               return vp.second;
            else
               return reinterpret_cast<variable_ptr>(0);
         }
         else
         {
            vm_const_itr_t itr = variable_map_.find(variable_name);
            if (variable_map_.end() == itr)
               return reinterpret_cast<variable_ptr>(0);
            else
               return itr->second.second;
         }
      }

      inline stringvar_ptr get_stringvar(const std::string& string_name)
      {
         if (!valid_symbol(string_name))
            return reinterpret_cast<stringvar_ptr>(0);
         else if (1 == string_name.size())
         {
            stringvar_pair_t& svp = short_stringvar_lut_[static_cast<std::size_t>(string_name[0])];
            if (svp.second)
               return svp.second;
            else
               return reinterpret_cast<stringvar_ptr>(0);
         }
         else
         {
            svm_const_itr_t itr = stringvar_map_.find(string_name);
            if (stringvar_map_.end() == itr)
               return reinterpret_cast<stringvar_ptr>(0);
            else
               return itr->second.second;
         }
      }

      inline function_ptr get_function(const std::string& function_name)
      {
         if (!valid_symbol(function_name))
            return reinterpret_cast<function_ptr>(0);
         else if (1 == function_name.size())
         {
            return short_function_lut_[static_cast<std::size_t>(function_name[0])];
         }
         else
         {
            fm_const_itr_t itr = function_map_.find(function_name);
            if (function_map_.end() == itr)
               return reinterpret_cast<function_ptr>(0);
            else
               return itr->second;
         }
      }

      inline T& variable_ref(const std::string& symbol_name)
      {
         static T null_var = T(0);
         if (!valid_symbol(symbol_name))
            return null_var;
         else if (1 == symbol_name.size())
         {
            variable_pair_t& vp = short_variable_lut_[static_cast<std::size_t>(symbol_name[0])];
            if (vp.second)
               return vp->second.ref();
            else
               return null_var;
         }
         else
         {
            vm_const_itr_t itr = variable_map_.find(symbol_name);
            if (variable_map_.end() == itr)
               return null_var;
            else
               return itr->second.second->ref();
         }
      }

      inline std::string& stringvar_ref(const std::string& symbol_name)
      {
         static std::string null_stringvar;
         if (!valid_symbol(symbol_name))
            return null_var;
         else if (1 == symbol_name.size())
         {
            stringvar_pair_t& svp = short_stringvar_lut_[static_cast<std::size_t>(symbol_name[0])];
            if (svp.second)
               return svp->second.ref();
            else
               return null_stringvar;
         }
         else
         {
            svm_const_itr_t itr = stringvar_map_.find(symbol_name);
            if (stringvar_map_.end() == itr)
               return null_var;
            else
               return itr->second.second->ref();
         }
      }

      inline bool is_constant_node(const std::string& symbol_name) const
      {
         if (1 == symbol_name.size())
         {
            return short_variable_lut_[static_cast<std::size_t>(symbol_name[0])].first;
         }
         else
         {
            vm_const_itr_t itr = variable_map_.find(symbol_name);
            if (variable_map_.end() == itr)
               return false;
            else
               return itr->second.first;
         }
      }

      inline bool is_constant_string(const std::string& symbol_name) const
      {
         if (1 == symbol_name.size())
         {
            return short_stringvar_lut_[static_cast<std::size_t>(symbol_name[0])].first;
         }
         else
         {
            svm_const_itr_t itr = stringvar_map_.find(symbol_name);
            if (stringvar_map_.end() == itr)
               return false;
            else
               return itr->second.first;
         }
      }

      inline bool create_variable(const std::string& variable_name, const T& value = T(0))
      {
         if (!valid_symbol(variable_name))
            return false;
         else if (symbol_exists(variable_name))
            return false;
         local_symbol_list_.push_back(value);
         T& t = local_symbol_list_.back();
         return add_variable(variable_name,t);
      }

      inline bool create_stringvar(const std::string& stringvar_name, const std::string& value = std::string(""))
      {
         if (!valid_symbol(stringvar_name))
            return false;
         else if (symbol_exists(stringvar_name))
            return false;
         local_stringvar_list_.push_back(value);
         std::string& s = local_stringvar_list_.back();
         return add_stringvar(stringvar_name,s);
      }

      inline bool add_variable(const std::string& variable_name, T& t, const bool is_constant = false)
      {
         if (!valid_symbol(variable_name))
            return false;
         else if (symbol_exists(variable_name))
            return false;
         else if (1 == variable_name.size())
         {
            variable_pair_t& vp = short_variable_lut_[static_cast<std::size_t>(variable_name[0])];
            vp.first = is_constant;
            vp.second = new variable_t(t);
            ++variable_count_;
         }
         else
         {
            for (std::size_t i = 0; i < details::reserved_symbols_size; ++i)
            {
               if (details::imatch(variable_name,details::reserved_symbols[i]))
               {
                  return false;
               }
            }
            vm_itr_t itr = variable_map_.find(variable_name);
            if (variable_map_.end() == itr)
            {
               variable_map_[variable_name] = std::make_pair(is_constant,new details::variable_node<T>(t));
               ++variable_count_;
            }
         }
         return true;
      }

      inline bool add_stringvar(const std::string& stringvar_name, std::string& s, const bool is_constant = false)
      {
         if (!valid_symbol(stringvar_name))
            return false;
         else if (symbol_exists(stringvar_name))
            return false;
         else if (1 == stringvar_name.size())
         {
            stringvar_pair_t& svp = short_stringvar_lut_[static_cast<std::size_t>(stringvar_name[0])];
            svp.first = is_constant;
            svp.second = new stringvar_t(s);
            ++stringvar_count_;
         }
         else
         {
            for (std::size_t i = 0; i < details::reserved_symbols_size; ++i)
            {
               if (details::imatch(stringvar_name,details::reserved_symbols[i]))
               {
                  return false;
               }
            }
            svm_itr_t itr = stringvar_map_.find(stringvar_name);
            if (stringvar_map_.end() == itr)
            {
               stringvar_map_[stringvar_name] = std::make_pair(is_constant,new details::stringvar_node<T>(s));
               ++stringvar_count_;
            }
         }
         return true;
      }

      inline bool add_function(const std::string& function_name, function_t& function)
      {
         if (!valid_symbol(function_name))
            return false;
         else if (symbol_exists(function_name))
            return false;
         else if (1 == function_name.size())
         {
            short_function_lut_[static_cast<std::size_t>(function_name[0])] = &function;
            ++function_count_;
         }
         else
         {
            for (std::size_t i = 0; i < details::reserved_symbols_size; ++i)
            {
               if (details::imatch(function_name,details::reserved_symbols[i]))
               {
                  return false;
               }
            }
            fm_itr_t itr = function_map_.find(function_name);
            if (function_map_.end() == itr)
            {
               function_map_[function_name] = &function;
               ++function_count_;
            }
            else
               return false;
         }
         return true;
      }

      inline bool remove_variable(const std::string& variable_name)
      {
         if (1 == variable_name.size())
         {
            variable_pair_t& vp = short_variable_lut_[static_cast<std::size_t>(variable_name[0])];
            if (0 == vp.second)
               return false;
            delete vp.second;
            vp.first = false;
            vp.second = 0;
            --variable_count_;
            return true;
         }
         else
         {
            vm_itr_t itr = variable_map_.find(variable_name);
            if (variable_map_.end() != itr)
            {
               delete (*itr).second.second;
               variable_map_.erase(itr);
               --variable_count_;
               return true;
            }
            else
               return false;
         }
      }

      inline bool remove_function(const std::string& function_name)
      {
         if (1 == function_name.size())
         {
            if (short_function_lut_[static_cast<std::size_t>(function_name[0])])
            {
               short_function_lut_[static_cast<std::size_t>(function_name[0])] = 0;
               --function_count_;
               return true;
            }
            else
               return false;
         }
         else
         {
            fm_itr_t itr = function_map_.find(function_name);
            if (function_map_.end() != itr)
            {
               function_map_.erase(itr);
               --function_count_;
               return true;
            }
            else
               return false;
         }
      }

      inline bool remove_stringvar(const std::string& string_name)
      {
         if (1 == string_name.size())
         {
            stringvar_pair_t& svp = short_stringvar_lut_[static_cast<std::size_t>(string_name[0])];
            if (0 == svp.second)
               return false;
            delete svp.second;
            svp.first = false;
            svp.second = 0;
            --stringvar_count_;
            return true;
         }
         else
         {
            svm_itr_t itr = stringvar_map_.find(string_name);
            if (stringvar_map_.end() != itr)
            {
               delete (*itr).second.second;
               stringvar_map_.erase(itr);
               --stringvar_count_;
               return true;
            }
            else
               return false;
         }
      }

      inline void add_constants()
      {
         add_pi();
         add_epsilon();
         add_infinity();
      }

      inline void add_pi()
      {
         static T pi = T(details::numeric::constant::pi);
         add_variable("pi",pi,true);
      }

      inline void add_epsilon()
      {
         static T epsilon = std::numeric_limits<T>::epsilon();
         add_variable("epsilon",epsilon,true);
      }

      inline void add_infinity()
      {
         static T infinity = std::numeric_limits<T>::infinity();
         add_variable("inf",infinity,true);
      }

      template <typename Allocator,
                template <typename, typename> class Sequence>
      inline std::size_t get_variable_list(Sequence<std::pair<std::string,T>,Allocator>& vlist) const
      {
         std::size_t count = 0;
         for (std::size_t i = 0; i < lut_size; ++i)
         {
            const variable_pair_t& vp = short_variable_lut_[static_cast<std::size_t>(i)];
            if (0 != vp.second)
            {
               vlist.push_back(std::make_pair(std::string("") + static_cast<char>(i),vp.second->value()));
               ++count;
            }
         }
         if (!variable_map_.empty())
         {
            vm_const_itr_t itr = variable_map_.begin();
            vm_const_itr_t end = variable_map_.end();
            while (end != itr)
            {
               vlist.push_back(std::make_pair((*itr).first,itr->second.second->ref()));
               ++itr;
               ++count;
            }
         }
         return count;
      }

      template <typename Allocator,
                template <typename, typename> class Sequence>
      inline std::size_t get_variable_list(Sequence<std::string,Allocator>& vlist) const
      {
         std::size_t count = 0;
         for (std::size_t i = 0; i < lut_size; ++i)
         {
            const variable_pair_t& vp = short_variable_lut_[static_cast<std::size_t>(i)];
            if (0 != vp.second)
            {
               vlist.push_back(std::string("") + static_cast<char>(i));
               ++count;
            }
         }
         if (!variable_map_.empty())
         {
            vm_const_itr_t itr = variable_map_.begin();
            vm_const_itr_t end = variable_map_.end();
            while (end != itr)
            {
               vlist.push_back((*itr).first);
               ++itr;
               ++count;
            }
         }
         return count;
      }

      template <typename Allocator,
                template <typename, typename> class Sequence>
      inline std::size_t get_stringvar_list(Sequence<std::pair<std::string,std::string>,Allocator>& svlist) const
      {
         std::size_t count = 0;
         for (std::size_t i = 0; i < lut_size; ++i)
         {
            const stringvar_pair_t& svp = short_stringvar_lut_[static_cast<std::size_t>(i)];
            if (0 != svp.second)
            {
               svlist.push_back(std::make_pair(std::string("") + static_cast<char>(i),svp.second->ref()));
               ++count;
            }
         }
         if (!stringvar_map_.empty())
         {
            svm_const_itr_t itr = stringvar_map_.begin();
            svm_const_itr_t end = stringvar_map_.end();
            while (end != itr)
            {
               svlist.push_back(std::make_pair((*itr).first,itr->second.second->ref()));
               ++itr;
               ++count;
            }
         }
         return count;
      }

      template <typename Allocator,
                template <typename, typename> class Sequence>
      inline std::size_t get_stringvar_list(Sequence<std::string,Allocator>& svlist) const
      {
         std::size_t count = 0;
         for (std::size_t i = 0; i < lut_size; ++i)
         {
            const stringvar_pair_t& svp = short_stringvar_lut_[static_cast<std::size_t>(i)];
            if (0 != svp.second)
            {
               svlist.push_back(std::string("") + static_cast<char>(i));
               ++count;
            }
         }
         if (!stringvar_map_.empty())
         {
            svm_const_itr_t itr = stringvar_map_.begin();
            svm_const_itr_t end = stringvar_map_.end();
            while (end != itr)
            {
               svlist.push_back((*itr).first);
               ++itr;
               ++count;
            }
         }
         return count;
      }

      inline bool symbol_exists(const std::string& symbol_name) const
      {
         /*
            Will return true if symbol_name exists as either a
            variable, stringvar or function name in any of the LUTs or maps.
         */
         if ((1 == symbol_name.size()) && short_variable_lut_[static_cast<std::size_t>(symbol_name[0])].second)
            return true;
         else if ((1 == symbol_name.size()) && short_stringvar_lut_[static_cast<std::size_t>(symbol_name[0])].second)
            return true;
         else if ((1 == symbol_name.size()) && short_function_lut_[static_cast<std::size_t>(symbol_name[0])])
            return true;
         else if (variable_map_.end() != variable_map_.find(symbol_name))
            return true;
         else if (stringvar_map_.end() != stringvar_map_.find(symbol_name))
            return true;
         else if (function_map_.end() != function_map_.find(symbol_name))
            return true;
         else
            return false;
      }

      inline bool is_variable(const std::string& variable_name) const
      {
         if ((1 == variable_name.size()) && short_variable_lut_[static_cast<std::size_t>(variable_name[0])].second)
            return true;
         else if (variable_map_.end() != variable_map_.find(variable_name))
            return true;
         else
            return false;
      }

      inline bool is_stringvar(const std::string& stringvar_name) const
      {
         if ((1 == stringvar_name.size()) && short_stringvar_lut_[static_cast<std::size_t>(stringvar_name[0])].second)
            return true;
         else if (stringvar_map_.end() != stringvar_map_.find(stringvar_name))
            return true;
         else
            return false;
      }

      inline bool is_function(const std::string& function_name) const
      {
         if ((1 == function_name.size()) && short_function_lut_[static_cast<std::size_t>(function_name[0])].second)
            return true;
         else if (function_map_.end() != function_map_.find(function_name))
            return true;
         else
            return false;
      }

   private:

      symbol_table(const symbol_table<T>&);
      symbol_table<T> operator=(const symbol_table<T>&);

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
         return true;
      }

      inline void clear_short_symbol_luts()
      {
         for (std::size_t i = 0; i < lut_size; ++i)
         {
            short_variable_lut_[i].first = false;
            short_variable_lut_[i].second = reinterpret_cast<variable_ptr>(0);

            short_stringvar_lut_[i].first = false;
            short_stringvar_lut_[i].second = reinterpret_cast<stringvar_ptr>(0);

            short_function_lut_[i] = reinterpret_cast<function_ptr>(0);
         }
      }

      variable_pair_t short_variable_lut_[lut_size];
      variable_map_t  variable_map_;

      function_ptr short_function_lut_[lut_size];
      function_map_t function_map_;

      stringvar_pair_t short_stringvar_lut_[lut_size];
      stringvar_map_t stringvar_map_;

      std::list<T> local_symbol_list_;
      std::list<std::string> local_stringvar_list_;

      std::size_t variable_count_;
      std::size_t function_count_;
      std::size_t stringvar_count_;
   };

   template <typename T> class parser;

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
      : expression_holder_(0),
        symbol_table_(0)
      {}

      expression(const expression& e)
      {
         expression_holder_ = e.expression_holder_;
         expression_holder_->ref_count++;
         symbol_table_ = e.symbol_table_;
      }

      expression& operator=(const expression& e)
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
         return *this;
      }

      inline bool operator!() const
      {
         return ((0 == expression_holder_) || (0 == expression_holder_->expr));
      }

     ~expression()
      {
         if (expression_holder_)
         {
            if (0 == --expression_holder_->ref_count)
            {
               if (!is_variable_node(expression_holder_->expr))
               {
                  delete expression_holder_;
               }
            }
         }
      }

      inline T value() const
      {
         if (expression_holder_ && expression_holder_->expr)
            return expression_holder_->expr->value();
         else
            return std::numeric_limits<T>::quiet_NaN();
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
         return (T(0) != value());
      }

      inline void register_symbol_table(symbol_table<T>& st)
      {
         symbol_table_ = &st;
      }

      inline symbol_table<T>* get_symbol_table()
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
      symbol_table<T>* symbol_table_;

      friend class parser<T>;
   };

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
         e_level13
      };

      typedef ifunction                <T>                        F;
      typedef details::expression_node <T>        expression_node_t;
      typedef details::literal_node    <T>           literal_node_t;
      typedef details::string_literal_node<T> string_literal_node_t;
      typedef details::unary_node      <T>             unary_node_t;
      typedef details::binary_node     <T>            binary_node_t;
      typedef details::trinary_node    <T>           trinary_node_t;
      typedef details::quaternary_node <T>        quaternary_node_t;
      typedef details::quinary_node    <T>           quinary_node_t;
      typedef details::senary_node     <T>            senary_node_t;
      typedef details::conditional_node<T>       conditional_node_t;
      typedef details::while_loop_node <T>        while_loop_node_t;
      typedef details::variable_node   <T>          variable_node_t;
      typedef details::stringvar_node  <T>         stringvar_node_t;
      typedef details::assignment_node <T>        assignment_node_t;
      typedef details::function_node <T,F>          function_node_t;
      typedef details::token           <T>                  token_t;
      typedef expression_node_t*                expression_node_ptr;

   public:

      enum optimization_level
      {
         e_none   = 0,
         e_level1 = 1,
         e_level2 = 2,
         e_level3 = 4,
         e_all    = 7
      };

      parser()
      : symbol_table_(0),
        symbol_name_caching_(false)
      {}

      inline bool compile(const std::string& expression_string, expression<T>& expr, const optimization_level& opt_level = e_all)
      {
         if (!validate_expression(expression_string))
         {
            return false;
         }
         error_description_ = "";
         expression_generator_.set_optimization_level(opt_level);
         expression_generator_.set_allocator(node_allocator_);
         if (!lexer_.process(expression_string))
         {
            set_error(lexer_.error());
            return false;
         }
         symbol_table_ = expr.get_symbol_table();
         symbol_name_cache_.clear();
         next_token();
         expression_node_ptr e = parse_expression();
         if ((0 != e) && (token_t::eof == current_token_.type))
         {
            expr.set_expression(e);
            return !(!expr);
         }
         else
         {
            set_error("parser::compile() - Incomplete expression!");
            symbol_name_cache_.clear();
            if (0 != e) delete e;
            return false;
         }
      }

      inline std::string error() const
      {
         return error_description_;
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
         if (symbol_name_cache_.empty())
            return 0;
         std::copy(symbol_name_cache_.begin(),
                   symbol_name_cache_.end(),
                   std::back_inserter(symbols_list));
         return symbol_name_cache_.size();
      }

   private:

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
         expression_node_ptr expr = parse_branch();

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
               case token_t::assign : current_state.set(e_level00,e_level00,details::e_assign); break;
               case token_t::lt     : current_state.set(e_level05,e_level06,details::    e_lt); break;
               case token_t::lte    : current_state.set(e_level05,e_level06,details::   e_lte); break;
               case token_t::eq     : current_state.set(e_level05,e_level06,details::    e_eq); break;
               case token_t::ne     : current_state.set(e_level05,e_level06,details::    e_ne); break;
               case token_t::gte    : current_state.set(e_level05,e_level06,details::   e_gte); break;
               case token_t::gt     : current_state.set(e_level05,e_level06,details::    e_gt); break;
               case token_t::add    : current_state.set(e_level07,e_level08,details::   e_add); break;
               case token_t::sub    : current_state.set(e_level07,e_level08,details::   e_sub); break;
               case token_t::div    : current_state.set(e_level10,e_level11,details::   e_div); break;
               case token_t::mul    : current_state.set(e_level10,e_level11,details::   e_mul); break;
               case token_t::mod    : current_state.set(e_level10,e_level11,details::   e_mod); break;
               case token_t::pow    : current_state.set(e_level12,e_level12,details::   e_pow); break;
               default              : if (token_t::symbol == current_token_.type)
                                      {
                                         static const std::string s_and  =    "and";
                                         static const std::string s_nand =   "nand";
                                         static const std::string s_or   =     "or";
                                         static const std::string s_nor  =    "nor";
                                         static const std::string s_xor  =    "xor";
                                         static const std::string s_in   =     "in";
                                         static const std::string s_like =   "like";
                                         static const std::string s_ilike = "ilike";
                                         if (details::imatch(current_token_.value,s_and))
                                         {
                                            current_state.set(e_level01,e_level02,details::e_and);
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

      template <std::size_t NumberofParameters>
      inline expression_node_ptr parse_function_call(const details::operator_type& opt_type)
      {
         expression_node_ptr branch[NumberofParameters];
         expression_node_ptr result  = 0;
         std::fill_n(branch,NumberofParameters,reinterpret_cast<expression_node_ptr>(0));
         scoped_delete<expression_node_t,NumberofParameters> sd(*this,branch);
         store_token();
         next_token();
         if (!token_is(token_t::lbracket))
         {
            return error_node();
         }
         for (int i = 0; i < static_cast<int>(NumberofParameters); ++i)
         {
            branch[i] = parse_expression();
            if (i < static_cast<int>(NumberofParameters - 1))
            {
               if (!token_is(token_t::comma))
               {
                  return error_node();
               }
            }
         }
         if (!token_is(token_t::rbracket))
         {
            return error_node();
         }
         else
            result = expression_generator_(opt_type,branch);
         sd.delete_ptr = false;
         return result;
      }

      template <std::size_t NumberofParameters>
      inline expression_node_ptr parse_function_call(ifunction<T>* function)
      {
         expression_node_ptr branch[NumberofParameters];
         expression_node_ptr result  = 0;
         std::fill_n(branch,NumberofParameters,reinterpret_cast<expression_node_ptr>(0));
         scoped_delete<expression_node_t,NumberofParameters> sd(*this,branch);
         next_token();
         if (!token_is(token_t::lbracket))
         {
            return error_node();
         }
         for (int i = 0; i < static_cast<int>(NumberofParameters); ++i)
         {
            branch[i] = parse_expression();
            if (i < static_cast<int>(NumberofParameters - 1))
            {
               if (!token_is(token_t::comma))
               {
                  return error_node();
               }
            }
         }
         if (!token_is(token_t::rbracket))
         {
            return error_node();
         }
         else
            result = expression_generator_.function(function,branch);
         sd.delete_ptr = false;
         return result;
      }

      inline expression_node_ptr parse_conditional_statement()
      {
         //Parse: [if][(][condition][,][consequent][,][alternative][)]
         expression_node_ptr condition   = 0;
         expression_node_ptr consequent  = 0;
         expression_node_ptr alternative = 0;
         next_token();
         if (token_is(token_t::lbracket))
            condition = parse_expression();
         else
            return error_node();
         if (token_is(token_t::comma))
            consequent = parse_expression();
         else
            return error_node();
         if (token_is(token_t::comma))
            alternative = parse_expression();
         else
            return error_node();
         if (token_is(token_t::rbracket))
            return expression_generator_.conditional(condition,consequent,alternative);
         else
            return error_node();
      }

      inline expression_node_ptr parse_while_loop()
      {
         //Parse: [while][(][test expr][)][{][expression][}]
         expression_node_ptr condition = 0;
         expression_node_ptr branch = 0;
         next_token();
         if (token_is(token_t::lbracket))
            condition = parse_expression();
         else
            return error_node();
         if (!token_is(token_t::rbracket))
            return error_node();
         if (token_is(token_t::lcrlbracket))
            branch = parse_expression();
         else
            return error_node();
         if (token_is(token_t::rcrlbracket))
            return expression_generator_.while_loop(condition,branch);
         else
            return error_node();
      }

      inline expression_node_ptr parse_special_function(const unsigned int id)
      {
         //Expect: $fDD(expr0,expr1,expr2) or $fDD(expr0,expr1,expr2,expr3)
         const details::operator_type opt_type = details::operator_type(id + 1000);
         const std::size_t NumberOfParameters = (id < (details::e_sf18 - 1000)) ? 3 : 4;
         expression_node_ptr branch3[3];
         expression_node_ptr branch4[4];
         expression_node_ptr* branch = (id < (details::e_sf18 - 1000)) ? &branch3[0] : &branch4[0];
         expression_node_ptr result  = 0;
         std::fill_n(branch3,3,reinterpret_cast<expression_node_ptr>(0));
         std::fill_n(branch4,4,reinterpret_cast<expression_node_ptr>(0));
         scoped_delete<expression_node_t,3> sd3(*this,branch3);
         scoped_delete<expression_node_t,4> sd4(*this,branch4);
         next_token();
         if (!token_is(token_t::lbracket))
         {
            return error_node();
         }
         for (std::size_t i = 0; i < NumberOfParameters; ++i)
         {
            branch[i] = parse_expression();
            if (i < (NumberOfParameters - 1))
            {
               if (!token_is(token_t::comma))
               {
                  return error_node();
               }
            }
         }
         if (!token_is(token_t::rbracket))
            return error_node();
         else
         {
            switch (NumberOfParameters)
            {
               case 3  : result = expression_generator_.special_function(opt_type,branch3); break;
               case 4  : result = expression_generator_.special_function(opt_type,branch4); break;
               default : return error_node();
            }
         }
         sd3.delete_ptr = false;
         sd4.delete_ptr = false;
         return result;
      }

      inline expression_node_ptr parse_symbol()
      {
         std::pair<bool,std::string> match_found_error(false,"");
         if (current_token_.value.size() > 1)
         {
            for (std::size_t i = 0; i < details::operation_list_size; ++i)
            {
               if (details::imatch(details::operation_list[i].name,current_token_.value))
               {
                  store_token();
                  std::string token_value = current_token_.value;
                  expression_node_ptr branch = reinterpret_cast<expression_node_ptr>(0);
                  switch (details::operation_list[i].num_params)
                  {
                     case 1 : branch = parse_function_call<1>(details::operation_list[i].type); break;
                     case 2 : branch = parse_function_call<2>(details::operation_list[i].type); break;
                     case 3 : branch = parse_function_call<3>(details::operation_list[i].type); break;
                     case 4 : branch = parse_function_call<4>(details::operation_list[i].type); break;
                     case 5 : branch = parse_function_call<5>(details::operation_list[i].type); break;
                     case 6 : branch = parse_function_call<6>(details::operation_list[i].type); break;
                  }
                  if (branch)
                  {
                     return branch;
                  }
                  else if (!match_found_error.first)
                  {
                     match_found_error.first = true;
                     match_found_error.second = token_value;
                     set_error("");
                  }
                  restore_token();
               }
            }

            if (match_found_error.first)
            {
               set_error("parser::parse_branch() - invalid argument count for function: " + match_found_error.second);
               return error_node();
            }
         }

         static const std::string symbol_if = "if";
         static const std::string symbol_while = "while";
         if (details::imatch(current_token_.value,symbol_if))
         {
            return parse_conditional_statement();
         }
         else if (details::imatch(current_token_.value,symbol_while))
         {
            return parse_while_loop();
         }
         else if ((current_token_.value.size() == 4) && '$' == current_token_.value[0] && details::imatch('f',current_token_.value[1]))
         {
            unsigned int id = (current_token_.value[2] - '0') * 10 + (current_token_.value[3] - '0');
            if (id <= 99)
               return parse_special_function(id);
            else
            {
               set_error("parser::parse_branch() - invalid special function: " + current_token_.value);
               return error_node();
            }
         }
         else if (symbol_table_)
         {
            const std::string symbol = current_token_.value;
            //Are we dealing with a variable or a special constant?
            expression_node_ptr variable = symbol_table_->get_variable(symbol);
            if (variable)
            {
               if (symbol_name_caching_)
               {
                  symbol_name_cache_.push_back(symbol);
               }
               if (symbol_table_->is_constant_node(symbol))
               {
                  variable = expression_generator_(variable->value());
               }
               next_token();
               return variable;
            }

            #ifndef exprtk_disable_string_capabilities
            //Are we dealing with a string variable?
            variable = symbol_table_->get_stringvar(symbol);
            if (variable)
            {
               if (symbol_name_caching_)
               {
                  symbol_name_cache_.push_back(symbol);
               }
               if (symbol_table_->is_constant_node(symbol))
               {
                  variable = expression_generator_(dynamic_cast<details::string_literal_node<T>*>(variable)->str());
               }
               next_token();
               return variable;
            }
            #endif

            //Are we dealing with a function?
            ifunction<T>* function = symbol_table_->get_function(symbol);
            if (function)
            {
               expression_node_ptr func_node = reinterpret_cast<expression_node_ptr>(0);
               switch (function->param_count)
               {
                  case  0 : func_node = expression_generator_.function(function); break;
                  case  1 : func_node = parse_function_call< 1>(function); break;
                  case  2 : func_node = parse_function_call< 2>(function); break;
                  case  3 : func_node = parse_function_call< 3>(function); break;
                  case  4 : func_node = parse_function_call< 4>(function); break;
                  case  5 : func_node = parse_function_call< 5>(function); break;
                  case  6 : func_node = parse_function_call< 6>(function); break;
                  case  7 : func_node = parse_function_call< 7>(function); break;
                  case  8 : func_node = parse_function_call< 8>(function); break;
                  case  9 : func_node = parse_function_call< 9>(function); break;
                  case 10 : func_node = parse_function_call<10>(function); break;
                  default : {
                              set_error("parser::parse_branch() - invalid number of parameters for function: " + symbol);
                              return expression_node_ptr(0);
                            }
               }
               if (func_node)
                  return func_node;
               else
               {
                  set_error("parser::parse_branch() - failed to generate node for function: " + symbol);
                  return error_node();
               }
            }
            else
            {
               set_error("parser::parse_branch() - undefined variable or function: " + symbol);
               return error_node();
            }
         }
         else
         {
            set_error("parser::parse_branch() - variable or function detected, yet symbol-table is invalid" + current_token_.value);
            return error_node();
         }
      }

      inline expression_node_ptr parse_branch()
      {
         switch (current_token_.type)
         {
            case token_t::number :
                                 {
                                    expression_node_ptr literal_exp = expression_generator_(current_token_.numeric_value);
                                    next_token();
                                    return literal_exp;
                                 }

            case token_t::symbol : return parse_symbol();

            #ifndef exprtk_disable_string_capabilities
            case token_t::string :
                                 {
                                    expression_node_ptr literal_exp = expression_generator_(current_token_.value);
                                    next_token();
                                    return literal_exp;
                                 }
            #endif

            case '(' :
                                 {
                                    next_token();
                                    expression_node_ptr branch = parse_expression();
                                    if (token_is(token_t::rbracket))
                                       return branch;
                                    else
                                       return error_node();
                                 }
            case '[' :
                                 {
                                    next_token();
                                    expression_node_ptr branch = parse_expression();
                                    if (token_is(token_t::rsqrbracket))
                                       return branch;
                                    else
                                       return error_node();
                                 }
            case '{' :
                                 {
                                    next_token();
                                    expression_node_ptr branch = parse_expression();
                                    if (token_is(token_t::rcrlbracket))
                                       return branch;
                                    else
                                       return error_node();
                                 }
            case '-' :
                                 {
                                    next_token();
                                    return expression_generator_(details::e_neg,parse_expression(e_level09));
                                 }
            case '+' :
                                 {
                                    next_token();
                                    return expression_generator_(details::e_pos,parse_expression(e_level09));
                                 }
            case token_t::eof :
                                 {
                                    set_error("parser::parse_branch() - expected a valid branch [1]");
                                    return error_node();
                                 }
            default :
                                 {
                                    set_error("parser::parse_branch() - expected a valid branch [2]");
                                    return error_node();
                                 }
         }
      }

      inline bool token_is(const typename token_t::token_type& ttype)
      {
         if (current_token_.type != ttype)
         {
            if (!((']' == current_token_.type) && (token_t::rbracket == ttype)))
            {
               set_error(std::string("parser::token_is() - expected: ") + static_cast<char>(ttype));
               return false;
            }
         }
         next_token();
         return true;
      }

      template <typename Type>
      class expression_generator
      {
      public:

         typedef details::expression_node<Type>* expression_node_ptr;

         inline void set_optimization_level(const optimization_level& optimization_level)
         {
            optimization_level_ = optimization_level;
         }

         inline void set_allocator(details::node_allocator& na)
         {
            node_allocator_ = &na;
         }

         inline expression_node_ptr operator()(const Type& v) const
         {
            return node_allocator_->allocate<literal_node_t>(v);
         }

         inline expression_node_ptr operator()(const std::string& s) const
         {
            return node_allocator_->allocate<string_literal_node_t>(s);
         }

         inline expression_node_ptr operator()(const details::operator_type& operation, expression_node_ptr (&branch)[1])
         {
            if (0 != branch[0])
               return synthesize_expression<unary_node_t,1>(operation,branch);
            else
               return error_node();
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
                   (details::e_xor  == operation);
         }

         inline bool is_level_optimizable(const std::size_t& level = 0) const
         {
            switch (level)
            {
               case 1  : return (e_level1 == (optimization_level_ & e_level1));
               case 2  : return (e_level2 == (optimization_level_ & e_level2));
               case 3  : return (e_level2 == (optimization_level_ & e_level2));
               case 0  : return (e_all    == (optimization_level_ &    e_all));
               default : return false;
            }
         }

         inline bool cov_optimizable(const details::operator_type& operation, expression_node_ptr (&branch)[2]) const
         {
            if (!operation_optimizable(operation))
               return false;
            else if (!is_level_optimizable(1))
               return false;
            else
               return (details::is_constant_node(branch[0]) && details::is_variable_node(branch[1]));
         }

         inline bool voc_optimizable(const details::operator_type& operation, expression_node_ptr (&branch)[2]) const
         {
            if (!operation_optimizable(operation))
               return false;
            else if (!is_level_optimizable(1))
               return false;
            else
               return (details::is_variable_node(branch[0]) && details::is_constant_node(branch[1]));
         }

         inline bool vov_optimizable(const details::operator_type& operation, expression_node_ptr (&branch)[2]) const
         {
            if (!operation_optimizable(operation))
               return false;
            else if (!is_level_optimizable(1))
               return false;
            else
               return (details::is_variable_node(branch[0]) && details::is_variable_node(branch[1]));
         }

         inline bool vovov1_optimizable(const details::operator_type& operation, expression_node_ptr (&branch)[2]) const
         {
            if (!operation_optimizable(operation))
               return false;
            else if (!is_level_optimizable(2))
               return false;
            else
               return (details::is_variable_node(branch[0]) && details::is_vov_node(branch[1]));
         }

         inline bool vovov2_optimizable(const details::operator_type& operation, expression_node_ptr (&branch)[2]) const
         {
            if (!operation_optimizable(operation))
               return false;
            else if (!is_level_optimizable(2))
               return false;
            else
               return (details::is_vov_node(branch[0]) && details::is_variable_node(branch[1]));
         }

         inline bool covov1_optimizable(const details::operator_type& operation, expression_node_ptr (&branch)[2]) const
         {
            if (!operation_optimizable(operation))
               return false;
            else if (!is_level_optimizable(2))
               return false;
            else
               return (details::is_constant_node(branch[0]) && details::is_vov_node(branch[1]));
         }

         inline bool covov2_optimizable(const details::operator_type& operation, expression_node_ptr (&branch)[2]) const
         {
            if (!operation_optimizable(operation))
               return false;
            else if (!is_level_optimizable(2))
               return false;
            else
               return (details::is_cov_node(branch[0]) && details::is_variable_node(branch[1]));
         }

         inline bool is_invalid_string_op(const details::operator_type& operation, expression_node_ptr (&branch)[2])
         {
            bool b0_string = details::is_string_node(branch[0]) || details::is_const_string_node(branch[0]);
            bool b1_string = details::is_string_node(branch[1]) || details::is_const_string_node(branch[1]);
            if ((b0_string || b1_string) && !(b0_string && b1_string))
               return true;
            if (!valid_string_operation(operation) && b0_string && b1_string)
               return true;
            else
               return false;
         }

         inline bool is_string_operation(const details::operator_type& operation, expression_node_ptr (&branch)[2])
         {
            bool b0_string = details::is_string_node(branch[0]) || details::is_const_string_node(branch[0]);
            bool b1_string = details::is_string_node(branch[1]) || details::is_const_string_node(branch[1]);
            return (b0_string && b1_string && valid_string_operation(operation));
         }

         // Note: Extended Optimisations
         // When using older C++ compilers due to the large number of type instantiations
         // required by the extended optimisations the compiler may crash or not be able
         // to compile this file properly.
         #define exprtk_disable_extended_optimisations
         #if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
            #if (defined(_MSC_VER) && (_MSC_VER <= 1400))
               #ifndef exprtk_disable_extended_optimisations
                  #define exprtk_disable_extended_optimisations
               #endif
            #endif
         #endif

         #ifndef exprtk_disable_extended_optimisations
         inline bool vovovov_optimizable(const details::operator_type& operation, expression_node_ptr (&branch)[2]) const
         {
            if (!operation_optimizable(operation))
               return false;
            else if (!is_level_optimizable(3))
               return false;
            else
               return (details::is_vov_node(branch[0]) && details::is_vov_node(branch[1]));
         }
         #else
         inline bool vovovov_optimizable(const details::operator_type&, expression_node_ptr (&)[2]) const
         {
            return false;
         }
         #endif

         inline expression_node_ptr operator()(const details::operator_type& operation, expression_node_ptr (&branch)[2])
         {
            if ((0 == branch[0]) || (0 == branch[1]))
               return error_node();
            else if (is_invalid_string_op(operation,branch))
               return error_node();
            else if (details::e_assign == operation)
               return synthesize_assignment_expression(operation,branch);
            else if (is_string_operation(operation,branch))
               return synthesize_string_expression(operation,branch);
            else if (cov_optimizable(operation,branch))
               return synthesize_cov_expression(operation,branch);
            else if (voc_optimizable(operation,branch))
               return synthesize_voc_expression(operation,branch);
            else if (vov_optimizable(operation,branch))
               return synthesize_vov_expression(operation,branch);
            else if (vovov1_optimizable(operation,branch))
               return synthesize_vovov1_expression(operation,branch);
            else if (vovov2_optimizable(operation,branch))
               return synthesize_vovov2_expression(operation,branch);
            else if (covov1_optimizable(operation,branch))
               return synthesize_covov1_expression(operation,branch);
            else if (covov2_optimizable(operation,branch))
               return synthesize_covov2_expression(operation,branch);
            else if (vovovov_optimizable(operation,branch))
               return synthesize_vovovov_expression(operation,branch);
            else
               return synthesize_expression<binary_node_t,2>(operation,branch);
         }

         inline expression_node_ptr operator()(const details::operator_type& operation, expression_node_ptr (&branch)[3])
         {
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
            expression_node_ptr branch[2] = { b0, b1 };
            if ((0 == b0) || (0 == b1))
               return error_node();
            else
               return expression_generator<Type>::operator()(operation,branch);
         }

         inline expression_node_ptr conditional(expression_node_ptr condition,
                                                expression_node_ptr consequent,
                                                expression_node_ptr alternative) const
         {
            //Can the condition be immediately evaluated? if so optimize.
            if ((0 == condition) || (0 == consequent) || (0 == alternative))
               return error_node();
            else if (details::is_constant_node(condition))
            {
               // True branch
               if (Type(0) != condition->value())
               {
                  node_allocator_->free(condition);
                  node_allocator_->free(alternative);
                  return consequent;
               }
               // False branch
               else
               {
                  node_allocator_->free(condition);
                  node_allocator_->free(consequent);
                  return alternative;
               }
            }
            else
               return node_allocator_->allocate<conditional_node_t>(condition,consequent,alternative);
         }

         inline expression_node_ptr while_loop(expression_node_ptr condition,
                                               expression_node_ptr branch) const
         {
            return node_allocator_->allocate<while_loop_node_t>(condition,branch);
         }

         inline expression_node_ptr special_function(const details::operator_type& operation, expression_node_ptr (&branch)[3])
         {
            if (!all_nodes_valid(branch))
               return error_node();
            switch (operation)
            {
               #define case_stmt(op0,op1) case op0 : return node_allocator_->allocate<details::sf3_node<Type,op1<Type> > >(operation,branch);
               case_stmt(details::e_sf00,details::sf00_op)
               case_stmt(details::e_sf01,details::sf01_op)
               case_stmt(details::e_sf02,details::sf02_op)
               case_stmt(details::e_sf03,details::sf03_op)
               case_stmt(details::e_sf04,details::sf04_op)
               case_stmt(details::e_sf05,details::sf05_op)
               case_stmt(details::e_sf06,details::sf06_op)
               case_stmt(details::e_sf07,details::sf07_op)
               case_stmt(details::e_sf08,details::sf08_op)
               case_stmt(details::e_sf09,details::sf09_op)
               case_stmt(details::e_sf10,details::sf10_op)
               case_stmt(details::e_sf11,details::sf11_op)
               case_stmt(details::e_sf12,details::sf12_op)
               case_stmt(details::e_sf13,details::sf13_op)
               case_stmt(details::e_sf14,details::sf14_op)
               case_stmt(details::e_sf15,details::sf15_op)
               case_stmt(details::e_sf16,details::sf16_op)
               case_stmt(details::e_sf17,details::sf17_op)
               default : return error_node();
               #undef case_stmt
            }
         }

         inline expression_node_ptr special_function(const details::operator_type& operation, expression_node_ptr (&branch)[4])
         {
            if (!all_nodes_valid(branch))
               return error_node();
            switch (operation)
            {
               #define case_stmt(op0,op1) case op0 : return node_allocator_->allocate<details::sf4_node<Type,op1<Type> > >(operation,branch);
               case_stmt(details::e_sf18,details::sf18_op)
               case_stmt(details::e_sf19,details::sf19_op)
               case_stmt(details::e_sf20,details::sf20_op)
               case_stmt(details::e_sf21,details::sf21_op)
               case_stmt(details::e_sf22,details::sf22_op)
               case_stmt(details::e_sf23,details::sf23_op)
               case_stmt(details::e_sf24,details::sf24_op)
               case_stmt(details::e_sf25,details::sf25_op)
               case_stmt(details::e_sf26,details::sf26_op)
               case_stmt(details::e_sf27,details::sf27_op)
               case_stmt(details::e_sf28,details::sf28_op)
               case_stmt(details::e_sf29,details::sf29_op)
               case_stmt(details::e_sf30,details::sf30_op)
               case_stmt(details::e_sf31,details::sf31_op)
               case_stmt(details::e_sf32,details::sf32_op)
               case_stmt(details::e_sf33,details::sf33_op)
               case_stmt(details::e_sf34,details::sf34_op)
               case_stmt(details::e_sf35,details::sf35_op)
               case_stmt(details::e_sf36,details::sf36_op)
               case_stmt(details::e_sf37,details::sf37_op)
               case_stmt(details::e_sf38,details::sf38_op)
               case_stmt(details::e_sf39,details::sf39_op)
               case_stmt(details::e_sf40,details::sf40_op)
               case_stmt(details::e_sf41,details::sf41_op)
               case_stmt(details::e_sf42,details::sf42_op)
               default : return error_node();
               #undef case_stmt
            }
         }

         template <std::size_t N>
         inline expression_node_ptr function(typename function_node_t::ifunction* f, expression_node_ptr (&b)[N])
         {
            expression_node_ptr result = synthesize_expression<function_node_t,N>(f,b);
            if (0 == result)
               return error_node();
            else
            {
               if (!all_nodes_valid(b))
                  return error_node();
               else if (N != f->param_count)
                  return error_node();
               function_node_t* func_node_ptr = dynamic_cast<function_node_t*>(result);
               if (func_node_ptr)
               {
                  if (func_node_ptr->init_branches(b))
                     return result;
                  else
                     return error_node();
               }
               else
                  return error_node();
            }
         }

         inline expression_node_ptr function(typename function_node_t::ifunction* f)
         {
            return node_allocator_->allocate<function_node_t>(f);
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

         template <std::size_t N>
         inline bool all_nodes_valid(expression_node_ptr (&b)[N]) const
         {
            for (std::size_t i = 0; i < N; ++i)
            {
               if (0 == b[i]) return false;
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

         inline expression_node_ptr synthesize_cov_expression(const details::operator_type& operation, expression_node_ptr (&branch)[2])
         {
            T  c = dynamic_cast<details::literal_node<T>* >(branch[0])->value();
            T& v = dynamic_cast<details::variable_node<T>*>(branch[1])->ref();
            node_allocator_->free(branch[0]);
            switch (operation)
            {
               #define case_stmt(op0,op1) case op0 : return node_allocator_->allocate_cr<typename details::cov_node<Type,op1<Type> > >(c,v);
               case_stmt(details:: e_add,details:: add_op)
               case_stmt(details:: e_sub,details:: sub_op)
               case_stmt(details:: e_mul,details:: mul_op)
               case_stmt(details:: e_div,details:: div_op)
               case_stmt(details:: e_mod,details:: mod_op)
               case_stmt(details:: e_pow,details:: pow_op)
               case_stmt(details::  e_lt,details::  lt_op)
               case_stmt(details:: e_lte,details:: lte_op)
               case_stmt(details::  e_gt,details::  gt_op)
               case_stmt(details:: e_gte,details:: gte_op)
               case_stmt(details::  e_eq,details::  eq_op)
               case_stmt(details::  e_ne,details::  ne_op)
               case_stmt(details:: e_and,details:: and_op)
               case_stmt(details::e_nand,details::nand_op)
               case_stmt(details::  e_or,details::  or_op)
               case_stmt(details:: e_nor,details:: nor_op)
               case_stmt(details:: e_xor,details:: xor_op)
               default : return error_node();
               #undef case_stmt
            }
         }

         inline expression_node_ptr synthesize_voc_expression(const details::operator_type& operation, expression_node_ptr (&branch)[2])
         {
            T& v = dynamic_cast<details::variable_node<T>*>(branch[0])->ref();
            T  c = dynamic_cast<details::literal_node<T>* >(branch[1])->value();
            node_allocator_->free(branch[1]);
            switch (operation)
            {
               #define case_stmt(op0,op1) case op0 : return node_allocator_->allocate_rc<typename details::voc_node<Type,op1<Type> > >(v,c);
               case_stmt(details:: e_add,details:: add_op)
               case_stmt(details:: e_sub,details:: sub_op)
               case_stmt(details:: e_mul,details:: mul_op)
               case_stmt(details:: e_div,details:: div_op)
               case_stmt(details:: e_mod,details:: mod_op)
               case_stmt(details:: e_pow,details:: pow_op)
               case_stmt(details::  e_lt,details::  lt_op)
               case_stmt(details:: e_lte,details:: lte_op)
               case_stmt(details::  e_gt,details::  gt_op)
               case_stmt(details:: e_gte,details:: gte_op)
               case_stmt(details::  e_eq,details::  eq_op)
               case_stmt(details::  e_ne,details::  ne_op)
               case_stmt(details:: e_and,details:: and_op)
               case_stmt(details::e_nand,details::nand_op)
               case_stmt(details::  e_or,details::  or_op)
               case_stmt(details:: e_nor,details:: nor_op)
               case_stmt(details:: e_xor,details:: xor_op)
               default : return error_node();
               #undef case_stmt
            }
         }

         inline expression_node_ptr synthesize_vov_expression(const details::operator_type& operation, expression_node_ptr (&branch)[2])
         {
            Type& v1 = dynamic_cast<details::variable_node<Type>*>(branch[0])->ref();
            Type& v2 = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
            switch (operation)
            {
               #define case_stmt(op0,op1) case op0 : return node_allocator_->allocate_rr<typename details::vov_node<Type,op1<Type> > >(v1,v2);
               case_stmt(details:: e_add,details:: add_op)
               case_stmt(details:: e_sub,details:: sub_op)
               case_stmt(details:: e_mul,details:: mul_op)
               case_stmt(details:: e_div,details:: div_op)
               case_stmt(details:: e_mod,details:: mod_op)
               case_stmt(details:: e_pow,details:: pow_op)
               case_stmt(details::  e_lt,details::  lt_op)
               case_stmt(details:: e_lte,details:: lte_op)
               case_stmt(details::  e_gt,details::  gt_op)
               case_stmt(details:: e_gte,details:: gte_op)
               case_stmt(details::  e_eq,details::  eq_op)
               case_stmt(details::  e_ne,details::  ne_op)
               case_stmt(details:: e_and,details:: and_op)
               case_stmt(details::e_nand,details::nand_op)
               case_stmt(details::  e_or,details::  or_op)
               case_stmt(details:: e_nor,details:: nor_op)
               case_stmt(details:: e_xor,details:: xor_op)
               default : return error_node();
               #undef case_stmt
            }
         }

         template <typename Operation1>
         inline expression_node_ptr synthesize_vovov1_expression_impl(Type& v0, expression_node_ptr node)
         {
            details::operator_type op = dynamic_cast<details::vov_base_node<Type>*>(node)->operation();
            switch (op)
            {
               #define case_stmt(op0,op1) case op0 : return node_allocator_->allocate_rr<typename details::vovov1_node<Type,Operation1,op1<Type> > >(v0,(*dynamic_cast<details::vov_node<T,op1<Type> >*>(node)));
               case_stmt(details:: e_add,details:: add_op)
               case_stmt(details:: e_sub,details:: sub_op)
               case_stmt(details:: e_mul,details:: mul_op)
               case_stmt(details:: e_div,details:: div_op)
               case_stmt(details:: e_mod,details:: mod_op)
               case_stmt(details:: e_pow,details:: pow_op)
               case_stmt(details::  e_lt,details::  lt_op)
               case_stmt(details:: e_lte,details:: lte_op)
               case_stmt(details::  e_gt,details::  gt_op)
               case_stmt(details:: e_gte,details:: gte_op)
               case_stmt(details::  e_eq,details::  eq_op)
               case_stmt(details::  e_ne,details::  ne_op)
               case_stmt(details:: e_and,details:: and_op)
               case_stmt(details::e_nand,details::nand_op)
               case_stmt(details::  e_or,details::  or_op)
               case_stmt(details:: e_nor,details:: nor_op)
               case_stmt(details:: e_xor,details:: xor_op)
               default : return error_node();
               #undef case_stmt
            }
         }

         inline expression_node_ptr synthesize_vovov1_expression(const details::operator_type& operation, expression_node_ptr (&branch)[2])
         {
            T& v0 = dynamic_cast<details::variable_node<T>*>(branch[0])->ref();
            expression_node_ptr result = error_node();
            switch (operation)
            {
               #define case_stmt(op0,op1) case op0 : result = synthesize_vovov1_expression_impl<op1<Type> >(v0,branch[1]); break;
               case_stmt(details:: e_add,details:: add_op)
               case_stmt(details:: e_sub,details:: sub_op)
               case_stmt(details:: e_mul,details:: mul_op)
               case_stmt(details:: e_div,details:: div_op)
               case_stmt(details:: e_mod,details:: mod_op)
               case_stmt(details:: e_pow,details:: pow_op)
               case_stmt(details::  e_lt,details::  lt_op)
               case_stmt(details:: e_lte,details:: lte_op)
               case_stmt(details::  e_gt,details::  gt_op)
               case_stmt(details:: e_gte,details:: gte_op)
               case_stmt(details::  e_eq,details::  eq_op)
               case_stmt(details::  e_ne,details::  ne_op)
               case_stmt(details:: e_and,details:: and_op)
               case_stmt(details::e_nand,details::nand_op)
               case_stmt(details::  e_or,details::  or_op)
               case_stmt(details:: e_nor,details:: nor_op)
               case_stmt(details:: e_xor,details:: xor_op)
               default : return error_node();
               #undef case_stmt
            }
            node_allocator_->free(branch[1]);
            return result;
         }

         template <typename Operation1>
         inline expression_node_ptr synthesize_vovov2_expression_impl(T& v0, expression_node_ptr node)
         {
            details::operator_type op = dynamic_cast<details::vov_base_node<T>*>(node)->operation();
            switch (op)
            {
               #define case_stmt(op0,op1) case op0 : return node_allocator_->allocate_rr<typename details::vovov2_node<T,op1<T>,Operation1> >((*dynamic_cast<details::vov_node<T,op1<T> >*>(node)),v0);
               case_stmt(details:: e_add,details:: add_op)
               case_stmt(details:: e_sub,details:: sub_op)
               case_stmt(details:: e_mul,details:: mul_op)
               case_stmt(details:: e_div,details:: div_op)
               case_stmt(details:: e_mod,details:: mod_op)
               case_stmt(details:: e_pow,details:: pow_op)
               case_stmt(details::  e_lt,details::  lt_op)
               case_stmt(details:: e_lte,details:: lte_op)
               case_stmt(details::  e_gt,details::  gt_op)
               case_stmt(details:: e_gte,details:: gte_op)
               case_stmt(details::  e_eq,details::  eq_op)
               case_stmt(details::  e_ne,details::  ne_op)
               case_stmt(details:: e_and,details:: and_op)
               case_stmt(details::e_nand,details::nand_op)
               case_stmt(details::  e_or,details::  or_op)
               case_stmt(details:: e_nor,details:: nor_op)
               case_stmt(details:: e_xor,details:: xor_op)
               default : return error_node();
               #undef case_stmt
            }
         }

         inline expression_node_ptr synthesize_vovov2_expression(const details::operator_type& operation, expression_node_ptr (&branch)[2])
         {
            T& v0 = dynamic_cast<details::variable_node<T>*>(branch[1])->ref();
            expression_node_ptr result = error_node();
            switch (operation)
            {
               #define case_stmt(op0,op1) case op0 : result = synthesize_vovov2_expression_impl<op1<Type> >(v0,branch[0]); break;
               case_stmt(details:: e_add,details:: add_op)
               case_stmt(details:: e_sub,details:: sub_op)
               case_stmt(details:: e_mul,details:: mul_op)
               case_stmt(details:: e_div,details:: div_op)
               case_stmt(details:: e_mod,details:: mod_op)
               case_stmt(details:: e_pow,details:: pow_op)
               case_stmt(details::  e_lt,details::  lt_op)
               case_stmt(details:: e_lte,details:: lte_op)
               case_stmt(details::  e_gt,details::  gt_op)
               case_stmt(details:: e_gte,details:: gte_op)
               case_stmt(details::  e_eq,details::  eq_op)
               case_stmt(details::  e_ne,details::  ne_op)
               case_stmt(details:: e_and,details:: and_op)
               case_stmt(details::e_nand,details::nand_op)
               case_stmt(details::  e_or,details::  or_op)
               case_stmt(details:: e_nor,details:: nor_op)
               case_stmt(details:: e_xor,details:: xor_op)
               default : return error_node();
               #undef case_stmt
            }
            node_allocator_->free(branch[0]);
            return result;
         }

         template <typename Operation1>
         inline expression_node_ptr synthesize_covov1_expression_impl(const T& c, expression_node_ptr node)
         {
            details::operator_type op = dynamic_cast<details::vov_base_node<T>*>(node)->operation();
            switch (op)
            {
               #define case_stmt(op0,op1) case op0 : return node_allocator_->allocate_rr<typename details::covov1_node<Type,Operation1,op1<Type> > >(c,(*dynamic_cast<details::vov_node<T,op1<T> >*>(node)));
               case_stmt(details:: e_add,details:: add_op)
               case_stmt(details:: e_sub,details:: sub_op)
               case_stmt(details:: e_mul,details:: mul_op)
               case_stmt(details:: e_div,details:: div_op)
               case_stmt(details:: e_mod,details:: mod_op)
               case_stmt(details:: e_pow,details:: pow_op)
               case_stmt(details::  e_lt,details::  lt_op)
               case_stmt(details:: e_lte,details:: lte_op)
               case_stmt(details::  e_gt,details::  gt_op)
               case_stmt(details:: e_gte,details:: gte_op)
               case_stmt(details::  e_eq,details::  eq_op)
               case_stmt(details::  e_ne,details::  ne_op)
               case_stmt(details:: e_and,details:: and_op)
               case_stmt(details::e_nand,details::nand_op)
               case_stmt(details::  e_or,details::  or_op)
               case_stmt(details:: e_nor,details:: nor_op)
               case_stmt(details:: e_xor,details:: xor_op)
               default : return error_node();
               #undef case_stmt
            }
         }

         inline expression_node_ptr synthesize_covov1_expression(const details::operator_type& operation, expression_node_ptr (&branch)[2])
         {
            T c = dynamic_cast<details::literal_node<Type>*>(branch[0])->value();
            expression_node_ptr result = error_node();
            switch (operation)
            {
               #define case_stmt(op0,op1) case op0 : result = synthesize_covov1_expression_impl<op1<Type> >(c,branch[1]); break;
               case_stmt(details:: e_add,details:: add_op)
               case_stmt(details:: e_sub,details:: sub_op)
               case_stmt(details:: e_mul,details:: mul_op)
               case_stmt(details:: e_div,details:: div_op)
               case_stmt(details:: e_mod,details:: mod_op)
               case_stmt(details:: e_pow,details:: pow_op)
               case_stmt(details::  e_lt,details::  lt_op)
               case_stmt(details:: e_lte,details:: lte_op)
               case_stmt(details::  e_gt,details::  gt_op)
               case_stmt(details:: e_gte,details:: gte_op)
               case_stmt(details::  e_eq,details::  eq_op)
               case_stmt(details::  e_ne,details::  ne_op)
               case_stmt(details:: e_and,details:: and_op)
               case_stmt(details::e_nand,details::nand_op)
               case_stmt(details::  e_or,details::  or_op)
               case_stmt(details:: e_nor,details:: nor_op)
               case_stmt(details:: e_xor,details:: xor_op)
               default : return error_node();
               #undef case_stmt
            }
            node_allocator_->free(branch[1]);
            return result;
         }

         template <typename Operation2>
         inline expression_node_ptr synthesize_covov2_expression_impl(expression_node_ptr node, T& v)
         {
            details::operator_type op = dynamic_cast<details::cov_base_node<T>*>(node)->operation();
            switch (op)
            {
               #define case_stmt(op0,op1) case op0 : return node_allocator_->allocate_rr<typename details::covov2_node<Type,op1<T>,Operation2> >((*dynamic_cast<details::cov_node<T,op1<T> >*>(node)),v);
               case_stmt(details:: e_add,details:: add_op)
               case_stmt(details:: e_sub,details:: sub_op)
               case_stmt(details:: e_mul,details:: mul_op)
               case_stmt(details:: e_div,details:: div_op)
               case_stmt(details:: e_mod,details:: mod_op)
               case_stmt(details:: e_pow,details:: pow_op)
               case_stmt(details::  e_lt,details::  lt_op)
               case_stmt(details:: e_lte,details:: lte_op)
               case_stmt(details::  e_gt,details::  gt_op)
               case_stmt(details:: e_gte,details:: gte_op)
               case_stmt(details::  e_eq,details::  eq_op)
               case_stmt(details::  e_ne,details::  ne_op)
               case_stmt(details:: e_and,details:: and_op)
               case_stmt(details::e_nand,details::nand_op)
               case_stmt(details::  e_or,details::  or_op)
               case_stmt(details:: e_nor,details:: nor_op)
               case_stmt(details:: e_xor,details:: xor_op)
               default : return error_node();
               #undef case_stmt
            }
         }

         inline expression_node_ptr synthesize_covov2_expression(const details::operator_type& operation, expression_node_ptr (&branch)[2])
         {
            T& v = dynamic_cast<details::variable_node<Type>*>(branch[1])->ref();
            expression_node_ptr result = error_node();
            switch (operation)
            {
               #define case_stmt(op0,op1) case op0 : result = synthesize_covov2_expression_impl<op1<Type> >(branch[0],v); break;
               case_stmt(details:: e_add,details:: add_op)
               case_stmt(details:: e_sub,details:: sub_op)
               case_stmt(details:: e_mul,details:: mul_op)
               case_stmt(details:: e_div,details:: div_op)
               case_stmt(details:: e_mod,details:: mod_op)
               case_stmt(details:: e_pow,details:: pow_op)
               case_stmt(details::  e_lt,details::  lt_op)
               case_stmt(details:: e_lte,details:: lte_op)
               case_stmt(details::  e_gt,details::  gt_op)
               case_stmt(details:: e_gte,details:: gte_op)
               case_stmt(details::  e_eq,details::  eq_op)
               case_stmt(details::  e_ne,details::  ne_op)
               case_stmt(details:: e_and,details:: and_op)
               case_stmt(details::e_nand,details::nand_op)
               case_stmt(details::  e_or,details::  or_op)
               case_stmt(details:: e_nor,details:: nor_op)
               case_stmt(details:: e_xor,details:: xor_op)
               default : return error_node();
               #undef case_stmt
            }
            node_allocator_->free(branch[0]);
            return result;
         }

         #ifndef exprtk_disable_extended_optimisations
         template <typename Op1, typename Op2>
         inline expression_node_ptr synthesize_vovovov_expression_impl2(expression_node_ptr (&node)[2])
         {
            details::operator_type op = dynamic_cast<details::vov_base_node<T>*>(node[1])->operation();
            switch (op)
            {
               #define case_stmt(op0,op1) case op0 : return node_allocator_->allocate_rr<typename details::vovovov_node<Type,Op1,Op2,op1<T> > >((*dynamic_cast<details::vov_node<T,Op1>*>(node[0])),(*dynamic_cast<details::vov_node<T,op1<T> >*>(node[1])));
               case_stmt(details:: e_add,details:: add_op)
               case_stmt(details:: e_sub,details:: sub_op)
               case_stmt(details:: e_mul,details:: mul_op)
               case_stmt(details:: e_div,details:: div_op)
               case_stmt(details:: e_mod,details:: mod_op)
               case_stmt(details:: e_pow,details:: pow_op)
               case_stmt(details::  e_lt,details::  lt_op)
               case_stmt(details:: e_lte,details:: lte_op)
               case_stmt(details::  e_gt,details::  gt_op)
               case_stmt(details:: e_gte,details:: gte_op)
               case_stmt(details::  e_eq,details::  eq_op)
               case_stmt(details::  e_ne,details::  ne_op)
               case_stmt(details:: e_and,details:: and_op)
               case_stmt(details::e_nand,details::nand_op)
               case_stmt(details::  e_or,details::  or_op)
               case_stmt(details:: e_nor,details:: nor_op)
               case_stmt(details:: e_xor,details:: xor_op)
               default : return error_node();
               #undef case_stmt
            }
         #else
         template <typename Op1, typename Op2>
         inline expression_node_ptr synthesize_vovovov_expression_impl2(expression_node_ptr (&)[2])
         {
            return error_node();
         #endif
         }

         #ifndef exprtk_disable_extended_optimisations
         template <typename Operation2>
         inline expression_node_ptr synthesize_vovovov_expression_impl1(expression_node_ptr (&node)[2])
         {
            details::operator_type op = dynamic_cast<details::vov_base_node<T>*>(node[0])->operation();
            switch (op)
            {
               #define case_stmt(op0,op1) case op0 : return synthesize_vovovov_expression_impl2<op1<T>,Operation2>(node); break;
               case_stmt(details:: e_add,details:: add_op)
               case_stmt(details:: e_sub,details:: sub_op)
               case_stmt(details:: e_mul,details:: mul_op)
               case_stmt(details:: e_div,details:: div_op)
               case_stmt(details:: e_mod,details:: mod_op)
               case_stmt(details:: e_pow,details:: pow_op)
               case_stmt(details::  e_lt,details::  lt_op)
               case_stmt(details:: e_lte,details:: lte_op)
               case_stmt(details::  e_gt,details::  gt_op)
               case_stmt(details:: e_gte,details:: gte_op)
               case_stmt(details::  e_eq,details::  eq_op)
               case_stmt(details::  e_ne,details::  ne_op)
               case_stmt(details:: e_and,details:: and_op)
               case_stmt(details::e_nand,details::nand_op)
               case_stmt(details::  e_or,details::  or_op)
               case_stmt(details:: e_nor,details:: nor_op)
               case_stmt(details:: e_xor,details:: xor_op)
               default : return error_node();
               #undef case_stmt
            }
         }
         #else
         template <typename Operation2>
         inline expression_node_ptr synthesize_vovovov_expression_impl1(expression_node_ptr (&)[2])
         {
            return error_node();
         }
         #endif

         #ifndef exprtk_disable_extended_optimisations
         inline expression_node_ptr synthesize_vovovov_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            expression_node_ptr result = error_node();
            switch (opr)
            {
               #define case_stmt(op0,op1) case op0 : result = synthesize_vovovov_expression_impl1<op1<T> >(branch); break;
               case_stmt(details:: e_add,details:: add_op)
               case_stmt(details:: e_sub,details:: sub_op)
               case_stmt(details:: e_mul,details:: mul_op)
               case_stmt(details:: e_div,details:: div_op)
               case_stmt(details:: e_mod,details:: mod_op)
               case_stmt(details:: e_pow,details:: pow_op)
               case_stmt(details::  e_lt,details::  lt_op)
               case_stmt(details:: e_lte,details:: lte_op)
               case_stmt(details::  e_gt,details::  gt_op)
               case_stmt(details:: e_gte,details:: gte_op)
               case_stmt(details::  e_eq,details::  eq_op)
               case_stmt(details::  e_ne,details::  ne_op)
               case_stmt(details:: e_and,details:: and_op)
               case_stmt(details::e_nand,details::nand_op)
               case_stmt(details::  e_or,details::  or_op)
               case_stmt(details:: e_nor,details:: nor_op)
               case_stmt(details:: e_xor,details:: xor_op)
               default : return error_node();
               #undef case_stmt
            }
            node_allocator_->free(branch[0]);
            node_allocator_->free(branch[1]);
            return result;
         }
         #else
         inline expression_node_ptr synthesize_vovovov_expression(const details::operator_type&, expression_node_ptr (&)[2])
         {
            return error_node();
         }
         #endif

         template <typename T0, typename T1>
         inline expression_node_ptr synthesize_sos_expression_impl(const details::operator_type& opr, T0 s0, T1 s1)
         {
            switch (opr)
            {
               #define case_stmt(op0,op1) case op0 : return node_allocator_->allocate_tt<typename details::sos_node<Type,T0,T1,op1<Type> >,T0,T1>(s0,s1);
               case_stmt(details::e_lt   ,details::   lt_op)
               case_stmt(details::e_lte  ,details::  lte_op)
               case_stmt(details::e_gt   ,details::   gt_op)
               case_stmt(details::e_gte  ,details::  gte_op)
               case_stmt(details::e_eq   ,details::   eq_op)
               case_stmt(details::e_ne   ,details::   ne_op)
               case_stmt(details::e_in   ,details::   in_op)
               case_stmt(details::e_like ,details:: like_op)
               case_stmt(details::e_ilike,details::ilike_op)
               default : return error_node();
               #undef case_stmt
            }
         }

         inline expression_node_ptr synthesize_sos_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            std::string& s0 = dynamic_cast<details::stringvar_node<Type>*>(branch[0])->ref();
            std::string& s1 = dynamic_cast<details::stringvar_node<Type>*>(branch[1])->ref();
            return synthesize_sos_expression_impl<std::string&,std::string&>(opr,s0,s1);
         }

         inline expression_node_ptr synthesize_socs_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            std::string& s0 = dynamic_cast<     details::stringvar_node<Type>*>(branch[0])->ref();
            std::string  s1 = dynamic_cast<details::string_literal_node<Type>*>(branch[1])->str();
            node_allocator_->free(branch[1]);
            return synthesize_sos_expression_impl<std::string&,const std::string>(opr,s0,s1);
         }

         inline expression_node_ptr synthesize_csos_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            std::string  s0 = dynamic_cast<details::string_literal_node<Type>*>(branch[0])->str();
            std::string& s1 = dynamic_cast<     details::stringvar_node<Type>*>(branch[1])->ref();
            node_allocator_->free(branch[0]);
            return synthesize_sos_expression_impl<const std::string,std::string&>(opr,s0,s1);
         }

         inline expression_node_ptr synthesize_csocs_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            std::string s0 = dynamic_cast<details::string_literal_node<Type>*>(branch[0])->str();
            std::string s1 = dynamic_cast<details::string_literal_node<Type>*>(branch[1])->str();
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
               node_allocator_->free(temp);
               result = node_allocator_->allocate<literal_node_t>(v);
            }
            node_allocator_->free(branch[0]);
            node_allocator_->free(branch[1]);
            return result;
         }

         #ifndef exprtk_disable_string_capabilities
         inline expression_node_ptr synthesize_string_expression(const details::operator_type& opr, expression_node_ptr (&branch)[2])
         {
            if (details::is_string_node(branch[0]))
            {
                    if (details::is_string_node(branch[1]))       return synthesize_sos_expression(opr,branch);
               else if (details::is_const_string_node(branch[1])) return synthesize_socs_expression(opr,branch);
            }
            else if (details::is_const_string_node(branch[0]))
            {
                    if (details::is_string_node(branch[1]))       return synthesize_csos_expression(opr,branch);
               else if (details::is_const_string_node(branch[1])) return synthesize_csocs_expression(opr,branch);
            }
            return error_node();
         }
         #else
         inline expression_node_ptr synthesize_string_expression(const details::operator_type&, expression_node_ptr (&)[2])
         {
            return error_node();
         }
         #endif

         template <typename NodeType, std::size_t N>
         inline expression_node_ptr synthesize_expression(const details::operator_type& operation, expression_node_ptr (&branch)[N])
         {
            if ((details::e_in == operation) || (details::e_like == operation) || (details::e_ilike == operation))
               return error_node();
            else if ((details::e_default != operation) && all_nodes_valid<N>(branch))
            {
               //Attempt simple constant folding optimization.
               expression_node_ptr expression_point = node_allocator_->allocate<NodeType>(operation,branch);
               if (is_constant_foldable<N>(branch))
               {
                  Type v = expression_point->value();
                  node_allocator_->free(expression_point);
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
            if (all_nodes_valid<N>(branch))
            {
               //Attempt simple constant folding optimization.
               expression_node_ptr expression_point = node_allocator_->allocate<NodeType>(f);
               dynamic_cast<function_node_t*>(expression_point)->init_branches(branch);
               if (is_constant_foldable<N>(branch))
               {
                  Type v = expression_point->value();
                  node_allocator_->free(expression_point);
                  return node_allocator_->allocate<literal_node_t>(v);
               }
               else
                  return expression_point;
            }
            else
               return error_node();
         }

         details::node_allocator* node_allocator_;
         optimization_level optimization_level_;
      };

      template <typename Type>
      class expression_optimizer
      {
      public:

         typedef details::expression_node<Type>* expression_node_ptr;

         inline void set_allocator(details::node_allocator& na)
         {
            node_allocator_ = &na;
         }

         inline void process(expression_node_ptr expr)
         {
            std::deque<expression_node_ptr> node_list;
            node_list.push_back(expr);
            expression_node_ptr node = reinterpret_cast<expression_node_ptr>(0);
            while (!node_list.empty())
            {
               node = node_list.front();
               node_list.pop_front();
               if (details::is_constant_node(node) || details::is_variable_node(node))
                  continue;
               else if (details::is_binary_node(node) && bn_type0_optimizable(node))
                  perform_bn_type0_optimization(node);
               else
                  add_sub_branches(node,node_list);
            }
         }

      private:

         template <typename Allocator,
                   template <typename, typename> class Sequence>
         void add_sub_branches(expression_node_ptr node, Sequence<expression_node_ptr,Allocator>& seq)
         {
            if (details::is_unary_node(node))
               seq.push_back(node->branch(0));
            else if (details::is_binary_node(node))
            {
               seq.push_back(node->branch(0));
               seq.push_back(node->branch(1));
            }
         }

         inline bool operation_optimizable(const details::operator_type& operation) const
         {
            return (details::e_add  == operation) || (details::e_sub  == operation) ||
                   (details::e_mul  == operation) || (details::e_div  == operation) ||
                   (details::e_mod  == operation) || (details::e_pow  == operation) ||
                   (details::e_lt   == operation) || (details::e_lte  == operation) ||
                   (details::e_gt   == operation) || (details::e_gte  == operation) ||
                   (details::e_eq   == operation) || (details::e_ne   == operation) ||
                   (details::e_and  == operation) || (details::e_nand == operation) ||
                   (details::e_or   == operation) || (details::e_nor  == operation) ||
                   (details::e_xor  == operation);
         }

         inline bool bn_type0_optimizable(const expression_node_ptr expr) const
         {
            const details::binary_node<Type>* node = dynamic_cast<const details::binary_node<Type>*>(expr);
            if (0 == node)
               return false;
            else if (!operation_optimizable(node->operation()))
               return false;
            expression_node_ptr b0 = node->branch(0);
            expression_node_ptr b1 = node->branch(1);
            if (details::is_variable_node(b0) && (details::is_variable_node(b1) || details::is_constant_node(b1)))
               return true;
            else if (details::is_constant_node(b0) && (details::is_variable_node(b1) || details::is_constant_node(b1)))
               return true;
            else
               return false;
         }

         inline void perform_bn_type0_optimization(const expression_node_ptr)
         {
         }

      private:

         details::node_allocator* node_allocator_;

      };

      inline bool check0(const char c0, const char c1,
                         const char v0, const char v1,
                         const char v2)
      {
         return ((c0 == v0) && ((c1 == v1) || (c1 == v2)));
      }

      inline bool check1(const char c0, const char c1,
                         const char v0, const char v1)
      {
         return ((c0 == v0) && (c1 == v1));
      }

      inline bool validate_expression(const std::string& expression_string)
      {
         if (expression_string.empty())
         {
            set_error("parser::validate_expression() - empty expression");
            return false;
         }

         //Check for certain illegal sequences of characters.
         std::stack<char> bracket_stack;
         for (std::size_t i = 0; i < (expression_string.size() - 1); ++i)
         {
            char c0 = expression_string[i];
            char c1 = expression_string[i + 1];
            if (details::is_invalid(c0))
            {
               set_error(std::string("parser::validate_expression() - invalid character: ") + c0);
               return false;
            }
            else if (
                     check0(c0,c1,'*','*','/') || check0(c0,c1,'*','%','^') ||
                     check0(c0,c1,'/','*','/') || check0(c0,c1,'/','%','^') ||
                     check0(c0,c1,'+','*','/') || check0(c0,c1,'+','%','^') ||
                     check0(c0,c1,'-','*','/') || check0(c0,c1,'-','%','^') ||
                     check0(c0,c1,'^','*','/') || check0(c0,c1,'^','^','%') ||
                     check0(c0,c1,'%','*','/') || check0(c0,c1,'%','^','%') ||
                     check0(c0,c1,'.','%','^') || check0(c0,c1,'.','*','/') ||
                     check0(c0,c1,',','%','^') || check0(c0,c1,',','*','/') ||
                     check0(c0,c1,'(','*','/') || check0(c0,c1,'(','%','^') ||
                     check0(c0,c1,'[','*','/') || check0(c0,c1,'[','%','^') ||
                     check0(c0,c1,'{','*','/') || check0(c0,c1,'{','%','^') ||
                     check0(c0,c1,'+',')',']') || check0(c0,c1,'-',')',']') ||
                     check0(c0,c1,'*',')',']') || check0(c0,c1,'/',')',']') ||
                     check0(c0,c1,'^',')',']') || check0(c0,c1,'%',')',']') ||
                     check1(c0,c1,'+','}'    ) || check1(c0,c1,'-','}'    ) ||
                     check1(c0,c1,'*','}'    ) || check1(c0,c1,'/','}'    ) ||
                     check1(c0,c1,'^','}'    ) || check1(c0,c1,'%','}'    ) ||
                     check1(c0,c1,'.','.'    ) || check1(c0,c1,'.','+'    ) ||
                     check1(c0,c1,'.','-'    ) || check1(c0,c1,'.','*'    ) ||
                     check1(c0,c1,'.','/'    ) || check1(c0,c1,',',','    )
                    )
            {
               set_error(std::string("parser::validate_expression() - invalid character combination: ") + expression_string.substr(i,2));
               return false;
            }
            else if (c0 == '(')
               bracket_stack.push(')');
            else if (c0 == '[')
               bracket_stack.push(']');
            else if (c0 == '{')
               bracket_stack.push('}');
            else if (details::is_right_bracket(c0))
            {
               if (bracket_stack.empty())
               {
                  set_error(std::string("parser::validate_expression() - invalid/mismatched bracket(s)[0]: ") + expression_string.substr(0,i));
                  return false;
               }
               else if (c0 != bracket_stack.top())
               {
                  set_error(std::string("parser::validate_expression() - invalid/mismatched bracket(s)[1]: ") + expression_string.substr(0,i));
                  return false;
               }
               else
                  bracket_stack.pop();
            }
         }

         if (!bracket_stack.empty())
         {
            if (1 == bracket_stack.size())
            {
               char c0 = expression_string[expression_string.size() - 1];
               if (details::is_right_bracket(c0))
               {
                  if (c0 == bracket_stack.top())
                     return true;
                  else
                  {
                     set_error(std::string("parser::validate_expression() - invalid/mismatched bracket(s)[2]: ") + expression_string);
                     return false;
                  }
               }
            }
            set_error(std::string("parser::validate_expression() - invalid/mismatched bracket(s)[3]: ") + expression_string);
            return false;
         }
         return true;
      }

      inline void set_error(const std::string& err_str)
      {
         //would it be better if this were a stack?
         if (error_description_.empty())
         {
            error_description_ = err_str;
         }
      }

   private:

      details::lexer<T> lexer_;
      details::token<T> current_token_;
      details::token<T> store_current_token_;
      expression_generator<T> expression_generator_;
      expression_optimizer<T> expression_optimizer_;
      details::node_allocator node_allocator_;
      symbol_table<T>* symbol_table_;
      std::string error_description_;
      bool symbol_name_caching_;
      std::deque<std::string> symbol_name_cache_;
   };

   template <typename T>
   inline T integrate(expression<T>& e,
                      T& x,
                      const T& r0, const T& r1,
                      const std::size_t number_of_intervals = 1000000)
   {
      if (r0 > r1) return T(0);
      T h = (r1 - r0) / (T(2.0) * number_of_intervals);
      T total_area = T(0);
      for (std::size_t i = 0; i < number_of_intervals; ++i)
      {
         x = r0 + T(2.0) * i * h;
         T y0 = e.value(); x += h;
         T y1 = e.value(); x += h;
         T y2 = e.value(); x += h;
         total_area += h * (y0 + T(4.0) * y1 + y2) / T(3.0);
      }
      return total_area;
   }

   template <typename T>
   inline T integrate(expression<T>& e,
                      const std::string& variable_name,
                      const T& r0, const T& r1,
                      const std::size_t number_of_intervals = 1000000)
   {
      symbol_table<T>* sym_table = e.get_symbol_table();
      if (0 == sym_table)
         return std::numeric_limits<T>::quiet_NaN();
      details::variable_node<T>* var = sym_table->get_variable(variable_name);
      if (var)
      {
         T& x = var->ref();
         return integrate(e,x,r0,r1,number_of_intervals);
      }
      else
         return std::numeric_limits<T>::quiet_NaN();
   }

   template <typename T>
   inline T derivative(expression<T>& e,
                       T& x,
                       const double& h = 0.00001)
   {
      T x_init = x;
      x = x_init + T(2.0) * h;
      T y0 = e.value();
      x = x_init + h;
      T y1 = e.value();
      x = x_init - h;
      T y2 = e.value();
      x = x_init - T(2.0) * h;
      T y3 = e.value();
      x = x_init;
      return (-y0 + T(8.0) * (y1 - y2) + y3) / (T(12.0) * h);
   }

   template <typename T>
   inline T derivative(expression<T>& e,
                       const std::string& variable_name,
                       const double& h = 0.00001)
   {
      symbol_table<T>* sym_table = e.get_symbol_table();
      if (0 == sym_table)
         return std::numeric_limits<T>::quiet_NaN();
      details::variable_node<T>* var = sym_table->get_variable(variable_name);
      if (var)
      {
         T& x = var->ref();
         return derivative(e,x,h);
      }
      else
         return std::numeric_limits<T>::quiet_NaN();
   }

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
                                             "clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)",
                                             "max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))",
                                             "if(avg(x,y) <= x + y, x - y, x * y) + 2 * pi / x",
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
                                             "clamp(-1.0, sin(2 * pi * xx) + cos(yy / 2 * pi), +1.0)",
                                             "max(3.33, min(sqrt(1 - sin(2 * xx) + cos(pi / yy) / 3), 1.11))",
                                             "if(avg(xx,yy) <= xx + yy, xx - yy, xx * yy) + 2 * pi / xx"
                                          };
      static const std::size_t expression_list_size = sizeof(expression_list) / sizeof(std::string);

      T  x = T(0);
      T  y = T(0);
      T xx = T(0);
      T yy = T(0);

      exprtk::symbol_table<T> symbol_table;
      symbol_table.add_constants();
      symbol_table.add_variable( "x", x);
      symbol_table.add_variable( "y", y);
      symbol_table.add_variable("xx",xx);
      symbol_table.add_variable("yy",yy);

      typedef typename std::deque<exprtk::expression<T> > expr_list_t;
      expr_list_t optimized_expr_list;
      expr_list_t unoptimized_expr_list;

      const std::size_t rounds = 100;

      //Generate optimised expressions
      {
         for (std::size_t r = 0; r < rounds; ++r)
         {
            optimized_expr_list.clear();
            exprtk::parser<T> parser;
            for (std::size_t i = 0; i < expression_list_size; ++i)
            {
               exprtk::expression<T> expression;
               expression.register_symbol_table(symbol_table);
               if (!parser.compile(expression_list[i],expression,exprtk::parser<T>::e_all))
               {
                  return false;
               }
               optimized_expr_list.push_back(expression);
            }
         }
      }

      //Generate unoptimised expressions
      {
         for (std::size_t r = 0; r < rounds; ++r)
         {
            unoptimized_expr_list.clear();
            exprtk::parser<T> parser;
            for (std::size_t i = 0; i < expression_list_size; ++i)
            {
               exprtk::expression<T> expression;
               expression.register_symbol_table(symbol_table);
               if (!parser.compile(expression_list[i],expression,exprtk::parser<T>::e_none))
               {
                  return false;
               }
               unoptimized_expr_list.push_back(expression);
            }
         }
      }

      struct execute
      {
         static inline T process(T& x, T& y, expression<T>& expression)
         {
            static const T lower_bound = T(-20.0);
            static const T upper_bound = T(+20.0);
            T delta = T(0.1);
            T total = T(0.0);
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
      for (std::size_t i = 0; i < optimized_expr_list.size(); ++i)
      {
         execute::process( x, y,  optimized_expr_list[i]);
         execute::process(xx,yy,  optimized_expr_list[i]);
         execute::process( x, y,unoptimized_expr_list[i]);
         execute::process(xx,yy,unoptimized_expr_list[i]);
      }
      return true;
   }

}

#ifdef WIN32
   #ifndef NOMINMAX
      #define NOMINMAX
   #endif
   #ifndef WIN32_LEAN_AND_MEAN
      #define WIN32_LEAN_AND_MEAN
   #endif
   #include <windows.h>
#else
   #include <sys/time.h>
   #include <sys/types.h>
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
      static const char* version = "2.718281828459045235";
      static const char* date    = "20111111";

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
