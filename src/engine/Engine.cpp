/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Engine.hpp"
#include "GL.hpp"
#include "Log.hpp"
#include "Programs.hpp"
#include <SFML/System/Clock.hpp>

CEngine::CEngine(const size_t width, const size_t height)
   : mWindow(sf::VideoMode(width, height), APPNAME, sf::Style::Titlebar | sf::Style::Close,
             sf::ContextSettings(24, 0, 0, 3, 0))
   , mAspect(width/float(height))
{
   mWindow.setVerticalSyncEnabled(true);
   sf::ContextSettings settings = mWindow.getSettings();
   Log::msg("openGL ", settings.majorVersion, '.', settings.minorVersion,  " version loaded");
   gRenderer.init();

   gl(glEnable, GL_DEPTH_TEST);
   gl(glEnable, GL_CULL_FACE);
   gl(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void CEngine::run()
{
   sf::Clock clock;

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

      const float timeDelta = clock.restart().asSeconds();
      mAbsTime += timeDelta;
      update(timeDelta);

      // clear the buffers
      gl(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // draw whole scene
      draw();

      // end the current frame (internally swaps the front and back buffers)
      mWindow.display();
   }
}
