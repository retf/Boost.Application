// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <boost/application.hpp>
#define BOOST_TEST_MODULE SimpleServerApplication
#include <boost/test/unit_test.hpp>

using namespace boost;

class myapp
{
public:
   myapp(application::context& context)
      : context_(context) { }
   
   int operator()()
   {
      return 0;
   }
   
private:
   application::context& context_;
};

BOOST_AUTO_TEST_CASE(simple_server_application)
{
   application::context app_context; 
   myapp app(app_context);

   boost::system::error_code ec;
   int ret = application::launch<application::server>(app, app_context, ec);

#if defined(BOOST_WINDOWS_API)   
#if !defined(__MINGW32__)   
   // 1063 (0x427)
   // The service process could not connect to the service controller.
   BOOST_CHECK(ec.value() == 1063);
   // on windows we need run on SCM
#endif
#else
   BOOST_CHECK(ret == 0);
#endif

}


