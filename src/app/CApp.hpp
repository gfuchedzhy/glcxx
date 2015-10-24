/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef CAPP_HPP
#define CAPP_HPP

#include "engine/CEngine.hpp"

/// @brief application class
class CApp : public CEngine
{
   public:
      /// @brief main loop
      void run();
};

#endif // CAPP_HPP
