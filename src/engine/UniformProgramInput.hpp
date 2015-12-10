/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_UNIFORMPROGRAMINPUT_HPP
#define ENGINE_UNIFORMPROGRAMINPUT_HPP

#include "GLSLInputVariable.hpp"

template<typename TData, typename TGLSLData>
class TUniformProgramInputImpl
{
      /// @brief location of program input inside program
      GLint mLocation;

      /// @brief holds actual buffer
      TData mUniformData;

   public:
      /// @brief constructor
      TUniformProgramInputImpl(const GLint location)
         : mLocation(location)
      {}

      /// @brief set uniform as program input
      void set(const TData& value)
      {
         if (mUniformData != value)
         {
            mUniformData = value;
            glsl::attachUniform(mLocation, glsl::TConverter<TGLSLData>::convert(mUniformData));
         }
      }

      /// @brief called after program was selected, nothing to do as uniforms
      /// remains attached during program selection change
      void select()
      {}
};

/// @brief tags
namespace tag
{
   struct vertex;
   struct fragment;
   struct all;
}

/// @brief holds state of program's uniform, use it as TProgram template
/// parameter, tag tells which shader should contain given attribute
template<typename DeclarationTag, typename TUniformTraits>
class TUniformProgramInput : public TUniformProgramInputImpl<typename TUniformTraits::tData, typename TUniformTraits::tGLSLData>
{
   public:
      /// @brief uniform datatype this program input accepts
      using tValueType = const typename TUniformTraits::tData&;

      /// @brief underlying uniform name
      using tName = typename TUniformTraits::tName;

      /// @brief ctstring containing glsl declaration of variable
      using tVertexShaderDeclaration = typename std::conditional<std::is_same<DeclarationTag, tag::vertex>::value
                                                                 || std::is_same<DeclarationTag, tag::all>::value,
                                                                 typename TUniformTraits::tDeclaration,
                                                                 cts("")>::type;
      using tFragmentShaderDeclaration = typename std::conditional<std::is_same<DeclarationTag, tag::fragment>::value
                                                                   || std::is_same<DeclarationTag, tag::all>::value,
                                                                   typename TUniformTraits::tDeclaration,
                                                                   cts("")>::type;

      /// @brief constructor
      TUniformProgramInput(const GLuint program)
         : TUniformProgramInputImpl<typename TUniformTraits::tData, typename TUniformTraits::tGLSLData>(TUniformTraits::getLocation(program))
      {}
};

#endif // ENGINE_BUFFEROBJECTPROGRAMINPUT_HPP
