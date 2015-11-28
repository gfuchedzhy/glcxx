/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_SKY_HPP
#define ENGINE_SKY_HPP

#include "ComplexRenderable.hpp"

/// @brief sky consists of 5 textured rects and some clouds
class CSky : public CComplexRenderable
{
   public:
      /// @brief constructor
      CSky();

      /// @brief draw
      void draw(const SContext& context) const override;
};

#endif
