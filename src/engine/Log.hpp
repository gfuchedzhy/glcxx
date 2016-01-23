/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
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
#include <glm/mat4x4.hpp>

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

   /// @brief stream SOnOff
   inline std::ostream& operator<<(std::ostream& stream, const Log::SOnOff& onOff)
   {
      return stream << (onOff.mValue ? "on" : "off");
   }

   /// @brief stream nullptr
   inline std::ostream& operator<<(std::ostream& stream, std::nullptr_t)
   {
      return stream << "null";
   }

   /// @brief stream vec2
   template<typename T, glm::precision P>
   inline std::ostream& operator<<(std::ostream& stream, const glm::tvec2<T, P>& vec)
   {
      return stream << "vec2(" << vec.x << ',' << vec.y << ')';
   }

   /// @brief stream vec3
   template<typename T, glm::precision P>
   inline std::ostream& operator<<(std::ostream& stream, const glm::tvec3<T, P>& vec)
   {
      return stream << "vec3(" << vec.x << ',' << vec.y << ',' << vec.z << ')';
   }

   /// @brief stream vec4
   template<typename T, glm::precision P>
   inline std::ostream& operator<<(std::ostream& stream, const glm::tvec4<T, P>& vec)
   {
      return stream << "vec4(" << vec.x << ',' << vec.y << ',' << vec.z << ',' << vec.w << ')';
   }

   /// @brief stream mat4x4
   template<typename T, glm::precision P>
   inline std::ostream& operator<<(std::ostream& stream, const glm::tmat4x4<T, P>& mat)
   {
      stream << "mat4x4(" << mat[0][0] << ',' << mat[1][0] << ',' << mat[2][0] << ',' << mat[3][0] << std::endl;
      stream << "       " << mat[0][1] << ',' << mat[1][1] << ',' << mat[2][1] << ',' << mat[3][1] << std::endl;
      stream << "       " << mat[0][2] << ',' << mat[1][2] << ',' << mat[2][2] << ',' << mat[3][2] << std::endl;
      return stream << "       " << mat[0][3] << ',' << mat[1][3] << ',' << mat[2][3] << ',' << mat[3][3] << std::endl;
   }

   /// @brief stream vector
   template<typename T>
   inline std::ostream& operator<<(std::ostream& stream, const std::vector<T>& v)
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
   inline std::ostream& operator<<(std::ostream& stream, const std::array<T, N>& arr)
   {
      stream << '[';
      for (size_t i = 0; i < N; ++i)
         stream << arr[i] << (i+1 < N ? ',' : ']');
      return stream;
   }

   /// @brief stream empty tuple
   inline std::ostream& operator<<(std::ostream& stream, const std::tuple<>&)
   {
      return stream << "()";
   }

   namespace detail
   {
      /// @brief helper to stream non empty tuple
      template<typename TArg1, typename... TArgs, size_t... I>
      inline void ostreamTupleHelper(std::ostream& stream, const std::tuple<TArg1, TArgs...>& t, std::index_sequence<I...>)
      {
         stream << '(' << std::get<0>(t);
         swallow(stream << ',' << std::get<I+1>(t));
         stream << ')';
      }
   }

   /// @brief stream non empty tuple
   template<typename TArg1, typename... TArgs>
   inline std::ostream& operator<<(std::ostream& stream, const std::tuple<TArg1, TArgs...>& t)
   {
      detail::ostreamTupleHelper(stream, t, std::make_index_sequence<sizeof...(TArgs)>{});
      return stream;
   }

   namespace detail
   {
      /// @brief log message to given stream
      template<typename... TArgs>
      inline void streamLog(std::ostream& s, const TArgs&... args)
      {
         swallow(s << args);
         s << std::endl;
      }
   }

   /// @brief log error
   template<typename... TArgs>
   inline void err(const TArgs&... args)
   {
      std::cerr << "ERROR: ";
      detail::streamLog(std::cerr, args...);
   }

   /// @brief log message
   template<typename... TArgs>
   inline void msg(const TArgs&... args)
   {
      detail::streamLog(std::cout, args...);
   }
}

#endif // ENGINE_LOG_HPP
