/*
 * Copyright 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_SRC_TEXTURE_INPUT_HPP
#define GLCXX_SRC_TEXTURE_INPUT_HPP

#include "texture.hpp"

namespace glcxx
{
   /// @brief base implementation for texture_input
   class texture_input_base
   {
         /// @brief location of program input inside program
         GLint mLocation;

         /// @brief holds actual texture
         texture_ptr mTexture;

         /// @brief sampler id
         GLint mSamplerID;

      public:
         /// @brief constructor
         texture_input_base(const GLint location, const GLint samplerID)
            : mLocation(location)
            , mSamplerID(samplerID)
         {}

         /// @brief set new texture object as program input
         void set(const texture_ptr& value);

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
   struct texture_input : public texture_input_base
   {
      public:
         /// @brief declaration tag
         using decl_tag = DeclTag;

         /// @brief ctstring containing glsl declaration of texture uniform
         using declaration = ct::string_cat<cts("uniform sampler2D "), TName, cts(";\n")>;

         /// @brief constructor
         texture_input(const GLuint program)
            : texture_input_base(glsl::get_uniform_loc(program, TName::chars), samplerID)
         {}

         /// @brief named set method
         template<typename TInputName>
         typename std::enable_if<std::is_same<TInputName, TName>::value>::type
         set(const texture_ptr& value)
         {
            texture_input_base::set(value);
         }
   };
}

#endif
