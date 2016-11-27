// Copyright 2015, 2016 Grygorii Fuchedzhy <gfuchedzhy@gmail.com>
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef GLCXX_SHADER_TYPE_HPP
#define GLCXX_SHADER_TYPE_HPP

#include "glcxx/gl.hpp"
#include "glcxx/utils.hpp"
#include <glm/glm.hpp>

namespace glcxx {

    /// @brief tags to place declarations to proper shaders
    namespace tag {
        struct vertex;
        struct geometry;
        struct fragment;
        struct vertgeom;
        struct geomfrag;
        struct vertfrag;
        struct all;
    }

    namespace shader_type {

        /// @brief return true if shader of type ShaderTag has declaration with
        /// DeclTag declaration tag
        template<typename ShaderTag, typename DeclTag> struct has_decl;

        template<typename DeclTag> struct has_decl<tag::vertex, DeclTag> {
            static constexpr bool value = ct::tuple_contains<std::tuple<tag::vertex, tag::vertgeom, tag::vertfrag, tag::all>,
                                                             DeclTag>::value;
        };
        template<typename DeclTag> struct has_decl<tag::geometry, DeclTag> {
            static constexpr bool value = ct::tuple_contains<std::tuple<tag::geometry, tag::vertgeom, tag::geomfrag, tag::all>,
                                                             DeclTag>::value;
        };
        template<typename DeclTag> struct has_decl<tag::fragment, DeclTag> {
            static constexpr bool value = ct::tuple_contains<std::tuple<tag::fragment, tag::vertfrag, tag::geomfrag, tag::all>,
                                                             DeclTag>::value;
        };

        /// @brief compile time shader type id
        template<typename T> struct id {};
        template<> struct id<float>          { static constexpr GLenum value = GL_FLOAT; };
        template<> struct id<double>         { static constexpr GLenum value = GL_DOUBLE; };
        template<> struct id<signed char>    { static constexpr GLenum value = GL_BYTE; };
        template<> struct id<unsigned char>  { static constexpr GLenum value = GL_UNSIGNED_BYTE; };
        template<> struct id<signed short>   { static constexpr GLenum value = GL_SHORT; };
        template<> struct id<unsigned short> { static constexpr GLenum value = GL_UNSIGNED_SHORT; };
        template<> struct id<signed int>     { static constexpr GLenum value = GL_INT; };
        template<> struct id<unsigned int>   { static constexpr GLenum value = GL_UNSIGNED_INT; };

        namespace detail {
            /// @brief compile time prefixes for shader types
            template<typename T> struct prefix_impl;
            template<> struct prefix_impl<bool>           { using type = cts("b");};
            template<> struct prefix_impl<int>            { using type = cts("i");};
            template<> struct prefix_impl<unsigned int>   { using type = cts("u");};
            template<> struct prefix_impl<short>          { using type = cts("i");};
            template<> struct prefix_impl<unsigned short> { using type = cts("u");};
            template<> struct prefix_impl<char>           { using type = cts("i");};
            template<> struct prefix_impl<unsigned char>  { using type = cts("u");};
            template<> struct prefix_impl<float>          { using type = cts(""); };
            template<> struct prefix_impl<double>         { using type = cts("d");};
            template<typename T> using prefix = typename prefix_impl<T>::type;

            /// @brief basic shader type names
            template<typename T> struct basic_name_impl;
            template<> struct basic_name_impl<bool>           { using type = cts("bool");};
            template<> struct basic_name_impl<int>            { using type = cts("int");};
            template<> struct basic_name_impl<unsigned int>   { using type = cts("uint");};
            template<> struct basic_name_impl<short>          { using type = cts("int");};
            template<> struct basic_name_impl<unsigned short> { using type = cts("uint");};
            template<> struct basic_name_impl<char>           { using type = cts("int");};
            template<> struct basic_name_impl<unsigned char>  { using type = cts("uint");};
            template<> struct basic_name_impl<float>          { using type = cts("float");};
            template<> struct basic_name_impl<double>         { using type = cts("double");};
            template<typename T> using basic_name = typename basic_name_impl<T>::type;
        }

        /// @brief traits for shader types
        template<typename ShaderType> struct traits {
            static constexpr GLenum id = shader_type::id<ShaderType>::value;
            static constexpr GLint components_num = 1;
            static constexpr GLint locations_num = 1;
            using basic_type = ShaderType;
            using name = detail::basic_name<ShaderType>;
        };

        /// specializations
        template<typename BasicType>
        struct traits<glm::tvec1<BasicType>> {
            static constexpr GLenum id = traits<BasicType>::id;
            static constexpr GLint components_num = 1;
            static constexpr GLint locations_num = 1;
            using basic_type = BasicType;
            using name = detail::basic_name<BasicType>;
        };

        template<typename BasicType>
        struct traits<glm::tvec2<BasicType>> {
            static constexpr GLenum id = traits<BasicType>::id;
            static constexpr GLint components_num = 2;
            static constexpr GLint locations_num = 1;
            using basic_type = BasicType;
            using name = ct::string_cat<detail::prefix<BasicType>, cts("vec2")>;
        };

        template<typename BasicType>
        struct traits<glm::tvec3<BasicType>> {
            static constexpr GLenum id = traits<BasicType>::id;
            static constexpr GLint components_num = 3;
            static constexpr GLint locations_num = 1;
            using basic_type = BasicType;
            using name = ct::string_cat<detail::prefix<BasicType>, cts("vec3")>;
        };

        template<typename BasicType>
        struct traits<glm::tvec4<BasicType>> {
            static constexpr GLenum id = traits<BasicType>::id;
            static constexpr GLint components_num = 4;
            static constexpr GLint locations_num = 1;
            using basic_type = BasicType;
            using name = ct::string_cat<detail::prefix<BasicType>, cts("vec4")>;
        };

        template<typename BasicType>
        struct traits<glm::tmat2x2<BasicType>> {
            static constexpr GLenum id = traits<BasicType>::id;
            static constexpr GLint components_num = 2;
            static constexpr GLint locations_num = 2;
            using basic_type = BasicType;
            using name = ct::string_cat<detail::prefix<BasicType>, cts("mat2")>;
        };

        template<typename BasicType>
        struct traits<glm::tmat3x3<BasicType>> {
            static constexpr GLenum id = traits<BasicType>::id;
            static constexpr GLint components_num = 3;
            static constexpr GLint locations_num = 3;
            using basic_type = BasicType;
            using name = ct::string_cat<detail::prefix<BasicType>, cts("mat3")>;
        };

        template<typename BasicType>
        struct traits<glm::tmat4x4<BasicType>> {
            static constexpr GLenum id = traits<BasicType>::id;
            static constexpr GLint components_num = 4;
            static constexpr GLint locations_num = 4;
            using basic_type = BasicType;
            using name = ct::string_cat<detail::prefix<BasicType>, cts("mat4")>;
        };

        template<typename ShaderType, size_t N>
        struct traits<std::array<ShaderType, N>> {
            static constexpr GLenum id = traits<ShaderType>::id;
            static constexpr GLint components_num = traits<ShaderType>::components_num;
            static constexpr GLint locations_num = N*traits<ShaderType>::locations_num;
            using basic_type = typename traits<ShaderType>::basic_type;
            using name = ct::string_cat<typename traits<ShaderType>::name, cts("["), ct::string_from<size_t, N>, cts("]")>;
        };

        /// @brief shortcut
        template<typename ShaderType>
        using name = typename traits<ShaderType>::name;
    }

    namespace detail {
        /// @brief fwd declaration of glsl_cast helper struct
        template<typename ShaderType> struct glsl_cast;

        /// @brief returns true if From is glsl convertible to To
        template<typename From, typename To>
        class is_glsl_convertible
        {
            static void test(...);
            template<typename U>
            static auto test(const U&) -> decltype(glsl_cast<To>::cast(std::declval<U>()));
        public:
            static constexpr bool value = !std::is_same<void, decltype(test(std::declval<From>()))>::value;
        };

        /// implementation of glsl_cast helper struct
        template<typename ShaderType>
        struct glsl_cast
        {
            // identical types, do nothing
            static inline const auto& cast(const ShaderType& data)
            {
                return data;
            }

            // statically cast if possible
            template<typename HostType>
            static inline auto cast(const HostType& data)
                -> decltype(static_cast<ShaderType>(data))
            {
                return static_cast<ShaderType>(data);
            }
        };

        template<typename ShaderType, size_t N>
        struct glsl_cast<std::array<ShaderType, N>> {
            using to_type = std::array<ShaderType, N>;

            // identical types, do nothing
            static inline const auto& cast(const to_type& data)
            {
                return data;
            }

            // cast N elements of host array into shader array if possible
            template<typename HostType, size_t M>
            static inline auto cast(const std::array<HostType, M>& data)
                -> std::enable_if_t<(N<=M && is_glsl_convertible<HostType, ShaderType>::value), to_type>
            {
                to_type ret;
                for (size_t i = 0; i < N; ++i)
                    ret[i] = glsl_cast<ShaderType>::cast(data[i]);
                return ret;
            }
        };
    }

    /// @brief cast host type to shader type if they differ, if not - do nothing
    template<typename ShaderType, typename HostType>
    inline auto glsl_cast(const HostType& data)
        -> decltype(detail::glsl_cast<ShaderType>::cast(data))
    {
        return detail::glsl_cast<ShaderType>::cast(data);
    }

    /// @brief return true if From is glsl convertible to To
    template<typename From, typename To>
    struct is_glsl_convertible : detail::is_glsl_convertible<From, To> {};
}

#endif
