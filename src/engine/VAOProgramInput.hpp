/*
 * Copyright 2016 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_VAOPROGRAMINPUT_HPP
#define ENGINE_VAOPROGRAMINPUT_HPP

#include "VAO.hpp"

/// @brief named attribute class, used for program declarations
template<typename TName, typename TAttribTraits>
struct TNamedAttrib
{
      using tName = TName;
      using tAttribTraits = TAttribTraits;
};

/// @brief holds state of program's vao
template<typename TNamedAttribTuple> class TVertexArrayObjectProgramInput;

/// @brief specialization for tuple
template<typename... TNamedAttribs>
class TVertexArrayObjectProgramInput<std::tuple<TNamedAttribs...>>
{
   public:
      /// @brief vao type
      using tVAO = TVertexArrayObject<std::tuple<TNamedAttribs...>>;

      /// @brief vao ptr
      using tVAOPtr = std::shared_ptr<tVAO>;

      /// @brief locations for all buffers inside vao
      using tLocations = typename tVAO::tLocations;

      /// @brief vao holds VBOs, which are always vertex shader inputs
      using tDeclTag = tag::vertex;

      /// @brief ctstring containing glsl declaration of attributes
      using tDeclaration = ct::string_cat<typename TNamedAttribs::tAttribTraits::template tDeclaration<typename TNamedAttribs::tName>...>;

      /// @brief constructor
      TVertexArrayObjectProgramInput(const GLuint program)
         : mLocations{{glsl::getAttribLocation(program, TNamedAttribs::tName::chars)...}}
      {}

      /// @brief named set method
      template<typename TName>
      typename std::enable_if<std::is_same<TName, cts("vao")>::value>::type
      set(const tVAOPtr& value)
      {
         if(mVAO != value)
         {
            mVAO = value;
            if (mVAO)
               mVAO->attach(mLocations);
            else
               tVAO::unBind();
         }
      }

      /// @brief called after program was selected, attaches current vao
      void select() const
      {
         if (mVAO)
            mVAO->attach(mLocations);
      }

      /// @brief draw using index buffer from vao
      void drawElements() const
      {
         assert(mVAO);
         mVAO->drawElements();
      }

  private:
      /// @brief locations for all buffers inside vao
      tLocations mLocations;

      /// @brief currently bound vao
      tVAOPtr mVAO;
};

#endif
