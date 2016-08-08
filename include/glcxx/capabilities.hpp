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

#ifndef GLCXX_CAPABILITIES_HPP
#define GLCXX_CAPABILITIES_HPP

#include "glcxx/gl.hpp"

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
            glBlendFunc(default_src_factor, default_dst_factor);
         }

         /// @brief constructor
         enable_blending_guard(GLenum sfactor = default_src_factor, GLenum dfactor = default_dst_factor)
         {
            glEnable(GL_BLEND);
            if (sfactor != default_src_factor || dfactor != default_dst_factor)
               glBlendFunc(sfactor, dfactor);
         }

         /// @brief destructor
         ~enable_blending_guard()
         {
            glDisable(GL_BLEND);
            set_default_factors();
         }
   };

   /// @brief RAII style depth test switch
   struct disable_depth_test_guard
   {
         /// @brief constructor
         disable_depth_test_guard()
         {
            glDisable(GL_DEPTH_TEST);
         }

         /// @brief destructor
         ~disable_depth_test_guard()
         {
            glEnable(GL_DEPTH_TEST);
         }
   };

   /// @brief RAII style depth mask switch
   struct disable_depth_mask_guard
   {
         /// @brief constructor
         disable_depth_mask_guard()
         {
            glDepthMask(GL_FALSE);
         }

         /// @brief destructor
         ~disable_depth_mask_guard()
         {
            glDepthMask(GL_TRUE);
         }
   };
}

#endif
