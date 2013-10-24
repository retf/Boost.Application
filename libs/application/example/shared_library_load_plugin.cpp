// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

// -----------------------------------------------------------------------------
// This example shows how to use shared_library class to load a plugin of a DSO.
// -----------------------------------------------------------------------------

#define BOOST_ALL_DYN_LINK
#define BOOST_LIB_DIAGNOSTIC

#include <boost/application.hpp>
#include "plugin_api.hpp"

#if defined( BOOST_WINDOWS_API )
   const boost::filesystem::path shared_library_path("C:/test/boost/application/plugin_library.dll");
#elif defined( BOOST_POSIX_API )
   const boost::filesystem::path shared_library_path("/test/boost/application/libplugin_library.so");
#else
#  error "Sorry, no boost application are available for this platform."
#endif

using namespace boost;

class my_application_functor_class
{
   // plugin entry point
   typedef my_plugin_api* (*pluginapi_create) (void);
   typedef void (*pluginapi_delete) (my_plugin_api* myplugin);
   
public:

   int operator()(application::context& context)
   {
      // my app logic here
      my_plugin_api* plugin = NULL;

      application::shared_library sl(application::library(shared_library_path));

      if(sl.search_symbol(application::symbol("create_my_plugin")))
      {
         plugin = ((pluginapi_create)sl(application::symbol("create_my_plugin")))();
      }

      if(plugin != NULL)
      {
         std::cout << "Plugin Version: " << plugin->version() << std::endl;
         std::cout << "Plugin Method:  " << plugin->calculate(1.5, 1.5) << std::endl;

         ((pluginapi_delete)sl(application::symbol("delete_my_plugin")))(plugin);
      }

      return 0;
   }

};

int main(int argc, char* argv[])
{ 
   BOOST_APPLICATION_FEATURE_SELECT

   try
   {
      my_application_functor_class app;
      application::context app_context;

      app_context.add_aspect<application::args>(
         make_shared<application::args>(argc, argv));

      return application::launch<application::common>(app, app_context);
   }
   catch(boost::system::system_error& se)
   {
      // error
      std::cerr << se.what() << std::endl;

      return 1;
   }
}

