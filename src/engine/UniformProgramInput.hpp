/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_UNIFORMPROGRAMINPUT_HPP
#define ENGINE_UNIFORMPROGRAMINPUT_HPP

#include "GLSLInputVariable.hpp"

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
class TUniformProgramInput
{
      /// @brief location of program input inside program
      GLint mLocation;

      /// @brief holds actual uniform
      typename TUniformTraits::tData mUniformData;

   public:
      /// @brief uniform datatype this program input accepts
      using tValueType = typename TUniformTraits::tData;

      /// @brief ctstring containing glsl declaration of variable, todo
      template<typename TName>
      using tVertexShaderDeclaration = typename std::conditional<std::is_same<DeclarationTag, tag::vertex>::value
                                                                 || std::is_same<DeclarationTag, tag::all>::value,
                                                                 typename TUniformTraits::template tDeclaration<TName>,
                                                                 cts("")>::type;
      template<typename TName>
      using tFragmentShaderDeclaration = typename std::conditional<std::is_same<DeclarationTag, tag::fragment>::value
                                                                   || std::is_same<DeclarationTag, tag::all>::value,
                                                                   typename TUniformTraits::template tDeclaration<TName>,
                                                                   cts("")>::type;

      /// @brief constructor
      TUniformProgramInput(const GLuint program, const char* name)
         : mLocation(glsl::getUniformLocation(program, name))
      {}

      /// @brief set uniform as program input
      void set(const tValueType& value)
      {
         if (mUniformData != value)
         {
            mUniformData = value;
            glsl::attachUniform(mLocation, glsl::TConverter<typename TUniformTraits::tGLSLData>::convert(mUniformData));
         }
      }

      /// @brief called after program was selected, nothing to do as uniforms
      /// remains attached during program selection change
      void select() const
      {}
};

#endif // ENGINE_BUFFEROBJECTPROGRAMINPUT_HPP
