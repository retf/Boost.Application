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

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

//[callplugcpp
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
   typedef my_plugin_api* (pluginapi_create) (void);
   typedef void (pluginapi_delete) (my_plugin_api* myplugin);
   
public:

   my_application_functor_class(application::context& context)
      : context_(context)
   {
   }

   int operator()()
   {
      // my app logic here
      my_plugin_api* plugin = NULL;

      application::shared_library sl(shared_library_path);

      if(sl.search_symbol("create_my_plugin"))
      {
         plugin = (sl.get<pluginapi_create>("create_my_plugin"))();
      }

      if(plugin != NULL)
      {
         std::cout << "Plugin Version: " << plugin->version() << std::endl;
         std::cout << "Plugin Method:  " << plugin->calculate(1.5, 1.5) << std::endl;

         (sl.get<pluginapi_delete>("delete_my_plugin"))(plugin);
      }

      return 0;
   }

private:
   application::context& context_;

};

int main(int argc, char* argv[])
{ 
   try
   {
      application::context app_context;
      my_application_functor_class app(app_context);

      app_context.insert<application::args>(
         boost::make_shared<application::args>(argc, argv));

      return application::launch<application::common>(app, app_context);
   }
   catch(boost::system::system_error& se)
   {
      // error
      std::cerr << se.what() << std::endl;

      return 1;
   }
}
//]
