/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

CCamera::SCache::SCache()
{
   mIsDirty.set();
}

void CCamera::perspective(float fovY, float aspect, float near, float far)
{
   if (fovY != mFovY || near != mNear || far  != mFar)
   {
      mFovY   = fovY;
      mAspect = aspect;
      mNear   = near;
      mFar    = far;
      mCache.mIsDirty.set(eProj);
      mCache.mIsDirty.set(eViewProj);
   }
}

void CCamera::dollyZoom(float dollyZoom)
{
   if (dollyZoom != mDollyZoom)
   {
      mDollyZoom = dollyZoom;
      mCache.mIsDirty.set(eProj);
      mCache.mIsDirty.set(eView);
      mCache.mIsDirty.set(eViewProj);
   }
}

void CCamera::lookAt(const glm::vec3& lookAt)
{
   if (lookAt != mLookAt)
   {
      mLookAt = lookAt;
      mCache.mIsDirty.set(eView);
      mCache.mIsDirty.set(eViewProj);
   }
}

void CCamera::orientation(const float orientation)
{
   if (orientation != mOrientation)
   {
      mOrientation = orientation;
      mCache.mIsDirty.set(eView);
      mCache.mIsDirty.set(eViewProj);
   }
}

void CCamera::eyeDistance(const float eyeDistance)
{
   if (eyeDistance != mEyeDistance)
   {
      mEyeDistance = eyeDistance;
      mCache.mIsDirty.set(eView);
      mCache.mIsDirty.set(eViewProj);
   }
}

void CCamera::pitch(const float pitch)
{
   if (pitch != mPitch)
   {
      mPitch = pitch;
      mCache.mIsDirty.set(eView);
      mCache.mIsDirty.set(eViewProj);
   }
}

const glm::mat4& CCamera::proj() const
{
   if (mCache.mIsDirty.test(eProj))
   {
      const float dollyFov = 2*atan(tan(0.5f*glm::radians(mFovY)) / mDollyZoom);
      mCache.mProj = glm::perspective(dollyFov, mAspect, mNear, mFar);
      mCache.mIsDirty.reset(eProj);
   }
   return mCache.mProj;
}

const glm::mat4& CCamera::view() const
{
   if (mCache.mIsDirty.test(eView))
   {
      mCache.mView = glm::translate(glm::mat4{}, glm::vec3{0.f, 0.f, -mDollyZoom*mEyeDistance});
      mCache.mView = glm::rotate(mCache.mView, glm::radians(mPitch-90.f), glm::vec3{1.f, 0.f, 0.f});
      mCache.mView = glm::rotate(mCache.mView, glm::radians(-mOrientation), glm::vec3{0.f, 0.f, 1.f});
      mCache.mView = glm::translate(mCache.mView, -mLookAt);
      mCache.mIsDirty.reset(eView);
   }
   return mCache.mView;
}

const glm::mat4& CCamera::viewProj() const
{
   if (mCache.mIsDirty.test(eViewProj))
   {
      mCache.mViewProj = proj() * view();
      mCache.mIsDirty.reset(eViewProj);
   }
   return mCache.mViewProj;
}
