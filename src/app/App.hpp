/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef APP_HPP
#define APP_HPP

#include "Engine.hpp"
#include "Aircraft.hpp"
#include "Camera.hpp"
#include "Terrain.hpp"
#include "Sphere.hpp"
#include "Sky.hpp"
#include <vector>
#include <memory>

/// @brief application class
class CApp : public CEngine
{
      /// @brief camera
      CCamera mCamera;

      /// @brief aircraft model
      CAircraft mAircraft;

      /// @brief sky
      CSky mSky;

      /// @brief ground
      CTerrain mTerrain;

      /// @brief bonus stars
      std::array<CAnimatedBillboard, 50> mStars;

      /// @brief if true arrows control camera, otherwise aircraft
      bool mIsCameraControl = false;

      /// @brief if true camera is bound to aircraft
      bool mCameraFollowsAircraft = true;

      /// @brief camera orientation in degrees relative to aircraft's yaw
      float mRelativeCameraOrientation = 0.f;

      /// @brief target camera pitch
      float mTargetCameraPitch = 0.f;

      /// @brief target camera eye distance
      float mTargetCameraEyeDistance = 30.f;

   public:
      /// @brief constructor
      CApp();

      /// @brief update
      void update(float timeDelta) override;

      /// @brief key presses handler
      void onKeyPressed(const sf::Event::KeyEvent& keyEvent) override;

      /// @brief draw
      void draw() const override;
};

#endif // APP_HPP
