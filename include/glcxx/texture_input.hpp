/*
 * Copyright 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_TEXTURE_INPUT_HPP
#define GLCXX_TEXTURE_INPUT_HPP

#include "texture.hpp"

namespace glcxx
{
   /// @brief base implementation for texture_input
   class texture_input_base
   {
         /// @brief location of program input inside program
         GLint _location;

         /// @brief holds actual texture
         texture_ptr _texture;

         /// @brief sampler id
         GLint _sampler_id;

      public:
         /// @brief constructor
         texture_input_base(const GLint location, const GLint sampler_id)
            : _location(location)
            , _sampler_id(sampler_id)
         {}

         /// @brief set new texture object as program input
         void set(const texture_ptr& value)
         {
            if (_texture != value)
            {
               if (_texture)
               {
                  glcxx_gl(glActiveTexture, GL_TEXTURE0 + _sampler_id);
                  _texture->unbind();
               }
               _texture = value;
               attach();
            }
         }

         /// @brief called after program was selected
         void select() const
         {
            attach();
         }

      private: // impl
         /// @brief attach texture
         void attach() const
         {
            if (_texture)
            {
               glcxx_gl(glActiveTexture, GL_TEXTURE0 + _sampler_id);
               _texture->bind();
               attach_uniform(_location, _sampler_id);
            }
         }
   };

   /// @brief holds state of program's texture object
   template<typename Name, GLint SamplerID = 0, typename DeclTag = tag::fragment>
   struct texture_input : public texture_input_base
   {
      public:
         /// @brief declaration tag
         using decl_tag = DeclTag;

         /// @brief ctstring containing glsl declaration of texture uniform
         using declaration = ct::string_cat<cts("uniform sampler2D "), Name, cts(";\n")>;

         /// @brief constructor
         texture_input(const GLuint program)
            : texture_input_base(get_uniform_loc(program, Name::chars), SamplerID)
         {}

         /// @brief named set method
         template<typename InputName>
         typename std::enable_if<std::is_same<InputName, Name>::value>::type
         set(const texture_ptr& value)
         {
            texture_input_base::set(value);
         }
   };
}

#endif
