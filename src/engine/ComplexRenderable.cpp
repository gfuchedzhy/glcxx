/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "ComplexRenderable.hpp"

// update objects' matrices if necessary, draw objects
void CComplexRenderable::draw() const
{
   const bool isDirty = dirty();
   auto&& m = model();
   for(auto&& p : mObjects)
   {
      if (isDirty)
      {
         p->externalModel(m);
      }
      p->draw();
   }
}
