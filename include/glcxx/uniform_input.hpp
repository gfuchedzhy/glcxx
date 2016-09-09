/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_UNIFORM_INPUT_HPP
#define GLCXX_UNIFORM_INPUT_HPP

#include "input_var.hpp"

namespace glcxx
{
   /// @brief base implementation for uniform program input
   template<typename UniformTraits>
   class uniform_input_base
   {
         /// @brief location of program input inside program
         GLint _location;

         /// @brief holds actual uniform
         typename UniformTraits::data _uniform_data;

      public:
         /// @brief constructor
         uniform_input_base(GLint location)
            : _location(location)
         {}

         /// @brief set uniform as program input
         void set(const typename UniformTraits::data& value)
         {
            if (_uniform_data != value)
            {
               _uniform_data = value;
               attach_uniform(_location, glsl_cast<typename UniformTraits::glsl_data>(_uniform_data));
            }
         }

         /// @brief called after program was selected, nothing to do as uniforms
         /// remains attached during program selection change
         void select() const
         {}
   };

   /// @brief holds state of program's uniform
   template<typename Name, typename UniformTraits, typename DeclTag = tag::vertex>
   class uniform_input : public uniform_input_base<UniformTraits>
   {
         /// @brief base implementation class
         using base = uniform_input_base<UniformTraits>;

      public:
         /// @brief declaration tag
         using decl_tag = DeclTag;

         /// @brief ctstring containing glsl declaration of variable
         using declaration = typename UniformTraits::template declaration<Name>;

         /// @brief constructor
         uniform_input(const GLuint program)
            : base(get_uniform_loc(program, Name::chars))
         {}

         /// @brief named set method
         template<typename InputName>
         typename std::enable_if<std::is_same<InputName, Name>::value>::type
         set(const typename UniformTraits::data& value)
         {
            base::set(value);
         }
   };
}

#endif
