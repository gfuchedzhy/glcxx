/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "App.hpp"
#include "Log.hpp"
#include "Programs.hpp"
#include <glm/gtc/matrix_transform.hpp>

CApp::CApp()
   : CEngine(800, 600)
{
   mCamera.perspective(35.f, mAspect, 0.1f, 1e3f);
   mCamera.eyeDistance(50.f);
   mCamera.pitch(30.f);
}

void CApp::update(float timeDelta)
{
   mAircraft.update(timeDelta);

   const float cameraSpeed = 70.f;
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
   {
      mCamera.orientation(std::min(120.f, mCamera.orientation() + timeDelta*cameraSpeed));
   }
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
   {
      mCamera.orientation(std::max(-120.f, mCamera.orientation() - timeDelta*cameraSpeed));
   }
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
   {
      mCamera.pitch(std::min(90.f, mCamera.pitch() + timeDelta*cameraSpeed));
   }
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
   {
      mCamera.pitch(std::max(-30.f, mCamera.pitch() - timeDelta*cameraSpeed));
   }

   const float cameraEyeDistanceSpeed = 40.f;
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
   {
      mCamera.eyeDistance(std::max(30.f, mCamera.eyeDistance() - timeDelta*cameraEyeDistanceSpeed));
   }
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
   {
      mCamera.eyeDistance(std::min(300.f, mCamera.eyeDistance() + timeDelta*cameraEyeDistanceSpeed));
   }

   auto p = gRenderer.get<cts("coloredPolygon")>();
   p->set<cts("uViewProj")>(mCamera.viewProj());
}

void CApp::draw() const
{
   mAircraft.draw();
}
