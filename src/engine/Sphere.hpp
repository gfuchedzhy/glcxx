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

#endif
