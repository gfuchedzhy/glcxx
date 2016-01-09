/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_MESH_HPP
#define ENGINE_MESH_HPP

#include "Renderable.hpp"
#include "BufferObject.hpp"
#include "Material.hpp"
#include "Programs.hpp"
#include <memory>

class aiMesh;

/// @brief geometry mesh that has some material property
class CMesh : public IRenderable
{
      /// @brief material properties
      std::shared_ptr<SMaterial> mMaterial;

      tRenderer::program_vao_ptr<cts("shaded-tex")> mVao;

      tRenderer::program_vao_ptr<cts("shaded-tex-nmap")> mVaoNMap;

   public:
      /// @brief constructor
      CMesh(const aiMesh& mesh, const std::shared_ptr<SMaterial>& material);

      /// @brief draw
      void draw(const SContext& context) const override;
};

#endif
