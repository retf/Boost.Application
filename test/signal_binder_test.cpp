// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <boost/application.hpp>
#include <boost/test/minimal.hpp>

using namespace boost;

struct handler_test
{
   int count_;
   bool called_;
   handler_test() : count_(0), called_(false) { }

   bool signal_handler1()
   {
      called_ = true;
      return false;
   }

   bool running()
   {
      count_++;
	  return !called_;
   }
};

class my_signal_binder : public application::signal_binder
{
public:
	my_signal_binder(application::context &app_context)
	   : application::signal_binder(app_context){}

   void start()
   {
      signal_binder::start();
   }
};

int test_main(int argc, char** argv)
{
   application::context app_context;
   my_signal_binder app_signal_binder(app_context);
   handler_test app_handler_test;

   app_signal_binder.start();

   application::handler<>::callback cb = boost::bind(
               &handler_test::signal_handler1, &app_handler_test);

   boost::system::error_code ec;
   app_signal_binder.bind(SIGABRT, cb, ec);

   raise(SIGABRT);

   BOOST_CHECK(!ec);

   app_signal_binder.bind(SIGINT, cb, ec);

   BOOST_CHECK(!ec);

   app_signal_binder.bind(SIGINT, cb, cb, ec);

   BOOST_CHECK(!ec);
   BOOST_CHECK(app_signal_binder.is_bound(SIGINT));

   app_signal_binder.unbind(SIGINT, ec);

   BOOST_CHECK(!app_signal_binder.is_bound(SIGINT));

   while(app_handler_test.running())
      std::cerr << "waiting..." << std::endl;

   BOOST_CHECK(app_handler_test.count_ > 0);

   return 0;
}




