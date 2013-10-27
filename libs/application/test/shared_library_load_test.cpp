// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/test/minimal.hpp>
#include <boost/application/shared_library.hpp>

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
      BOOST_CHECK(sl.is_loaded());
   }

   {
      boost::system::error_code ec;
      shared_library sl(library(shared_library_path), ec);
      BOOST_CHECK(sl.is_loaded());
   }

   {
      shared_library sl;
      sl.load(library(shared_library_path));
      BOOST_CHECK(sl.is_loaded());
   }

   {
      shared_library sl;
      boost::system::error_code ec;
      sl.load(library(shared_library_path), ec);
      BOOST_CHECK(sl.is_loaded());
   }

#if defined( BOOST_WINDOWS_API )
   {
      shared_library sl(library(shared_library_path), load_with_altered_search_path );
      BOOST_CHECK(sl.is_loaded());
   }

   {
      boost::system::error_code ec;
      shared_library sl(library(shared_library_path), load_with_altered_search_path, ec);
      BOOST_CHECK(sl.is_loaded());
   }

   {
      shared_library sl;
      sl.load(library(shared_library_path), load_with_altered_search_path);
      BOOST_CHECK(sl.is_loaded());
   }

   {
      shared_library sl;      
      boost::system::error_code ec;
      sl.load(library(shared_library_path), load_with_altered_search_path, ec);
      BOOST_CHECK(sl.is_loaded());
   }
#elif defined( BOOST_POSIX_API )

   {
      shared_library sl(library(shared_library_path), rtld_lazy | rtld_global);
      BOOST_CHECK(sl.is_loaded());
   }

   {
      boost::system::error_code ec;
      shared_library sl(library(shared_library_path), rtld_lazy | rtld_global, ec);
      BOOST_CHECK(sl.is_loaded());
   }

   {
      shared_library sl;
      sl.load(library(shared_library_path), rtld_lazy | rtld_global);
      BOOST_CHECK(sl.is_loaded());
   }

   {
      shared_library sl;
      boost::system::error_code ec;
      sl.load(library(shared_library_path), rtld_lazy | rtld_global, ec);
      BOOST_CHECK(sl.is_loaded());
   }
#endif

   {  // unload
      shared_library sl(library(shared_library_path));
      BOOST_CHECK(sl.is_loaded());
      sl.unload();
      BOOST_CHECK(!sl.is_loaded());
   }

   return 0;
}
