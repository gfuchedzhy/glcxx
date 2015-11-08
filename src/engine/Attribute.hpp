/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_ATTRIBUTE_HPP
#define ENGINE_ATTRIBUTE_HPP

#include "GL.hpp"
#include "Utils.hpp"

/// @brief single attribute
/// @tparam glType gl enum like GL_FLOAT
/// @tparam N number of glType components in memory layout
/// @tparam EXTRA number of extra components filled with default values, for
/// example N=3, EXTRA=1 (x,y,z) position in memory will be translated to vec4
/// with extra component filled with default value (x,y,z,1)
/// @tparam TName is compile time string representing name of attribute
template<GLenum glType, size_t N, size_t EXTRA, typename TName>
struct TAttributeData : std::array<typename TGLSLToCPPType<glType>::type, N>
{
      static_assert(N >= 1 && N + EXTRA <= 4, "attribute number should be in range [1,4]");

      /// @brief base typedef
      using tBaseArray = std::array<typename TGLSLToCPPType<glType>::type, N>;

      /// @brief ctstring containing name of attribute
      using tName = TName;

      /// @brief ctstring containing glsl declaration of attribute
      using tDeclaration = ct::string_cat<cts("attribute vec"), ct::string<'0' + N + EXTRA, ' '>, tName, cts(";")>;

      /// @brief return glType id
      static constexpr GLenum glTypeID = glType;

      /// @brief return attribute size
      static constexpr size_t size = N;

      /// @brief default constructor
      TAttributeData() = default;

      /// @brief constructor allowing initialization from {...}
      template<typename... T>
      TAttributeData(const T&... args)
         : tBaseArray({args...})
      {
         static_assert(sizeof...(T) == N, "wrong number of initializers passed to attribute");
      }
};

template<typename... TAttributes>
struct TAttributeDataPack : std::tuple<TAttributes...>
{
      /// @brief base typedef
      using tBaseTuple = std::tuple<TAttributes...>;

      /// @brief return attribute number
      static constexpr size_t attributeNum = sizeof...(TAttributes);

      /// @brief ctstring containing combined name of attributes
      using tName = ct::string_sub_from<1, ct::string_cat<ct::string_cat<cts(","), typename TAttributes::tName>...>>;

      /// @brief ctstring containing glsl declaration of attributes
      using tDeclaration = ct::string_cat<typename TAttributes::tDeclaration...>;

      /// @brief returns offset of given attribute in attribute pack
      template<int index>
      struct offsetof
      {
            static constexpr void* value = (void*)((char*)(&std::get<index>(*(tBaseTuple*)(0))));
      };

      /// @brief type holding locations for attribute pack
      using tLocations = std::array<GLint, attributeNum>;

      /// @brief returns locations of attributes for given program
      static inline tLocations getLocations(GLuint program)
      {
         tLocations locations = doGetLocations(program, std::make_index_sequence<attributeNum>{});
         Log::msg("attribute locations ", tName::chars, "=", locations);
         return doGetLocations(program, std::make_index_sequence<attributeNum>{});
      }

      /// @brief implementation of location getter
      template<size_t... I>
      static inline tLocations doGetLocations(GLuint program, std::index_sequence<I...>)
      {
         return {gl(glGetAttribLocation, program, std::tuple_element<I, tBaseTuple>::type::tName::chars)...};
      }

      /// @brief default constructor
      TAttributeDataPack() = default;

      /// @brief constructor allowing initialization from {...}
      TAttributeDataPack(const TAttributes&... attributes)
         : std::tuple<TAttributes...>(attributes...)
      {}

      /// @brief get index of named attribute in compile time
      template<typename TName, int index = sizeof...(TAttributes) - 1>
         static constexpr int indexByName(TName, std::integral_constant<int, index> = std::integral_constant<int, index>{})
      {
         return std::is_same<TName, typename std::tuple_element<index, tBaseTuple>::type::tName>::value ? index :
            indexByName(TName{},std::integral_constant<int, index-1>{});
      }

      /// @brief terminator for attribute index calculator
      template<typename TName>
      static constexpr int indexByName(TName, std::integral_constant<int, -1>)
      {
         return -1;
      }

      /// @brief get attribute by compile time name
      template<typename TName>
      auto& get()
      {
         constexpr int index = indexByName(TName{});
         static_assert(-1 != index, "attribute name not found");
         return std::get<index>(*this);
      }

      /// @brief get attribute by compile time name
      template<typename TName>
      const auto& get() const
      {
         constexpr int index = indexByName(TName{});
         static_assert(-1 != index, "attribute name not found");
         return std::get<index>(*this);
      }
};

#endif // ENGINE_ATTRIBUTE_HPP
