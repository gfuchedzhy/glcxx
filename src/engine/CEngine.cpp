/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "CEngine.hpp"

CEngine::CEngine()
   : mWindow(sf::VideoMode(800, 600, sf::Style::Titlebar | sf::Style::Close), APPNAME)
{
}
