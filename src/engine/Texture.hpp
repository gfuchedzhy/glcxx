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
         gl(glBindTexture, GL_TEXTURE_2D, mID);
      }

      /// @brief unbinds texture
      static void unBind()
      {
         gl(glBindTexture, GL_TEXTURE_2D, 0);
      }

   private:
      /// @brief texture id
      GLuint mID;
};

/// @brief holds state of program's texture object, use it as TProgram template
/// parameter
template<typename TName, int samplerID = 0>
class TTextureProgramInput
{
   public:
      /// @brief ctstring naming texture
      using tName = TName;

      /// @brief type of texture objects this program input accepts
      using tValueType = std::shared_ptr<CTexture>;

      /// @brief ctstring containing glsl declaration of texture uniform
      using tFragmentShaderDeclaration = ct::string_cat<cts("uniform sampler2D "), tName, cts(";")>;

      /// @brief nothing to declare for vertex shader
      using tVertexShaderDeclaration = cts("");

      /// @brief constructor
      TTextureProgramInput(const GLuint program)
         : mLocation(glsl::TInputType<true>::template getLocation<tName>(program))
      {}

      /// @brief set new texture object as program input, isSelected should be
      /// true if program this program input belongs to is currently selected
      void set(tValueType value, bool isSelected)
      {
         if (mTexture != value)
         {
            if (isSelected)
            {
               // detach old input, attach new one
               detach();
               mTexture = value;
               attach();
            }
            else
            {
               // if attached but not selected and detach is not scheduled yet -
               // schedule detach, @note that we don't need texture itself to
               // perform detach, but it should be alive until then, so save it
               if (mIsAttached && !mTextureForDelayedDetach)
                  mTextureForDelayedDetach = mTexture;
               mTexture = value;
            }
         }
      }

      /// @brief called after program was selected, perform delayed detach,
      /// attach texture
      void select()
      {
         if (mTextureForDelayedDetach)
         {
            CTexture::unBind();
            gl(glActiveTexture, GL_TEXTURE0);
            mIsAttached = false;
            mTextureForDelayedDetach = nullptr;
         }
         attach();
      }

   private:
      /// @brief location of program input inside program
      GLint mLocation;

      /// @brief true if this program input is currently attached to a program
      bool mIsAttached = false;

      /// @brief holds actual texture
      tValueType mTexture;

      /// @brief if not nullptr holds outdated texture, that should be detached
      /// when current program will be selected
      tValueType mTextureForDelayedDetach;

   private: // impl
      /// @brief attach texture
      void attach()
      {
         if (mTexture)
         {
            gl(glActiveTexture, GL_TEXTURE0 + samplerID);
            mTexture->bind();
            glsl::attachUniform(mLocation, samplerID);
            mIsAttached = true;
         }
      }

      /// @brief detach texture
      void detach()
      {
         if (mTexture && mIsAttached)
         {
            CTexture::unBind();
            gl(glActiveTexture, GL_TEXTURE0);
            mIsAttached = false;
         }
      }
};

#endif // ENGINE_TEXTURE_HPP
