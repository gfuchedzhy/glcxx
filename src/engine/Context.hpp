/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_CONTEXT_HPP
#define ENGINE_CONTEXT_HPP

/// @brief additional context info for drawing
struct SContext
{
      /// @brief if true objects should draw their frames of reference
      bool mDrawDebugFrames = false;

      /// @brief if true draw health bars
      bool mDrawHealthBars  = false;

      /// @brief if true draw debug normals
      bool mDrawNormals  = false;
};

#endif
