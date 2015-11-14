/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_MODEL_HPP
#define ENGINE_MODEL_HPP

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

class IRenderableCachedModel : public IRenderableModel
{
   protected:
      mutable glm::mat4 mCachedModel;
      mutable bool mIsCachedModelDirty = true;
   public:
      void setModel(const glm::mat4& model) override
      {
         IRenderableModel::setModel(model);
         markDirty();
      }

      void markDirty() const
      {
         mIsCachedModelDirty = true;
      }

      const glm::mat4& getCachedModel() const
      {
         if (mIsCachedModelDirty)
         {
            updateCachedModel(mCachedModel);
            mIsCachedModelDirty = false;
         }
         return mCachedModel;
      }

      virtual void updateCachedModel(glm::mat4& cachedModel) const = 0;
};

class CModel
{
      /// @brief position
      glm::vec3 mPos;

      /// @brief pitch
      float mPitch = 0.f;

      /// @brief pitch
      float mRoll = 0.f;

      /// @brief yaw
      float mYaw = 0.f;

      /// @brief automatic cache
      mutable struct SCache
      {
            glm::mat4 mModel;
            bool mIsDirty = true;
      } mCache;

   public:
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

      /// @brief returns model matrix
      const glm::mat4& model() const;
};

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

#endif // ENGINE_MODEL_HPP
