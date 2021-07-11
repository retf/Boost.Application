

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST
//[intro
#include <iostream>
#include <boost/application.hpp>
#define BOOST_TEST_MODULE  Tunit
#include <boost/test/unit_test.hpp>

// other unit
#include "myapp.h"

BOOST_AUTO_TEST_CASE(tunit)
{
   auto& argc = boost::unit_test::framework::master_test_suite().argc;
   auto& argv = boost::unit_test::framework::master_test_suite().argv;

   myapp app;
   application::global_context_ptr ctx = application::global_context::create();

   this_application()->insert<application::args>(
      boost::make_shared<application::args>(argc, argv));

   BOOST_CHECK(application::launch<application::common>(app, ctx) == 0);

   application::global_context::destroy();

}
