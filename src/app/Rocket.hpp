/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ROCKET_HPP
#define ROCKET_HPP

#include "Flame.hpp"
#include "Rect.hpp"
#include "Renderable.hpp"

/// @brief rocket
class CRocket : public IRenderable
{
      /// @brief true if it was launched
      bool mIsActive = false;

      /// @brief true if collided or ran out of fuel
      bool mExploded = false;

      /// @brief exaust flame
      CJetFlame mFlame;

      /// @brief explosion animation
      CAnimatedBillboard mExplosion;

      /// @brief current rocket speed
      float mSpeed;

      /// @brief time from launch
      float mTime = 0;

   public:
      /// @brief constructor
      CRocket()
         : mFlame(1, 400, 200)
      {
         mExplosion.size({100, 100});
      }

      /// @brief return true of active
      bool active() const { return mIsActive; }

      /// @brief set flame texture
      void flameTexture(tTexturePtr tex) { mFlame.texture(std::move(tex)); }

      /// @brief set explosion texture
      void explosionTexture(tTexturePtr tex) { mExplosion.texture(std::move(tex)); }

      /// @brief launch rocket from pos in dir with initialSpeed
      void launch(const glm::vec3& pos, const glm::vec3& dir, float initialSpeed);

      /// @brief update
      void update(float timeDelta);

      /// @brief draw
      void draw(const CContext& context) const override;
};

#endif
