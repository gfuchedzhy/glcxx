/*
 * Copyright 2015, 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAM_HPP
#define ENGINE_PROGRAM_HPP

#include "Shader.hpp"
#include "BufferObject.hpp"
#include "VAOProgramInput.hpp"
#include <tuple>

/// @brief resource holder for opengl program object
class CProgramBase
{
      /// @brief disabled stuff
      CProgramBase(const CProgramBase&) = delete;
      CProgramBase& operator=(const CProgramBase& other) = delete;

      /// @brief delegate constructor
      CProgramBase(const std::string& src, bool hasGeometryShader);

   public:
      /// @brief constuctor
      CProgramBase(const char* name, const char* declarations, const char* src, bool hasGeometryShader);

      /// @brief destructor
      ~CProgramBase();

      /// @brief selects program
      void select() const
      {
         gl(glUseProgram, mObject);
      }

      /// @brief draw arrays
      void drawArrays(GLsizei size, GLenum mode) const
      {
         gl(glDrawArrays, mode, 0, size);
      }

   protected:
      /// @brief program object id
      GLuint mObject;

   private:
      /// @brief shaders, geometry shader is optional
      CShader mVertexShader;
      CShader mFragmentShader;
      std::unique_ptr<CShader> mGeometryShader;
};

namespace detail
{
   /// @brief value is true it T has valid instance of set<TName> member
   template<typename TName, typename T>
   struct THasNamedSetMethod
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
   struct THasNamedSetMethod<TName, std::tuple<T1, TRest...>>
   {
         static constexpr size_t index = THasNamedSetMethod<TName, T1>::value ? 0 : 1 + THasNamedSetMethod<TName, std::tuple<TRest...>>::index;
         static constexpr bool   value = (index != sizeof...(TRest) + 1);
   };

   /// @brief terminator of specialization for tuple
   template<typename TName>
   struct THasNamedSetMethod<TName, std::tuple<>>
   {
         static constexpr size_t index = 0;
         static constexpr bool   value = false;
   };

   /// @brief program impl
   template<typename TName, bool hasGeometryShader, typename TProgramInputTuple>
   class TProgramImpl;

   /// @brief only tuple of program inputs is a valid parameter for program
   template<typename TName, bool hasGeometryShader, typename... TProgramInput>
   class TProgramImpl<TName, hasGeometryShader, std::tuple<TProgramInput...>> : public CProgramBase, public TProgramInput...
   {
         /// @brief inputs tuple
         using tInputs = std::tuple<TProgramInput...>;

         /// @brief returns input type which has valid set<TSetName> method
         template<typename TSetName>
         using input_type = typename std::tuple_element<THasNamedSetMethod<TSetName, tInputs>::index, tInputs>::type;

         /// @brief returns argument type of set<TSetName> method found in input tuple
         template<typename TSetName>
         using set_arg_type = typename ct::function_traits<decltype(&input_type<TSetName>::template set<TSetName>)>::template arg_type<0>;

      public:
         /// @brief program name
         using tName = TName;

         /// @brief program defines
         using tDefines = ct::string_all_rep<
            ct::string_toupper<ct::string_sub<TName, ct::string_find<TName, cts("-")>::value>>,
            cts("_"),
            cts("\n#define ")>;

         /// @brief ctstring containing glsl declarations of all program inputs
         using tShaderDeclarations = ct::string_cat<tDefines,
                                                    cts("\n#ifdef VERTEX\n"),
                                                    typename std::conditional<glsl::TShaderHasDecl<tag::vertex, typename TProgramInput::tDeclTag>::value,
                                                                              typename TProgramInput::tDeclaration,
                                                                              cts("")>::type...,
                                                    cts("#elif defined GEOMETRY\n"),
                                                    typename std::conditional<glsl::TShaderHasDecl<tag::geometry, typename TProgramInput::tDeclTag>::value,
                                                                              typename TProgramInput::tDeclaration,
                                                                              cts("")>::type...,
                                                    cts("#else\n"),
                                                    typename std::conditional<glsl::TShaderHasDecl<tag::fragment, typename TProgramInput::tDeclTag>::value,
                                                                              typename TProgramInput::tDeclaration,
                                                                              cts("")>::type...,
                                                    cts("#endif\n"),
                                                    typename std::conditional<hasGeometryShader, cts("#define HAS_GEOMETRY\n"), cts("")>::type>;
         /// @brief constructor
         TProgramImpl(const char* src)
            : CProgramBase(tName::chars, tShaderDeclarations::chars, src, hasGeometryShader)
            , TProgramInput(mObject)...
         {}

         /// @brief @see CProgramBase::select
         void select() const
         {
            CProgramBase::select();
            swallow(TProgramInput::select());
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
         static constexpr bool value = glsl::TShaderHasDecl<tag::geometry, typename T::tDeclTag>::value;
   };
   template<> struct is_geom<tag::geometry> { static constexpr bool value = true; };

   /// @brief find all separate TNamedAttribs in TInputTuple, replace them with
   /// aggregated vao input
   template<typename TInputTuple>
   class program_input_traits
   {
         /// @brief check if type is instantiation of TNamedAttrib template
         template<typename T> struct instantiation_of_named_attrib
            : ct::instantiation_of<T, TNamedAttrib> {};

         /// @brief all TNamedAttribs
         using tVAOInputs = ct::tuple_filter<TInputTuple, instantiation_of_named_attrib>;

         /// @brief rest inputs
         using tRestOfInputs = ct::tuple_filter<TInputTuple, instantiation_of_named_attrib, true>;

         /// @brief rest inputs with tag stripped
         using tRestOfInputsNoTags = ct::tuple_strip<tag::geometry, tRestOfInputs>;

     public:
         /// @brief true if has geometry shader
         static constexpr bool hasGeometryShader = ct::tuple_any_of<is_geom, tRestOfInputs>::value;

         /// @brief resulting program inputs
         using resultingInputs = typename std::conditional<0 == std::tuple_size<tVAOInputs>::value, //empty?
                                                           tRestOfInputsNoTags,
                                                           ct::tuple_append<tRestOfInputsNoTags, TVertexArrayObjectProgramInput<tVAOInputs>>>::type;
   };
}

/// @brief program
template<typename TName, typename TInputTuple>
using TProgram = detail::TProgramImpl<TName,
                                      detail::program_input_traits<TInputTuple>::hasGeometryShader,
                                      typename detail::program_input_traits<TInputTuple>::resultingInputs>;

#endif // ENGINE_PROGRAM_HPP
