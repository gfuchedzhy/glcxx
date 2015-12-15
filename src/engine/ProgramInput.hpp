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
      void set(const typename std::enable_if<std::is_same<tName, TName>::value, typename tImpl::tValueType>::type& value)
      {
         tImpl::set(value);
      }

      TProgramInput(GLuint program)
         : tImpl(program, tName::chars)
      {}
};

/// @brief aggregator of program inputs, using recursive inheritance to bring
/// all set methods visible for overload in derived class
template<typename T, typename... TRest>
struct TProgramInputs : public T, public TProgramInputs<TRest...>
{
      /// @brief constructor
      TProgramInputs(GLuint program)
         : T(program)
         , TProgramInputs<TRest...>(program)
      {}

      /// @brief pull set visibility from base classes
      using T::set;
      using TProgramInputs<TRest...>::set;
};

/// @brief terminating partial specialization of TProgramInputs
template<typename T>
struct TProgramInputs<T> : public T
{
      /// @brief inherit constructor
      using T::T;

      /// @brief pull set visibility from base classes
      using T::set;
};

/// @brief append some program inputs to this input list
namespace detail
{
   template<typename TInput, typename... T> struct TPIAppendImpl;
   template<typename... T1, typename... T2>
   struct TPIAppendImpl<TProgramInputs<T1...>, T2...>
   {
         using type = TProgramInputs<T1..., T2...>;
   };
}

/// @brief shortcut
template<typename TInput, typename... T>
using pinput_append = typename detail::TPIAppendImpl<TInput, T...>::type;

#endif
