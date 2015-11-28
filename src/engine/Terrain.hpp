/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_TERRAIN_HPP
#define ENGINE_TERRAIN_HPP

#include "Rect.hpp"

/// @brief terrain is rectangle with texture
class CTerrain : public CTexturedRect
{
   public:
      /// @brief constructor
      CTerrain();

      /// @brief draw
      void draw(const SContext& context) const override;
};

#endif
