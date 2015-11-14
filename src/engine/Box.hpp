/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_BOX_HPP
#define ENGINE_BOX_HPP

#include <glm/gtx/transform.hpp>
#include "Model.hpp"

class CBox : public IRenderableCachedModel
{
      glm::vec3 mColor;
      glm::vec3 mDimensions;

      void updateCachedModel(glm::mat4& cachedModel) const override
      {
         cachedModel = mModel * glm::scale(mDimensions);
      }

   public:
      CBox(const glm::vec3& color, const glm::vec3& dimensions = glm::vec3(1.f, 1.f, 1.f));
      void draw() const override;
};

#endif // ENGINE_BOX_HPP
