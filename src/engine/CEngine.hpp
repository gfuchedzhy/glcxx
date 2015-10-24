/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_CENGINE_HPP
#define ENGINE_CENGINE_HPP

#include <SFML/Window.hpp>

/// @brief engine class
class CEngine
{
   public:
      /// @brief constructor
      CEngine();

   protected:
      /// @brief window object, also holds gl context
      sf::Window mWindow;
};

#endif // ENGINE_CENGINE_HPP
