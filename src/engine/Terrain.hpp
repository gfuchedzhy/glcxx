/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_TERRAIN_HPP
#define ENGINE_TERRAIN_HPP

#include "Model.hpp"
#include "Texture.hpp"
#include "VAO.hpp"

/// @brief dummy terrain is bunch of rectangles covering area under skydome
class CTerrain : public IRenderableModel
{
      /// @brief texture
      tTexturePtr mTexture;

      /// @brief vao
      tIndexedVAO<ct::named_type<cts("aPos"), glm::vec3>,
                  ct::named_type<cts("aUV"),  glm::vec2>> mVAO;

      /// @brief terrain center
      glm::vec2 mCenter;

      /// @brief terrain radius
      float mRadius;

   public:
      /// @brief constructor
      CTerrain(tTexturePtr tex);

      /// @brief set region where to draw terrain
      void center(const glm::vec2& center) { mCenter = center; }
      void radius(float radius) { mRadius = radius; }

      /// @brief draw
      void draw(const CContext& context) const override;
};

#endif
