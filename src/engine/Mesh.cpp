/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Mesh.hpp"
#include "Programs.hpp"
#include <assimp/scene.h>

CMesh::CMesh(const aiMesh& mesh, const std::shared_ptr<SMaterial>& material)
   : mMaterial(material)
{
   if (!mesh.HasPositions())
      throw std::runtime_error{"loaded mesh doesn't have positions"};
   mPos = std::make_shared<TBufferObject<glm::vec3>>(reinterpret_cast<const glm::vec3*>(mesh.mVertices),
                                                     mesh.mNumVertices);
   if (!mesh.HasTextureCoords(0))
      throw std::runtime_error{"loaded mesh doesn't have texture coords"};

   // texture coords in loaded by assimp has 3 components so reusing raw memory
   // is not an option, we have to repack it
   std::vector<glm::vec2> uv;
   uv.reserve(mesh.mNumVertices);
   for (size_t t = 0; t < mesh.mNumVertices; ++t)
      uv.push_back({mesh.mTextureCoords[0][t].x, -mesh.mTextureCoords[0][t].y});
   mUV = std::make_shared<TBufferObject<glm::vec2>>(&uv[0], mesh.mNumVertices);

   for (size_t f = 0; f < mesh.mNumFaces; ++f)
   {
      const aiFace& face = mesh.mFaces[f];
      std::copy(face.mIndices, face.mIndices + face.mNumIndices, back_inserter(mIndices));
   }
}

void CMesh::draw(const SContext& context) const
{
   auto p = gRenderer.get<cts("texturedPolygon")>();
   p->set<cts("aPos")>(mPos);
   p->set<cts("aUV")>(mUV);

   if (mMaterial->mDiffuseMap)
      p->set<cts("uTexture")>(mMaterial->mDiffuseMap);

   gl(glDrawElements, GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT, &mIndices[0]);
}
