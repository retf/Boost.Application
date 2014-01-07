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

using namespace boost;

int test_main(int argc, char** argv)
{   
   application::args myargs(argc, argv);

   BOOST_CHECK(myargs.argc());
   
   std::vector< std::string > argvec = myargs.arg_vector();

   BOOST_CHECK(argvec.size());

   return 0;
}



