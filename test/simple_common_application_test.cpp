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

class myapp
{
public:
   // param
   int operator()(application::context& context)
   {
      return 0;
   }
};

int test_main(int argc, char** argv)
{   
   myapp app;
   application::context app_context;

   BOOST_CHECK(application::launch<application::common>(app, app_context) == 0);
   return 0;
}



