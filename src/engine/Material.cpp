/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Material.hpp"
#include <assimp/scene.h>

SMaterial::SMaterial(const aiMaterial& material, const std::string& textureDirectory)
{
   aiColor3D color;

   if (AI_SUCCESS != material.Get(AI_MATKEY_COLOR_DIFFUSE, color))
      throw std::runtime_error{"couldn't load diffuse color"};
   mDiffuse = {color.r, color.g, color.b};

   if (AI_SUCCESS != material.Get(AI_MATKEY_COLOR_AMBIENT, color))
      throw std::runtime_error{"couldn't load ambient color"};
   mAmbient = {color.r, color.g, color.b};

   if (AI_SUCCESS != material.Get(AI_MATKEY_COLOR_SPECULAR, color))
      throw std::runtime_error{"couldn't load specular color"};
   mSpecular = {color.r, color.g, color.b};

   if (AI_SUCCESS != material.Get(AI_MATKEY_SHININESS, mShininess))
      throw std::runtime_error{"couldn't load shininess"};

   aiString path;
   if (AI_SUCCESS == material.GetTexture(aiTextureType_DIFFUSE, 0, &path))
      mDiffuseMap = std::make_shared<CTexture>(textureDirectory + path.data);

   /// @todo bug in assimp library: normal map is available as height map for
   /// some reason
   if (AI_SUCCESS == material.GetTexture(aiTextureType_HEIGHT, 0, &path))
      mNormalMap = std::make_shared<CTexture>(textureDirectory + path.data);
}
