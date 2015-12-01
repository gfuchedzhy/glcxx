/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Model.hpp"
#include <glm/gtc/matrix_transform.hpp>

void CModel::pos(const glm::vec3& pos)
{
   if (pos != mPos)
   {
      mPos = pos;
      mCache.mIsDirty = true;
   }
}

void CModel::scale(const glm::vec3& scale)
{
   if (scale != mScale)
   {
      mScale = scale;
      mCache.mIsDirty = true;
   }
}

void CModel::pitch(const float pitch)
{
   if (pitch != mPitch)
   {
      mPitch = pitch;
      mCache.mIsDirty = true;
   }
}

void CModel::roll(const float roll)
{
   if (roll != mRoll)
   {
      mRoll = roll;
      mCache.mIsDirty = true;
   }
}

void CModel::yaw(const float yaw)
{
   if (yaw != mYaw)
   {
      mYaw = yaw;
      mCache.mIsDirty = true;
   }
}

void CModel::externalModel(const glm::mat4& externalModel)
{
   if (externalModel != mExternalModel)
   {
      mExternalModel = externalModel;
      mCache.mIsDirty = true;
   }
}

const glm::mat4& CModel::model() const
{
   if (mCache.mIsDirty)
   {
      mCache.mModel = glm::translate(mExternalModel, mPos);
      mCache.mModel = glm::rotate(mCache.mModel, glm::radians(mYaw), glm::vec3{0.f, 0.f, 1.f});
      mCache.mModel = glm::rotate(mCache.mModel, glm::radians(mRoll), glm::vec3{0.f, 1.f, 0.f});
      mCache.mModel = glm::rotate(mCache.mModel, glm::radians(mPitch), glm::vec3{1.f, 0.f, 0.f});
      mCache.mModel = glm::scale(mCache.mModel, mScale);
      mCache.mIsDirty = false;
   }
   return mCache.mModel;
}
