// Copyright 2011-2013 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <iostream>

//[plugcpp

#if defined(_WIN32)
#   define LIBRARY_API __declspec(dllexport)
#else
#   define LIBRARY_API
#endif
   
#include "plugin_api.hpp"

extern "C" LIBRARY_API my_plugin_api* create_my_plugin(void);
extern "C" LIBRARY_API void delete_my_plugin(my_plugin_api* myplugin);

class my_plugin_sum : public my_plugin_api
{
public:

   float version() 
   { 
      return 1.0;  
   };

   float calculate(float x, float y) 
   { 
      return x+y; 
   };
   
   ~my_plugin_sum()
   {
      std::cout << ";o)" << std::endl;
   }
};

my_plugin_api* create_my_plugin(void)
{
   my_plugin_api *myplugin = new my_plugin_sum();
   return myplugin;
}

void delete_my_plugin(my_plugin_api* myplugin)
{
   delete myplugin;
}
//]

