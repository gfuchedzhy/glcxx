/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_RECT_HPP
#define ENGINE_RECT_HPP

#include "Model.hpp"
#include "Texture.hpp"

/// @brief renderable rectangle that has texture and model
class CTexturedRect : public IRenderableModel
{
      /// @brief texture
      std::shared_ptr<CTexture> mTexture;

   public:
      /// @brief constructor
      CTexturedRect();

      /// @brief set texture
      void texture(std::shared_ptr<CTexture> tex) { mTexture = tex; }

      /// @brief draw
      void draw(const SContext& context) const override;
};

/// @brief base class for billboards, renderable rectangle has size and position
/// in space and always directed perpendicular to camera view
class CBillboard : public IRenderable
{
   protected:
      /// @brief position
      glm::vec3 mPos;

      /// @brief size
      glm::vec2 mSize;

   public:
      /// @brief set position
      void pos(const glm::vec3& pos) { mPos = pos; }

      /// @brief set size
      void size(const glm::vec2& size) { mSize = size; }
};

/// @brief billboard with texture
class CTexturedBillboard : public CBillboard
{
      /// @brief texture
      std::shared_ptr<CTexture> mTexture;

   public:
      /// @brief constructor
      CTexturedBillboard();

      /// @brief set texture
      void texture(std::shared_ptr<CTexture> tex) { mTexture = tex; }

      /// @brief draw
      void draw(const SContext& context) const override;
};

#endif
