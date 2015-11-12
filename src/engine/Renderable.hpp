/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_RENDERABLE_HPP
#define ENGINE_RENDERABLE_HPP

class IRenderable
{
   public:
      virtual ~IRenderable() = default;
      virtual void draw() const = 0;
};

#endif // ENGINE_RENDERABLE_HPP
