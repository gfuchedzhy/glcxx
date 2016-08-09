// Copyright 2015, 2016 Grygoriy Fuchedzhy
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef GLCXX_UTILS_HPP
#define GLCXX_UTILS_HPP

// disable warning about too long symbol names because of template programming,
// according to msdn it is safe to disable this warning
#ifdef _MSC_VER
#pragma warning(disable : 4503)
#endif

#include <utility>
#include <tuple>
#include <cmath>
#include <ostream>

namespace glcxx
{
   /// @brief evaluate expression for every element of expansion pack in order
   namespace detail {
      using swallow_type = int[];
   }
#define glcxx_swallow(expression) (void)::glcxx::detail::swallow_type{0, ((expression), 0)...}

   /// @brief stream nullptr
   inline std::ostream& operator<<(std::ostream& stream, std::nullptr_t)
   {
      return stream << "nullptr";
   }

   /// @brief stream empty tuple
   inline std::ostream& operator<<(std::ostream& stream, const std::tuple<>&)
   {
      return stream << "()";
   }

   namespace detail
   {
      /// @brief helper to stream non empty tuple
      template<typename Arg1, typename... Args, size_t... I>
      inline void ostream_tuple(std::ostream& stream, const std::tuple<Arg1, Args...>& t, std::index_sequence<I...>)
      {
         stream << '(' << std::get<0>(t);
         glcxx_swallow(stream << ',' << std::get<I+1>(t));
         stream << ')';
      }
   }

   /// @brief stream non empty tuple
   template<typename Arg1, typename... Args>
   inline std::ostream& operator<<(std::ostream& stream, const std::tuple<Arg1, Args...>& t)
   {
      detail::ostream_tuple(stream, t, std::make_index_sequence<sizeof...(Args)>{});
      return stream;
   }

   /// @brief ct stands for compile time
   namespace ct
   {
      /// @brief return true if T is specialization of Template
      template<typename T, template<typename...> class Template>
      struct specialization_of : std::false_type {};

      /// @brief specialization
      template<template<typename...> class Template, typename... Params>
      struct specialization_of<Template<Params...>, Template> : std::true_type {};

      /// @brief function traits
      template<typename Func> struct function_traits;

      /// @brief specialization for free functions
      template<typename R, typename... Args>
      struct function_traits<R(Args...)>
      {
            using ret_type = R;

            template<size_t I>
            using arg_type = typename std::tuple_element<I, std::tuple<Args...>>::type;
      };

      /// @brief specialization for member functions
      template<typename T, typename R, typename... Args>
      struct function_traits<R(T::*)(Args...)>
      {
            using ret_type = R;

            template<size_t I>
            using arg_type = typename std::tuple_element<I, std::tuple<Args...>>::type;
      };

      /// @brief return index of first type T in tuple that has
      /// Pred<T,PredParams>::value == true
      template<typename Tuple, template<typename...> class Pred, typename... PredParams> struct tuple_find_if;

      /// @brief specialization for tuple
      template<template<typename...> class Pred, typename... PredParams, typename T1, typename... Rest>
      struct tuple_find_if<std::tuple<T1, Rest...>, Pred, PredParams...>
      {
            static constexpr size_t value = Pred<T1, PredParams...>::value
               ? 0 : 1 + tuple_find_if<std::tuple<Rest...>, Pred, PredParams...>::value;
      };

      /// @brief terminator specialization for tuple
      template<template<typename...> class Pred, typename... PredParams>
      struct tuple_find_if<std::tuple<>, Pred, PredParams...>
      {
            static constexpr size_t value = 0;
      };

      /// @brief return index of T in Tuple
      template<typename Tuple, typename T>
      struct tuple_find
      {
            static constexpr size_t value = tuple_find_if<Tuple, std::is_same, T>::value;
      };

      /// @brief return true if tuple contains T
      template<typename Tuple, typename T> struct tuple_contains;

      /// @brief specialization for tuple
      template<typename... TupleArgs, typename T>
      struct tuple_contains<std::tuple<TupleArgs...>, T>
      {
            static constexpr bool value = tuple_find<std::tuple<TupleArgs...>, T>::value != sizeof...(TupleArgs);
      };

      /// @brief tuple concatanation impl
      template<typename... Tuples> struct tuple_cat_impl;
      template<typename... T1, typename... T2, typename... RestTuples>
      struct tuple_cat_impl<std::tuple<T1...>, std::tuple<T2...>, RestTuples...>
      {
            using type = typename tuple_cat_impl<std::tuple<T1..., T2...>, RestTuples...>::type;
      };

      /// @brief tuple concatanation impl terminator
      template<typename... T>
      struct tuple_cat_impl<std::tuple<T...>>
      {
         using type = std::tuple<T...>;
      };

      /// @brief tuple concatanation impl empty terminator
      template<>
      struct tuple_cat_impl<>
      {
            using type = std::tuple<>;
      };

      /// @brief tuple concatanation shortcut
      template<typename... Tuples>
      using tuple_cat = typename tuple_cat_impl<Tuples...>::type;

      /// @brief append more types to given tuple
      template<typename Tuple, typename... T>
      using tuple_append = tuple_cat<Tuple, std::tuple<T...>>;

      /// @brief build tuple that contains only types T for which
      /// Pred<T,PredParams>::value != inverse
      template<typename Tuple, template<typename...> class Pred, bool inverse = false, typename... PredParams>
      struct tuple_filter_impl;

      /// @brief specialization for tuple
      template<template<typename...> class Pred, typename... PredParams, typename T1, typename... Rest, bool inverse>
      struct tuple_filter_impl<std::tuple<T1, Rest...>, Pred, inverse, PredParams...>
      {
            using type = tuple_cat<typename std::conditional<inverse != Pred<T1, PredParams...>::value,
                                                             std::tuple<T1>,
                                                             std::tuple<>>::type,
                                   typename tuple_filter_impl<std::tuple<Rest...>, Pred, inverse, PredParams...>::type>;
      };

      /// @brief terminator specialization
      template<template<typename...> class Pred, typename... PredParams, bool inverse>
      struct tuple_filter_impl<std::tuple<>, Pred, inverse, PredParams...>
      {
            using type = std::tuple<>;
      };

      /// @brief shortcut
      template<typename Tuple, template<typename...> class Pred, bool inverse = false, typename... PredParams>
      using tuple_filter = typename tuple_filter_impl<Tuple, Pred, inverse, PredParams...>::type;

      /// @brief strip Strip types from tuple
      template<typename Strip, typename Tuple> struct tuple_strip_impl;

      /// @brief specialization for tuple
      template<typename Strip, typename... T>
      struct tuple_strip_impl<Strip, std::tuple<T...>>
      {
            using type = tuple_cat<
               typename std::conditional<std::is_same<Strip, T>::value,
                                         std::tuple<>,
                                         std::tuple<T> >::type...>;
      };

      /// @brief shortcut
      template<typename Strip, typename Tuple>
      using tuple_strip = typename tuple_strip_impl<Strip, Tuple>::type;

      /// @brief true if predicate Pred is true for any of tuple members
      template<typename Tuple, template<typename...> class Pred, typename... PredParams> struct tuple_any_of;

      /// @brief specialization for tuple
      template<typename First, typename... Rest, template<typename...> class Pred, typename... PredParams>
      struct tuple_any_of<std::tuple<First, Rest...>, Pred,  PredParams...>
         : std::integral_constant<bool, Pred<First, PredParams...>::value || tuple_any_of<std::tuple<Rest...>, Pred, PredParams...>::value>
      {};

      /// @brief terminator specialization
      template<template<typename...> class Pred, typename... PredParams>
      struct tuple_any_of<std::tuple<>, Pred, PredParams...>
         : std::false_type {};

      /// @brief compile time string
      template<char... s>
      struct string
      {
            static constexpr size_t length = sizeof...(s);
            static constexpr char chars[length+1] = {s..., '\0'};
      };

      /// @brief this definition is required if we want to odr-use this array
      template<char... s>
      constexpr char string<s...>::chars[];

      /// @brief string concatanation impl
      template<typename... Strings> struct string_cat_impl;
      template<char... str1, char... str2, typename... RestStrings>
      struct string_cat_impl<string<str1...>, string<str2...>, RestStrings...>
      {
            using type = typename string_cat_impl<string<str1..., str2...>, RestStrings...>::type;
      };

      /// @brief string concatanation impl terminator
      template<char... str>
      struct string_cat_impl<string<str...>>
      {
         using type = string<str...>;
      };

      /// @brief string concatanation impl empty terminator
      template<>
      struct string_cat_impl<>
      {
            using type = string<>;
      };

      /// @brief string concatanation shortcut
      template<typename... Strings>
      using string_cat = typename string_cat_impl<Strings...>::type;

      /// @brief strip all occurances of given char from given ct string
      template<char c, typename String> struct string_strip_char_impl;
      template<char c, char... str>
      struct string_strip_char_impl<c, string<str...>>
      {
         using type = string_cat<typename std::conditional<c == str, string<>, string<str>>::type...>;
      };
      /// @brief shortcut
      template<char c, typename String>
      using string_strip_char = typename string_strip_char_impl<c, String>::type;

      /// @brief returns substring [start,end)
      template<size_t start, size_t end, size_t...I, char...str>
      auto string_sub_helper(string<str...>, std::index_sequence<I...>)
         -> string_cat<typename std::conditional< (I<start) || (I>=end), string<>, string<str>>::type...>;

      /// @brief shortcut
      template<typename String, size_t start, size_t end = String::length>
      using string_sub = decltype(string_sub_helper<start, end>(String{}, std::make_index_sequence<String::length>{}));

      /// @brief string find
      template<typename String, typename SubString> struct string_find;

      /// @brief string find
      template<char first, char... rest, char... substr>
      struct string_find<string<first, rest...>, string<substr...>>
      {
            static constexpr size_t value = std::is_same<string<substr...>,
                                                         string_sub<string<first, rest...>, 0, string<substr...>::length>
                                                        >::value
                                            ? 0 : 1 + string_find<string<rest...>, string<substr...>>::value;
      };

      /// @brief string find
      template<char... substr>
      struct string_find<string<>, string<substr...>>
      {
            static constexpr int value = 0;
      };

      /// @brief string rfind
      template<typename String, typename SubString> struct string_rfind
      {
            static constexpr size_t value = (String::length == string_find<String, SubString>::value) ? String::length
               : String::length - string_find<String, SubString>::value - 1;
      };

      /// @brief string single replace
      template<typename String, typename Old, typename New>
      struct string_single_rep_impl
      {
            using type = typename std::conditional<String::length == string_find<String, Old>::value,
                                                   String,
                                                   string_cat<string_sub<String, 0, string_find<String, Old>::value>,
                                                              New,
                                                              string_sub<String, string_find<String, Old>::value + Old::length>>
                                                   >::type;
      };
      /// @brief string single replace shortcut
      template<typename String, typename Old, typename New>
      using string_single_rep = typename string_single_rep_impl<String, Old, New>::type;

      /// @brief string replace all impl
      template<typename String, typename Old, typename New, bool NoOccurances = false>
      struct string_all_rep_impl
      {
            using single_rep = string_single_rep<String, Old, New>;
            using type = typename string_all_rep_impl<single_rep, Old, New,
                                                      std::is_same<String, single_rep>::value>::type;
      };

      /// @brief string replace all impl terminator
      template<typename String, typename Old, typename New>
      struct string_all_rep_impl<String, Old, New, true>
      {
            using type = String;
      };

      /// @brief string replace all shortcut
      template<typename String, typename Old, typename New>
      using string_all_rep = typename string_all_rep_impl<String, Old, New>::type;

      /// @brief string from impl
      template<typename T, T Val> struct string_from_impl;

      /// @brief string from unsigned
      template<size_t Val> struct string_from_impl<size_t, Val>
      {
            using type = string_cat<typename std::conditional<0==Val/10,
                                                              string<>,
                                                              typename string_from_impl<size_t, Val/10>::type>::type,
                                    string<'0' + (Val%10)>>;
      };
      template<>
      struct string_from_impl<size_t, 0u>
      {
            using type = string<'0'>;
      };

      /// @brief string from signed
      template<int val> struct string_from_impl<int, val>
      {
            using type = string_cat<typename std::conditional< val>=0, string<>, string<'-'>>::type,
            typename string_from_impl<size_t, size_t(std::abs(val))>::type>;
      };

      /// @brief string from shortcut
      template<typename T, T val>
      using string_from = typename string_from_impl<T, val>::type;

      /// @brief string to upper case
      template<typename T> struct string_toupper_impl;
      template<char... c>
      struct string_toupper_impl<string<c...>>
      {
            using type = string<((c >= 'a' && c <= 'z') ? (c + 'A' - 'a') : ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z')) ? c : '_')...>;
      };

      /// @brief string toupper shortcut
      template<typename String>
      using string_toupper = typename string_toupper_impl<String>::type;
   }
}

/// @brief macros to generate compile time string type, maximum length supported
/// is 64 chars
#define glcxx_make_ctstring_helper1( str, offset) ((offset) < sizeof((str)) ? (str)[(offset)] : '\0')
#define glcxx_make_ctstring_helper2( str, offset) glcxx_make_ctstring_helper1(str, offset), glcxx_make_ctstring_helper1(str, offset+1)
#define glcxx_make_ctstring_helper4( str, offset) glcxx_make_ctstring_helper2(str, offset), glcxx_make_ctstring_helper2(str, offset+2)
#define glcxx_make_ctstring_helper8( str, offset) glcxx_make_ctstring_helper4(str, offset), glcxx_make_ctstring_helper4(str, offset+4)
#define glcxx_make_ctstring_helper16(str, offset) glcxx_make_ctstring_helper8(str, offset), glcxx_make_ctstring_helper8(str, offset+8)
#define glcxx_make_ctstring_helper32(str, offset) glcxx_make_ctstring_helper16(str, offset), glcxx_make_ctstring_helper16(str, offset+16)
#define glcxx_make_ctstring_helper64(str, offset) glcxx_make_ctstring_helper32(str, offset), glcxx_make_ctstring_helper32(str, offset+32)
#define cts(str) glcxx::ct::string_strip_char<'\0', glcxx::ct::string<glcxx_make_ctstring_helper64(str, 0)>>

#endif
