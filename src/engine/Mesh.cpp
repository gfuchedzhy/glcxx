/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Mesh.hpp"
#include "Programs.hpp"
#include <assimp/scene.h>

CMesh::CMesh(const aiMesh& mesh, std::shared_ptr<SMaterial> material)
   : mMaterial(std::move(material))
{
   if (!mesh.HasPositions())
      throw std::runtime_error{"loaded mesh doesn't have positions"};
   mVAO.upload<cts("aPos")>(reinterpret_cast<const glm::vec3*>(mesh.mVertices),
                            mesh.mNumVertices);
   if (!mesh.HasTextureCoords(0))
      throw std::runtime_error{"loaded mesh doesn't have texture coords"};

   // texture coords in loaded by assimp has 3 components so reusing raw memory
   // is not an option, we have to repack it
   std::vector<glm::vec2> uv;
   uv.reserve(mesh.mNumVertices);
   for (size_t t = 0; t < mesh.mNumVertices; ++t)
      uv.push_back({mesh.mTextureCoords[0][t].x, -mesh.mTextureCoords[0][t].y});
   mVAO.upload<cts("aUV")>(&uv[0], mesh.mNumVertices);

   if (!mesh.HasNormals())
      throw std::runtime_error{"loaded mesh doesn't have normals"};
   mVAO.upload<cts("aNorm")>(reinterpret_cast<const glm::vec3*>(mesh.mNormals),
                             mesh.mNumVertices);

   if (mMaterial->mNormalMap && !mesh.HasTangentsAndBitangents())
      throw std::runtime_error{"loaded mesh has normal map but doesn't have tangents"};
   mVAO.upload<cts("aTan")>(reinterpret_cast<const glm::vec3*>(mesh.mTangents),
                            mesh.mNumVertices);

   std::vector<GLushort> indices;
   for (size_t f = 0; f < mesh.mNumFaces; ++f)
   {
      const aiFace& face = mesh.mFaces[f];
      std::copy(face.mIndices, face.mIndices + face.mNumIndices, back_inserter(indices));
   }
   mVAO.upload<cts("indices")>(&indices[0], indices.size(), GL_TRIANGLES);
}

void CMesh::draw(const SContext& context) const
{
   assert(mMaterial->mDiffuseMap);

   if (mMaterial->mNormalMap)
   {
      auto& p = gRenderer.get<cts("shaded-tex-nmap")>();
      p.set<cts("uTexture")>(mMaterial->mDiffuseMap);
      p.set<cts("uAmbient")>(mMaterial->mAmbient);
      p.set<cts("uDiffuse")>(mMaterial->mDiffuse);
      p.set<cts("uSpecular")>(mMaterial->mSpecular);
      p.set<cts("uShininess")>(mMaterial->mShininess);
      p.set<cts("uNormalMap")>(mMaterial->mNormalMap);
      p.drawElements(mVAO);
   }
   else
   {
      auto& p = gRenderer.get<cts("shaded-tex")>();
      p.set<cts("uTexture")>(mMaterial->mDiffuseMap);
      p.set<cts("uAmbient")>(mMaterial->mAmbient);
      p.set<cts("uDiffuse")>(mMaterial->mDiffuse);
      p.set<cts("uSpecular")>(mMaterial->mSpecular);
      p.set<cts("uShininess")>(mMaterial->mShininess);
      p.drawElements(mVAO);
   }
}
