// Copyright 2011-2012 Renato Tegon Forti
// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
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

   BOOST_CHECK(argc >= 2);
   boost::filesystem::path shared_library_path = std::string(argv[1]);
   shared_library_path /= "libtest_library" + shared_library::suffix();
   std::cout << "Library: " << shared_library_path;

   {
      shared_library sl(shared_library_path);
      BOOST_CHECK(sl.search_symbol("say_hello"));
      BOOST_CHECK(sl.search_symbol("lib_version"));
      BOOST_CHECK(sl.search_symbol("integer_g"));
      BOOST_CHECK(!sl.search_symbol("i_do_not_exist"));
   }

   return 0;
}

