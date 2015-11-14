/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_MODEL_HPP
#define ENGINE_MODEL_HPP

#include "Renderable.hpp"
#include <glm/glm.hpp>

class IRenderableModel : public IRenderable
{
   protected:
      glm::mat4 mModel;
   public:
      IRenderableModel(const glm::mat4& model = glm::mat4{})
         : mModel(model)
      {}

      /// @brief setter/getter
      virtual void setModel(const glm::mat4& model) { mModel = model; }
      auto& getModel() const { return mModel; }
};

class ICachedModel
{
      mutable glm::mat4 mCachedModel;
      mutable bool mIsDirty = true;
   public:
      void markDirty() const
      {
         mIsDirty = true;
      }

      const glm::mat4& getCachedModel() const
      {
         if (mIsDirty)
         {
            updateCachedModel(mCachedModel);
            mIsDirty = false;
         }
         return mCachedModel;
      }

      virtual void updateCachedModel(glm::mat4& cachedModel) const = 0;
};

#endif // ENGINE_MODEL_HPP
