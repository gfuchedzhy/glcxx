/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef AIRCRAFT_HPP
#define AIRCRAFT_HPP

#include "ComplexRenderable.hpp"
#include "Rect.hpp"

/// @brief simple model of aircraft made out of boxes
class CAircraft : public CComplexRenderable
{
      /// @brief propeller, we need this field separately to apply animation to it
      CComplexRenderable* mProp;

      /// @brief healthbar list todo
      std::vector<std::pair<CHealthBar, IRenderableModel*>> mHealthBars;

   public:
      /// @brief constructor
      CAircraft();

      /// @brief update, animates propeller
      void update(float timeDelta);

      /// @brief draw
      void draw(const SContext& context) const override;
};

#endif // AIRCRAFT_HPP
