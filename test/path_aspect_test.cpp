// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

// #define BOOST_ALL_DYN_LINK
// #define BOOST_LIB_DIAGNOSTIC

#include <iostream>
#include <boost/application.hpp>
#include <boost/test/minimal.hpp>

#if defined( BOOST_WINDOWS_API )
#elif defined( BOOST_POSIX_API )
#endif

using namespace boost;

int test_main(int argc, char** argv)
{   
   filesystem::path module_path_name;

   application::default_path path;

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

   return 0;
}

