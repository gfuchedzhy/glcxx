/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_SKY_HPP
#define ENGINE_SKY_HPP

#include "Model.hpp"
#include "Billboard.hpp"
#include "VAO.hpp"

/// @brief sky consists of 5 textured rects and some clouds todo
class CSky : public IRenderableModel
{
      /// @brief clouds implemented using billboards
      std::vector<CTexturedBillboard> mClouds;

      /// @brief dome texture
      tTexturePtr mTexture;

      /// @brief vao
      tIndexedVAO<ct::named_type<cts("aPos"), glm::vec3>,
                  ct::named_type<cts("aUV"),  glm::vec2>> mVAO;

   public:
      /// @brief constructor
      CSky(tTexturePtr tex, float horizonDistance);

      /// @brief draw
      void draw(const CContext& context) const override;
};

#endif
