/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_CONTEXT_HPP
#define ENGINE_CONTEXT_HPP

#include "Programs.hpp"
#include "Renderer.hpp"
#include "GLState.hpp"

/// @brief drawing context
class CContext
{
      /// @brief context settings
      sf::ContextSettings mContextSettings{24, 0, 0, 3, 3, sf::ContextSettings::Core};

      /// @brief underlying sfml context
      sf::Context mInternalContext{mContextSettings, 1, 1};

      /// @brief renderer holding programs
      using tRenderer = TRenderer<cts("regular-col"),
                                  cts("regular-tex"),
                                  cts("shaded-col"),
                                  cts("shaded-tex"),
                                  cts("shaded-tex-nmap"),
                                  cts("billboard-tex"),
                                  cts("billboard-tex-sprite"),
                                  cts("billboard-hb"),
                                  cts("particlesys-tex-sprite-flame")>;
      // renderer state doesn't change logical const of context
      mutable tRenderer mRenderer;

   public:
      /// @brief constructor
      CContext()
      {
         // setup default flags
         gl(glEnable, GL_DEPTH_TEST);
         gl(glEnable, GL_CULL_FACE);
         gl(glBlendFunc, SEnableBlendingGuard::defaultSrcFactor, SEnableBlendingGuard::defaultDstFactor);
      }

      /// @brief return drawing program
      template<typename TName>
      auto& getProgram() const
      {
         return mRenderer.getProgram<TName>();
      }

      /// @brief return context settings
      const auto& contextSettings() const
      {
         return mContextSettings;
      }

      /// additional flags to enable/disable various features
      /// @brief if true draw health bars
      bool mDrawHealthBars  = false;

      /// @brief if true draw debug normals
      bool mDrawNormals  = false;
};

#endif
