/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_BOX_HPP
#define ENGINE_BOX_HPP

#include <glm/glm.hpp>
#include "Renderable.hpp"
#include "Model.hpp"

class CBox : IRenderable, public CModel
{
      glm::vec3 mColor;
   public:
      CBox(const glm::vec3& color);
      void draw() const override;
};

#endif // ENGINE_BOX_HPP
