

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

//[intro
#include <iostream>
#include <boost/application.hpp>
#include <boost/test/minimal.hpp>

// other unit
#include "myapp.h"

int test_main(int argc, char** argv)
{   
   myapp app;
   application::global_context_ptr ctx = application::global_context::create();

   this_application()->insert<application::args>(
      boost::make_shared<application::args>(argc, argv));

   BOOST_CHECK(application::launch<application::common>(app, ctx) == 0);

   application::global_context::destroy();
   
   return 0;
}
