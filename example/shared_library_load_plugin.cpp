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

using namespace boost;

class my_application_functor_class {
   // plugin entry point
   typedef my_plugin_api* (pluginapi_create) (void);
   typedef void (pluginapi_delete) (my_plugin_api* myplugin);
   
public:

   my_application_functor_class(application::context& context, const filesystem::path& shared_library_path)
      : context_(context)
      , shared_library_path_(shared_library_path)
   {}

   int operator()() {
      // my app logic here
      application::shared_library sl(shared_library_path_);

      if (!sl.search_symbol("create_my_plugin")) {
         return 2;
      }

      my_plugin_api* plugin = (sl.get<pluginapi_create>("create_my_plugin"))();

      if (plugin == NULL) {
         return 3;
      }

      std::cout << "Plugin Version: " << plugin->version() << std::endl;
      std::cout << "Plugin Method:  " << plugin->calculate(1.5, 1.5) << std::endl;

      (sl.get<pluginapi_delete>("delete_my_plugin"))(plugin);
      return 0;
   }

private:
   application::context& context_;
   boost::filesystem::path shared_library_path_;
};

int main(int argc, char* argv[])
{ 
   BOOST_ASSERT(argc >= 2);

   try {
      application::context app_context;
      my_application_functor_class app(
         app_context, std::string(argv[1]) + "/libplugin_library" + application::shared_library::suffix());

      app_context.insert<application::args>(
         boost::make_shared<application::args>(argc, argv));

      return application::launch<application::common>(app, app_context);
   } catch(boost::system::system_error& se) {
      // error
      std::cerr << se.what() << std::endl;

      return 1;
   }
}
//]
