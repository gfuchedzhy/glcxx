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

      /// @brief flags if current corresponding children model should be updated
      mutable std::vector<bool> mIsObjectsMatrixDirty;

      /// @brief flags if all children models should be updated
      mutable bool mIsMatrixDirty = true;

   public:
      /// @brief constructors
      CComplexRenderable() = default;
      CComplexRenderable(std::initializer_list<tObject> list)
         : mObjects(list)
         , mIsObjectsMatrixDirty(list.size(), true)
      {}

      /// @brief appends objects to list
      void append(std::initializer_list<tObject> list)
      {
         mObjects.insert(mObjects.end(), list.begin(), list.end());
         mIsObjectsMatrixDirty.insert(mIsObjectsMatrixDirty.end(), list.size(), true);
      }

      /// @brief appends objects to list
      const glm::mat4& subObjectModel(size_t idx)
      {
         return mObjects[idx].first;
      }

      /// @brief appends objects to list
      void subObjectModel(size_t idx, const glm::mat4& m)
      {
         mObjects[idx].first = m;
         mIsObjectsMatrixDirty[idx] = true;
      }

      using IRenderableModel::model;

      /// @brief sets model matrix, marks dirty
      void model(const glm::mat4& model) override
      {
         IRenderableModel::model(model);
         mIsMatrixDirty = true;
      }

      /// @brief draw
      void draw() const override;
};

#endif // ENGINE_COMPLEXRENDERABLE_HPP
