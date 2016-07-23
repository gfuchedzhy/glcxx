/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_SRC_TEXTURE_HPP
#define GLCXX_SRC_TEXTURE_HPP

#include <memory>
#include "input_var.hpp"

// todo namespace
// namespace glcxx
// {
   /// @brief texture resource holder
   class texture
   {
         /// @brief disabled stuff
         texture(const texture&) = delete;
         texture& operator=(const texture& other) = delete;

         /// @brief texture id
         GLuint mID;

      protected:
         /// @brief texture target
         GLenum mTarget = 0;

      public:
         /// @brief reads texture from file, uploads to gpu
         texture(GLenum target)
            : mTarget(target)
         {
            gl(glGenTextures, 1, &mID);
         }

         /// @brief frees texture
         ~texture()
         {
            gl(glDeleteTextures, 1, &mID);
         }

         /// @brief binds texture
         void bind() const
         {
            gl(glBindTexture, mTarget, mID);
         }

         /// @brief unbinds texture
         void unbind()
         {
            gl(glBindTexture, mTarget, 0);
         }
   };

   /// @brief texture ptr
   using texture_ptr = std::shared_ptr<texture>;
// }

#endif
