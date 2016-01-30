/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_SKY_HPP
#define ENGINE_SKY_HPP

#include "Model.hpp"
#include "Rect.hpp"
#include "VAO.hpp"

/// @brief sky consists of 5 textured rects and some clouds todo
class CSky : public IRenderableModel
{
      /// @brief clouds implemented using billboards
      std::array<CTexturedBillboard, 9> mClouds;

      /// @brief dome texture
      tTexturePtr mTexture;

      /// @brief vao
      TVertexArrayObject<true,
                         ct::named_type<cts("aPos"), glm::vec3>,
                         ct::named_type<cts("aUV"),  glm::vec2>> mVAO;

   public:
      /// @brief constructor
      CSky(tTexturePtr tex);

      /// @brief draw
      void draw(const CContext& context) const override;
};

#endif
