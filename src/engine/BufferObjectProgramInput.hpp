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
      /// @brief buffer objects ptr
      using tValueType = std::shared_ptr<TBufferObject<typename TAttribTraits::tData>>;

      /// @brief buffer object underlying data type
      using tData = typename TAttribTraits::tData;

      /// @brief ctstring containing glsl declaration of variable, attributes
      /// declarations go to vertex shader only todo
      template<typename TName>
      using tVertexShaderDeclaration = typename TAttribTraits::template tDeclaration<TName>;
      template<typename TName>
      using tFragmentShaderDeclaration = cts("");

      /// @brief constructor
      TBufferObjectProgramInput(const GLuint program, const char* name)
         : mLocation(glsl::getAttribLocation(program, name))
      {}

      /// @brief set new buffer object as program input
      void set(const tValueType& value)
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
      tValueType mBuffer;

   private: // impl
      /// @brief attach buffer
      void attach() const
      {
         if (mBuffer)
            mBuffer->bind();
         TAttribTraits::attach(mLocation);
         TBufferObject<typename TAttribTraits::tData>::unBind();
      }
};

#endif // ENGINE_BUFFEROBJECTPROGRAMINPUT_HPP
