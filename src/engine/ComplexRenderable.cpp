/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "ComplexRenderable.hpp"

// update objects' matrices if necessary, draw objects
void CComplexRenderable::draw() const
{
   assert(mObjects.size() == mIsObjectsMatrixDirty.size());
   for(size_t i = 0; i < mObjects.size(); ++i)
   {
      auto&& p = mObjects[i];
      assert(p.second);
      if (mIsMatrixDirty || mIsObjectsMatrixDirty[i])
      {
         p.second->setModel(mModel * p.first);
         mIsObjectsMatrixDirty[i] = false;
      }
      p.second->draw();
   }
   mIsMatrixDirty = false;
}
