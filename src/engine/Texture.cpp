/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include <gli/load.hpp>
#include "Texture.hpp"

CTexture::CTexture(const std::string& filename)
{
   Log::msg("loading texture from ", filename);
   gli::texture texture = gli::load(filename);
   if(texture.empty())
      throw std::runtime_error{"failed to load texture"};

   gli::gl GL;
   mTarget = GL.translate(texture.target());
   bool valid = (GL_TEXTURE_2D == mTarget);
   valid = (1 == texture.layers()) && valid;
   valid = (1 == texture.faces()) && valid;
   if (!valid)
      throw std::runtime_error{"unsupported texture format"};

   const GLint levels = static_cast<GLint>(texture.levels()) - 1;
   const gli::gl::format format = GL.translate(texture.format());
   const glm::tvec3<GLsizei> dimensions(texture.dimensions());

   gl(glGenTextures, 1, &mID);
   bind();
   gl(glTexParameteri, mTarget, GL_TEXTURE_BASE_LEVEL, 0);
   gl(glTexParameteri, mTarget, GL_TEXTURE_MAX_LEVEL, levels);
   gl(glTexParameteri, mTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   gl(glTexParameteri, mTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   gl(glTexStorage2D, mTarget, levels, format.Internal, dimensions.x, dimensions.y);
   for(int l = 0; l < levels; ++l)
   {
      assert(gli::TARGET_2D == texture.target());
      const glm::tvec3<GLsizei> subImageDimensions(texture.dimensions(l));
      if(gli::is_compressed(texture.format()))
         gl(glCompressedTexSubImage2D,
            mTarget, l, 0, 0, subImageDimensions.x, subImageDimensions.y,
            format.Internal, static_cast<GLsizei>(texture.size(l)),
            texture.data(0, 0, l));
      else
         gl(glTexSubImage2D,
            mTarget, l, 0, 0, subImageDimensions.x, subImageDimensions.y,
            format.External, format.Type, texture.data(0, 0, l));
   }
   unBind();
}

void CTextureProgramInputImpl::set(tTexturePtr value, bool isSelected)
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
         // schedule detach
         if (mIsAttached && !mTextureForDelayedDetach)
            mTextureForDelayedDetach = mTexture;
         mTexture = value;
      }
   }
}

void CTextureProgramInputImpl::select()
{
   if (mTextureForDelayedDetach)
   {
      mTextureForDelayedDetach->unBind();
      gl(glActiveTexture, GL_TEXTURE0);
      mIsAttached = false;
      mTextureForDelayedDetach = nullptr;
   }
   attach();
}

void CTextureProgramInputImpl::attach()
{
   if (mTexture)
   {
      gl(glActiveTexture, GL_TEXTURE0 + mSamplerID);
      mTexture->bind();
      glsl::attachUniform(mLocation, mSamplerID);
      mIsAttached = true;
   }
}

void CTextureProgramInputImpl::detach()
{
   if (mTexture && mIsAttached)
   {
      mTexture->unBind();
      gl(glActiveTexture, GL_TEXTURE0);
      mIsAttached = false;
   }
}
