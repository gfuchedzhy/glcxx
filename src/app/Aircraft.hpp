/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef AIRCRAFT_HPP
#define AIRCRAFT_HPP

#include "ComplexRenderable.hpp"

class CAircraft : public CComplexRenderable
{
      /// @brief propeller, we need this field separately to apply animation to it
      CComplexRenderable* mProp;

   public:
      CAircraft();

      void update(float timeDelta);
};

#endif // AIRCRAFT_HPP
