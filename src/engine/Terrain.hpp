/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_TERRAIN_HPP
#define ENGINE_TERRAIN_HPP

#include "Model.hpp"
#include "Texture.hpp"
#include "VAO.hpp"

/// @brief dummy terrain is rectangle with texture
class CTerrain : public IRenderableModel
{
      /// @brief texture
      std::shared_ptr<CTexture> mTexture;

      /// @brief vao
      TVertexArrayObject<true,
                         ct::named_type<cts("aPos"), glm::vec3>,
                         ct::named_type<cts("aUV"),  glm::vec2>> mVAO;

   public:
      /// @brief constructor
      CTerrain(std::shared_ptr<CTexture> tex);

      /// @brief draw
      void draw(const CContext& context) const override;
};

#endif
