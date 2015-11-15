/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_RECT_HPP
#define ENGINE_RECT_HPP

#include "Model.hpp"
#include "Texture.hpp"

class CTexturedRect : public IRenderableModel
{
      std::shared_ptr<CTexture> mTexture;
   public:
      CTexturedRect();
      void texture(std::shared_ptr<CTexture> tex) { mTexture = tex; }
      void draw() const override;
};

#endif
