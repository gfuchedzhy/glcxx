/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_UTILS_HPP
#define ENGINE_UTILS_HPP

#include <utility>
#include <tuple>
#include <cmath>
#include <random>

/// @brief evaluate expression for every element of expansion pack in order
#define swallow(expression) (void)(int[]){0, ((expression), 0)...}

/// @brief random generator
extern std::mt19937 random_gen;

/// @brief ct stands for compile time
namespace ct
{
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

   /// @brief value contains index of type in tuple
   template<typename T, typename Tuple> struct type_index_in_pack_impl;

   /// @brief specialization for tuple
   template<typename T, typename T1, typename... TRest>
   struct type_index_in_pack_impl<T, std::tuple<T1, TRest...>>
   {
      static constexpr size_t value = std::is_same<T1, T>::value ? 0 : 1 + type_index_in_pack_impl<T, std::tuple<TRest...>>::value;
   };

   /// @brief terminator specialization for tuple
   template<typename T>
   struct type_index_in_pack_impl<T, std::tuple<>>
   {
      static constexpr size_t value = 0;
   };

   /// @brief shortcut
   template<typename T, typename... Pack>
   struct type_index_in_pack
   {
         static constexpr size_t value = type_index_in_pack_impl<T, std::tuple<Pack...>>::value;
   };

   /// @brief tuple concatanation impl
   template<typename... Tuples> struct tuple_cat_impl;
   template<typename... T1, typename... T2, typename... TRestTuples>
   struct tuple_cat_impl<std::tuple<T1...>, std::tuple<T2...>, TRestTuples...>
   {
         using type = typename tuple_cat_impl<std::tuple<T1..., T2...>, TRestTuples...>::type;
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

   /// @brief strip TStrip types from tuple
   template<typename TStrip, typename Tuple> struct tuple_strip_impl;

   /// @brief specialization for tuple
   template<typename TStrip, typename... T>
   struct tuple_strip_impl<TStrip, std::tuple<T...>>
   {
         using type = tuple_cat<
            typename std::conditional<std::is_same<TStrip, T>::value,
                                      std::tuple<>,
                                      std::tuple<T> >::type...>;
   };

   /// @brief shortcut
   template<typename TStrip, typename Tuple>
   using tuple_strip = typename tuple_strip_impl<TStrip, Tuple>::type;

   /// @brief true if tuple contains given type
   template<typename T, typename Tuple> struct tuple_contains;

   /// @brief specialization for tuple
   template<typename T, typename... TupleMembers>
   struct tuple_contains<T, std::tuple<TupleMembers...>>
      : public std::integral_constant<bool, sizeof...(TupleMembers) != type_index_in_pack<T, TupleMembers...>::value>
   {};

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
   template<typename... TStrings> struct string_cat_impl;
   template<char... str1, char... str2, typename... TRestStrings>
   struct string_cat_impl<string<str1...>, string<str2...>, TRestStrings...>
   {
      using type = typename string_cat_impl<string<str1..., str2...>, TRestStrings...>::type;
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
   template<typename... TStrings>
   using string_cat = typename string_cat_impl<TStrings...>::type;

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

   /// @brief returns substring [start,end)
   template<size_t start, size_t end, size_t...I, char...str>
   auto string_sub_helper(string<str...>, std::index_sequence<I...>)
      -> string_cat<typename std::conditional< (I<start) || (I>=end), string<>, string<str>>::type...>;

   /// @brief shortcut
   template<typename TString, size_t start, size_t end = TString::length>
   using string_sub = decltype(string_sub_helper<start, end>(TString{}, std::make_index_sequence<TString::length>{}));

   /// @brief string find
   template<typename TString, typename TSubString> struct string_find;

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
   template<typename TString, typename TSubString> struct string_rfind
   {
         static constexpr size_t value = (TString::length == string_find<TString, TSubString>::value) ? TString::length
            : TString::length - string_find<TString, TSubString>::value - 1;
   };

   /// @brief string single replace
   template<typename TString, typename TOld, typename TNew>
   struct string_single_rep_impl
   {
         using type = typename std::conditional<TString::length == string_find<TString, TOld>::value,
                                                TString,
                                                string_cat<string_sub<TString, 0, string_find<TString, TOld>::value>,
                                                           TNew,
                                                           string_sub<TString, string_find<TString, TOld>::value + TOld::length>>
                                                >::type;
   };
   /// @brief string single replace shortcut
   template<typename TString, typename TOld, typename TNew>
   using string_single_rep = typename string_single_rep_impl<TString, TOld, TNew>::type;

   /// @brief string replace all impl
   template<typename TString, typename TOld, typename TNew, bool noOccurances = false>
   struct string_all_rep_impl
   {
         using single_rep = string_single_rep<TString, TOld, TNew>;
         using type = typename string_all_rep_impl<single_rep, TOld, TNew,
                                                   std::is_same<TString, single_rep>::value>::type;
   };

   /// @brief string replace all impl terminator
   template<typename TString, typename TOld, typename TNew>
   struct string_all_rep_impl<TString, TOld, TNew, true>
   {
         using type = TString;
   };

   /// @brief string replace all shortcut
   template<typename TString, typename TOld, typename TNew>
   using string_all_rep = typename string_all_rep_impl<TString, TOld, TNew>::type;

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

   /// @brief string to upper case
   template<typename T> struct string_toupper_impl;
   template<char... c>
   struct string_toupper_impl<string<c...>>
   {
         using type = string<((c >= 'a' && c <= 'z') ? (c + 'A' - 'a') : ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z')) ? c : '_')...>;
   };

   /// @brief string toupper shortcut
   template<typename TString>
   using string_toupper = typename string_toupper_impl<TString>::type;
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
