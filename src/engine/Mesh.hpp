/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_MESH_HPP
#define ENGINE_MESH_HPP

#include "Renderable.hpp"
#include "BufferObject.hpp"
#include "Material.hpp"
#include <memory>

class aiMesh;

/// @brief geometry mesh that has some material property
class CMesh : public IRenderable
{
      /// @brief material properties
      std::shared_ptr<SMaterial> mMaterial;

      /// @brief position buffer
      std::shared_ptr<TBufferObject<glm::vec3>> mPos;

      /// @brief indices
      std::vector<GLushort> mIndices;

      /// @brief texture coord buffer
      std::shared_ptr<TBufferObject<glm::vec2>> mUV;

      /// @brief normal buffer
      std::shared_ptr<TBufferObject<glm::vec3>> mNormals;

   public:
      /// @brief constructor
      CMesh(const aiMesh& mesh, const std::shared_ptr<SMaterial>& material);

      /// @brief draw
      void draw(const SContext& context) const override;
};

#endif
