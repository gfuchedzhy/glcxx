/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "App.hpp"
#include "engine/Log.hpp"

/// @brief entry point
int main()
{
   CApp app;
   Log::msg("main loop start");
   app.run();
   Log::msg("main loop end");
}
