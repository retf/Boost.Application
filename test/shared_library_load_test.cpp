// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <boost/application/shared_library.hpp>
#include <boost/test/minimal.hpp>

// Unit Tests

int test_main(int argc, char* argv[])
{
   using namespace boost::application;

   const boost::filesystem::path shared_library_path(argv[1]);

   {
      shared_library sl(shared_library_path);
      BOOST_CHECK(sl.is_loaded());

      shared_library sl2;
      BOOST_CHECK(!sl2.is_loaded());

      swap(sl, sl2);
      BOOST_CHECK(!sl.is_loaded());
      BOOST_CHECK(sl2.is_loaded());
   }

   {
      boost::system::error_code ec;
      shared_library sl(argv[1], ec);
      BOOST_CHECK(sl.is_loaded());
   }

   {
      shared_library sl;
      sl.load(shared_library_path);
      BOOST_CHECK(sl.is_loaded());
   }

   {
      shared_library sl;
      boost::system::error_code ec;
      sl.load(shared_library_path, ec);
      BOOST_CHECK(sl.is_loaded());
   }

#if defined( BOOST_WINDOWS_API )
#if !defined(__MINGW32__)   
   {
      shared_library sl(shared_library_path, load_with_altered_search_path );
      BOOST_CHECK(sl.is_loaded());
   }

   {
      boost::system::error_code ec;
      shared_library sl(shared_library_path, load_with_altered_search_path, ec);
      BOOST_CHECK(sl.is_loaded());
   }

   {
      shared_library sl;
      sl.load(shared_library_path, load_with_altered_search_path);
      BOOST_CHECK(sl.is_loaded());
   }

   {
      shared_library sl;
      boost::system::error_code ec;
      sl.load(shared_library_path, load_with_altered_search_path, ec);
      BOOST_CHECK(sl.is_loaded());
   }
#endif   
#elif defined( BOOST_POSIX_API )

   {
      shared_library sl(shared_library_path, rtld_lazy | rtld_global);
      BOOST_CHECK(sl.is_loaded());
   }

   {
      boost::system::error_code ec;
      shared_library sl(shared_library_path, rtld_lazy | rtld_global, ec);
      BOOST_CHECK(sl.is_loaded());
   }

   {
      shared_library sl;
      sl.load(shared_library_path, rtld_lazy | rtld_global);
      BOOST_CHECK(sl.is_loaded());
   }

   {
      shared_library sl;
      boost::system::error_code ec;
      sl.load(shared_library_path, rtld_lazy | rtld_global, ec);
      BOOST_CHECK(sl.is_loaded());
   }
#endif

   {  // unload
      shared_library sl(shared_library_path);
      BOOST_CHECK(sl.is_loaded());
      sl.unload();
      BOOST_CHECK(!sl.is_loaded());
   }

   return 0;
}
