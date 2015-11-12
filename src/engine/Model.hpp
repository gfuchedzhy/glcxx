/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_MODEL_HPP
#define ENGINE_MODEL_HPP

#include <glm/glm.hpp>

/// @brief base class for objects which are models
class CModel
{
   protected:
      glm::mat4 mModel;
   public:
      CModel(const glm::mat4& model = glm::mat4{})
         : mModel(model)
      {}

      void setModel(const glm::mat4& model);
      auto& getModel() const;
};

inline void CModel::setModel(const glm::mat4& model)
{
   mModel = model;
}

inline auto& CModel::getModel() const
{
   return mModel;
}

#endif // ENGINE_MODEL_HPP
