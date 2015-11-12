/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_GLSLINPUTVARIABLE_HPP
#define ENGINE_GLSLINPUTVARIABLE_HPP

#include <glm/glm.hpp>
#include "GL.hpp"
#include "Utils.hpp"

namespace glsl
{
   /// @brief compile time conversion from c++ types to glsl types
   template<typename T> struct TTypeID {};
   template<> struct TTypeID<float>          { static constexpr GLenum id = GL_FLOAT; };
   template<> struct TTypeID<double>         { static constexpr GLenum id = GL_DOUBLE; };
   template<> struct TTypeID<signed char>    { static constexpr GLenum id = GL_BYTE; };
   template<> struct TTypeID<unsigned char>  { static constexpr GLenum id = GL_UNSIGNED_BYTE; };
   template<> struct TTypeID<signed short>   { static constexpr GLenum id = GL_SHORT; };
   template<> struct TTypeID<unsigned short> { static constexpr GLenum id = GL_UNSIGNED_SHORT; };
   template<> struct TTypeID<signed int>     { static constexpr GLenum id = GL_INT; };
   template<> struct TTypeID<unsigned int>   { static constexpr GLenum id = GL_UNSIGNED_INT; };

   /// @brief traits for uniforms and attributes
   template<bool isUniform>
   struct TInputType
   {
         using tSpecifier = cts("uniform");
         template<typename TName>
         static auto getLocation(GLuint program)
         {
            const auto location = gl(glGetUniformLocation, program, TName::chars);
            Log::msg("uniform location ", TName::chars, "=", location);
            return location;
         }
   };
   /// @brief specialization for attributes
   template<>
   struct TInputType<false>
   {
         using tSpecifier = cts("attribute");
         template<typename TName>
         static auto getLocation(GLuint program)
         {
            const auto location = gl(glGetAttribLocation, program, TName::chars);
            Log::msg("attribute location ", TName::chars, "=", location);
            return location;
         }
   };

   /// @brief compile time glsl prefixes for glsl
   template<typename Type> struct TTypePrefix;
   template<> struct TTypePrefix<bool>         { using tPrefix = cts("b");};
   template<> struct TTypePrefix<int>          { using tPrefix = cts("i");};
   template<> struct TTypePrefix<unsigned int> { using tPrefix = cts("u");};
   template<> struct TTypePrefix<float>        { using tPrefix = cts(""); };
   template<> struct TTypePrefix<double>       { using tPrefix = cts("d");};
   template<typename Type> using  tTypePrefix = typename TTypePrefix<Type>::tPrefix;

   /// @brief basic glsl type names
   template<typename Type> struct TBasicTypeName;
   template<> struct TBasicTypeName<bool>         { using tName = cts("bool");};
   template<> struct TBasicTypeName<int>          { using tName = cts("int");};
   template<> struct TBasicTypeName<unsigned int> { using tName = cts("uint");};
   template<> struct TBasicTypeName<float>        { using tName = cts("float");};
   template<> struct TBasicTypeName<double>       { using tName = cts("double");};
   template<typename Type> using  tBasicTypeName = typename TBasicTypeName<Type>::tName;

   /// @brief traits for glsl types
   template<typename TypeTo, size_t EXTRA, template<typename, glm::precision> class THolder>
   struct TTypeTraits;

   template<typename TypeTo, size_t EXTRA>
   struct TTypeTraits<TypeTo, EXTRA, glm::tvec1>
   {
         static constexpr size_t componentsNum = 1u;
         static constexpr size_t locationsNum = 1u;
         static_assert(EXTRA <= 3, "too many extra components given");
         using tTypeName = typename std::conditional<0==EXTRA,
                                                     tBasicTypeName<TypeTo>,
                                                     ct::string_cat<tTypePrefix<TypeTo>, cts("vec"), ct::string<'1'+EXTRA>>>::type;
   };

   template<typename TypeTo, size_t EXTRA>
   struct TTypeTraits<TypeTo, EXTRA, glm::tvec2>
   {
         static constexpr size_t componentsNum = 2u;
         static constexpr size_t locationsNum = 1u;
         static_assert(EXTRA <= 2, "too many extra components given");
         using tTypeName = ct::string_cat<tTypePrefix<TypeTo>, cts("vec"), ct::string<'2'+EXTRA>>;
   };

   template<typename TypeTo, size_t EXTRA>
   struct TTypeTraits<TypeTo, EXTRA, glm::tvec3>
   {
         static constexpr size_t componentsNum = 3u;
         static constexpr size_t locationsNum = 1u;
         static_assert(EXTRA <= 1, "too many extra components given");
         using tTypeName = ct::string_cat<tTypePrefix<TypeTo>, cts("vec"), ct::string<'3'+EXTRA>>;
   };

   template<typename TypeTo, size_t EXTRA>
   struct TTypeTraits<TypeTo, EXTRA, glm::tvec4>
   {
         static constexpr size_t componentsNum = 4u;
         static constexpr size_t locationsNum = 1u;
         static_assert(EXTRA == 0, "vec4 can't have extra components");
         using tTypeName = ct::string_cat<tTypePrefix<TypeTo>, cts("vec4")>;
   };

   template<typename TypeTo, size_t EXTRA>
   struct TTypeTraits<TypeTo, EXTRA, glm::tmat4x4>
   {
         static constexpr size_t componentsNum = 4u;
         static constexpr size_t locationsNum = 4u;
         static_assert(0==EXTRA, "extra components doesn't make sense for matrices");
         using tTypeName = ct::string_cat<tTypePrefix<TypeTo>, cts("mat4")>;
   };

   /// @brief shortcut
   template<typename TypeTo, size_t EXTRA, template<typename, glm::precision> class THolder>
   using tTypeName = typename TTypeTraits<TypeTo, EXTRA, THolder>::tTypeName;

   /// @brief functions to attach uniforms
   void attachUniform(GLint location, float val)
   {
      gl(glUniform1f, location, val);
   }
   template<size_t N>
   void attachUniform(GLint location, const std::array<float, N>& val)
   {
      for (int i = 0; i < N; ++i)
      {
         gl(glUniform1f, location, val[i]);
      }
   }
   template<glm::precision P>
   void attachUniform(GLint location, const glm::tvec2<float, P>& val)
   {
      gl(glUniform2f, location, val.x, val.y);
   }
   template<glm::precision P>
   void attachUniform(GLint location, const glm::tvec3<float, P>& val)
   {
      gl(glUniform3f, location, val.x, val.y, val.z);
   }
   template<glm::precision P>
   void attachUniform(GLint location, const glm::tvec4<float, P>& val)
   {
      gl(glUniform4f, location, val.x, val.y, val.z, val.w);
   }
   template<glm::precision P>
   void attachUniform(GLint location, const glm::tmat4x4<float, P>& val)
   {
      gl(glUniformMatrix4fv, location, 1, GL_FALSE, &val[0][0]);
   }
   template<size_t N, glm::precision P>
   void attachUniform(GLint location, const std::array<glm::tmat4x4<float, P>, N>& val)
   {
      gl(glUniformMatrix4fv, location, N, GL_FALSE, &val[0][0][0]);
   }
   /// @brief array version
   template<size_t N, typename T>
   void attachUniform(GLint location, const std::array<T, N>& val)
   {
      for (size_t i = 0; i < N; ++i)
      {
         attachUniform(location+i, val[i]);
      }
   }

   /// @brief traits class for input variable to shader attribute/uniform
   /// @tparam TName name of var for compile time querying, declarations, etc
   /// @tparam THolder glm template holding values, like glm::tvec3 or glm::tmat4
   /// @tparam TypeFrom type on cpu side
   /// @tparam TypeTo cpp type equivalent to glsl type which should be used on gpu side
   /// @tparam isUniform if true this is a uniform, otherwise it is an attribute
   /// @tparam EXTRA number of extra components on gpu side, only valid for
   /// glm::tvecx types, for example tvec3 declared with EXTRA=1 will result in
   /// vec4 type on gpu side
   /// @tparam N if N is 1 this is traits class for single value, if N > 1 this
   /// is array, e.g. vec3 varname[2]
   template<typename TName, template<typename, glm::precision> class THolder, typename TypeFrom, typename TypeTo, bool isUniform, size_t EXTRA, size_t N>
   struct TInputVarTraits : std::array<TypeFrom, N>
   {
         static_assert(N >= 1, "glsl type size should be at least 1");

         /// @brief basic type,  e.g. glm::vec2, or float for glm::tvec1, does not 
         using tBasicType = typename std::conditional<std::is_same<THolder<TypeFrom, glm::defaultp>,
                                                                   glm::tvec1<TypeFrom, glm::defaultp>>::value,
                                                      TypeFrom,
                                                      THolder<TypeFrom, glm::defaultp>>::type;

         /// @brief data type, basic type or array of basic types
         using tData = typename std::conditional<1==N, tBasicType, std::array<tBasicType, N>>::type;

         /// @brief ctstring containing name of variable
         using tName = TName;

         /// @brief variable size
         static constexpr size_t size = N;

         /// @brief ctstring containing glsl declaration of variable
         using tDeclaration = ct::string_cat<typename TInputType<isUniform>::tSpecifier, cts(" "),
                                             tTypeName<TypeTo, EXTRA, THolder>, cts(" "),
                                             tName,
                                             typename std::conditional<1==size, cts(""),
                                                                       ct::string_cat<cts("["), ct::string_from<size_t, size>, cts("]")>>::type,
                                             cts(";")>;

         /// @brief location
         using tLocation = GLint;

         /// @brief returns location of variable for given program
         static tLocation getLocation(GLuint program)
         {
            return TInputType<isUniform>::template getLocation<tName>(program);
         }

         /// @brief attach for uniform
         template<typename TDummy = int> // to enable sfinae
         static void attach(tLocation location, const tData& data, typename std::enable_if<isUniform, TDummy>::type dummy = 0)
         {
            attachUniform(location, data);
         }

         /// @brief attach for attributes
         template<typename TDummy = int> // to enable sfinae
         static void attach(tLocation location, size_t stride = sizeof(tData), size_t offset = 0u, const tData* ptr = nullptr, typename std::enable_if<!isUniform, TDummy>::type dummy = 0)
         {
            constexpr size_t locationsNum  = TTypeTraits<TypeTo, EXTRA, THolder>::locationsNum;
            constexpr size_t componentsNum = TTypeTraits<TypeTo, EXTRA, THolder>::componentsNum;
            for (size_t n = 0; n < N; ++n)
            {
               for (size_t i = 0; i < locationsNum; ++i)
               {
                  const size_t locationOffset = i + n*locationsNum;
                  gl(glEnableVertexAttribArray, location + locationOffset);
                  gl(glVertexAttribPointer,
                     location + locationOffset,
                     componentsNum,
                     TTypeID<TypeFrom>::id,
                     GL_FALSE, // not normalized
                     stride,
                     (void*)((const char*)ptr + offset + locationOffset*componentsNum*sizeof(TypeFrom)));
               }
            }
         }

         /// @brief detach for attributes
         template<typename TDummy = int> // to enable sfinae
         static void detach(tLocation location, typename std::enable_if<!isUniform, TDummy>::type dummy = 0)
         {
            constexpr size_t locationsNum  = TTypeTraits<TypeTo, EXTRA, THolder>::locationsNum;
            for (size_t n = 0; n < N; ++n)
            {
               for (size_t i = 0; i < locationsNum; ++i)
               {
                  const size_t locationOffset = i + n*locationsNum;
                  gl(glDisableVertexAttribArray, location + locationOffset);
               }
            }
         }
   };
}

/// @brief shortcuts
template<typename TName, template<typename, glm::precision> class THolder, typename TypeFrom, typename TypeTo = float, size_t EXTRA = 0>
using TAttrib = glsl::TInputVarTraits<TName, THolder, TypeFrom, TypeTo, false, EXTRA, 1>;

template<size_t N, typename TName, template<typename, glm::precision> class THolder, typename TypeFrom, typename TypeTo = float, size_t EXTRA = 0>
using TAttribArr = glsl::TInputVarTraits<TName, THolder, TypeFrom, TypeTo, false, EXTRA, N>;

template<typename TName, template<typename, glm::precision> class THolder, typename TypeFrom, typename TypeTo = float, size_t EXTRA = 0>
using TUniform = glsl::TInputVarTraits<TName, THolder, TypeFrom, TypeTo, true, EXTRA, 1>;

template<size_t N, typename TName, template<typename, glm::precision> class THolder, typename TypeFrom, typename TypeTo = float, size_t EXTRA = 0>
using TUniformArr = glsl::TInputVarTraits<TName, THolder, TypeFrom, TypeTo, true, EXTRA, N>;

#endif // ENGINE_GLSLINPUTVARIABLE_HPP
