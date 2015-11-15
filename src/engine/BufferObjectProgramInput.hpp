/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_BUFFEROBJECTPROGRAMINPUT_HPP
#define ENGINE_BUFFEROBJECTPROGRAMINPUT_HPP

#include "BufferObject.hpp"
#include <memory>

/// @brief holds state of program's buffer object, use it as TProgram template
/// parameter
template<typename TAttribTraits>
class TBufferObjectProgramInput
{
   public:
      /// @brief type of buffer objects this program input accepts
      using tValueType = std::shared_ptr<TBufferObject<TAttribTraits>>;

      /// @brief buffer object underlying data type
      using tData = typename TAttribTraits::tData;

      /// @brief buffer object underlying attribute name
      using tName = typename TAttribTraits::tName;

      /// @brief ctstring containing glsl declaration of variable, attributes
      /// declarations go to vertex shader only
      using tVertexShaderDeclaration = typename TAttribTraits::tDeclaration;
      using tFragmentShaderDeclaration = cts("");

      /// @brief constructor
      TBufferObjectProgramInput(const GLuint program)
         : mLocation(TAttribTraits::getLocation(program))
      {}

      /// @brief set new buffer object as program input, isSelected should be
      /// true if program this program input belongs to is currently selected
      void set(tValueType value, bool isSelected)
      {
         if (mBuffer != value)
         {
            if (isSelected)
            {
               // detach old input, attach new one
               detach();
               mBuffer = value;
               attach();
            }
            else
            {
               // if attached but not selected and detach is not scheduled yet -
               // schedule detach, @note that we don't need buffer itself to
               // perform detach, but it should be alive until then, so save it
               if (mIsAttached && !mBufferForDelayedDetach)
                  mBufferForDelayedDetach = mBuffer;
               mBuffer = value;
            }
         }
      }

      /// @brief called after program was selected, perform delayed detach,
      /// attach buffer
      void select()
      {
         if (mBufferForDelayedDetach)
         {
            TAttribTraits::detach(mLocation);
            mIsAttached = false;
            mBufferForDelayedDetach = nullptr;
         }
         attach();
      }

   private:
      /// @brief location of program input inside program
      typename TAttribTraits::tLocation mLocation;

      /// @brief true if this program input is currently attached to a program
      bool mIsAttached = false;

      /// @brief holds actual buffer
      tValueType mBuffer;

      /// @brief if not nullptr holds outdated buffer, that should be detached
      /// when current program will be selected
      tValueType mBufferForDelayedDetach;

   private: // impl
      /// @brief attach buffer
      void attach()
      {
         /// @todo for some reason glVertexAttribPointer settings do not persist
         /// after switching to different program
         if (mBuffer /* && !mIsAttached */)
         {
            mBuffer->bind();
            TAttribTraits::attach(mLocation);
            mBuffer->unBind();
            mIsAttached = true;
         }
      }

      /// @brief detach buffer
      void detach()
      {
         if (mBuffer && mIsAttached)
         {
            TAttribTraits::detach(mLocation);
            mIsAttached = false;
         }
      }
};

#endif // ENGINE_BUFFEROBJECTPROGRAMINPUT_HPP
