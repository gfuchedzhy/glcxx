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
/// parameter, todo add comments
template<typename DeclarationTag, typename TUniformTraits>
class TUniformProgramInput
{
   public:
      /// @brief uniform datatype this program input accepts
      using tValueType = const typename TUniformTraits::tData&;

      /// @brief underlying unform name
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
         : mLocation(TUniformTraits::getLocation(program))
      {}

      /// @brief set new buffer object as program input, isSelected should be
      /// true if program this program input belongs to is currently selected
      void set(tValueType value, bool isSelected)
      {
         if (mUniformData != value)
         {
            mUniformData = value;
            // it is allowed to attach only if current program is selected
            if (isSelected)
            {
               TUniformTraits::attach(mLocation, mUniformData);
            }
            mIsAttached = isSelected;
         }
      }

      /// @brief called after program was selected, perform delayed detach,
      /// attach buffer
      void select()
      {
         if (!mIsAttached)
         {
            TUniformTraits::attach(mLocation, mUniformData);
         }
      }

   private:
      /// @brief location of program input inside program
      typename TUniformTraits::tLocation mLocation;

      /// @brief true if this program input is currently qattached to a program
      bool mIsAttached = false;

      /// @brief holds actual buffer
      typename TUniformTraits::tData mUniformData;
};

#endif // ENGINE_BUFFEROBJECTPROGRAMINPUT_HPP
