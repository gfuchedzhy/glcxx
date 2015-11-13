/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "App.hpp"
#include "Box.hpp"
#include <glm/gtc/matrix_transform.hpp>

CApp::CApp()
   : CEngine(800, 600)
   , mBox({0.f, 1.f, 1.f})
{}

void CApp::run()
{
   CEngine::run();
}

// @todo cleanup this quick'n'dirty porting code
const float aspect = 800/600.f;
const glm::mat4 proj = glm::perspective(45.0f, aspect, 0.1f, 100.f);
const glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3(0.f, 0.f, -3.f));
const glm::mat4 rotateX = glm::rotate(glm::mat4(), 0.5f, glm::vec3(1.f, 0.f, 0.f));
const glm::mat4 projTransRotX = proj*translate*rotateX;

void CApp::update(float timeDelta)
{
   static const float angularSpeed = 1.f;
   float angle = mAbsTime*angularSpeed;
   angle -= (int(angle)/360)*360;
   const glm::mat4 result = projTransRotX*glm::rotate(glm::mat4(), angle, glm::vec3(0.f, 1.f, 0.f));
   mBox.setModel(result);
}

void CApp::draw() const
{
   mBox.draw();
}
