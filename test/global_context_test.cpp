// Copyright 2014 Benjamin Dieckmann
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#define BOOST_TEST_MODULE global_context_test

#include <iostream>
#include <boost/application.hpp>
#include <boost/test/unit_test.hpp>

using namespace boost;

BOOST_AUTO_TEST_CASE(create_global_context)
{
   application::global_context_ptr ctx =
           application::global_context::create();
   BOOST_CHECK(ctx.get()!=0);

   application::global_context::destroy();
}

BOOST_AUTO_TEST_CASE(get_global_context)
{
   application::global_context_ptr ctx =
           application::global_context::create();
   BOOST_CHECK(application::global_context::get());

   application::global_context::destroy();
}

BOOST_AUTO_TEST_CASE(destroy_global_context)
{
   application::global_context_ptr ctx =
           application::global_context::create();

   application::global_context::destroy();
}

BOOST_AUTO_TEST_CASE(throw_create_global_context_twice)
{
   application::global_context_ptr ctx = application::global_context::create();

   BOOST_CHECK_THROW(application::global_context::create(), boost::system::system_error);

   application::global_context::destroy();
}

BOOST_AUTO_TEST_CASE(throw_destroy_global_context_twice)
{
   application::global_context_ptr ctx =
           application::global_context::create();

   application::global_context::destroy();
   BOOST_CHECK_THROW(application::global_context::destroy(), boost::system::system_error);
}

BOOST_AUTO_TEST_CASE(throw_after_destroy_global_context)
{
   application::global_context_ptr ctx =
           application::global_context::create();

   application::global_context::destroy();
   BOOST_CHECK_THROW(application::global_context::get(), boost::system::system_error);
}

BOOST_AUTO_TEST_CASE(throw_before_create_global_context)
{
   BOOST_CHECK_THROW(application::global_context::get(), boost::system::system_error);
}












