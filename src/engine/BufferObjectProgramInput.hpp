/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_BUFFEROBJECTPROGRAMINPUT_HPP
#define ENGINE_BUFFEROBJECTPROGRAMINPUT_HPP

#include "BufferObject.hpp"
#include <memory>

/// @brief holds state of program's buffer object
template<typename TAttribTraits>
class TBufferObjectProgramInput
{
   public:
      /// @brief buffer object ptr
      using tBufferPtr = std::shared_ptr<TBufferObject<typename TAttribTraits::tData>>;

      /// @brief buffer object underlying data type
      using tData = typename TAttribTraits::tData;

      /// @brief ctstring containing glsl declaration of variable
      template<typename TName> using tDeclaration = typename TAttribTraits::template tDeclaration<TName>;

      /// @brief constructor
      TBufferObjectProgramInput(const GLuint program, const char* name)
         : mLocation(glsl::getAttribLocation(program, name))
      {}

      /// @brief set new buffer object as program input
      void set(const tBufferPtr& value)
      {
         if (mBuffer != value)
         {
            // detach old input, attach new one
            if (mBuffer)
               TAttribTraits::detach(mLocation);
            mBuffer = value;
            attach();
         }
      }

      /// @brief called after program was selected, attach buffer
      void select() const
      {
         attach();
      }

   private:
      /// @brief location of program input inside program
      GLint mLocation;

      /// @brief holds actual buffer
      tBufferPtr mBuffer;

   private: // impl
      /// @brief attach buffer
      void attach() const
      {
         if (mBuffer)
            mBuffer->bind();
         TAttribTraits::attach(mLocation);
         if (mBuffer)
            mBuffer->unBind();
      }
};

#endif // ENGINE_BUFFEROBJECTPROGRAMINPUT_HPP
