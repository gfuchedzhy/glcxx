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
      /// @brief object list, subobject may be complex renderable itself
      std::vector<std::shared_ptr<IRenderableModel>> mObjects;
   public:
      using tIniList = std::initializer_list<std::shared_ptr<IRenderableModel>>;

      /// @brief constructors
      CComplexRenderable() = default;
      CComplexRenderable(tIniList list)
         : mObjects(list)
      {}

      /// @brief appends objects to list
      void append(tIniList list)
      {
         mObjects.insert(mObjects.end(), list.begin(), list.end());
      }

      /// @brief draw
      void draw(const SContext& context) const override;
};

#endif // ENGINE_COMPLEXRENDERABLE_HPP
