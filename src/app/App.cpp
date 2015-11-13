/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "App.hpp"
#include "Log.hpp"
#include "Box.hpp"
#include "Programs.hpp"
#include <glm/gtc/matrix_transform.hpp>

CApp::CApp()
   : CEngine(800, 600)
   , mBox({0.f, 1.f, 1.f})
{
   mCamera.perspective(35.f, mAspect, 0.1f, 1e2f);
   mCamera.eyeDistance(10.f);
   mCamera.pitch(30.f);
}

void CApp::update(float timeDelta)
{
   static const float angularSpeed = 90.f;
   float angle = mAbsTime*angularSpeed;
   angle -= (int(angle)/360)*360;
   mCamera.orientation(angle);
   auto p = gRenderer.get<cts("coloredPolygon")>();
   p->set<cts("uViewProj")>(mCamera.viewProj());
}

void CApp::draw() const
{
   mBox.draw();
}
