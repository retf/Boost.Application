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

   boost::system::error_code ec;
   int ret = application::launch<application::server>(app, app_context, ec);

#if defined(BOOST_WINDOWS_API)      
   // 1063 (0x427)
   // The service process could not connect to the service controller.
   BOOST_CHECK(ec.value() == 1063);
   // on windows we need run on SCM
#else
   BOOST_CHECK(ret == 0);
#endif

   return 0;
}


