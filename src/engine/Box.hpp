/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_BOX_HPP
#define ENGINE_BOX_HPP

#include <glm/gtx/transform.hpp>
#include "Model.hpp"

/// @brief renderable box that has color and model
class CBox : public IRenderableModel
{
      /// @brief color
      glm::vec3 mColor;

   public:
      /// @brief constructor
      CBox(const glm::vec3& color)
         : mColor(color)
      {}

      /// @brief draw
      void draw(const SContext& context) const override;
};

#endif // ENGINE_BOX_HPP
