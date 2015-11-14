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
   static const float angularSpeed = 10.f;
   float angle = mAbsTime*angularSpeed;
   angle -= (int(angle)/360)*360;
   mCamera.orientation(angle);
   auto p = gRenderer.get<cts("coloredPolygon")>();
   p->set<cts("uViewProj")>(mCamera.viewProj());
}

void CApp::draw() const
{
   mAircraft.draw();
}
