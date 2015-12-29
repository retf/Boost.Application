// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <boost/application.hpp>
#include <boost/test/minimal.hpp>

using namespace boost;

int test_main(int argc, char** argv)
{   
   filesystem::path module_path_name;

#if defined( BOOST_WINDOWS_API )
   wchar_t module_name[MAX_PATH];

   if (GetModuleFileNameW(0, module_name, sizeof(module_name)) > 0)
   {
      module_path_name = module_name;
   }
#elif defined( BOOST_POSIX_API )
   std::string command = argv[0];
   char resolved_path[PATH_MAX];

   // realpath -returns the canonicalized absolute pathname
   if (realpath (command.c_str(), resolved_path))
   {
      module_path_name = std::string (resolved_path);
   }
#endif

   application::path path;

   {
      BOOST_CHECK(filesystem::current_path() == path.current_path());
   }
   
   {
      filesystem::path module_path(module_path_name);
      BOOST_CHECK(module_path.stem() == path.executable_name());
   }

   {
      filesystem::path module_path(module_path_name);
      BOOST_CHECK(module_path.filename() == path.executable_full_name());
   }

   {
      filesystem::path module_path(module_path_name);
      BOOST_CHECK(module_path.parent_path() == path.executable_path());
   }

   {
      BOOST_CHECK(module_path_name == path.executable_path_name());
   }

   std::string isempty;
   BOOST_CHECK(!isempty.size());

   isempty = path.app_data_path().string();
   BOOST_CHECK(isempty.size());

   isempty = path.home_path().string();
   BOOST_CHECK(isempty.size());
  
   isempty = path.config_path().string();
   BOOST_CHECK(isempty.size());

   isempty = path.app_data_path().string();
   BOOST_CHECK(isempty.size());

   isempty = path.temp_path().string();
   BOOST_CHECK(isempty.size());
 
   return 0;
}

