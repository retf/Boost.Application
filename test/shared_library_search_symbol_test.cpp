// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/application/shared_library.hpp>
#include <boost/test/minimal.hpp>

#if defined( BOOST_WINDOWS_API )
   const boost::filesystem::path shared_library_path("C:/test/boost/application/test_library.dll");
#elif defined( BOOST_POSIX_API )
   const boost::filesystem::path shared_library_path("/test/boost/application/libtest_library.so");
#else
#  error "Sorry, no boost application are available for this platform."
#endif

// Unit Tests

int test_main(int, char*[])
{
   using namespace boost::application;

   {
      shared_library sl(library(shared_library_path));
      BOOST_CHECK(sl.search_symbol(symbol("say_hello")));
      BOOST_CHECK(sl.search_symbol(symbol("lib_version")));
      BOOST_CHECK(!sl.search_symbol(symbol("i_do_not_exist")));
   }

   return 0;
}

