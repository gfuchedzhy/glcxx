/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_MESH_HPP
#define ENGINE_MESH_HPP

#include "Renderable.hpp"
#include "Material.hpp"
#include "VAO.hpp"
#include <memory>

class aiMesh;

/// @brief geometry mesh that has some material property
class CMesh : public IRenderable
{
      /// @brief material properties
      std::shared_ptr<SMaterial> mMaterial;

      /// @brief vao
      TVertexArrayObject<true,
                         ct::named_type<cts("aPos"),  glm::vec3>,
                         ct::named_type<cts("aUV"),   glm::vec2>,
                         ct::named_type<cts("aNorm"), glm::vec3>,
                         ct::named_type<cts("aTan"),  glm::vec3>> mVAO;

   public:
      /// @brief constructor
      CMesh(const aiMesh& mesh, const std::shared_ptr<SMaterial>& material);

      /// @brief draw
      void draw(const SContext& context) const override;
};

#endif
