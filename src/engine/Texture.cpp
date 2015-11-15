/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Texture.hpp"
#include "Log.hpp"
#include <SFML/Graphics/Image.hpp>

CTexture::CTexture(const std::string& filename)
{
   Log::msg("loading texture from ", filename);
   sf::Image img;
   if (img.loadFromFile(filename))
   {
      auto size = img.getSize();
      gl(glGenTextures, 1, &mID);
      bind();
      gl(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)img.getPixelsPtr());
      gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      unBind();
   }
   else
      throw std::runtime_error{"texture loading failed"};
}
