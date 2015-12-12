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
      /// @brief buffer objects ptr
      using tValueType = std::shared_ptr<TBufferObject<typename TAttribTraits::tData>>;

      /// @brief buffer object underlying data type
      using tData = typename TAttribTraits::tData;

      /// @brief ctstring containing glsl declaration of variable
      template<typename TName> using tDeclaration = typename TAttribTraits::template tDeclaration<TName>;

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
         if (mBuffer)
            mBuffer->unBind();
      }
};

/// @brief holds state of program's index buffer
class CIndexBufferProgramInput
{
      /// @brief buffer
      std::shared_ptr<CIndexBuffer> mBuffer;

   public:
      /// @brief set new buffer object as program input
      template<typename TName>
      void set(const typename std::enable_if<std::is_same<TName, cts("indices")>::value, std::shared_ptr<CIndexBuffer>>::type& value)
      {
         if (mBuffer != value)
         {
            mBuffer = value;
            attach();
         }
      }

      /// @brief called after program was selected, attach buffer
      void select() const
      {
         attach();
      }

      /// @brief draw with current index buffer
      void draw() const
      {
         assert(mBuffer);
         mBuffer->draw();
      }

      /// @brief draw with given pointer
      template<typename T>
      void draw(const T* data, GLsizei size, GLenum mode) const
      {
         assert(!mBuffer);
         constexpr GLenum id = glsl::TTypeID<T>::id;
         static_assert(GL_UNSIGNED_BYTE == id || GL_UNSIGNED_SHORT == id || GL_UNSIGNED_INT == id, "unsupported index type provided");
         gl(glDrawElements, mode, size, id, data);
      }

   private: // impl
      /// @brief attach buffer
      void attach() const
      {
         if (mBuffer)
            mBuffer->bind();
         else
            CIndexBuffer::unBind();
      }
};

#endif // ENGINE_BUFFEROBJECTPROGRAMINPUT_HPP
