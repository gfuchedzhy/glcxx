/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_CAMERA_HPP
#define ENGINE_CAMERA_HPP

#include <glm/glm.hpp>
#include <bitset>

class CCamera
{
      /// @brief near, far
      float mNear = 0.1f;
      float mFar  = 1e5f;

      /// @brief vertical field of view in degrees
      float mFovY = 35.f;

      /// @brief aspect ratio (w/h)
      float mAspect = 1.f;

      /// @brief lookAt point from lookat to eye
      glm::vec3 mLookAt;

      /// @brief orientation angle in degrees, ccw
      float mOrientation = 0.f;

      /// @brief distance from lookAt to eye
      float mEyeDistance = 10.f;

      /// @brief pitch angle in degrees, 0 is horizontal view
      float mPitch = 0.f;

      enum EDirtyFlag
      {
         eProj = 0,
         eView,
         eViewProj
      };

      /// @brief automatic cache
      mutable struct SCache
      {
            SCache();
            glm::mat4 mView;
            glm::mat4 mProj;
            glm::mat4 mViewProj;
            std::bitset<3> mIsDirty;
      } mCache;

   public:
      /// @brief sets perspective
      void perspective(float fovY, float aspect, float near, float far);

      /// @brief sets lookAt
      void lookAt(const glm::vec3& lookAt);

      /// @brief returns lookAt point
      const glm::vec3& lookAt() const;

      /// @brief sets orientation
      void orientation(float orientation);

      /// @brief returns orientation
      float orientation() const;

      /// @brief sets eye distance
      void eyeDistance(float eyeDistance);

      /// @brief returns eye distance
      float eyeDistance() const;

      /// @brief sets pitch
      void pitch(float pitch);

      /// @brief returns pitch
      float pitch() const;

      /// @brief returns up
      glm::vec3 up() const;

      /// @brief returns right
      glm::vec3 right() const;

      /// @brief returns back
      glm::vec3 back() const;

      /// @brief returns eye position
      glm::vec3 eye() const;

      /// @brief return perspective scale, multiplier to convert from eye space
      /// into projective space (world coordinates after viewProjection
      /// traansformation or NDC before perspective division) by x and y axes
      glm::vec2 perspectiveScale() const;

      /// @brief returns projection matrix
      const glm::mat4& proj() const;

      /// @brief returns view matrix
      const glm::mat4& view() const;

      /// @brief returns view-projection matrix
      const glm::mat4& viewProj() const;
};

inline glm::vec3 CCamera::up() const
{
   const auto& v = view();
   return glm::vec3(v[0][1], v[1][1], v[2][1]);
}

inline glm::vec3 CCamera::right() const
{
   const auto& v = view();
   return glm::vec3(v[0][0], v[1][0], v[2][0]);
}

inline glm::vec3 CCamera::back() const
{
   const auto& v = view();
   return glm::vec3(v[0][2], v[1][2], v[2][2]);
}

inline glm::vec3 CCamera::eye() const
{
   return lookAt() + eyeDistance()*back();
}

inline glm::vec2 CCamera::perspectiveScale() const
{
   // proj[1][1] = 1/tan(fovY)
   const float p11 = proj()[1][1];
   return {p11/mAspect, p11};
}

inline const glm::vec3& CCamera::lookAt() const
{
   return mLookAt;
}

inline float CCamera::orientation() const
{
   return mOrientation;
}

inline float CCamera::eyeDistance() const
{
   return mEyeDistance;
}

inline float CCamera::pitch() const
{
   return mPitch;
}

#endif // ENGINE_CAMERA_HPP
