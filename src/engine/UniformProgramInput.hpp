/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_UNIFORMPROGRAMINPUT_HPP
#define ENGINE_UNIFORMPROGRAMINPUT_HPP

#include "Uniform.hpp"

/// @brief holds state of program's uniform, use it as TProgram template
/// parameter
template<typename TUniform>
class TUniformProgramInput
{
   public:
      /// @brief uniform datatype this program input accepts
      using tValueType = const TUniform&;

      /// @brief buffer object underlying data type
      using tData = TUniform;

      /// @brief constructor
      TUniformProgramInput(const GLuint program)
         : mLocation(TUniform::getLocation(program))
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
               mUniformData.attach(mLocation);
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
            mUniformData.attach(mLocation);
         }
      }

   private:
      /// @brief locations of program input inside program
      typename tData::tLocation mLocation;

      /// @brief true if this program input is currently attached to a program
      bool mIsAttached = false;

      /// @brief holds actual buffer
      TUniform mUniformData;
};

#endif // ENGINE_BUFFEROBJECTPROGRAMINPUT_HPP
