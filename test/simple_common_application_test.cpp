// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <boost/application.hpp>
#define BOOST_TEST_MODULE SimpleCommonApplication
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

BOOST_AUTO_TEST_CASE(simple_common_application)
{   
   application::context app_context;
   myapp app(app_context);

   BOOST_CHECK(application::launch<application::common>(app, app_context) == 0);
}



