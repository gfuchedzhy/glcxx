/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_SRC_UNIFORM_INPUT_HPP
#define GLCXX_SRC_UNIFORM_INPUT_HPP

#include "input_var.hpp"

namespace glcxx
{
   /// @brief base implementation for uniform program input
   template<typename TUniformTraits>
   class uniform_input_base
   {
         /// @brief location of program input inside program
         GLint mLocation;

         /// @brief holds actual uniform
         typename TUniformTraits::data mUniformData;

      public:
         /// @brief constructor
         uniform_input_base(GLint location)
            : mLocation(location)
         {}

         /// @brief set uniform as program input
         void set(const typename TUniformTraits::data& value)
         {
            if (mUniformData != value)
            {
               mUniformData = value;
               attach_uniform(mLocation, glsl_cast<typename TUniformTraits::glsl_data>(mUniformData));
            }
         }

         /// @brief called after program was selected, nothing to do as uniforms
         /// remains attached during program selection change
         void select() const
         {}
   };

   /// @brief holds state of program's uniform
   template<typename TName, typename TUniformTraits, typename DeclTag = tag::vertex>
   class uniform_input : public uniform_input_base<TUniformTraits>
   {
         /// @brief base implementation class
         using base = uniform_input_base<TUniformTraits>;

      public:
         /// @brief declaration tag
         using decl_tag = DeclTag;

         /// @brief ctstring containing glsl declaration of variable
         using declaration = typename TUniformTraits::template declaration<TName>;

         /// @brief constructor
         uniform_input(const GLuint program)
            : base(get_uniform_loc(program, TName::chars))
         {}

         /// @brief named set method
         template<typename TInputName>
         typename std::enable_if<std::is_same<TInputName, TName>::value>::type
         set(const typename TUniformTraits::data& value)
         {
            base::set(value);
         }
   };
}

#endif
