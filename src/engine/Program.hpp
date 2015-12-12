/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAM_HPP
#define ENGINE_PROGRAM_HPP

#include "Shader.hpp"
#include "ProgramInput.hpp"
#include <tuple>

/// @brief resource holder for opengl program object
class CProgramBase
{
      /// @brief disabled stuff
      CProgramBase(const CProgramBase&) = delete;
      CProgramBase& operator=(const CProgramBase& other) = delete;
   public:
      /// @brief constuctor
      CProgramBase(const std::string& vertexSrc, const std::string& fragmentSrc);

      /// @brief destructor
      ~CProgramBase();

      /// @brief selects program
      void select() const
      {
         gl(glUseProgram, mObject);
      }

      /// @brief draw with current index buffer
      void draw(const std::shared_ptr<CIndexBuffer>& buffer) const
      {
         assert(buffer);
         buffer->bind();
         buffer->draw();
      }

      /// @brief draw with given pointer
      template<typename T>
      void draw(const T* data, GLsizei size, GLenum mode) const
      {
         constexpr GLenum id = glsl::TTypeID<T>::id;
         static_assert(GL_UNSIGNED_BYTE == id || GL_UNSIGNED_SHORT == id || GL_UNSIGNED_INT == id, "unsupported index type provided");
         CIndexBuffer::unBind();
         gl(glDrawElements, mode, size, id, data);
      }

   protected:
      /// @brief program object id
      GLuint mObject = 0;

   private:
      /// @brief shaders
      CShader mVertexShader;
      CShader mFragmentShader;
};

template<typename... TProgramInput>
class TProgram : public CProgramBase, public TProgramInputs<TProgramInput...>
{
   public:
      /// @brief ctstring containing glsl declarations of all program inputs
      using tVertexShaderDeclaration = ct::string_cat<typename TProgramInput::tVertexShaderDeclaration...>;
      using tFragmentShaderDeclaration = ct::string_cat<typename TProgramInput::tFragmentShaderDeclaration...>;

      /// @brief constructor
      TProgram(const std::string& vertexSrc, const std::string& fragmentSrc)
         : CProgramBase(tVertexShaderDeclaration::chars + vertexSrc, tFragmentShaderDeclaration::chars + fragmentSrc)
         , TProgramInputs<TProgramInput...>(mObject)
      {}

      /// @brief @see CProgramBase::select
      void select() const
      {
         CProgramBase::select();
         swallow(TProgramInput::select());
      }
};

#endif // ENGINE_PROGRAM_HPP
