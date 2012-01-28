/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Author: Arash Partow (1999-2012)                           *
 * URL: http://www.partow.net/programming/exprtk/index.html   *
 *                                                            *
 * Copyright notice:                                          *
 * Free use of the Mathematical Expression Toolkit Library is *
 * permitted under the guidelines and in accordance with the  *
 * most current version of the Common Public License.         *
 * http://www.opensource.org/licenses/cpl1.0.php              *
 *                                                            *
 * Example expressions:                                       *
 * (00) (y+x/y)*(x-y/x)                                       *
 * (01) (x^2/sin(2*pi/y))-x/2                                 *
 * (02) sqrt(1-(x^2))                                         *
 * (03) 1-sin(2*x)+cos(pi/y)                                  *
 * (04) a*exp(2*t)+c                                          *
 * (05) if(((x+2)==3)and((y+5)<=9),1+w,2/z)                   *
 * (06) if(avg(x,y)<=x+y,x-y,x*y)+2*pi/x                      *
 * (07) z:=x+sin(2*pi/y)                                      *
 * (08) u<-2*(pi*z)/(w:=x+cos(y/pi))                          *
 * (09) clamp(-1,sin(2*pi*x)+cos(y/2*pi),+1)                  *
 * (10) inrange(-2,m,+2)==if(({-2<=m}and[m<=+2]),1,0)         *
 *                                                            *
 **************************************************************
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
                ('\t' == c);
      }

      inline bool is_operator_char(const char& c)
      {
         return ('*' == c) || ('/' == c) ||
                ('+' == c) || ('-' == c) ||
                ('^' == c) || ('<' == c) ||
                ('>' == c) || ('=' == c) ||
                (',' == c) || ('!' == c) ||
                ('(' == c) || (')' == c) ||
                ('[' == c) || (']' == c) ||
                ('{' == c) || ('}' == c) ||
                ('%' == c) || (':' == c) ;
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
                ('$' != c);
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
         else
            return false;
      }

      static const std::string reserved_symbols[] =
                                  {
                                     "abs", "acos", "and", "asin", "atan", "atan2", "avg", "ceil", "clamp",
                                     "cos", "cosh", "cot", "csc", "deg2grad", "deg2rad", "equal", "exp",
                                     "floor", "for", "grad2deg", "hyp", "inrange", "log", "log10", "logn",
                                     "max", "min", "mod", "mul", "nand", "nor", "not", "not_equal", "or",
                                     "rad2deg", "root", "round", "roundn", "sec", "shl", "shr", "sin",
                                     "sinh", "sqrt", "sum", "tan", "tanh", "while", "xor"
                                  };
      static const std::size_t reserved_symbols_size = sizeof(reserved_symbols) / sizeof(std::string);

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
            #undef exprtk_register_real_type_tag

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

         }

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
            assign      =   5,
            shr         =   6,
            shl         =   7,
            lte         =   8,
            ne          =   9,
            gte         =  10,
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

         token(token_type ttype,
               const char* begin, const char* end,
               const T& numeric_value = T(0))
         : type(ttype),
           value(std::string(begin,end)),
           numeric_value(numeric_value)
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

         inline void scan_token()
         {
            while ((s_end != s_itr) && is_whitespace(*s_itr))
            {
               ++s_itr;
            }

            if (s_end == s_itr)
            {
               return;
            }
            else if (is_operator_char(*s_itr))
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
                     token_list_.push_back(token_t(ttype,s_itr,s_itr + 2));
                     s_itr += 2;
                     return;
                  }
               }
               if ('<' == *s_itr)
                  token_list_.push_back(token_t(token_t::lt,s_itr,s_itr + 1));
               else if ('>' == *s_itr)
                  token_list_.push_back(token_t(token_t::gt,s_itr,s_itr + 1));
               else
                  token_list_.push_back(token_t(typename token_t::token_type((*s_itr)),s_itr,s_itr + 1));
               ++s_itr;
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
            if ('$' == (*s_itr))
            {
               scan_special_function();
               return;
            }
            else
            {
               set_error(std::string("scan_token() - error invalid token: ") + std::string(s_itr,s_itr + 2));
               token_list_.push_back(error(s_itr,s_itr + 1));
               ++s_itr;
            }
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
              else if (('e' == (*s_itr)) || ('E' == (*s_itr)))
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
            T nval = T(atof(std::string(begin,s_itr).c_str()));
            token_list_.push_back(token_t(token_t::number,begin,s_itr,nval));
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

            if (!(('$' == *s_itr) &&
                  ('f' == *(s_itr + 1)) &&
                  ('(' == *(s_itr + 4)) &&
                  (is_digit(*(s_itr + 2))) &&
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

         inline void set_error(const std::string& s)
         {
            if (error_description_.empty())
            {
               error_description_ = s;
            }
         }

         inline token_t error(const char* begin, const char* end)
         {
            return token_t(token_t::error,begin,end);
         }

      private:

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
                  default      : return std::numeric_limits<T>::infinity();
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
                  default      : return std::numeric_limits<T>::infinity();
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
                  default       : return std::numeric_limits<T>::infinity();
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
                  case e_xor    : return (arg0 != arg1) ? T(1) : T(0);
                  case e_root   : return root<T>(arg0,arg1);
                  case e_equal  : return arg0 == arg1;
                  case e_nequal : return arg0 != arg1;
                  case e_hyp    : return hyp<T>(arg0,arg1);
                  case e_avg    : return (arg0 + arg1) >> 1;
                  case e_sum    : return (arg0 + arg1);
                  case e_prod   : return (arg0 * arg1);
                  case e_shr    : return arg0 >> arg1;
                  case e_shl    : return arg0 << arg1;
                  default       : return std::numeric_limits<T>::infinity();
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
            e_function
         };

         typedef T value_type;
         typedef expression_node<T>* expression_ptr;

         virtual ~expression_node()
         {}

         virtual inline T value()
         {
            return std::numeric_limits<T>::quiet_NaN();
         }

         virtual inline node_type type() const
         {
            return e_none;
         }
      };

      template <typename T>
      inline bool branch_deletable(expression_node<T>* expr)
      {
         return expression_node<T>::e_variable != expr->type();
      }

      template <typename T>
      class literal_node : public expression_node<T>
      {
      public:

         explicit literal_node(const T& value)
         : value_(value)
         {}

         inline T value()
         {
            return value_;
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_constant;
         }

      private:

         T value_;
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
            if (branch_ && branch_deletable_)  { delete branch_; branch_ = 0; }
         }

         inline T value()
         {
            const T arg = branch_->value();
            return numeric::process<T>(operation_,arg);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_unary;
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

         inline T value()
         {
            const T arg0 = branch_[0].first->value();
            const T arg1 = branch_[1].first->value();
            return numeric::process<T>(operation_,arg0,arg1);
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_binary;
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

         inline T value()
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
               default        : return std::numeric_limits<T>::infinity();
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

         inline T value()
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
               default     : return std::numeric_limits<T>::infinity();
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

         inline T value()
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
               default     : return std::numeric_limits<T>::infinity();
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

         inline T value()
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
               default        : return std::numeric_limits<T>::infinity();
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
           test_deletable_(expression_node<T>::e_variable != test_->type()),
           consequent_deletable_(expression_node<T>::e_variable != consequent_->type()),
           alternative_deletable_(expression_node<T>::e_variable != alternative_->type())
         {}

        ~conditional_node()
         {
            if (test_        &&        test_deletable_) delete test_;
            if (consequent_  &&  consequent_deletable_) delete consequent_;
            if (alternative_ && alternative_deletable_) delete alternative_;
         }

         inline T value()
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
           test_deletable_(expression_node<T>::e_variable != test_->type()),
           branch_deletable_(expression_node<T>::e_variable != branch_->type())
         {}

        ~while_loop_node()
         {
            if (test_   && test_deletable_) delete test_;
            if (branch_ && branch_deletable_) delete branch_;
         }

         inline T value()
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

         inline T value()
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
      T variable_node<T>::null_value = T(std::numeric_limits<T>::infinity());

      template <typename T>
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

         inline T value()
         {
            const T x = trinary_node<T>::branch_[0].first->value();
            const T y = trinary_node<T>::branch_[1].first->value();
            const T z = trinary_node<T>::branch_[2].first->value();
            switch (trinary_node<T>::operation_)
            {
               case e_sf00 : return (x + y) / z;
               case e_sf01 : return (x + y) * z;
               case e_sf02 : return (x - y) / z;
               case e_sf03 : return (x - y) * z;
               case e_sf04 : return (x * y) + z;
               case e_sf05 : return (x * y) - z;
               case e_sf06 : return (x * y) / z;
               case e_sf07 : return (x * y) * z;
               case e_sf08 : return (x / y) + z;
               case e_sf09 : return (x / y) - z;
               case e_sf10 : return (x / y) / z;
               case e_sf11 : return (x / y) * z;
               case e_sf12 : return z / (x + y);
               case e_sf13 : return z / (x - y);
               case e_sf14 : return z / (x * y);
               case e_sf15 : return z / (x / y);
               case e_sf16 : return z - (x / y);
               case e_sf17 : return z - (x / y);
               default     : return std::numeric_limits<T>::infinity();
            }
         }
      };

      template <typename T>
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

         inline T value()
         {
            const T x = quaternary_node<T>::branch_[0].first->value();
            const T y = quaternary_node<T>::branch_[1].first->value();
            const T z = quaternary_node<T>::branch_[2].first->value();
            const T w = quaternary_node<T>::branch_[3].first->value();
            switch (quaternary_node<T>::operation_)
            {
               case e_sf18 : return w + ((x + y) / z);
               case e_sf19 : return w + ((x + y) * z);
               case e_sf20 : return w + ((x - y) / z);
               case e_sf21 : return w + ((x - y) * z);
               case e_sf22 : return w + ((x * y) / z);
               case e_sf23 : return w + ((x * y) * z);
               case e_sf24 : return w + ((x / y) + z);
               case e_sf25 : return w + ((x / y) / z);
               case e_sf26 : return w + ((x / y) * z);
               case e_sf27 : return w - ((x + y) / z);
               case e_sf28 : return w - ((x + y) * z);
               case e_sf29 : return w - ((x - y) / z);
               case e_sf30 : return w - ((x - y) * z);
               case e_sf31 : return w - ((x * y) / z);
               case e_sf32 : return w - ((x * y) * z);
               case e_sf33 : return w - ((x / y) / z);
               case e_sf34 : return w - ((x / y) * z);
               case e_sf35 : return ((x + y) * z) - w;
               case e_sf36 : return ((x - y) * z) - w;
               case e_sf37 : return ((x * y) * z) - w;
               case e_sf38 : return ((x / y) * z) - w;
               case e_sf39 : return ((x + y) / z) - w;
               case e_sf40 : return ((x - y) / z) - w;
               case e_sf41 : return ((x * y) / z) - w;
               case e_sf42 : return ((x / y) / z) - w;
               default     : return std::numeric_limits<T>::infinity();
            }
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

         inline T value()
         {
            variable_node<T>* var_node_ptr = dynamic_cast<variable_node<T>*>(binary_node<T>::branch_[0].first);
            if (var_node_ptr)
            {
               T& result = dynamic_cast<variable_node<T>*>(binary_node<T>::branch_[0].first)->ref();
               result = binary_node<T>::branch_[1].first->value();
               return result;
            }
            else
               return std::numeric_limits<T>::infinity();
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
         void init_branches(expression_ptr (&b)[NumBranches])
         {
            for (std::size_t i = 0; i < NumBranches; ++i)
            {
               if (b[i])
               {
                  branch_[i] = std::make_pair(b[i],branch_deletable(b[i]));
               }
            }
         }

         inline bool operator <(const function_node<T,IFunction>& fn) const
         {
            return this < (&fn);
         }

         inline T value()
         {
            T v[N];
            if (function->param_count)
            {
               for (std::size_t i = 0; i < function->param_count; ++i)
               {
                  if (v[i])
                     v[i] = branch_[i].first->value();
                  else
                     std::numeric_limits<T>::infinity();
               }
            }
            switch (function->param_count)
            {
               case  0 : return (*function)();
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
               default : return std::numeric_limits<T>::infinity();
            }
         }

         inline typename expression_node<T>::node_type type() const
         {
            return expression_node<T>::e_function;
         }

      private:

         ifunction* function;
         branch_t branch_[N];
      };

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

         template <typename node_type,
                   typename T1, typename T2>
         inline expression_node<typename node_type::value_type>* allocate(const T1& t1, const T2& t2) const
         {
            return new node_type(t1,t2);
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
         return std::numeric_limits<T>::infinity();
      }

      inline virtual T operator()(const T&)
      {
         return std::numeric_limits<T>::infinity();
      }

      inline virtual T operator()(const T&,const T&)
      {
         return std::numeric_limits<T>::infinity();
      }

      inline virtual T operator()(const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::infinity();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::infinity();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::infinity();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::infinity();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::infinity();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::infinity();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::infinity();
      }

      inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      {
         return std::numeric_limits<T>::infinity();
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
         if (!function_map_.empty())
         {
            function_map_.clear();
         }
         for (std::size_t i = 0; i < lut_size; ++i)
         {
            if (short_variable_lut_[i].second) delete short_variable_lut_[i].second;
            if (short_function_lut_[i]) delete short_function_lut_[i];
         }
         clear_short_symbol_luts();
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

      inline bool is_constant(const std::string& symbol_name) const
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

      inline bool add_variable(const std::string& variable_name, T& t, const bool is_constant = false)
      {
         if (!valid_symbol(variable_name))
            return false;
         else if (symbol_exits(variable_name))
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

      inline bool add_function(const std::string& function_name, function_t& function)
      {
         if (!valid_symbol(function_name))
            return false;
         else if (symbol_exits(function_name))
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
               delete itr->second;
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
            vm_itr_t itr = function_map_.find(function_name);
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
                template <typename,typename> class Sequence>
      inline std::size_t get_variable_list(Sequence<std::pair<std::string,T>,Allocator>& vlist)
      {
         std::size_t count = 0;
         for (std::size_t i = 0; i < lut_size; ++i)
         {
            variable_pair_t& vp = short_variable_lut_[static_cast<std::size_t>(i)];
            if (0 != vp.second)
            {
               vlist.push_back(std::make_pair(std::string("")+static_cast<char>(i),vp.second->value()));
               ++count;
            }
         }
         if (!variable_map_.empty())
         {
            vm_itr_t itr = variable_map_.begin();
            vm_itr_t end = variable_map_.end();
            while (end != itr)
            {
               vlist.push_back(std::make_pair((*itr).first,itr->second.second->ref()));
               ++itr;
               ++count;
            }
         }
         return count;
      }

      inline bool symbol_exits(const std::string& symbol_name) const
      {
         /*
            Will return true if either a variable or function
            name has already been defined in any of the LUTs
            or maps.
         */
         if ((1 == symbol_name.size()) && short_variable_lut_[static_cast<std::size_t>(symbol_name[0])].second)
            return true;
         else if ((1 == symbol_name.size()) && short_function_lut_[static_cast<std::size_t>(symbol_name[0])])
            return true;
         else if (variable_map_.end() != variable_map_.find(symbol_name))
            return true;
         else if (function_map_.end() != function_map_.find(symbol_name))
            return true;
         else
            return false;
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
                   ('_' !=  symbol[i])
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
            short_function_lut_[i] = reinterpret_cast<function_ptr>(0);
         }
      }

      variable_pair_t short_variable_lut_[lut_size];
      variable_map_t  variable_map_;

      function_ptr short_function_lut_[lut_size];
      function_map_t function_map_;

      std::size_t variable_count_;
      std::size_t function_count_;
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
            if (expr && details::expression_node<T>::e_variable != expr->type())
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

      inline bool operator!()
      {
         return ((0 == expression_holder_) || (0 == expression_holder_->expr));
      }

     ~expression()
      {
         if (expression_holder_)
         {
            if (0 == --expression_holder_->ref_count)
            {
               if (details::expression_node<T>::e_variable != expression_holder_->expr->type())
               {
                  delete expression_holder_;
               }
            }
         }
      }

      inline T operator()()
      {
         return value();
      }

      inline T value()
      {
         if (expression_holder_ && expression_holder_->expr)
            return expression_holder_->expr->value();
         else
            return std::numeric_limits<T>::quiet_NaN();
      }

      inline operator T()
      {
         return value();
      }

      inline operator bool()
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

      typedef ifunction                <T>                  F;
      typedef details::expression_node <T>  expression_node_t;
      typedef details::literal_node    <T>     literal_node_t;
      typedef details::unary_node      <T>       unary_node_t;
      typedef details::binary_node     <T>      binary_node_t;
      typedef details::trinary_node    <T>     trinary_node_t;
      typedef details::quaternary_node <T>  quaternary_node_t;
      typedef details::quinary_node    <T>     quinary_node_t;
      typedef details::senary_node    <T>       senary_node_t;
      typedef details::conditional_node<T> conditional_node_t;
      typedef details::while_loop_node<T>   while_loop_node_t;
      typedef details::variable_node   <T>    variable_node_t;
      typedef details::sf3_node        <T>         sf3_node_t;
      typedef details::sf4_node        <T>         sf4_node_t;
      typedef details::assignment_node <T>  assignment_node_t;
      typedef details::function_node <T,F>    function_node_t;
      typedef details::token           <T>            token_t;
      typedef expression_node_t*          expression_node_ptr;

   public:

      parser()
      : symbol_table_(0)
      {}

      inline bool compile(const std::string& expression_string, expression<T>& expr)
      {
         if (!validate_expression(expression_string))
         {
            return false;
         }
         set_error("");
         expression_generator_.set_allocator(node_allocator_);
         if (!lexer_.process(expression_string))
         {
            set_error(lexer_.error());
            return false;
         }
         symbol_table_ = expr.get_symbol_table();
         next_token();
         expression_node_ptr e = parse_expression();
         if ((0 != e) && (current_token_.type == token_t::eof))
         {
            expr.set_expression(e);
            return !(!expr);
         }
         else
         {
            set_error("parser::compile() - Incomplete expression!");
            if (0 != e) delete e;
            return false;
         }
      }

      inline std::string error() const
      {
         return error_description_;
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
               default              : if (current_token_.type == token_t::symbol)
                                      {
                                         static const std::string s_and  =  "and";
                                         static const std::string s_nand = "nand";
                                         static const std::string s_or   =   "or";
                                         static const std::string s_nor  =  "nor";
                                         static const std::string s_xor  =  "xor";
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
                                      }
                                      break_loop = true;
            }
            if (break_loop)
               break;
            else if (current_state.left < precedence)
               break;
            next_token();
            expr = expression_generator_(current_state.operation,expr,parse_expression(current_state.right));
         }
         return expr;
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
                  if (p_[i] && details::expression_node<T>::e_variable != p_[i]->type())
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
            return reinterpret_cast<expression_node_ptr>(0);
         }
         for (int i = 0; i < static_cast<int>(NumberofParameters); ++i)
         {
            branch[i] = parse_expression();
            if (i < static_cast<int>(NumberofParameters - 1))
            {
               if (!token_is(token_t::comma))
               {
                  return reinterpret_cast<expression_node_ptr>(0);
               }
            }
         }
         if (!token_is(token_t::rbracket))
         {
            return reinterpret_cast<expression_node_ptr>(0);
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
            return reinterpret_cast<expression_node_ptr>(0);
         }
         for (int i = 0; i < static_cast<int>(NumberofParameters); ++i)
         {
            branch[i] = parse_expression();
            if (i < static_cast<int>(NumberofParameters - 1))
            {
               if (!token_is(token_t::comma))
               {
                  return reinterpret_cast<expression_node_ptr>(0);
               }
            }
         }
         if (!token_is(token_t::rbracket))
         {
            return reinterpret_cast<expression_node_ptr>(0);
         }
         else
            result = expression_generator_.function(function,branch);
         sd.delete_ptr = false;
         return result;
      }

      inline expression_node_ptr parse_conditional_statement()
      {
         expression_node_ptr condition   = 0;
         expression_node_ptr consequent  = 0;
         expression_node_ptr alternative = 0;
         next_token();
         if (token_is(token_t::lbracket))
            condition = parse_expression();
         else
            return reinterpret_cast<expression_node_ptr>(0);
         if (token_is(token_t::comma))
            consequent = parse_expression();
         else
            return reinterpret_cast<expression_node_ptr>(0);
         if (token_is(token_t::comma))
            alternative = parse_expression();
         else
            return reinterpret_cast<expression_node_ptr>(0);
         if (token_is(token_t::rbracket))
            return expression_generator_.conditional(condition,consequent,alternative);
         else
            return reinterpret_cast<expression_node_ptr>(0);
      }

      inline expression_node_ptr parse_while_loop()
      {
         expression_node_ptr condition = 0;
         expression_node_ptr branch = 0;
         next_token();
         if (token_is(token_t::lbracket))
            condition = parse_expression();
         else
            return reinterpret_cast<expression_node_ptr>(0);
         if (!token_is(token_t::rbracket))
            return reinterpret_cast<expression_node_ptr>(0);
         if (token_is(token_t::lcrlbracket))
            branch = parse_expression();
         else
            return reinterpret_cast<expression_node_ptr>(0);
         if (token_is(token_t::rcrlbracket))
            return expression_generator_.while_loop(condition,branch);
         else
            return reinterpret_cast<expression_node_ptr>(0);
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
            return reinterpret_cast<expression_node_ptr>(0);
         }
         for (std::size_t i = 0; i < NumberOfParameters; ++i)
         {
            branch[i] = parse_expression();
            if (i < (NumberOfParameters - 1))
            {
               if (!token_is(token_t::comma))
               {
                  return reinterpret_cast<expression_node_ptr>(0);
               }
            }
         }
         if (!token_is(token_t::rbracket))
            return reinterpret_cast<expression_node_ptr>(0);
         else
         {
            switch (NumberOfParameters)
            {
               case 3  : result = expression_generator_.special_function(opt_type,branch3); break;
               case 4  : result = expression_generator_.special_function(opt_type,branch4); break;
               default : return reinterpret_cast<expression_node_ptr>(0);
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
               return reinterpret_cast<expression_node_ptr>(0);
            }
         }

         static const std::string s_if = "if";
         static const std::string s_while = "while";
         if (details::imatch(current_token_.value,s_if))
         {
            return parse_conditional_statement();
         }
         else if (details::imatch(current_token_.value,s_while))
         {
            return parse_while_loop();
         }
         else if ((current_token_.value.size() == 4) && '$' == current_token_.value[0] && 'f' == current_token_.value[1])
         {
            unsigned int id = (current_token_.value[2] - '0') * 10 + (current_token_.value[3] - '0');
            if (id <= 99)
               return parse_special_function(id);
            else
            {
               set_error("parser::parse_branch() - invalid special function: " + current_token_.value);
               return reinterpret_cast<expression_node_ptr>(0);
            }
         }
         else if (symbol_table_)
         {
            const std::string symbol = current_token_.value;
            expression_node_ptr variable = symbol_table_->get_variable(symbol);
            if (variable)
            {
               if (symbol_table_->is_constant(symbol))
               {
                  variable = expression_generator_(variable->value());
               }
               next_token();
               return variable;
            }
            ifunction<T>* function = symbol_table_->get_function(symbol);
            if (function)
            {
               expression_node_ptr func_node = reinterpret_cast<expression_node_ptr>(0);
               switch (function->param_count)
               {
                  case 0 : func_node = expression_generator_.function(function); break;
                  case 1 : func_node = parse_function_call<1>(function); break;
                  case 2 : func_node = parse_function_call<2>(function); break;
                  case 3 : func_node = parse_function_call<3>(function); break;
                  case 4 : func_node = parse_function_call<4>(function); break;
                  case 5 : func_node = parse_function_call<5>(function); break;
                  case 6 : func_node = parse_function_call<6>(function); break;
                  case 7 : func_node = parse_function_call<7>(function); break;
                  case 8 : func_node = parse_function_call<8>(function); break;
                  case 9 : func_node = parse_function_call<9>(function); break;
                  default : {
                              set_error("parser::parse_branch() - invalid number of parameters for function: " + symbol);
                              return expression_node_ptr(0);
                            }
               }
               if (func_node)
               {
                  return func_node;
               }
            }
            set_error("parser::parse_branch() - undefined variable or function: " + symbol);
            return expression_node_ptr(0);
         }
         else
         {
            set_error("parser::parse_branch() - variable or function detected, yet symbol-table is invalid" + current_token_.value);
            return expression_node_ptr(0);
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

            case '(' :
                                 {
                                    next_token();
                                    expression_node_ptr branch = parse_expression();
                                    if (token_is(token_t::rbracket))
                                       return branch;
                                    else
                                       return reinterpret_cast<expression_node_ptr>(0);
                                 }
            case '[' :
                                 {
                                    next_token();
                                    expression_node_ptr branch = parse_expression();
                                    if (token_is(token_t::rsqrbracket))
                                       return branch;
                                    else
                                       return reinterpret_cast<expression_node_ptr>(0);
                                 }
            case '{' :
                                 {
                                    next_token();
                                    expression_node_ptr branch = parse_expression();
                                    if (token_is(token_t::rcrlbracket))
                                       return branch;
                                    else
                                       return reinterpret_cast<expression_node_ptr>(0);
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
                                    return reinterpret_cast<expression_node_ptr>(0);
                                 }
            default :
                                 {
                                    set_error("parser::parse_branch() - expected a valid branch [2]");
                                    return reinterpret_cast<expression_node_ptr>(0);
                                 }
         }
      }

      inline bool token_is(const typename token_t::token_type& ttype)
      {
         if (current_token_.type != ttype)
         {
            if (!((current_token_.type == ']') && (token_t::rbracket == ttype)))
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

         inline void set_allocator(details::node_allocator& na)
         {
            node_allocator_ = &na;
         }

         inline expression_node_ptr operator()(const Type& v) const
         {
            return node_allocator_->allocate<literal_node_t>(v);
         }

         inline expression_node_ptr operator()(const details::operator_type& operation, expression_node_ptr (&branch)[1])
         {
            return synthesize_expression<unary_node_t,1>(operation,branch);
         }

         inline expression_node_ptr operator()(const details::operator_type& operation, expression_node_ptr (&branch)[2])
         {
            if (details::e_assign == operation)
            {
               if (details::expression_node<T>::e_variable == branch[0]->type())
                  return synthesize_expression<assignment_node_t,2>(operation,branch);
               else
                  return reinterpret_cast<expression_node_ptr>(0);
            }
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
            if (details::e_assign == operation)
            {
               if (details::expression_node<T>::e_variable == branch[0]->type())
                  return synthesize_expression<assignment_node_t,2>(operation,branch);
               else
                  return reinterpret_cast<expression_node_ptr>(0);
            }
            else
               return synthesize_expression<binary_node_t,2>(operation,branch);
         }

         inline expression_node_ptr conditional(expression_node_ptr condition,
                                                expression_node_ptr consequent,
                                                expression_node_ptr alternative) const
         {
            //Can the condition be immediately evaluated? if so optimise.
            if (expression_node_t::e_constant == condition->type())
            {
               if (condition->value() != Type(0))
               {
                  node_allocator_->free(condition);
                  node_allocator_->free(alternative);
                  return consequent;
               }
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
            return synthesize_expression<sf3_node_t,3>(operation,branch);
         }

         inline expression_node_ptr special_function(const details::operator_type& operation, expression_node_ptr (&branch)[4])
         {
            return synthesize_expression<sf4_node_t,4>(operation,branch);
         }

         template <std::size_t N>
         inline expression_node_ptr function(typename function_node_t::ifunction* f, expression_node_ptr (&b)[N])
         {
            expression_node_ptr result = synthesize_expression<function_node_t,N>(f,b);
            if (result)
            {
               function_node_t* func_node_ptr = dynamic_cast<function_node_t*>(result);
               if (func_node_ptr)
               {
                  func_node_ptr->init_branches(b);
               }
               else
               {
                  node_allocator_->free(result);
                  result = reinterpret_cast<expression_node_ptr>(0);
               }
            }
            return result;
         }

         inline expression_node_ptr function(typename function_node_t::ifunction* f)
         {
            return node_allocator_->allocate<function_node_t>(f);
         }

      private:

         template <typename NodePtr>
         inline bool is_contant_node(NodePtr n) const
         {
            return (expression_node_t::e_constant == n->type());
         }

         template <std::size_t N, typename NodePtr>
         inline bool is_constant_foldable(NodePtr (&b)[N]) const
         {
            for (std::size_t i = 0; i < N; ++i)
            {
               if (b[i] && !is_contant_node<NodePtr>(b[i])) return false;
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

         template <typename NodeType, std::size_t N>
         inline expression_node_ptr synthesize_expression(const details::operator_type& operation, expression_node_ptr (&branch)[N])
         {
            if ((details::e_default != operation) && all_nodes_valid<N>(branch))
            {
               //Attempt simple constant folding optimisation.
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
               return reinterpret_cast<expression_node_ptr>(0);
         }

         template <typename NodeType, std::size_t N>
         inline expression_node_ptr synthesize_expression(F* f, expression_node_ptr (&branch)[N])
         {
            if (all_nodes_valid<N>(branch))
            {
               //Attempt simple constant folding optimisation.
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
               return reinterpret_cast<expression_node_ptr>(0);
         }

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
                     check0(c0,c1,'*','*','/') ||
                     check0(c0,c1,'*','%','^') ||
                     check0(c0,c1,'/','*','/') ||
                     check0(c0,c1,'/','%','^') ||
                     check0(c0,c1,'+','*','/') ||
                     check0(c0,c1,'+','%','^') ||
                     check0(c0,c1,'-','*','/') ||
                     check0(c0,c1,'-','%','^') ||
                     check0(c0,c1,'^','*','/') ||
                     check0(c0,c1,'^','^','%') ||
                     check0(c0,c1,'%','*','/') ||
                     check0(c0,c1,'%','^','%') ||
                     check0(c0,c1,'.','%','^') ||
                     check0(c0,c1,'.','*','/') ||
                     check0(c0,c1,',','%','^') ||
                     check0(c0,c1,',','*','/') ||
                     check0(c0,c1,'(','*','/') ||
                     check0(c0,c1,'(','%','^') ||
                     check0(c0,c1,'[','*','/') ||
                     check0(c0,c1,'[','%','^') ||
                     check0(c0,c1,'{','*','/') ||
                     check0(c0,c1,'{','%','^') ||
                     check0(c0,c1,'+',')',']') ||
                     check0(c0,c1,'-',')',']') ||
                     check0(c0,c1,'*',')',']') ||
                     check0(c0,c1,'/',')',']') ||
                     check0(c0,c1,'^',')',']') ||
                     check0(c0,c1,'%',')',']') ||
                     check1(c0,c1,'+','}'    ) ||
                     check1(c0,c1,'-','}'    ) ||
                     check1(c0,c1,'*','}'    ) ||
                     check1(c0,c1,'/','}'    ) ||
                     check1(c0,c1,'^','}'    ) ||
                     check1(c0,c1,'%','}'    ) ||
                     check1(c0,c1,'.','.'    ) ||
                     check1(c0,c1,'.','+'    ) ||
                     check1(c0,c1,'.','-'    ) ||
                     check1(c0,c1,'.','*'    ) ||
                     check1(c0,c1,'.','/'    ) ||
                     check1(c0,c1,',',','    )
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
      details::node_allocator node_allocator_;
      symbol_table<T>* symbol_table_;
      std::string error_description_;
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
      details::variable_node<T>* var = e.get_symbol_table()->get_variable(variable_name);
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
      details::variable_node<T>* var = e.get_symbol_table()->get_variable(variable_name);
      if (var)
      {
         T& x = var->ref();
         return derivative(e,x,h);
      }
      else
         return std::numeric_limits<T>::quiet_NaN();
   }

   namespace information
   {
      static const char* library = "Mathematical Expression Toolkit";
      static const char* version = "2.718281828";
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
