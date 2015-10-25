/*
 * Copyright 2015 Grygoriy Fuchedzhy <grygoriy.fuchedzhy@gmail.com>
 */

#ifndef LOG_HPP
#define LOG_HPP

#include <iostream>

namespace Log
{
   template<std::ostream& s>
   inline void output()
   {
      s << std::endl;
   }

   template<std::ostream& s, typename TArg1, typename... TRestArgs>
   inline void output(const TArg1& arg1, const TRestArgs&... restArgs)
   {
      s << arg1;
      output<s>(restArgs...);
   }

   template<typename... TArgs>
   inline void msg(const TArgs&... args)
   {
      output<std::cout>(args...);
   }

   template<typename... TArgs>
   inline void err(const TArgs&... args)
   {
      output<std::cerr>(args...);
   }
}

#endif // LOG_HPP
