/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_TEXTURE_HPP
#define ENGINE_TEXTURE_HPP

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

/// @brief make texture
inline tTexturePtr make_texture(const std::string& filename)
{
   return std::make_shared<CTexture>(filename);
}

/// @brief base implementation for TTextureProgramInput
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

/// @brief holds state of program's texture object
template<typename TName, GLint samplerID = 0, typename DeclTag = tag::fragment>
struct TTextureProgramInput : public CTextureProgramInputImpl
{
   public:
      /// @brief declaration tag
      using tDeclTag = DeclTag;

      /// @brief ctstring containing glsl declaration of texture uniform
      using tDeclaration = ct::string_cat<cts("uniform sampler2D "), TName, cts(";\n")>;

      /// @brief constructor
      TTextureProgramInput(const GLuint program)
         : CTextureProgramInputImpl(glsl::getUniformLocation(program, TName::chars), samplerID)
      {}

      /// @brief named set method
      template<typename TInputName>
      typename std::enable_if<std::is_same<TInputName, TName>::value>::type
      set(const tTexturePtr& value)
      {
         CTextureProgramInputImpl::set(value);
      }
};

#endif // ENGINE_TEXTURE_HPP
