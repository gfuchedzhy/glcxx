/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef GLCXX_SRC_PROGRAM_HPP
#define GLCXX_SRC_PROGRAM_HPP

#include "shader.hpp"
#include "vao_input.hpp"

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
            glcxx_gl(glUseProgram, mObject);
         }

      protected:
         /// @brief program object id
         GLuint mObject;
   };

   class program_base : public program_res_holder
   {
         /// @brief shaders, geometry shader is optional
         shader mVertexShader;
         shader mFragmentShader;
         std::unique_ptr<shader> mGeometryShader;

      public:
         /// @brief constuctor
         program_base(const std::string& glsl_version, const std::string& src, bool has_geom_shader);
   };

   namespace detail
   {
      /// @brief value is true it T has valid instance of set<TName> member
      template<typename TName, typename T>
      struct has_named_set_method
      {
         private:
            template<typename U, typename = decltype(&U::template set<TName>)>
            static std::true_type test(int*);
            template<typename U>
            static std::false_type test(...);

         public:
            static constexpr bool value = decltype(test<T>(nullptr))::value;
      };

      /// @brief specialization for tuple searches for tuple member that has valid
      /// set<TName> member
      template<typename TName, typename T1, typename... TRest>
      struct has_named_set_method<TName, std::tuple<T1, TRest...>>
      {
            static constexpr size_t index = has_named_set_method<TName, T1>::value ? 0 : 1 + has_named_set_method<TName, std::tuple<TRest...>>::index;
            static constexpr bool   value = (index != sizeof...(TRest) + 1);
      };

      /// @brief terminator of specialization for tuple
      template<typename TName>
      struct has_named_set_method<TName, std::tuple<>>
      {
            static constexpr size_t index = 0;
            static constexpr bool   value = false;
      };

      /// @brief program impl
      template<typename TName, bool HasGeomShader, typename TProgramInputTuple>
      class program;

      /// @brief only tuple of program inputs is a valid parameter for program
      template<typename TName, bool HasGeomShader, typename... TProgramInput>
      class program<TName, HasGeomShader, std::tuple<TProgramInput...>> : public program_base, public TProgramInput...
      {
            /// @brief inputs tuple
            using input_tuple = std::tuple<TProgramInput...>;

            /// @brief returns input type which has valid set<TSetName> method
            template<typename TSetName>
            using input_type = typename std::tuple_element<has_named_set_method<TSetName, input_tuple>::index, input_tuple>::type;

            /// @brief returns argument type of set<TSetName> method found in input tuple
            template<typename TSetName>
            using set_arg_type = typename ct::function_traits<decltype(&input_type<TSetName>::template set<TSetName>)>::template arg_type<0>;

         public:
            /// @brief program defines
            using defines = ct::string_all_rep<
               ct::string_toupper<ct::string_sub<TName, ct::string_find<TName, cts("-")>::value>>,
               cts("_"),
               cts("\n#define ")>;

            /// @brief ctstring containing glsl declarations of all program inputs
            using declarations = ct::string_cat<defines,
                                                cts("\n#ifdef VERTEX\n"),
                                                typename std::conditional<shader_has_decl<tag::vertex, typename TProgramInput::decl_tag>::value,
                                                                          typename TProgramInput::declaration,
                                                                          cts("")>::type...,
                                                cts("#elif defined GEOMETRY\n"),
                                                typename std::conditional<shader_has_decl<tag::geometry, typename TProgramInput::decl_tag>::value,
                                                                          typename TProgramInput::declaration,
                                                                          cts("")>::type...,
                                                cts("#else\n"),
                                                typename std::conditional<shader_has_decl<tag::fragment, typename TProgramInput::decl_tag>::value,
                                                                          typename TProgramInput::declaration,
                                                                          cts("")>::type...,
                                                cts("#endif\n"),
                                                typename std::conditional<HasGeomShader, cts("#define HAS_GEOMETRY\n"), cts("")>::type>;

            /// @brief constructor
            program(const std::string& glsl_version, const std::string& src)
               try : program_base(glsl_version, std::string(declarations::chars) + src, HasGeomShader)
                   , TProgramInput(mObject)...
            {}
            catch(glprogram_error& e) // add usefull info to exception and rethrow
            {
               e.prepend(" program failed:\n");
               e.prepend(TName::chars);
               e.append("\n");
               e.append(glsl_version.c_str());
               e.append(declarations::chars);
               e.append(src.c_str());
               throw;
            }

            /// @brief @see program_base::select
            void select() const
            {
               program_base::select();
               glcxx_swallow(TProgramInput::select());
            }

            /// @brief forward named set method to base class which has valid
            /// definition of it
            /// @note unfortunately it is impossible to write something like
            /// using TProgramInput::set...; to bring all base setter into current
            /// class' scope, so we need this forwarding function
            template<typename TSetName>
            void set(set_arg_type<TSetName> value)
            {
               input_type<TSetName>::template set<TSetName>(value);
            }
      };

      /// @brief return true if program input requires geometry shader, either it
      /// has uniform for geometry shader, or marked explicitly with tag::geometry
      template<typename T> struct is_geom
      {
            static constexpr bool value = shader_has_decl<tag::geometry, typename T::decl_tag>::value;
      };
      template<> struct is_geom<tag::geometry> { static constexpr bool value = true; };

      /// @brief find all separate TNamedAttribs in TInputTuple, replace them with
      /// aggregated vao input
      template<typename TInputTuple>
      class program_input_traits
      {
            /// @brief check if type is instantiation of TNamedAttrib template
            template<typename T> struct instantiation_of_named_attrib
               : ct::instantiation_of<T, attrib_input> {};

            /// @brief all TNamedAttribs
            using vao_inputs = ct::tuple_filter<TInputTuple, instantiation_of_named_attrib>;

            /// @brief rest inputs
            using rest_of_inputs = ct::tuple_filter<TInputTuple, instantiation_of_named_attrib, true>;

            /// @brief rest inputs with tag stripped
            using rest_of_inputs_no_tags = ct::tuple_strip<tag::geometry, rest_of_inputs>;

         public:
            /// @brief true if has geometry shader
            static constexpr bool has_geom_shader = ct::tuple_any_of<rest_of_inputs, is_geom>::value;

            /// @brief resulting program inputs
            using resulting_inputs = typename std::conditional<0 == std::tuple_size<vao_inputs>::value, //empty?
                                                              rest_of_inputs_no_tags,
                                                              ct::tuple_append<rest_of_inputs_no_tags, vao_input<vao_inputs>>>::type;
      };
   }

   /// @brief program
   template<typename TName, typename TInputTuple>
   using program = detail::program<TName,
                                   detail::program_input_traits<TInputTuple>::has_geom_shader,
                                   typename detail::program_input_traits<TInputTuple>::resulting_inputs>;
}

#endif
