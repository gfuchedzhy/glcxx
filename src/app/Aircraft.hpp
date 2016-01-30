/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef AIRCRAFT_HPP
#define AIRCRAFT_HPP

#include "Model.hpp"
#include "Mesh.hpp"
#include "Billboard.hpp"
#include "Flame.hpp"
#include "Rocket.hpp"

/// @brief simple model of aircraft made out of boxes
class CAircraft : public IRenderableModel
{
      /// @brief meshes
      std::vector<CMesh> mMeshes;

      /// @brief flames, left and right
      std::array<CJetFlame, 2> mFlames;

      /// @brief rockets, left and right
      std::array<CRocket, 6> mRockets;

      /// @brief aircrafts speed in m/s
      float mSpeed = 80;

      /// @brief healthbar list
      std::array<CHealthBar, 3> mHealthBars;

   public:
      /// @brief constructor
      CAircraft();

      /// @brief launch rocket
      bool launchRocket();

      /// @brief return speed
      float speed() const { return mSpeed; }

      /// @brief set speed
      void speed(float speed) { mSpeed = speed; }

      /// @brief randomly decreases healthbars
      void randomDamage();

      /// @brief repair most damaged detail by 50%
      void repair();

      /// @brief update, animates propeller
      void update(float timeDelta);

      /// @brief draw
      void draw(const CContext& context) const override;
};

#endif // AIRCRAFT_HPP
