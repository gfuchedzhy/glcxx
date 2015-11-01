/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#include "GL.hpp"
#include <tuple>

/// @brief single attribute, sentinel used to differ types that are logically
/// different but have same properties
template<GLenum glType, size_t N, int sentinel = 0>
struct TAttributeData : std::array<typename TGLSLToCPPType<glType>::type, N>
{
      static_assert(N >= 1 && N <= 4, "attribute number should be in range [1,4]");

      /// @brief base typedef
      using tBaseArray = std::array<typename TGLSLToCPPType<glType>::type, N>;

      /// @brief return glType id
      static constexpr GLenum glTypeID() { return glType; }

      /// @brief return glType id
      static constexpr size_t size() { return N; }

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
      static constexpr size_t attributeNum() { return sizeof...(TAttributes); }

      /// @brief returns offset of given attribute in attribute pack
      template<int index>
      static constexpr size_t attributeOffset()
      {
         return size_t((char*)(&std::get<index>(*(tBaseTuple*)(0))));
      }

      /// @brief default constructor
      TAttributeDataPack() = default;

      /// @brief constructor allowing initialization from {...}
      TAttributeDataPack(const TAttributes&... attributes)
         : std::tuple<TAttributes...>(attributes...)
      {}
};

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

      /// @brief upload data to buffer
      void bindData(const TAttributeDataPack* attr, size_t size)
      {
         gl(glBindBuffer, GL_ARRAY_BUFFER, mID);
         gl(glBufferData, GL_ARRAY_BUFFER, size*sizeof(TAttributeDataPack), attr, GL_STATIC_DRAW);
         gl(glBindBuffer, GL_ARRAY_BUFFER, 0);
      }

      /// @brief binds attribute data to given locations
      void bind(std::array<GLuint, TAttributeDataPack::attributeNum()> locations)
      {
         gl(glBindBuffer, GL_ARRAY_BUFFER, mID);
         doBind(locations, std::make_index_sequence<TAttributeDataPack::attributeNum()>());
         gl(glBindBuffer, GL_ARRAY_BUFFER, 0);
      }

   private:
      /// @brief calls attrib pointer function for every attribute
      template<size_t... I>
      void doBind(std::array<GLuint, TAttributeDataPack::attributeNum()> locations, std::index_sequence<I...>)
      {
         using swallow = int[];
         (void)swallow{(gl(glVertexAttribPointer,
                           locations[I], 
                           std::tuple_element<I, typename TAttributeDataPack::tBaseTuple>::type::size(),
                           std::tuple_element<I, typename TAttributeDataPack::tBaseTuple>::type::glTypeID(),
                           GL_FALSE, // not normalized
                           sizeof(TAttributeDataPack), // stride
                           (void*)TAttributeDataPack::template attributeOffset<I>() // offset
                           ), 0)...};
      }

   private:
      /// @brief buffer id
      GLuint mID;
};
