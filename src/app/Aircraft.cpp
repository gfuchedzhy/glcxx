/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "Aircraft.hpp"
#include "Context.hpp"
#include "Programs.hpp"
#include <algorithm>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

CAircraft::CAircraft()
   : mFlames{{{0.8, 300, 100}, {0.8, 300, 100}}}
{
   Assimp::Importer importer;
   const aiScene* scene = importer.ReadFile("res/Su-35_SuperFlanker/Su-35_SuperFlanker.obj",
                                            aiProcess_CalcTangentSpace       |
                                            aiProcess_JoinIdenticalVertices  |
                                            aiProcess_SortByPType);
   if (!scene)
      throw std::runtime_error{"couldn't load aircraft model"};

   std::vector<std::shared_ptr<SMaterial>> materials;
   for (size_t m = 0; m < scene->mNumMaterials; ++m)
   {
      const aiMaterial* material = scene->mMaterials[m];
      assert(material);
      materials.push_back(std::make_shared<SMaterial>(*material, "res/Su-35_SuperFlanker/"));
   }

   for (size_t m = 0; m < scene->mNumMeshes; ++m)
   {
      const aiMesh* const mesh = scene->mMeshes[m];
      assert(mesh);
      mMeshes.emplace_back(std::make_unique<CMesh>(*mesh, materials[mesh->mMaterialIndex]));
   }

   auto flameTex = std::make_shared<CTexture>("res/flame-sprite.dds");
   for(auto& f : mFlames)
      f.texture(flameTex);

   for (auto&& h : mHealthBars)
      h.size({3, 0.5});
}

void CAircraft::update(float timeDelta)
{
   if (0 != mSpeed && pos().z <= 0)
   {
      mSpeed = 0;
      for (auto&& h : mHealthBars)
         h.value(0);
   }

   const glm::vec3& fwd = forward();

   pos(pos() + fwd*mSpeed*timeDelta);

   // place healthbars
   for (size_t i = 0; i < mHealthBars.size(); ++i)
      mHealthBars[i].pos(pos() + (-5.f + 7*i)*fwd);

   // place flames in engines of this model
   const glm::vec3 flamePos = pos() - 6.2f*fwd - 0.2f*up();
   mFlames[0].pos(flamePos + 1.15f*right());
   mFlames[1].pos(flamePos - 1.15f*right());
   for(auto& f : mFlames)
   {
      f.dir(-fwd);
      f.sourceVelocity(mSpeed*fwd);
      f.update(timeDelta);
   }
}

void CAircraft::draw(const SContext& context) const
{
   auto p = gRenderer.get<cts("shaded-tex")>();
   p->set<cts("uModel")>(model());
   auto p2 = gRenderer.get<cts("shaded-tex-nmap")>();
   p2->set<cts("uModel")>(model());

   gl(glEnable, GL_BLEND);
   for (const auto& m: mMeshes)
      m->draw(context);
   gl(glDisable, GL_BLEND);

   for (auto& f : mFlames)
      f.draw(context);

   if (context.mDrawHealthBars)
   {
      for (const auto& h : mHealthBars)
         h.draw(context);
   }
}

void CAircraft::randomDamage()
{
   std::uniform_real_distribution<float> d(0, 1);
   for (auto&& h : mHealthBars)
      h.value(d(random_gen));
}

void CAircraft::repair()
{
   auto it = std::min_element(begin(mHealthBars), end(mHealthBars),
                              [] (const CHealthBar& a, const CHealthBar& b)
                              {
                                 return a.value() < b.value();
                              });
   assert(it != end(mHealthBars));
   it->value(std::min(it->value() + 0.5f, 1.f));
}
