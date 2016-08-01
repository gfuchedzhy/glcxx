/*
 * Copyright 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_SRC_CAPABILITIES_HPP
#define GLCXX_SRC_CAPABILITIES_HPP

#include "gl.hpp"

namespace glcxx
{
   /// @brief RAII style blending switch
   struct enable_blending_guard
   {
         /// @brief default blending factor
         static constexpr GLenum default_src_factor = GL_SRC_ALPHA;
         static constexpr GLenum default_dst_factor = GL_ONE_MINUS_SRC_ALPHA;

         /// @brief set default blending factors
         static void set_default_factors()
         {
            glcxx_gl(glBlendFunc, default_src_factor, default_dst_factor);
         }

         /// @brief constructor
         enable_blending_guard(GLenum sfactor = default_src_factor, GLenum dfactor = default_dst_factor)
         {
            glcxx_gl(glEnable, GL_BLEND);
            if (sfactor != default_src_factor || dfactor != default_dst_factor)
               glcxx_gl(glBlendFunc, sfactor, dfactor);
         }

         /// @brief destructor
         ~enable_blending_guard()
         {
            glcxx_gl(glDisable, GL_BLEND);
            set_default_factors();
         }
   };

   /// @brief RAII style depth test switch
   struct disable_depth_test_guard
   {
         /// @brief constructor
         disable_depth_test_guard()
         {
            glcxx_gl(glDisable, GL_DEPTH_TEST);
         }

         /// @brief destructor
         ~disable_depth_test_guard()
         {
            glcxx_gl(glEnable, GL_DEPTH_TEST);
         }
   };

   /// @brief RAII style depth mask switch
   struct disable_depth_mask_guard
   {
         /// @brief constructor
         disable_depth_mask_guard()
         {
            glcxx_gl(glDepthMask, GL_FALSE);
         }

         /// @brief destructor
         ~disable_depth_mask_guard()
         {
            glcxx_gl(glDepthMask, GL_TRUE);
         }
   };
}

#endif
