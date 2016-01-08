/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_BUFFEROBJECTPROGRAMINPUT_HPP
#define ENGINE_BUFFEROBJECTPROGRAMINPUT_HPP

#include "BufferObject.hpp"
#include <memory>

/// @brief base implementation for TBufferObjectProgramInput
template<typename TAttribTraits>
class TBufferObjectProgramInputImpl
{
   public:
      /// @brief declaration tag, attributes always go to vertex shader
      using tDeclTag = tag::vertex;

      /// @brief buffer object ptr
      using tBufferPtr = std::shared_ptr<TBufferObject<typename TAttribTraits::tData>>;

      /// @brief buffer object underlying data type
      using tData = typename TAttribTraits::tData;

      /// @brief constructor
      TBufferObjectProgramInputImpl(const GLint location)
         : mLocation(location)
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

/// @brief holds state of program's buffer object
template<typename TName, typename TAttribTraits>
class TBufferObjectProgramInput : public TBufferObjectProgramInputImpl<TAttribTraits>
{
      /// @brief base implementation class
      using tBase = TBufferObjectProgramInputImpl<TAttribTraits>;

   public:
      /// @brief ctstring containing glsl declaration of variable
      using tDeclaration = typename TAttribTraits::template tDeclaration<TName>;

      /// @brief constructor
      TBufferObjectProgramInput(const GLuint program)
         : tBase(glsl::getAttribLocation(program, TName::chars))
      {}

      /// @brief named set method
      template<typename TInputName>
      typename std::enable_if<std::is_same<TInputName, TName>::value>::type
      set(const typename tBase::tBufferPtr& value)
      {
         tBase::set(value);
      }
};

#endif // ENGINE_BUFFEROBJECTPROGRAMINPUT_HPP
