/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "ComplexRenderable.hpp"

// update objects' matrices if necessary, draw objects
void CComplexRenderable::draw() const
{
   for(auto&& p : mObjects)
   {
      assert(p.second);
      if (mIsMatrixDirty)
         p.second->setModel(mModel * p.first);
      p.second->draw();
   }
   mIsMatrixDirty = false;
}
