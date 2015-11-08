/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_UTILS_HPP
#define ENGINE_UTILS_HPP

#include <utility>

/// @brief evaluate expression for every element of expansion pack in order
#define swallow(expression) (void)(int[]){0, ((expression), 0)...}

/// @brief ct stands for compile time
namespace ct
{
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

   /// @brief helper function to deduce type of string concatanation
   template<char... str1, char... str2, typename... TRestStrings>
   auto string_cat_helper(string<str1...> s1, string<str2...> s2, TRestStrings... rest)
      -> decltype(string_cat_helper(std::declval<string<str1..., str2...>>(), rest...));

   /// @brief termination helper function to deduce type of string concatanation
   template<char... str>
   auto string_cat_helper(string<str...>) -> string<str...>;

   /// @brief string concatanation
   template<typename... TStrings>
   using string_cat = decltype(string_cat_helper(std::declval<TStrings>()...));

   /// @brief strip all occurances of given char from given ct string
   template<char c, typename TString> struct string_strip_char_impl;
   template<char c, char... str>
   struct string_strip_char_impl<c, string<str...>>
   {
      using type = string_cat<typename std::conditional<c == str, string<>, string<str>>::type...>;
   };
   /// @brief shortcut
   template<char c, typename TString>
   using string_strip_char = typename string_strip_char_impl<c, TString>::type;

   /// @brief returns substring [start,end]
   template<size_t start, size_t end, size_t...I, char...str>
   auto string_sub_helper(string<str...>, std::index_sequence<I...>)
      -> string_cat<typename std::conditional< (I<start) || (I>end), string<>, string<str>>::type...>;

   /// @brief shortcut
   template<size_t start, size_t end, typename TString>
   using string_sub = decltype(string_sub_helper<start, end>(TString{}, std::make_index_sequence<TString::length>{}));

   /// @brief shortcut, returns range [start,length]
   template<size_t start, typename TString>
   using string_sub_from = decltype(string_sub_helper<start, TString::length>(TString{}, std::make_index_sequence<TString::length>{}));
}

/// @brief macros to generate compile time string type, maximum length supported
/// is 64 chars
#define make_ctstring_helper1( str, offset) ((offset) < sizeof((str)) ? (str)[(offset)] : '\0')
#define make_ctstring_helper2( str, offset) make_ctstring_helper1(str, offset), make_ctstring_helper1(str, offset+1)
#define make_ctstring_helper4( str, offset) make_ctstring_helper2(str, offset), make_ctstring_helper2(str, offset+2)
#define make_ctstring_helper8( str, offset) make_ctstring_helper4(str, offset), make_ctstring_helper4(str, offset+4)
#define make_ctstring_helper16(str, offset) make_ctstring_helper8(str, offset), make_ctstring_helper8(str, offset+8)
#define make_ctstring_helper32(str, offset) make_ctstring_helper16(str, offset), make_ctstring_helper16(str, offset+16)
#define make_ctstring_helper64(str, offset) make_ctstring_helper32(str, offset), make_ctstring_helper32(str, offset+32)
#define cts(str) ct::string_strip_char<'\0', ct::string<make_ctstring_helper64(str, 0)>>

#endif // ENGINE_UTILS_HPP