/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_LOG_HPP
#define ENGINE_LOG_HPP

#include "Utils.hpp"
#include <iostream>
#include <tuple>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Log
{
   /// @brief struct for convenient logging of bool values as on/off strings
   struct SOnOff
   {
         SOnOff(bool value)
            : mValue(value)
         {}
         bool mValue;
   };
}

namespace std
{
   /// @brief stream SOnOff
   inline ostream& operator<<(ostream& stream, const Log::SOnOff& onOff)
   {
      return stream << (onOff.mValue ? "on" : "off");
   }

   /// @brief stream vec2
   template<typename T, glm::precision P>
   inline ostream& operator<<(ostream& stream, const glm::tvec2<T, P>& vec)
   {
      return stream << "vec2(" << vec.x << ',' << vec.y << ')';
   }

   /// @brief stream vec3
   template<typename T, glm::precision P>
   inline ostream& operator<<(ostream& stream, const glm::tvec3<T, P>& vec)
   {
      return stream << "vec3(" << vec.x << ',' << vec.y << ',' << vec.z << ')';
   }

   /// @brief stream vec4
   template<typename T, glm::precision P>
   inline ostream& operator<<(ostream& stream, const glm::tvec4<T, P>& vec)
   {
      return stream << "vec4(" << vec.x << ',' << vec.y << ',' << vec.z << ',' << vec.w << ')';
   }

   /// @brief stream vector
   template<typename T>
   inline ostream& operator<<(ostream& stream, const std::vector<T>& v)
   {
      stream << '[';
      for (size_t i = 0; i < v.size(); ++i)
      {
         stream << v[i];
         if (i+1 < v.size())
            stream << ',';
      }
      return stream << ']';
   }

   /// @brief stream array
   template<typename T, size_t N>
   inline ostream& operator<<(ostream& stream, const std::array<T, N>& arr)
   {
      stream << '[';
      for (size_t i = 0; i < N; ++i)
         stream << arr[i] << (i+1 < N ? ',' : ']');
      return stream;
   }

   /// @brief stream empty tuple
   inline ostream& operator<<(ostream& stream, const std::tuple<>&)
   {
      return stream << "()";
   }

   namespace detail
   {
      /// @brief helper to stream non empty tuple
      template<typename TArg1, typename... TArgs, size_t... I>
      inline void ostreamTupleHelper(ostream& stream, const std::tuple<TArg1, TArgs...>& t, std::index_sequence<I...>)
      {
         stream << '(' << std::get<0>(t);
         swallow(stream << ',' << std::get<I+1>(t));
         stream << ')';
      }
   }

   /// @brief stream non empty tuple
   template<typename TArg1, typename... TArgs, size_t... I>
   inline ostream& operator<<(ostream& stream, const std::tuple<TArg1, TArgs...>& t)
   {
      detail::ostreamTupleHelper(stream, t, std::make_index_sequence<sizeof...(TArgs)>{});
      return stream;
   }
}

namespace Log
{
   namespace detail
   {
      /// @brief log message to given stream
      template<typename... TArgs>
      inline void streamLog(std::ostream& s, TArgs&&... args)
      {
         swallow(s << args);
         s << std::endl;
      }
   }

   /// @brief log error
   template<typename... TArgs>
   inline void err(TArgs&&... args)
   {
      std::cerr << "ERROR: ";
      detail::streamLog(std::cerr, std::forward<TArgs>(args)...);
   }

   /// @brief log message
   template<typename... TArgs>
   inline void msg(TArgs&&... args)
   {
      detail::streamLog(std::cout, std::forward<TArgs>(args)...);
   }
}

#endif // ENGINE_LOG_HPP
