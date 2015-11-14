/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_COMPLEXRENDERABLE_HPP
#define ENGINE_COMPLEXRENDERABLE_HPP

#include "Model.hpp"
#include <utility>
#include <memory>
#include <vector>

/// @brief renderable model that consists of composition of other renderable models
class CComplexRenderable : public IRenderableModel
{
   public:
      /// @brief pair of object's matrix in this complex renderable's space and
      /// renderable model
      using tObject = std::pair<glm::mat4, std::shared_ptr<IRenderableModel>>;
   private:
      /// @brief object list, itself might be complex renderable
      std::vector<tObject> mObjects;

      /// @brief flags if children models should be updated
      mutable bool mIsMatrixDirty = true;

   public:
      /// @brief constructors
      CComplexRenderable() = default;
      CComplexRenderable(std::initializer_list<tObject> list)
         : mObjects(list)
      {}

      /// @brief appends objects to list
      void append(std::initializer_list<tObject> list)
      {
         mObjects.insert(mObjects.end(), list.begin(), list.end());
      }

      /// @brief sets model matrix, marks dirty
      void setModel(const glm::mat4& model) override
      {
         IRenderableModel::setModel(model);
         mIsMatrixDirty = true;
      }

      /// @brief draw
      void draw() const override;
};

#endif // ENGINE_COMPLEXRENDERABLE_HPP
