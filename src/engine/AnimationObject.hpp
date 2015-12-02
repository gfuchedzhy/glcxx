/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_ANIMATIONOBJECT_HPP
#define ENGINE_ANIMATIONOBJECT_HPP

class CAnimationObject
{
   protected:
      /// @brief number of frames in current object
      unsigned int mFrameNumber = 32;

      /// @brief local time measured in frames
      float mCurrentFrame = 0.f;

   public:
      /// @brief set total frame number
      void frameNumber(unsigned int frameNumber) { mFrameNumber = frameNumber; }

      /// @brief update current frame
      void update(float timeDelta)
      {
         static int fps = 60;
         mCurrentFrame += fps*timeDelta;
         mCurrentFrame -= mFrameNumber*int(mCurrentFrame/mFrameNumber);
      }
};


#endif
