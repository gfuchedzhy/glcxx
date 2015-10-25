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
   inline void output(TArg1&& arg1, TRestArgs&&... restArgs)
   {
      s << arg1;
      output<s>(std::forward<TRestArgs>(restArgs)...);
   }

   template<typename... TArgs>
   inline void msg(TArgs&&... args)
   {
      output<std::cout>(std::forward<TArgs>(args)...);
   }

   template<typename... TArgs>
   inline void err(TArgs&&... args)
   {
      output<std::cerr>(std::forward<TArgs>(args)...);
   }
}

#endif // LOG_HPP
