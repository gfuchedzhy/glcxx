/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_UNIFORMPROGRAMINPUT_HPP
#define ENGINE_UNIFORMPROGRAMINPUT_HPP

#include "GLSLInputVariable.hpp"

/// @brief base implementation for TUniformProgramInput
template<typename TUniformTraits>
class TUniformProgramInputImpl
{
      /// @brief location of program input inside program
      GLint mLocation;

      /// @brief holds actual uniform
      typename TUniformTraits::tData mUniformData;

   public:
      /// @brief constructor
      TUniformProgramInputImpl(GLint location)
         : mLocation(location)
      {}

      /// @brief set uniform as program input
      void set(const typename TUniformTraits::tData& value)
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

/// @brief holds state of program's uniform
template<typename TName, typename TUniformTraits, typename DeclTag = tag::vertex>
class TUniformProgramInput : public TUniformProgramInputImpl<TUniformTraits>
{
      /// @brief base implementation class
      using tBase = TUniformProgramInputImpl<TUniformTraits>;

   public:
      /// @brief declaration tag
      using tDeclTag = DeclTag;

      /// @brief ctstring containing glsl declaration of variable
      using tDeclaration = typename TUniformTraits::template tDeclaration<TName>;

      /// @brief constructor
      TUniformProgramInput(const GLuint program)
         : tBase(glsl::getUniformLocation(program, TName::chars))
      {}

      /// @brief named set method
      template<typename TInputName>
      typename std::enable_if<std::is_same<TInputName, TName>::value>::type
      set(const typename TUniformTraits::tData& value)
      {
         tBase::set(value);
      }
};

#endif // ENGINE_BUFFEROBJECTPROGRAMINPUT_HPP
