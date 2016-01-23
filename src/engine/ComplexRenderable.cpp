/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "ComplexRenderable.hpp"

// update objects' matrices if necessary, draw objects
void CComplexRenderable::draw(const SContext& context) const
{
   const bool isDirty = dirty();
   const auto& m = model();
   for(const auto& p : mObjects)
   {
      if (isDirty)
      {
         p->externalModel(m);
      }
      p->draw(context);
   }
}
