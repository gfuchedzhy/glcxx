/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_TEXTURE_HPP
#define ENGINE_TEXTURE_HPP

#include "GL.hpp"
#include "GLSLInputVariable.hpp"
#include <string>
#include <memory>

/// @brief texture object
class CTexture
{
      /// @brief disabled stuff
      CTexture(const CTexture&) = delete;
      CTexture& operator=(const CTexture& other) = delete;
   public:
      /// @brief reads texture from file, uploads to gpu
      CTexture(const std::string& filename);

      /// @brief frees texture
      ~CTexture()
      {
         gl(glDeleteTextures, 1, &mID);
      }

      /// @brief binds texture
      void bind() const
      {
         gl(glBindTexture, mTarget, mID);
      }

      /// @brief unbinds texture
      void unBind()
      {
         gl(glBindTexture, mTarget, 0);
      }

   private:
      /// @brief texture id
      GLuint mID;

      /// @brief texture target
      GLenum mTarget;
};

/// @brief texture ptr
using tTexturePtr = std::shared_ptr<CTexture>;

/// @brief impl class for texture program input
class CTextureProgramInputImpl
{
      /// @brief location of program input inside program
      GLint mLocation;

      /// @brief holds actual texture
      tTexturePtr mTexture;

      /// @brief sampler id
      GLint mSamplerID;

   public:
      /// @brief constructor
      CTextureProgramInputImpl(const GLint location, const GLint samplerID)
         : mLocation(location)
         , mSamplerID(samplerID)
      {}

      /// @brief set new texture object as program input
      void set(const tTexturePtr& value);

      /// @brief called after program was selected
      void select() const
      {
         attach();
      }

   private: // impl
      /// @brief attach texture
      void attach() const;
};

/// @brief holds state of program's texture object, use it as TProgram template
/// parameter
template<GLint samplerID = 0>
struct TTextureProgramInput : public CTextureProgramInputImpl
{
   public:
      /// @brief type of texture objects this program input accepts
      using tValueType = tTexturePtr;

      /// @brief ctstring containing glsl declaration of texture uniform todo
      template<typename TName>
      using tFragmentShaderDeclaration = ct::string_cat<cts("uniform sampler2D "), TName, cts(";")>;

      /// @brief nothing to declare for vertex shader todo
      template<typename TName>
      using tVertexShaderDeclaration = cts("");

      /// @brief constructor
      TTextureProgramInput(const GLuint program, const char* name)
         : CTextureProgramInputImpl(glsl::TInputType<true>::getLocation(program, name), samplerID)
      {}
};

#endif // ENGINE_TEXTURE_HPP
