/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Sky.hpp"
#include "Texture.hpp"
#include "Rect.hpp"

CSky::CSky()
{
   const float size = 2000;
   auto tex = std::make_shared<CTexture>("sky.dds");
   std::array<std::shared_ptr<CTexturedRect>, 5> sky;
   for (auto&& x : sky)
   {
      x = std::make_shared<CTexturedRect>();
      x->texture(tex);
      x->scale(glm::vec3(size, size, size));
      append({x});
   }
   sky[0]->pitch(90);
   sky[0]->pos(glm::vec3(0, size/2, 0));
   sky[1]->pitch(180);
   sky[1]->pos(glm::vec3(0, 0, size/2));
   sky[2]->pitch(90);
   sky[2]->roll(90);
   sky[2]->pos(glm::vec3(-size/2, 0, 0));
   sky[3]->pitch(90);
   sky[3]->roll(-90);
   sky[3]->pos(glm::vec3(size/2, 0, 0));
   sky[4]->pitch(-90);
   sky[4]->pos(glm::vec3(0, -size/2, 0));
   pos(glm::vec3(0, 0, size/4));
}
