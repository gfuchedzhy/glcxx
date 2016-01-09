/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_ENGINE_HPP
#define ENGINE_ENGINE_HPP

#include <SFML/Window.hpp>
#include <chrono>
#include "Context.hpp"

/// @brief engine class
class CEngine
{
   public:
      /// @brief constructor
      CEngine(const size_t width, const size_t height);

      /// @brief destructor
      virtual ~CEngine() = default;

      /// @brief draw
      virtual void update(float timeDelta) = 0;

      /// @brief key presses handler
      virtual void onKeyPressed(const sf::Event::KeyEvent& keyEvent) = 0;

      /// @brief draw
      virtual void draw() const = 0;

      /// @brief main loop
      virtual void run();

   protected:
      /// @brief window object, also holds gl context
      sf::Window mWindow;

      /// @brief aspect ratio
      float mAspect;

      /// @brief current absolute time
      std::chrono::steady_clock::time_point mTime;

      /// @brief struct for fps counting
      struct
      {
            float mTime = 0.f;
            unsigned int mFrameNumber = 0;
      } mFPS;

      /// @brief drawing context
      SContext mContext;
};

#endif // ENGINE_ENGINE_HPP
