/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_UTILS_HPP
#define ENGINE_UTILS_HPP

#include <utility>
#include <cmath>

/// @brief evaluate expression for every element of expansion pack in order
#define swallow(expression) (void)(int[]){0, ((expression), 0)...}

/// @brief ct stands for compile time
namespace ct
{
   /// @brief wrapper to name type T with ctstring name TName
   template<typename TName, typename T>
   struct TNamedType
   {
         using tName = TName;
         using type = T;
   };

   /// @brief tuple traits
   template<typename NamedTuple>
   struct TTupleTraits
   {
         /// @brief get index of named tuple at compile time
         template<typename TName, int index = std::tuple_size<NamedTuple>::value - 1>
         static constexpr int indexByName(TName, std::integral_constant<int, index> = std::integral_constant<int, index>{})
         {
            return std::is_same<TName, typename std::tuple_element<index, NamedTuple>::type::tName>::value ? index :
               indexByName(TName{}, std::integral_constant<int, index-1>{});
         }

         /// @brief terminator indexByName
         template<typename TName>
         static constexpr int indexByName(TName, std::integral_constant<int, -1>)
         {
            return -1;
         }
   };

   /// @brief tuple traits
   template<typename T, typename... Types>
   struct TTypeIndexInPack
   {
         static constexpr int value = TTupleTraits<std::tuple<TNamedType<Types, Types>...>>::indexByName(T{});
   };

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

   /// @brief string from impl
   template<typename T, T val> struct string_from_impl;

   /// @brief string from unsigned
   template<size_t val> struct string_from_impl<size_t, val>
   {
         using type = string_cat<typename std::conditional<0==val/10,
                                                           string<>,
                                                           typename string_from_impl<size_t, val/10>::type>::type,
                                 string<'0' + (val%10)>>;
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
