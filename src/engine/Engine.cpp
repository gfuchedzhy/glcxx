/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Engine.hpp"
#include "GL.hpp"
#include "Log.hpp"
#include "Programs.hpp"
#include <SFML/System/Clock.hpp>

CEngine::CEngine(const size_t width, const size_t height)
   : mWindow(sf::VideoMode(width, height), APPNAME, sf::Style::Titlebar | sf::Style::Close, gRenderer.contextSettings())
   , mAspect(width/float(height))
{
   mWindow.setVerticalSyncEnabled(true);

   /// @todo workaround before proper move to VAOs
   gl(glGenVertexArrays, 1, &mVAO);
   gl(glBindVertexArray, mVAO);

   gl(glEnable, GL_DEPTH_TEST);
   gl(glEnable, GL_CULL_FACE);
   gl(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   mClock.restart();
}

CEngine::~CEngine()
{
   /// @todo workaround before proper move to VAOs
   gl(glDeleteVertexArrays, 1, &mVAO);
}

void CEngine::run()
{
   while (true)
   {
      // handle events
      sf::Event event;
      while (mWindow.pollEvent(event))
      {
         switch(event.type)
         {
            case sf::Event::Closed:
               Log::msg("close event occured");
               return;
            case sf::Event::KeyPressed:
               onKeyPressed(event.key);
               break;
            default:
               break;
         }
      }

      const float timeDelta = mClock.restart().asSeconds();

      // fps counting
      mFPS.mTime += timeDelta;
      mFPS.mFrameNumber++;
      if (mFPS.mTime > 5)
      {
         Log::msg("fps: ", mFPS.mFrameNumber/mFPS.mTime);
         mFPS.mFrameNumber = 0;
         mFPS.mTime = 0;
      }

      // update scene
      update(timeDelta);

      // clear the buffers
      gl(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // draw whole scene
      draw();

      // end the current frame (internally swaps the front and back buffers)
      mWindow.display();
   }
}
