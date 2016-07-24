/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_SRC_SHADER_HPP
#define GLCXX_SRC_SHADER_HPP

#include <string>
#include "gl.hpp"

//todo
// namespace glcxx
// {
   /// @brief shader holder
   class shader
   {
         /// @brief disabled stuff
         shader(const shader&) = delete;
         shader& operator=(const shader& other) = delete;

         /// @brief shader object id
         GLuint mObject = 0;

         friend class CProgramBase;

      public:
         /// @brief constructor
         shader(const std::string& src, GLenum shader_type);

         /// @brief destructor
         ~shader();
   };
// }

#endif
