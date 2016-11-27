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
            template<> struct prefix_impl<bool>         { using type = cts("b");};
            template<> struct prefix_impl<int>          { using type = cts("i");};
            template<> struct prefix_impl<unsigned int> { using type = cts("u");};
            template<> struct prefix_impl<float>        { using type = cts(""); };
            template<> struct prefix_impl<double>       { using type = cts("d");};
            template<typename T> using prefix = typename prefix_impl<T>::type;

            /// @brief basic shader type names
            template<typename T> struct basic_name_impl;
            template<> struct basic_name_impl<bool>         { using type = cts("bool");};
            template<> struct basic_name_impl<int>          { using type = cts("int");};
            template<> struct basic_name_impl<unsigned int> { using type = cts("uint");};
            template<> struct basic_name_impl<float>        { using type = cts("float");};
            template<> struct basic_name_impl<double>       { using type = cts("double");};
            template<typename T> using basic_name = typename basic_name_impl<T>::type;
        }

        /// @brief traits for shader types
        template<typename ShaderType> struct traits {
            static constexpr GLenum id = shader_type::id<ShaderType>::value;
            static constexpr size_t components_num = 1u;
            static constexpr size_t locations_num = 1u;
            using name = detail::basic_name<ShaderType>;
        };

        // todo remove
        template<typename BasicType>
        struct traits<glm::tvec1<BasicType>> {
            static constexpr GLenum id = traits<BasicType>::id;
            static constexpr size_t components_num = 1u;
            static constexpr size_t locations_num = 1u;
            using name = detail::basic_name<BasicType>;
        };

        /// specializations
        template<typename BasicType>
        struct traits<glm::tvec2<BasicType>> {
            static constexpr GLenum id = traits<BasicType>::id;
            static constexpr size_t components_num = 2u;
            static constexpr size_t locations_num = 1u;
            using name = ct::string_cat<detail::prefix<BasicType>, cts("vec2")>;
        };

        template<typename BasicType>
        struct traits<glm::tvec3<BasicType>> {
            static constexpr GLenum id = traits<BasicType>::id;
            static constexpr size_t components_num = 3u;
            static constexpr size_t locations_num = 1u;
            using name = ct::string_cat<detail::prefix<BasicType>, cts("vec3")>;
        };

        template<typename BasicType>
        struct traits<glm::tvec4<BasicType>> {
            static constexpr GLenum id = traits<BasicType>::id;
            static constexpr size_t components_num = 4u;
            static constexpr size_t locations_num = 1u;
            using name = ct::string_cat<detail::prefix<BasicType>, cts("vec4")>;
        };

        template<typename BasicType>
        struct traits<glm::tmat2x2<BasicType>> {
            static constexpr GLenum id = traits<BasicType>::id;
            static constexpr size_t components_num = 2u;
            static constexpr size_t locations_num = 2u;
            using name = ct::string_cat<detail::prefix<BasicType>, cts("mat2")>;
        };

        template<typename BasicType>
        struct traits<glm::tmat3x3<BasicType>> {
            static constexpr GLenum id = traits<BasicType>::id;
            static constexpr size_t components_num = 3u;
            static constexpr size_t locations_num = 3u;
            using name = ct::string_cat<detail::prefix<BasicType>, cts("mat3")>;
        };

        template<typename BasicType>
        struct traits<glm::tmat4x4<BasicType>> {
            static constexpr GLenum id = traits<BasicType>::id;
            static constexpr size_t components_num = 4u;
            static constexpr size_t locations_num = 4u;
            using name = ct::string_cat<detail::prefix<BasicType>, cts("mat4")>;
        };

        template<typename ShaderType, size_t N>
        struct traits<std::array<ShaderType, N>> {
            static constexpr GLenum id = traits<ShaderType>::id;
            static constexpr size_t components_num = traits<ShaderType>::components_num;
            static constexpr size_t locations_num = N*traits<ShaderType>::locations_num;
            using name = ct::string_cat<typename traits<ShaderType>::name, cts("["), ct::string_from<size_t, N>, cts("]")>;
        };

        /// @brief shortcut
        template<typename ShaderType>
        using name = typename traits<ShaderType>::name;
    }

    /// cast host type to shader type if they differ, if not - do nothing
    /// @brief cast simple types, e.g. int->float or ivec2->vec2
    template<typename ShaderType, typename HostType>
    inline typename std::conditional<std::is_same<ShaderType, HostType>::value, const ShaderType&, ShaderType>::type
    glsl_cast(const HostType& data)
    {
        return data;
    }

    /// @brief cast arrays, e.g. ivec2[3]->vec2[3]
    template<typename ShaderType, typename HostUnderlyingType, size_t N>
    inline typename std::conditional<std::is_same<typename ShaderType::value_type, HostUnderlyingType>::value, const ShaderType&, ShaderType>::type
    glsl_cast(const std::array<HostUnderlyingType, N>& data)
    {
        ShaderType shader_data;
        std::copy(data.begin(), data.end(), shader_data.begin());
        return shader_data;
    }
}

#endif