/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_RENDERABLE_HPP
#define ENGINE_RENDERABLE_HPP

struct SContext;

/// @brief generic renderable object
class IRenderable
{
   public:
      /// @brief destructor
      virtual ~IRenderable() = default;

      /// @brief draws itself
      virtual void draw(const SContext& context) const = 0;
};

#endif // ENGINE_RENDERABLE_HPP
