/*
 * Copyright 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_SRC_EXCEPT_HPP
#define GLCXX_SRC_EXCEPT_HPP

#include <stdexcept>
#include <string>

namespace glcxx
{
   class glprogram_error : public std::exception
   {
         std::string mMsg;

      public:
         explicit glprogram_error(const std::string& msg);

         /// @brief modify message
         void prepend(const std::string& msg);
         void append(const std::string& msg);

         const char* what() const noexcept override;
   };

   struct shader_compilation_error : glprogram_error
   {
         using glprogram_error::glprogram_error;
   };

   struct shader_linking_error : glprogram_error
   {
         using glprogram_error::glprogram_error;
   };

   /// @brief thrown when program cannot find location of uniform or attribute
   struct input_location_error : glprogram_error
   {
         using glprogram_error::glprogram_error;
   };
}

#endif
