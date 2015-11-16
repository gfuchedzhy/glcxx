/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef AIRCRAFT_HPP
#define AIRCRAFT_HPP

#include "ComplexRenderable.hpp"

/// @brief simple model of aircraft made out of boxes
class CAircraft : public CComplexRenderable
{
      /// @brief propeller, we need this field separately to apply animation to it
      CComplexRenderable* mProp;

   public:
      /// @brief constructor
      CAircraft();

      /// @brief update, animates propeller
      void update(float timeDelta);
};

#endif // AIRCRAFT_HPP
