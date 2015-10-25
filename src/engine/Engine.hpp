/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_ENGINE_HPP
#define ENGINE_ENGINE_HPP

#include <SFML/Window.hpp>

/// @brief engine class
class CEngine
{
   public:
      /// @brief constructor
      CEngine();

      /// @brief main loop
      void run();

   protected:
      /// @brief window object, also holds gl context
      sf::Window mWindow;
};

#endif // ENGINE_ENGINE_HPP
