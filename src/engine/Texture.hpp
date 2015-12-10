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
      void set(tTexturePtr value);

      /// @brief called after program was selected
      void select()
      {
         attach();
      }

   private: // impl
      /// @brief attach texture
      void attach();
};

/// @brief holds state of program's texture object, use it as TProgram template
/// parameter
template<typename TName, GLint samplerID = 0>
struct TTextureProgramInput : public CTextureProgramInputImpl
{
   public:
      /// @brief ctstring naming texture
      using tName = TName;

      /// @brief type of texture objects this program input accepts
      using tValueType = tTexturePtr;

      /// @brief ctstring containing glsl declaration of texture uniform
      using tFragmentShaderDeclaration = ct::string_cat<cts("uniform sampler2D "), tName, cts(";")>;

      /// @brief nothing to declare for vertex shader
      using tVertexShaderDeclaration = cts("");

      /// @brief constructor
      TTextureProgramInput(const GLuint program)
         : CTextureProgramInputImpl(glsl::TInputType<true>::template getLocation<TName>(program), samplerID)
      {}
};

#endif // ENGINE_TEXTURE_HPP
