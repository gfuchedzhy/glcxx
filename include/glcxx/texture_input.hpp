// Copyright 2016 Grygoriy Fuchedzhy
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef GLCXX_TEXTURE_INPUT_HPP
#define GLCXX_TEXTURE_INPUT_HPP

#include "glcxx/texture.hpp"

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
                  glActiveTexture(GL_TEXTURE0 + _sampler_id);
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
               glActiveTexture(GL_TEXTURE0 + _sampler_id);
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
