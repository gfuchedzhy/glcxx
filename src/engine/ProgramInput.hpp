/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAMINPUT_HPP
#define ENGINE_PROGRAMINPUT_HPP

#include "BufferObjectProgramInput.hpp"
#include "UniformProgramInput.hpp"
#include "Texture.hpp"

/// @brief tags
namespace tag
{
   struct vertex;
   struct fragment;
   struct all;
}

/// @brief program input selector, for uniforms returns uniform program input,
/// for attributes returns buffer program input
template<typename T> struct TProgramInputSelector
{
      using type = typename std::conditional<T::isAttribute, TBufferObjectProgramInput<T>, TUniformProgramInput<T>>::type;
};

/// @brief specialization for textures, just returns given type
template<GLint samplerID>
struct TProgramInputSelector<TTextureProgramInput<samplerID>>
{
      using type = TTextureProgramInput<samplerID>;
};

/// @brief named program input
/// @tparam TInputName input name
/// @tparam T type, currently supported TUniform<..>, TUniformArr<..>,
/// TAttrib<..>, TAttribArr<..>, TTextureProgramInput<..>
/// @tparam DeclarationTag tells which shader should contain given input
template<typename TInputName, typename T, typename DeclarationTag = tag::vertex>
struct TProgramInput : public TProgramInputSelector<T>::type
{
      /// @brief name
      using tName = TInputName;

      /// @brief program implementation type
      using tImpl = typename TProgramInputSelector<T>::type;

      /// @brief ctstrings containing glsl declarations for this input
      using tVertexShaderDeclaration = typename std::conditional<std::is_same<DeclarationTag, tag::vertex>::value
                                                                 || std::is_same<DeclarationTag, tag::all>::value,
                                                                 typename tImpl::template tDeclaration<TInputName>,
                                                                 cts("")>::type;
      using tFragmentShaderDeclaration = typename std::conditional<std::is_same<DeclarationTag, tag::fragment>::value
                                                                   || std::is_same<DeclarationTag, tag::all>::value,
                                                                   typename tImpl::template tDeclaration<TInputName>,
                                                                   cts("")>::type;
      /// @brief forward named set to impl set
      template<typename TName>
      typename std::enable_if<std::is_same<tName, TName>::value>::type
      set(typename ct::function_traits<decltype(&tImpl::set)>::template arg_type<0> value)
      {
         tImpl::set(value);
      }

      TProgramInput(GLuint program)
         : tImpl(program, tName::chars)
      {}
};

#endif
