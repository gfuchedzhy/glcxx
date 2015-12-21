/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_PROGRAM_HPP
#define ENGINE_PROGRAM_HPP

#include "Shader.hpp"
#include "ProgramInput.hpp"
#include <tuple>

/// @brief resource holder for opengl program object
class CProgramBase
{
      /// @brief disabled stuff
      CProgramBase(const CProgramBase&) = delete;
      CProgramBase& operator=(const CProgramBase& other) = delete;
   public:
      /// @brief constuctor
      CProgramBase(const std::string& vertexSrc, const std::string& fragmentSrc);

      /// @brief destructor
      ~CProgramBase();

      /// @brief selects program
      void select() const
      {
         gl(glUseProgram, mObject);
      }

      /// @brief draw with current index buffer, if size provided draw exactly
      /// size indices, otherwise draw whole buffer
      void draw(const std::shared_ptr<CIndexBuffer>& buffer, GLsizei size = -1) const
      {
         assert(buffer);
         buffer->bind();
         buffer->draw(size);
      }

      /// @brief draw with given pointer
      template<typename T>
      void draw(const T* data, GLsizei size, GLenum mode) const
      {
         constexpr GLenum id = glsl::TTypeID<T>::id;
         static_assert(GL_UNSIGNED_BYTE == id || GL_UNSIGNED_SHORT == id || GL_UNSIGNED_INT == id, "unsupported index type provided");
         CIndexBuffer::unBind();
         gl(glDrawElements, mode, size, id, data);
      }

   protected:
      /// @brief program object id
      GLuint mObject = 0;

   private:
      /// @brief shaders
      CShader mVertexShader;
      CShader mFragmentShader;
};

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

/// @brief program
template<typename TName, typename TProgramInputTuple> class TProgram;

/// @brief only tuple of program inputs is a valid parameter for program
template<typename TName, typename... TProgramInput>
class TProgram<TName, std::tuple<TProgramInput...>> : public CProgramBase, public TProgramInput...
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
      using tVertexShaderDeclaration = ct::string_cat<tDefines, cts("\n#define VERTEX\n"), typename TProgramInput::tVertexShaderDeclaration...>;
      using tFragmentShaderDeclaration = ct::string_cat<tDefines, cts("\n#define FRAGMENT\n"),  typename TProgramInput::tFragmentShaderDeclaration...>;

      /// @brief constructor
      TProgram(const std::string& src)
         : CProgramBase(tVertexShaderDeclaration::chars + src, tFragmentShaderDeclaration::chars + src)
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

#endif // ENGINE_PROGRAM_HPP
