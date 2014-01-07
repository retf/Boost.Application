// Copyright 2011-2013 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#define BOOST_ALL_DYN_LINK
#define BOOST_LIB_DIAGNOSTIC

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/name_generator.hpp>
#include <boost/lexical_cast.hpp>

#include "plugin_api.hpp"

#include <iostream>

#if defined(_WIN32)
#   define LIBRARY_API __declspec(dllexport)
#else
#   define LIBRARY_API
#endif
   
extern "C" LIBRARY_API my_plugin_api* create_my_plugin(void);
extern "C" LIBRARY_API void delete_my_plugin(my_plugin_api* myplugin);

class my_plugin_sum : public my_plugin_api
{
public:

   float version() 
   { 
      return 1.0;  
   };

   std::string transform_string(const std::string& source)
   { 
      boost::uuids::uuid dns_namespace_uuid;

      boost::uuids::name_generator gen(dns_namespace_uuid);
      boost::uuids::uuid u = gen(source);

      return boost::lexical_cast<std::string>(u);
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

