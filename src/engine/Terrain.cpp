/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Terrain.hpp"

CTerrain::CTerrain()
{
   texture(std::make_shared<CTexture>("res/ground.dds"));
   const float s = 3e4;
   scale(glm::vec3(s, s, s));
}

void CTerrain::draw(const SContext& context) const
{
   gl(glDisable, GL_DEPTH_TEST);
   CTexturedRect::draw(context);
   gl(glEnable, GL_DEPTH_TEST);
}
