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

   /// @brief get uniform location
   inline auto getUniformLocation(GLuint program, const char* name)
   {
      const auto location = gl(glGetUniformLocation, program, name);
      Log::msg("uniform location ", name, "=", location);
      return location;
   }

   /// @brief get attribute location
   inline auto getAttribLocation(GLuint program, const char* name)
   {
      const auto location = gl(glGetAttribLocation, program, name);
      Log::msg("attribute location ", name, "=", location);
      return location;
   }

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

   /// @brief functions to attach uniforms, replace huge amount of different c
   /// functions with properly overloaded fucntions, as their ammount is huge,
   /// use macro to reduce repitition
#define declareAttachUniformFunctions(type, functionSuffix)             \
   inline void attachUniform(GLint location, type val)                  \
   {                                                                    \
      gl(glUniform1##functionSuffix, location, val);                    \
   }                                                                    \
   template<glm::precision P>                                           \
   inline void attachUniform(GLint location, const glm::tvec2<type, P>& val) \
   {                                                                    \
      gl(glUniform2##functionSuffix, location, val.x, val.y);           \
   }                                                                    \
   template<glm::precision P>                                           \
   inline void attachUniform(GLint location, const glm::tvec3<type, P>& val) \
   {                                                                    \
      gl(glUniform3##functionSuffix, location, val.x, val.y, val.z);    \
   }                                                                    \
   template<glm::precision P>                                           \
   inline void attachUniform(GLint location, const glm::tvec4<type, P>& val) \
   {                                                                    \
      gl(glUniform4##functionSuffix, location, val.x, val.y, val.z, val.w); \
   }                                                                    \
   template<size_t N>                                                   \
   inline void attachUniform(GLint location, const std::array<type, N>& val) \
   {                                                                    \
      gl(glUniform1##functionSuffix##v, location, N, &val[0]);          \
   }                                                                    \
   template<size_t N, glm::precision P>                                 \
   inline void attachUniform(GLint location, const std::array<glm::tvec2<type, P>, N>& val) \
   {                                                                    \
      gl(glUniform2##functionSuffix##v, location, N, &val[0][0]);       \
   }                                                                    \
   template<size_t N, glm::precision P>                                 \
   inline void attachUniform(GLint location, const std::array<glm::tvec3<type, P>, N>& val) \
   {                                                                    \
      gl(glUniform3##functionSuffix##v, location, &val[0][0]);          \
   }                                                                    \
   template<size_t N, glm::precision P>                                 \
   inline void attachUniform(GLint location, const std::array<glm::tvec4<type, P>, N>& val) \
   {                                                                    \
      gl(glUniform4##functionSuffix##v, location, &val[0][0]);          \
   }

   /// @brief actual function definitions
   declareAttachUniformFunctions(float, f);
   declareAttachUniformFunctions(int, i);
   declareAttachUniformFunctions(unsigned int, ui);

   /// @brief overloads for matrices
   template<glm::precision P>
   inline void attachUniform(GLint location, const glm::tmat4x4<float, P>& val)
   {
      gl(glUniformMatrix4fv, location, 1, GL_FALSE, &val[0][0]);
   }
   template<size_t N, glm::precision P>
   inline void attachUniform(GLint location, const std::array<glm::tmat4x4<float, P>, N>& val)
   {
      gl(glUniformMatrix4fv, location, N, GL_FALSE, &val[0][0][0]);
   }

   /// @brief converter which does nothing if cpu type and glsl type exactly
   /// match and converts data if they differ
   template<typename TGLSLData>
   struct TConverter
   {
         /// @brief converter for simple type, e.g. int->float or ivec2->vec2
         template<typename TData>
         static inline TGLSLData convert(const TData& data)
         {
            return data;
         }

         /// @brief converter for arrays types, e.g. int[2]->float[2]
         template<typename TData, size_t N>
         static inline TGLSLData convert(const std::array<TData, N>& data)
         {
            TGLSLData glslData;
            for (size_t i = 0; i < N; ++i)
               glslData[i] = data[i];
            return glslData;
         }

         /// @brief does nothing return same reference, this method should be
         /// optimized out
         static inline const TGLSLData& convert(const TGLSLData& data)
         {
            return data;
         }
   };

   /// @brief traits class for input variable to shader attribute/uniform
   /// @tparam THolder glm template holding values, like glm::tvec3 or glm::tmat4
   /// @tparam TypeFrom type on cpu side
   /// @tparam TypeTo cpp type equivalent to glsl type which should be used on gpu side
   /// @tparam isUniform if true this is a uniform, otherwise it is an attribute
   /// @tparam EXTRA number of extra components on gpu side, only valid for
   /// glm::tvecx types, for example tvec3 declared with EXTRA=1 will result in
   /// vec4 type on gpu side
   /// @tparam N if N is 1 this is traits class for single value, if N > 1 this
   /// is array, e.g. vec3 varname[2]
   template<template<typename, glm::precision> class THolder, typename TypeFrom, typename TypeTo, bool isUniform, size_t EXTRA, size_t N>
   struct TInputVarTraits : std::array<TypeFrom, N>
   {
         static_assert(N >= 1, "glsl type size should be at least 1");

         /// @brief true if this type is attribute
         static constexpr bool isAttribute = !isUniform;

         /// @brief basic type,  e.g. glm::vec2, or float for glm::tvec1, does not 
         template<typename T>
         using tBasicType = typename std::conditional<std::is_same<THolder<T, glm::defaultp>,
                                                                   glm::tvec1<T, glm::defaultp>>::value,
                                                      T,
                                                      THolder<T, glm::defaultp>>::type;

         /// @brief data type, basic type or array of basic types
         using tData = typename std::conditional<1==N, tBasicType<TypeFrom>, std::array<tBasicType<TypeFrom>, N>>::type;

         /// @brief data type, basic type or array of basic types
         using tGLSLData = typename std::conditional<1==N, tBasicType<TypeTo>, std::array<tBasicType<TypeTo>, N>>::type;

         /// @brief variable size
         static constexpr size_t size = N;

         /// @brief ctstring containing glsl declaration of variable
         template<typename TName>
         using tDeclaration = ct::string_cat<typename std::conditional<isUniform, cts("uniform "), cts("attribute ")>::type,
                                             tTypeName<TypeTo, EXTRA, THolder>, cts(" "),
                                             TName,
                                             typename std::conditional<1==size, cts(""),
                                                                       ct::string_cat<cts("["), ct::string_from<size_t, size>, cts("]")>>::type,
                                             cts(";\n")>;

         /// @brief attach for attributes
         template<typename TDummy = int> // to enable sfinae
         static void attach(GLint location, const tData* ptr = nullptr, typename std::enable_if<isAttribute, TDummy>::type dummy = 0)
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
                     sizeof(tData),
                     (void*)((const char*)ptr + locationOffset*componentsNum*sizeof(TypeFrom)));
               }
            }
         }

         /// @brief detach for attributes
         template<typename TDummy = int> // to enable sfinae
         static void detach(GLint location, typename std::enable_if<isAttribute, TDummy>::type dummy = 0)
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
template<template<typename, glm::precision> class THolder, typename TypeFrom, typename TypeTo = float, size_t EXTRA = 0>
using TAttrib = glsl::TInputVarTraits<THolder, TypeFrom, TypeTo, false, EXTRA, 1>;

template<size_t N, template<typename, glm::precision> class THolder, typename TypeFrom, typename TypeTo = float, size_t EXTRA = 0>
using TAttribArr = glsl::TInputVarTraits<THolder, TypeFrom, TypeTo, false, EXTRA, N>;

template<template<typename, glm::precision> class THolder, typename TypeFrom, typename TypeTo = float, size_t EXTRA = 0>
using TUniform = glsl::TInputVarTraits<THolder, TypeFrom, TypeTo, true, EXTRA, 1>;

template<size_t N, template<typename, glm::precision> class THolder, typename TypeFrom, typename TypeTo = float, size_t EXTRA = 0>
using TUniformArr = glsl::TInputVarTraits<THolder, TypeFrom, TypeTo, true, EXTRA, N>;

#endif // ENGINE_GLSLINPUTVARIABLE_HPP
