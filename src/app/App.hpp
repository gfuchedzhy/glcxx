/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef APP_HPP
#define APP_HPP

#include "Engine.hpp"
#include "Aircraft.hpp"
#include "Camera.hpp"
#include "Rect.hpp"
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
      CTexturedRect mGround;

      /// @brief clouds implemented using billboards
      std::array<CTexturedBillboard, 6> mClouds;

      /// @brief sphere
      CSphere mSphere;

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
