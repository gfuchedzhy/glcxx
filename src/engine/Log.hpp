/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef ENGINE_LOG_HPP
#define ENGINE_LOG_HPP

#include <iostream>
#include <sstream>

namespace Log
{
   /// @brief termination template for helper variadic template for concatWithDelim
   template<typename TArg>
   inline void addToStream(std::ostringstream& stream, const std::string& delimiter, TArg&& arg)
   {
      stream << std::forward<TArg>(arg);
   }

   /// @brief helper variadic template for concatWithDelim
   template<typename TArg1, typename... TRestArgs>
   inline void addToStream(std::ostringstream& stream, const std::string& delimiter, TArg1&& arg1, TRestArgs&&... restArgs)
   {
      stream << std::forward<TArg1>(arg1) << delimiter;
      addToStream(stream, delimiter, std::forward<TRestArgs>(restArgs)...);
   }

   /// @brief concatanates parameters separated by delimiter into single string
   template<typename... TArgs>
   inline std::string concatWithDelim(const std::string& delimiter, TArgs&&... args)
   {
      std::ostringstream stream;
      addToStream(stream, delimiter, std::forward<TArgs>(args)...);
      return stream.str();
   }

   /// @brief log message
   template<typename... TArgs>
   inline void msg(TArgs&&... args)
   {
      std::cout << concatWithDelim("", std::forward<TArgs>(args)...) << std::endl;
   }

   /// @brief log error
   template<typename... TArgs>
   inline void err(TArgs&&... args)
   {
      std::cerr << "ERROR: " << concatWithDelim("", std::forward<TArgs>(args)...) << std::endl;
   }
}

#endif // ENGINE_LOG_HPP
