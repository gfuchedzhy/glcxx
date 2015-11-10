/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_UNIFORM_HPP
#define ENGINE_UNIFORM_HPP

#include <glm/glm.hpp>
#include "GL.hpp"
#include "Utils.hpp"

template<typename TData>
struct TUniformTraits;

template<>
struct TUniformTraits<glm::mat4>
{
      static constexpr char glslType[] = "mat4";
      static void attach(GLint location, const glm::mat4* ptr, size_t count)
      {
         gl(glUniformMatrix4fv, location, count, false, &(*ptr)[0][0]);;
      }
};

template<typename TData, typename TName, size_t N = 1>
struct TUniform : std::array<TData, N>
{
      static_assert(N >= 1, "uniform number should be at least 1");

      /// @brief data type
      using tData = TData;

      /// @brief ctstring containing name of uniform
      using tName = TName;

      /// @brief return uniform size
      static constexpr size_t size = N;

      /// @brief type traits
      using tTypeTraits = TUniformTraits<tData>;

      /// @brief base typedef
      using tBaseArray = std::array<tData, size>;

      /// @brief ctstring containing glsl declaration of uniform
      using tDeclaration = ct::string_cat<cts("uniform "), cts(tTypeTraits::glslType), cts(" "), tName,
                                          typename std::conditional<1==size,
                                                                    cts(""),
                                                                    ct::string_cat<cts("["),
                                                                                   ct::string_from<size_t, size>,
                                                                                   cts("]")>>::type,
                                          cts(";")>;

      /// @brief default constructor
      TUniform() = default;

      /// @brief constructor allowing initialization from {...}
      template<typename... T>
      TUniform(const T&... args)
         : tBaseArray({args...})
      {
         static_assert(sizeof...(T) == N, "wrong number of initializers passed to uniform");
      }

      using tLocation = GLint;

      /// @brief returns location of uniform for given program
      static tLocation getLocation(GLuint program)
      {
         tLocation location = gl(glGetUniformLocation, program, tName::chars);
         Log::msg("uniform location ", tName::chars, "=", location);
         return location;
      }

      void attach(GLint location)
      {
         tTypeTraits::attach(location, &(*this)[0], size);
      }
};

#endif // ENGINE_ATTRIBUTE_HPP
