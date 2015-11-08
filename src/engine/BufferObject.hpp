/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_BUFFEROBJECT_HPP
#define ENGINE_BUFFEROBJECT_HPP

#include "Attribute.hpp"

/// @brief buffer object
template<typename TAttributeDataPack>
class TBufferObject
{
   public:
      /// @brief creates buffer
      TBufferObject()
      {
         gl(glGenBuffers, 1, &mID);
      }

      /// @brief frees buffer
      ~TBufferObject()
      {
         gl(glDeleteBuffers, 1, &mID);
      }

      /// @brief binds buffer
      void bind()
      {
         gl(glBindBuffer, GL_ARRAY_BUFFER, mID);
      }

      /// @brief unbinds buffer
      void unBind()
      {
         gl(glBindBuffer, GL_ARRAY_BUFFER, 0);
      }

      /// @brief upload data to buffer
      void upload(const TAttributeDataPack* attr, size_t size)
      {
         bind();
         gl(glBufferData, GL_ARRAY_BUFFER, size*sizeof(TAttributeDataPack), attr, GL_STATIC_DRAW);
         unBind();
      }

   private:
      /// @brief disabled stuff
      TBufferObject(const TBufferObject&) = delete;
      TBufferObject& operator=(const TBufferObject& other) = delete;

   private:
      /// @brief buffer id
      GLuint mID;
};

/// @brief holds state of program's buffer object, use it as TProgram template
/// parameter
template<typename TAttributeDataPack>
class TBufferObjectProgramInput
{
   public:
      /// @brief type of buffer objects this program input accepts
      using tValueType = std::shared_ptr<TBufferObject<TAttributeDataPack>>;

      /// @brief buffer object underlying data type
      using tData = TAttributeDataPack;

      /// @brief constructor
      TBufferObjectProgramInput(const GLuint program)
         : mLocations(tData::getLocations(program))
      {}

      /// @brief set new buffer object as program input, isSelected should be
      /// true if program this program input belongs to is currently selected
      void set(tValueType value, bool isSelected)
      {
         if (mBuffer != value)
         {
            if (isSelected)
            {
               // detach old input, attach new one
               detach();
               mBuffer = value;
               attach();
            }
            else
            {
               // if attached but not selected and detach is not scheduled yet -
               // schedule detach, @note that we don't need buffer itself to
               // perform detach, but it should be alive until then, so save it
               if (mIsAttached && !mBufferForDelayedDetach)
                  mBufferForDelayedDetach = mBuffer;
               mBuffer = value;
            }
         }
      }

      /// @brief called after program was selected, perform delayed detach,
      /// attach buffer
      void select()
      {
         if (mBufferForDelayedDetach)
         {
            tData::detach(mLocations);
            mIsAttached = false;
            mBufferForDelayedDetach = nullptr;
         }
         attach();
      }

   private:
      /// @brief locations of program input inside program
      typename tData::tLocations mLocations;

      /// @brief true if this program input is currently attached to a program
      bool mIsAttached = false;

      /// @brief holds actual buffer
      tValueType mBuffer;

      /// @brief if not nullptr holds outdated buffer, that should be detached
      /// when current program will be selected
      tValueType mBufferForDelayedDetach;

   private: // impl
      /// @brief attach buffer
      void attach()
      {
         if (mBuffer && !mIsAttached)
         {
            mBuffer->bind();
            tData::attach(mLocations);
            mBuffer->unBind();
            mIsAttached = true;
         }
      }

      /// @brief detach buffer
      void detach()
      {
         if (mBuffer && mIsAttached)
         {
            tData::detach(mLocations);
            mIsAttached = false;
         }
      }
};

#endif // ENGINE_BUFFEROBJECT_HPP
