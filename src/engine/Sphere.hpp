/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_SPHERE_HPP
#define ENGINE_SPHERE_HPP

#include "Model.hpp"
#include "Texture.hpp"

/// @brief generates indices, vertices, tex coords and optionally tangents for
/// halfsphere
void generateHalfSphere(size_t N,
                        std::vector<glm::vec3>& vertices,
                        std::vector<glm::vec2>& texCoords,
                        std::vector<GLushort>& indices,
                        std::vector<glm::vec3>* tangents = nullptr);

/// @brief renderable sphere that has color and model
class CSphere : public IRenderableModel
{
      /// @brief color
      glm::vec3 mColor;

   public:
      /// @brief constructor
      CSphere(const glm::vec3& color);

      /// @brief draw
      void draw(const CContext& context) const override;
};

/// @brief renderable sphere that has texture and model
class CTexturedSphere : public IRenderableModel
{
      /// @brief texture
      std::shared_ptr<CTexture> mTexture;

      /// @brief normal map
      std::shared_ptr<CTexture> mNormalMap;

   public:
      /// @brief constructor
      CTexturedSphere();

      /// @brief set texture
      void texture(std::shared_ptr<CTexture> tex) { mTexture = std::move(tex); }

      /// @brief set normal map
      void normalMap(std::shared_ptr<CTexture> normalMap) { mNormalMap = std::move(normalMap); }

      /// @brief draw
      void draw(const CContext& context) const override;
};

#endif
