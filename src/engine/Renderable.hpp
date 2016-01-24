/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_RENDERABLE_HPP
#define ENGINE_RENDERABLE_HPP

class CContext;

/// @brief generic renderable object
class IRenderable
{
   public:
      /// @brief destructor
      virtual ~IRenderable() = default;

      /// @brief draws itself
      virtual void draw(const CContext& context) const = 0;
};

#endif // ENGINE_RENDERABLE_HPP
