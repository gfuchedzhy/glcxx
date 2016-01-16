/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Engine.hpp"
#include "GL.hpp"
#include "Log.hpp"
#include "Programs.hpp"
#include "GLState.hpp"

CEngine::CEngine(const size_t width, const size_t height)
   : mWindow(sf::VideoMode(width, height), APPNAME, sf::Style::Titlebar | sf::Style::Close, gRenderer.contextSettings())
   , mAspect(width/float(height))
   , mTime(std::chrono::steady_clock::now())
{
   mWindow.setVerticalSyncEnabled(true);

   gl(glEnable, GL_DEPTH_TEST);
   gl(glEnable, GL_CULL_FACE);
   gl(glBlendFunc, SEnableBlendingGuard::defaultSrcFactor, SEnableBlendingGuard::defaultDstFactor);
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

      using namespace std::chrono;
      const steady_clock::time_point newTime = steady_clock::now();
      const float timeDelta = duration_cast<duration<float>>(newTime - mTime).count();
      mTime = newTime;

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
