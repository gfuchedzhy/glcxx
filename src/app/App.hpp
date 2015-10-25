/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef APP_HPP
#define APP_HPP

#include "engine/Engine.hpp"

/// @brief application class
class CApp : public CEngine
{
   public:
      /// @brief main loop
      void run();
};

#endif // APP_HPP
