/*
 * Copyright 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_GLSTATE_HPP
#define ENGINE_GLSTATE_HPP

/// @brief RAII style blending switch
struct SEnableBlendingGuard
{
      /// @brief default blending factor
      static constexpr GLenum defaultSrcFactor = GL_SRC_ALPHA;
      static constexpr GLenum defaultDstFactor = GL_ONE_MINUS_SRC_ALPHA;

      /// @brief constructor
      SEnableBlendingGuard(GLenum sfactor = defaultSrcFactor, GLenum dfactor = defaultDstFactor)
      {
         gl(glEnable, GL_BLEND);
         if (sfactor != defaultSrcFactor || dfactor != defaultDstFactor)
            gl(glBlendFunc, sfactor, dfactor);
      }

      /// @brief destructor
      ~SEnableBlendingGuard()
      {
         gl(glDisable, GL_BLEND);
         gl(glBlendFunc, defaultSrcFactor, defaultDstFactor);
      }
};

/// @brief RAII style depth test switch
struct SDisableDepthTestGuard
{
      /// @brief constructor
      SDisableDepthTestGuard()
      {
         gl(glDisable, GL_DEPTH_TEST);
      }

      /// @brief destructor
      ~SDisableDepthTestGuard()
      {
         gl(glEnable, GL_DEPTH_TEST);
      }
};

/// @brief RAII style depth mask switch
struct SDisableDepthMaskGuard
{
      /// @brief constructor
      SDisableDepthMaskGuard()
      {
         gl(glDepthMask, GL_FALSE);
      }

      /// @brief destructor
      ~SDisableDepthMaskGuard()
      {
         gl(glDepthMask, GL_TRUE);
      }
};

#endif
