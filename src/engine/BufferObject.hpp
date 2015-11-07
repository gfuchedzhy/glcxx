/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "GL.hpp"
#include "Utils.hpp"

#ifndef ENGINE_BUFFEROBJECT_HPP
#define ENGINE_BUFFEROBJECT_HPP

/// @brief single attribute, sentinel used to differ types that are logically
/// different but have same properties
template<GLenum glType, size_t N, typename TName>
struct TAttributeData : std::array<typename TGLSLToCPPType<glType>::type, N>
{
      static_assert(N >= 1 && N <= 4, "attribute number should be in range [1,4]");

      /// @brief base typedef
      using tBaseArray = std::array<typename TGLSLToCPPType<glType>::type, N>;

      /// @brief ctstring containing name of current attribute
      using tName = TName;

      /// @brief return glType id
      static constexpr GLenum glTypeID = glType;

      /// @brief return attribute size
      static constexpr size_t size = N;

      /// @brief default constructor
      TAttributeData() = default;

      /// @brief constructor allowing initialization from {...}
      template<typename... T>
      TAttributeData(const T&... args)
         : tBaseArray({args...})
      {
         static_assert(sizeof...(T) == N, "wrong number of initializers passed to attribute");
      }
};

template<typename... TAttributes>
struct TAttributeDataPack : std::tuple<TAttributes...>
{
      /// @brief base typedef
      using tBaseTuple = std::tuple<TAttributes...>;

      /// @brief return attribute number
      static constexpr size_t attributeNum = sizeof...(TAttributes);

      /// @brief returns offset of given attribute in attribute pack
      template<int index>
      struct offsetof
      {
            static constexpr void* value = (void*)((char*)(&std::get<index>(*(tBaseTuple*)(0))));
      };

      /// @brief get index of named attribute in compile time
      template<typename TName, int index = sizeof...(TAttributes) - 1>
      static constexpr int indexByName(TName, std::integral_constant<int, index> = std::integral_constant<int, index>{})
      {
         return std::is_same<TName, typename std::tuple_element<index, tBaseTuple>::type::tName>::value ? index :
            indexByName(TName{},std::integral_constant<int, index-1>{});
      }

      /// @brief terminator for attribute index calculator
      template<typename TName>
      static constexpr int indexByName(TName, std::integral_constant<int, -1>)
      {
         return -1;
      }

      /// @brief get attribute by compile time name
      template<typename TName>
      auto& get()
      {
         constexpr int index = indexByName(TName{});
         static_assert(-1 != index, "attribute name not found");
         return std::get<index>(*this);
      }

      /// @brief get attribute by compile time name
      template<typename TName>
      const auto& get() const
      {
         constexpr int index = indexByName(TName{});
         static_assert(-1 != index, "attribute name not found");
         return std::get<index>(*this);
      }

      /// @brief default constructor
      TAttributeDataPack() = default;

      /// @brief constructor allowing initialization from {...}
      TAttributeDataPack(const TAttributes&... attributes)
         : std::tuple<TAttributes...>(attributes...)
      {}
};

/// @brief forward declaration
template<typename TAttributeDataPack>
class TBufferObject;

/// @brief buffer ptr
template<typename TAttributeDataPack>
using TBufferObjectPtr = std::shared_ptr<TBufferObject<TAttributeDataPack>>;

/// @brief buffer object
template<typename TAttributeDataPack>
class TBufferObject
{
   public:
      /// @brief factory method returning buffer objects
      static TBufferObjectPtr<TAttributeDataPack> create()
      {
         return std::make_shared<TBufferObject>();
      }

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

      /// @brief upload data to buffer
      void upload(const TAttributeDataPack* attr, size_t size)
      {
         gl(glBindBuffer, GL_ARRAY_BUFFER, mID);
         gl(glBufferData, GL_ARRAY_BUFFER, size*sizeof(TAttributeDataPack), attr, GL_STATIC_DRAW);
         gl(glBindBuffer, GL_ARRAY_BUFFER, 0);
      }

      /// @brief attaches attribute data to given locations
      void attach(std::array<GLuint, TAttributeDataPack::attributeNum> locations)
      {
         gl(glBindBuffer, GL_ARRAY_BUFFER, mID);
         doAttach(locations, std::make_index_sequence<TAttributeDataPack::attributeNum>{});
         gl(glBindBuffer, GL_ARRAY_BUFFER, 0);
      }

      /// @brief detaches attribute data
      void detach(std::array<GLuint, TAttributeDataPack::attributeNum> locations)
      {
         doDetach(locations, std::make_index_sequence<TAttributeDataPack::attributeNum>{});
      }

   private:
      /// @brief enables every attribute, then calls attrib pointer for each one
      template<size_t... I>
      void doAttach(std::array<GLuint, TAttributeDataPack::attributeNum> locations, std::index_sequence<I...>)
      {
         swallow(gl(glEnableVertexAttribArray, I));
         swallow(gl(glVertexAttribPointer,
                    locations[I],
                    std::tuple_element<I, typename TAttributeDataPack::tBaseTuple>::type::size,
                    std::tuple_element<I, typename TAttributeDataPack::tBaseTuple>::type::glTypeID,
                    GL_FALSE, // not normalized
                    sizeof(TAttributeDataPack), // stride
                    TAttributeDataPack::template offsetof<I>::value // offset
                    ));
      }

      /// @brief disables every attribute
      template<size_t... I>
      void doDetach(std::array<GLuint, TAttributeDataPack::attributeNum> locations, std::index_sequence<I...>)
      {
         swallow(gl(glDisableVertexAttribArray, I));
      }

      /// @brief disabled stuff
      TBufferObject(const TBufferObject&) = delete;
      TBufferObject& operator=(const TBufferObject& other) = delete;

   private:
      /// @brief buffer id
      GLuint mID;
};

#endif // ENGINE_BUFFEROBJECT_HPP
