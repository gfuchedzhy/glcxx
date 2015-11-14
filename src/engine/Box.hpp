/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_BOX_HPP
#define ENGINE_BOX_HPP

#include <glm/glm.hpp>
#include "RenderableModel.hpp"

class CBox : public IRenderableModel
{
      glm::vec3 mColor;
   public:
      CBox(const glm::vec3& color);
      void draw() const override;
};

#endif // ENGINE_BOX_HPP
