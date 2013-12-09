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

struct handler_test
{
   bool parameter_handler(application::context &context)
   {
      return true;
   }

   bool singleton_handler()
   {
      return true;
   }
};

int test_main(int argc, char** argv)
{
   handler_test app_handler_test;
   application::context app_context;

   {
      application::handler<> h;

      BOOST_CHECK(!h.parameter_callback_is_valid());
      BOOST_CHECK(!h.singleton_callback_is_valid());
   }

   {
      application::handler<>::parameter_callback callback = boost::bind<bool>(
         &handler_test::parameter_handler, &app_handler_test, _1);

      application::handler<> h(callback);
      BOOST_CHECK(h.parameter_callback_is_valid());

      application::handler<>::parameter_callback* parameter = 0;
      BOOST_CHECK(h.callback(parameter));
      BOOST_CHECK((*parameter)(app_context));
   }

   {
      application::handler<>::singleton_callback callback = boost::bind<bool>(
         &handler_test::singleton_handler, &app_handler_test);

      application::handler<> h(callback);
      BOOST_CHECK(h.singleton_callback_is_valid());

      application::handler<>::singleton_callback* singleton = 0;
      BOOST_CHECK(h.callback(singleton));
      BOOST_CHECK((*singleton)());
   }

   {
      application::handler<>::parameter_callback callback = boost::bind<bool>(
         &handler_test::parameter_handler, &app_handler_test, _1);

      application::handler<> h;
      h.callback(callback);

      BOOST_CHECK(h.parameter_callback_is_valid());

      application::handler<>::parameter_callback* parameter = 0;
      BOOST_CHECK(h.callback(parameter));
      BOOST_CHECK((*parameter)(app_context));
   }

   {
      application::handler<>::singleton_callback callback = boost::bind<bool>(
         &handler_test::singleton_handler, &app_handler_test);

      application::handler<> h;
      h.callback(callback);

      BOOST_CHECK(h.singleton_callback_is_valid());

      application::handler<>::singleton_callback* singleton = 0;
      BOOST_CHECK(h.callback(singleton));
      BOOST_CHECK((*singleton)());
   }

   return 0;
}




