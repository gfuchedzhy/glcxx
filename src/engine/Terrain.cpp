/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Terrain.hpp"
#include "GLState.hpp"

CTerrain::CTerrain()
{
   texture(std::make_shared<CTexture>("res/ground.dds"));
   const float s = 3e4;
   scale(glm::vec3(s, s, s));
}

void CTerrain::draw(const CContext& context) const
{
   SDisableDepthTestGuard lock;
   CTexturedRect::draw(context);
}
