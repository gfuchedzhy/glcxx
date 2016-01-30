/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_BILLBOARD_HPP
#define ENGINE_BILLBOARD_HPP

#include "Model.hpp"
#include "Texture.hpp"
#include "AnimationObject.hpp"

/// @brief base class for billboards, renderable rectangle has size and position
/// in space and always directed perpendicular to camera view
class CBillboard : public IRenderable
{
   protected:
      /// @brief position
      glm::vec3 mPos;

      /// @brief size
      glm::vec2 mSize = glm::vec2(1.0, 1.0);

   public:
      /// @brief set position
      void pos(const glm::vec3& pos) { mPos = pos; }

      /// @brief return position
      const glm::vec3& pos() const { return mPos; }

      /// @brief set size
      void size(const glm::vec2& size) { mSize = size; }
};

/// @brief billboard with texture
class CTexturedBillboard : public CBillboard
{
   protected:
      /// @brief texture
      std::shared_ptr<CTexture> mTexture;

   public:
      /// @brief set texture
      void texture(std::shared_ptr<CTexture> tex) { mTexture = std::move(tex); }

      /// @brief draw
      void draw(const CContext& context) const override;
};

/// @brief billboard with atlased animation
class CAnimatedBillboard : public CBillboard, public CAtlasedAnimationObject
{
   public:
      /// @brief draw
      void draw(const CContext& context) const override;
};

/// @brief billboard with texture
class CHealthBar : public CBillboard
{
      /// @brief value in range [0, 1]
      float mValue;

   public:
      /// @brief constructor
      CHealthBar(float value = 1.f)
         : mValue(value)
      {}

      /// @brief sets value
      void value(float value) { mValue = value; }

      /// @brief returns value
      float value() const { return mValue; }

      /// @brief draw
      void draw(const CContext& context) const override;
};

#endif
