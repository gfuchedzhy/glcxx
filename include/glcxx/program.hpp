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

#ifndef GLCXX_PROGRAM_HPP
#define GLCXX_PROGRAM_HPP

#include "glcxx/shader.hpp"
#include "glcxx/vao_input.hpp"

namespace glcxx
{
    /// @brief resource holder for opengl program object
    class program_res_holder
    {
        /// @brief disabled stuff
        program_res_holder(const program_res_holder&) = delete;
        program_res_holder& operator=(const program_res_holder&) = delete;

    public:
        /// @brief constructor
        program_res_holder();

        /// @brief destructor
        ~program_res_holder();

        /// @brief selects program
        void select() const
        {
            glUseProgram(_object);
        }

    protected:
        /// @brief program object id
        GLuint _object;
    };

    class program_base : public program_res_holder
    {
        /// @brief shaders, geometry shader is optional
        shader _vertex_shader;
        shader _fragment_shader;
        std::unique_ptr<shader> _geometry_shader;

    public:
        /// @brief constructor
        program_base(const std::string& glsl_version, const std::string& src, bool has_geom_shader);

        /// @brief prepends program name based #defines and declarations to src
        static std::string prepend_header_to_program(std::string name, const char* declarations, const std::string& src);
    };

    namespace detail
    {
        /// @brief value is true it T has valid instance of set<Name> member
        template<typename Name, typename T>
        struct has_named_set_method
        {
        private:
            template<typename U, typename = decltype(&U::template set<Name>)>
            static std::true_type test(int*);
            template<typename U>
            static std::false_type test(...);

        public:
            static constexpr bool value = decltype(test<T>(nullptr))::value;
        };

        /// @brief specialization for tuple searches for tuple member that has valid
        /// set<Name> member
        template<typename Name, typename T1, typename... Rest>
        struct has_named_set_method<Name, std::tuple<T1, Rest...>>
        {
            static constexpr size_t index = has_named_set_method<Name, T1>::value ? 0 : 1 + has_named_set_method<Name, std::tuple<Rest...>>::index;
            static constexpr bool   value = (index != sizeof...(Rest) + 1);
        };

        /// @brief terminator of specialization for tuple
        template<typename Name>
        struct has_named_set_method<Name, std::tuple<>>
        {
            static constexpr size_t index = 0;
            static constexpr bool   value = false;
        };

        /// @brief program impl
        template<bool HasGeomShader, typename ProgramInputTuple>
        class program_impl;

        /// @brief only tuple of program inputs is a valid parameter for program
        template<bool HasGeomShader, typename... ProgramInput>
        class program_impl<HasGeomShader, std::tuple<ProgramInput...>> : public program_base, public ProgramInput...
        {
            /// @brief returns input type which has valid set<Name> method
            template<typename Name>
            using input_type = typename std::tuple_element<has_named_set_method<Name, std::tuple<ProgramInput...>>::index,
                                                           std::tuple<ProgramInput...>>::type;

            /// @brief returns argument type of set<Name> method found in input tuple
            template<typename Name>
            using set_arg_type = typename ct::function_traits<decltype(&input_type<Name>::template set<Name>)>::template arg_type<0>;

        public:
            /// @brief ctstring containing glsl declarations of all program inputs
            using declarations = ct::string_cat<cts("\n#ifdef VERTEX\n"),
                                                typename std::conditional<shader_has_decl<tag::vertex, typename ProgramInput::decl_tag>::value,
                                                                          typename ProgramInput::declaration,
                                                                          cts("")>::type...,
                                                cts("#elif defined GEOMETRY\n"),
                                                typename std::conditional<shader_has_decl<tag::geometry, typename ProgramInput::decl_tag>::value,
                                                                          typename ProgramInput::declaration,
                                                                          cts("")>::type...,
                                                cts("#else\n"),
                                                typename std::conditional<shader_has_decl<tag::fragment, typename ProgramInput::decl_tag>::value,
                                                                          typename ProgramInput::declaration,
                                                                          cts("")>::type...,
                                                cts("#endif\n"),
                                                typename std::conditional<HasGeomShader, cts("#define HAS_GEOMETRY\n"), cts("")>::type>;

            /// @brief constructor
            program_impl(const std::string& name, const std::string& glsl_version, const std::string& src)
                try : program_base(glsl_version, prepend_header_to_program(name, declarations::chars, src), HasGeomShader)
                    , ProgramInput(_object)...
            {}
            catch(glprogram_error& e) // add usefull info to exception and rethrow
            {
                e.prepend(name + " program failed:\n");
                e.append(prepend_header_to_program(name, declarations::chars, src));
                throw;
            }

            /// @brief @see program_base::select
            void select() const
            {
                program_base::select();
                glcxx_swallow(static_cast<const ProgramInput*>(this)->select());
            }

            /// @brief forward named set method to base class which has valid
            /// definition of it
            /// @note unfortunately it is impossible to write something like
            /// using ProgramInput::set...; to bring all base setter into current
            /// class' scope, so we need this forwarding function
            template<typename Name>
                void set(set_arg_type<Name> value)
            {
                input_type<Name>::template set<Name>(value);
            }
        };

        /// @brief return true if program input requires geometry shader, either it
        /// has uniform for geometry shader, or marked explicitly with tag::geometry
        template<typename T> struct is_geom
        {
            static constexpr bool value = shader_has_decl<tag::geometry, typename T::decl_tag>::value;
        };
        template<> struct is_geom<tag::geometry> { static constexpr bool value = true; };

        /// @brief find all separate NamedAttribs in InputTuple, replace them with
        /// aggregated vao input
        template<typename InputTuple>
        class program_input_traits
        {
            /// @brief check if type is specialization of NamedAttrib template
            template<typename T> struct specialization_of_vao_input
                : ct::specialization_of<T, vao_input> {};

            /// @brief all NamedAttribs
            using vao_inputs = ct::tuple_filter<InputTuple, specialization_of_vao_input>;

            /// @brief rest inputs
            using rest_of_inputs = ct::tuple_filter<InputTuple, specialization_of_vao_input, true>;

            /// @brief rest inputs with tag stripped
            using rest_of_inputs_no_tags = ct::tuple_strip<tag::geometry, rest_of_inputs>;

        public:
            /// @brief true if has geometry shader
            static constexpr bool has_geom_shader = ct::tuple_any_of<rest_of_inputs, is_geom>::value;

            /// @brief resulting program inputs
            using resulting_inputs = ct::tuple_cat<std::tuple<vao_input_impl<vao_inputs>>, rest_of_inputs_no_tags>;
        };

        /// @brief program with processed inputs
        template<typename InputTuple>
        struct program : public program_impl<program_input_traits<InputTuple>::has_geom_shader,
                                             typename program_input_traits<InputTuple>::resulting_inputs>
        {
            // @brief base class
            using base = program_impl<program_input_traits<InputTuple>::has_geom_shader,
                                      typename program_input_traits<InputTuple>::resulting_inputs>;

            /// @brief inherited constructor
            using base::base;
        };

        /// @brief meta functor returning true if 2 inputs have same name
        template<typename T1, typename T2> struct is_same_name {
            static constexpr bool value = std::is_same<typename T1::name, typename T2::name>::value;
        };
        // geometry tag doesn't have name, so just return false
        template<typename T2> struct is_same_name<tag::geometry, T2> { static constexpr bool value = false; };
        template<typename T1> struct is_same_name<T1, tag::geometry> { static constexpr bool value = false; };
        template<> struct is_same_name<tag::geometry, tag::geometry> { static constexpr bool value = false; };

        template<typename BaseProgram, typename AdditionalInputTuple> class derive_program;
        template<typename... Inputs, typename AdditionalInputTuple>
        class derive_program<program<std::tuple<Inputs...>>, AdditionalInputTuple>
        {
            // filter out Inputs that are overriden with same name in AdditionalInputTuple
            using filtered_inputs = ct::tuple_cat<
                typename std::conditional<
                    ct::tuple_any_of<AdditionalInputTuple, is_same_name, Inputs>::value,
                    std::tuple<>,
                    std::tuple<Inputs>>::type...>;

          public:
            using type = program<ct::tuple_cat<filtered_inputs, AdditionalInputTuple>>;
        };
    }

    /// @brief program shortcut
    template<typename... Inputs>
    using program = detail::program<std::tuple<Inputs...>>;

    /// @brief derive program shortcut
    template<typename BaseProgram, typename... AdditionalInputs>
    using derive_program = typename detail::derive_program<BaseProgram, std::tuple<AdditionalInputs...>>::type;
}

#endif
