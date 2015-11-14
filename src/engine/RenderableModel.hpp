/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_RENDERABLEMODEL_HPP
#define ENGINE_RENDERABLEMODEL_HPP

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

#endif // ENGINE_RENDERABLEMODEL_HPP
