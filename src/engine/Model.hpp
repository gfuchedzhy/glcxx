/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_MODEL_HPP
#define ENGINE_MODEL_HPP

#include "Renderable.hpp"
#include <glm/glm.hpp>

class CModel
{
      /// @brief scale
      glm::vec3 mScale = glm::vec3(1.f, 1.f, 1.f);

      /// @brief position
      glm::vec3 mPos;

      /// @brief pitch
      float mPitch = 0.f;

      /// @brief pitch
      float mRoll = 0.f;

      /// @brief yaw
      float mYaw = 0.f;

      /// @brief external model
      glm::mat4 mExternalModel;

      /// @brief automatic cache
      mutable struct SCache
      {
            glm::mat4 mModel;
            bool mIsDirty = true;
      } mCache;

   public:
      /// @brief returns true if cache is dirty
      bool dirty() const;

      /// @brief sets scale
      void scale(const glm::vec3& scale);

      /// @brief returns scale
      const glm::vec3& scale() const;

      /// @brief sets pos
      void pos(const glm::vec3& pos);

      /// @brief returns pos
      const glm::vec3& pos() const;

      /// @brief sets pitch
      void pitch(float pitch);

      /// @brief returns pitch
      float pitch() const;

      /// @brief sets roll
      void roll(float roll);

      /// @brief returns roll
      float roll() const;

      /// @brief sets yaw
      void yaw(float yaw);

      /// @brief returns yaw
      float yaw() const;

      /// @brief sets external model
      void externalModel(const glm::mat4& externalModel);

      /// @brief returns external model
      const glm::mat4& externalModel() const;

      /// @brief returns resulting model matrix
      const glm::mat4& model() const;

      /// @brief returns model's forward vector
      glm::vec3 forward() const;

      /// @brief returns model's up
      glm::vec3 up() const;

      /// @brief returns model's right
      glm::vec3 right() const;
};

class IRenderableModel : public IRenderable, public CModel {};

inline glm::vec3 CModel::forward() const
{
   auto&& m = model();
   return glm::vec3(m[1][0], m[1][1], m[1][2]);
}

inline glm::vec3 CModel::right() const
{
   auto&& m = model();
   return glm::vec3(m[0][0], m[0][1], m[0][2]);
}

inline glm::vec3 CModel::up() const
{
   auto&& m = model();
   return glm::vec3(m[2][0], m[2][1], m[2][2]);
}

inline bool CModel::dirty() const
{
   return mCache.mIsDirty;
}

inline const glm::vec3& CModel::scale() const
{
   return mScale;
}

inline const glm::vec3& CModel::pos() const
{
   return mPos;
}

inline float CModel::pitch() const
{
   return mPitch;
}

inline float CModel::roll() const
{
   return mRoll;
}

inline float CModel::yaw() const
{
   return mYaw;
}

inline const glm::mat4& CModel::externalModel() const
{
   return mExternalModel;
}

#endif // ENGINE_MODEL_HPP
