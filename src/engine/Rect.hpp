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
      void draw() const override;
};

/// @brief renderable rectangle has texture, size and position in space and
/// always directed perpendicular to camera view
class CBillboard : public IRenderable
{
      /// @brief texture
      std::shared_ptr<CTexture> mTexture;

      /// @brief position
      glm::vec3 mPos;

      /// @brief size
      glm::vec2 mSize;

   public:
      /// @brief constructor
      CBillboard();

      /// @brief set position
      void pos(const glm::vec3& pos) { mPos = pos; }

      /// @brief set size
      void size(const glm::vec2& size) { mSize = size; }

      /// @brief set texture
      void texture(std::shared_ptr<CTexture> tex) { mTexture = tex; }

      /// @brief draw
      void draw() const override;
};

#endif
