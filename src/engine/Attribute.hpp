/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_ATTRIBUTE_HPP
#define ENGINE_ATTRIBUTE_HPP

#include "GL.hpp"
#include "GLSLInputVariable.hpp"

/// @brief generic structure containing arbitrary number of arbitrary attributes,
/// e.g. tuple<glm::vec3, glm::vec2, int>
template<typename... TAttribTraits>
struct TAttribPack : std::tuple<typename TAttribTraits::tData...>
{
      /// @brief base typedef
      using tBaseTuple = std::tuple<typename TAttribTraits::tData...>;

      /// @brief attribute number
      static constexpr size_t attributeNum = sizeof...(TAttribTraits);

      /// @brief default constructor
      TAttribPack() = default;

      /// @brief constructor allowing initialization from {...}
      TAttribPack(const typename TAttribTraits::tData&... attributes)
         : tBaseTuple(attributes...)
      {}

      /// @brief get attribute by compile time name
      template<typename TName>
      auto&& get()
      {
         constexpr int index = ct::TTupleTraits<std::tuple<TAttribTraits...>>::indexByName(TName{});
         static_assert(-1 != index, "attribute name not found");
         return std::get<index>(*this);
      }

      /// @brief get attribute by compile time name
      template<typename TName>
      auto&& get() const
      {
         constexpr int index = ct::TTupleTraits<std::tuple<TAttribTraits...>>::indexByName(TName{});
         static_assert(-1 != index, "attribute name not found");
         return std::get<index>(*this);
      }
};

/// @brief traits for attribute pack
template<typename... TAttributeTraits>
struct TAttribPackTraits
{
      /// @brief data typedef
      using tData = TAttribPack<TAttributeTraits...>;

      /// @brief ctstring containing combined name of attributes, e.g. "aPos,aColor"
      using tName = ct::string_sub_from<1, ct::string_cat<ct::string_cat<cts(","), typename TAttributeTraits::tName>...>>;

      /// @brief attribute number
      static constexpr size_t attributeNum = sizeof...(TAttributeTraits);

      /// @brief ctstring containing glsl declaration of attributes
      using tDeclaration = ct::string_cat<typename TAttributeTraits::tDeclaration...>;

      /// @brief returns offset of given attribute in attribute pack
      template<int index>
      struct offsetof
      {
            /// @todo this is not entirely standard conformant, but currently it
            /// works for gcc
            static constexpr void* value = &std::get<index>(*(tData*)(0));
      };

      /// @brief type holding locations for attribute pack
      using tLocation = std::array<GLint, attributeNum>;

      /// @brief returns locations of attributes for given program
      static tLocation getLocation(GLuint program)
      {
         return {TAttributeTraits::getLocation(program)...};
      }

      /// @brief attaches attribute data to given location, ptr = 0 for buffered attributes
      static void attach(const tLocation& location, const TAttribPackTraits::tData* ptr = nullptr)
      {
         doAttach(location, ptr, std::make_index_sequence<attributeNum>{});
      }

      /// @brief detaches attribute data
      static void detach(const tLocation& location)
      {
         doDetach(location, std::make_index_sequence<attributeNum>{});
      }

   private: // impl
      /// @brief impl of attach
      template<size_t... I>
      static void doAttach(const tLocation& location, const TAttribPackTraits::tData* ptr, std::index_sequence<I...>)
      {
         swallow((TAttributeTraits::attach(location[I],
                                           sizeof(TAttribPackTraits::tData),
                                           (size_t)(offsetof<I>::value),
                                           reinterpret_cast<const typename TAttributeTraits::tData*>(ptr))));
      }

      /// @brief impl of detach
      template<size_t... I>
      static void doDetach(const tLocation& location, std::index_sequence<I...>)
      {
         swallow(TAttributeTraits::detach(location[I]));
      }
};

/// @brief this definition is required if we want to odr-use this constexpr
template<typename... TAttributeTraits>
template<int index>
constexpr void* TAttribPackTraits<TAttributeTraits...>::offsetof<index>::value;

#endif // ENGINE_ATTRIBUTE_HPP
