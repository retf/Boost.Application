// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

// #define BOOST_ALL_DYN_LINK
// #define BOOST_LIB_DIAGNOSTIC

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <boost/application.hpp>
#include <boost/test/minimal.hpp>
#include <boost/uuid/string_generator.hpp>

using namespace boost;

boost::singularity<application::context> global_context;
inline application::context& this_application() {
   return global_context.get_global();
}

class myapp
{
public:

   bool instace_aready_running_param_false(application::context &context)
   {
      return false;
   }

   bool instace_aready_running_param_true(application::context &context)
   {
      return true;
   }

   bool instace_aready_running_false()
   {
      return false;
   }

   bool instace_aready_running_true()
   {
      return true;
   }

};

int test_main(int argc, char** argv)
{   
   // test no ensure_single_instance
   {
      myapp app; application::context cxt; boost::system::error_code ec;

      BOOST_CHECK(!application::detail::ensure_single_instance<application::context>()(cxt, ec)); 
      BOOST_CHECK(!ec);
   }
	
   // test ensure_single_instance instace_aready_running_param_false
   {
      myapp app; application::context cxt; boost::system::error_code ec;

      boost::uuids::string_generator gen;
      boost::uuids::uuid appuuid = gen("{8F66E4AD-ECA5-475D-8784-4BAA329EF9F2}");

      application::handler<>::parameter_callback callback 
         = boost::bind<bool>(&myapp::instace_aready_running_param_false, &app, _1);

      cxt.insert<application::limit_single_instance>(
         boost::make_shared<application::limit_single_instance_default_behaviour>(appuuid, callback));

      BOOST_CHECK(!application::detail::ensure_single_instance<application::context>()(cxt, ec)); 
      BOOST_CHECK(!ec);
      BOOST_CHECK(application::detail::ensure_single_instance<application::context>()(cxt, ec)); 
      BOOST_CHECK(!ec);
   }
	
   // test ensure_single_instance instace_aready_running_param_true
   {
      myapp app; application::context cxt; boost::system::error_code ec;

      boost::uuids::string_generator gen;
      boost::uuids::uuid appuuid = gen("{7F66E4AD-ECA5-475D-8784-4BAA329EF9F2}");

      application::handler<>::parameter_callback callback 
         = boost::bind<bool>(&myapp::instace_aready_running_param_true, &app, _1);

      cxt.insert<application::limit_single_instance>(
         boost::make_shared<application::limit_single_instance_default_behaviour>(appuuid, callback));

      BOOST_CHECK(!application::detail::ensure_single_instance<application::context>()(cxt, ec)); 
      BOOST_CHECK(!ec);
      BOOST_CHECK(!application::detail::ensure_single_instance<application::context>()(cxt, ec)); 
      BOOST_CHECK(!ec);
   }
	
   // test ensure_single_instance instace_aready_running_false
   {
      myapp app; boost::system::error_code ec;

      boost::uuids::string_generator gen;
      boost::uuids::uuid appuuid = gen("{6F66E4AD-ECA5-475D-8784-4BAA329EF9F2}");

      boost::singularity<application::context>::create_global();

      application::handler<>::singleton_callback callback 
         = boost::bind<bool>(&myapp::instace_aready_running_false, &app);

      this_application().insert<application::limit_single_instance>(
         boost::make_shared<application::limit_single_instance_default_behaviour>(appuuid, callback));

      BOOST_CHECK(!application::detail::ensure_single_instance<singularity<application::context> >()(global_context, ec)); 
      BOOST_CHECK(!ec);
      BOOST_CHECK(application::detail::ensure_single_instance<singularity<application::context> >()(global_context, ec)); 
      BOOST_CHECK(!ec);

      boost::singularity<application::context>::destroy();
   }

   // test ensure_single_instance instace_aready_running_true
   {
      myapp app; boost::system::error_code ec;

      boost::uuids::string_generator gen;
      boost::uuids::uuid appuuid = gen("{5F66E4AD-ECA5-475D-8784-4BAA329EF9F2}");

      boost::singularity<application::context>::create_global();

      application::handler<>::singleton_callback callback 
         = boost::bind<bool>(&myapp::instace_aready_running_true, &app);

      this_application().insert<application::limit_single_instance>(
         boost::make_shared<application::limit_single_instance_default_behaviour>(appuuid, callback));

      BOOST_CHECK(!application::detail::ensure_single_instance<singularity<application::context> >()(global_context, ec)); 
      BOOST_CHECK(!ec);
      BOOST_CHECK(!application::detail::ensure_single_instance<singularity<application::context> >()(global_context, ec)); 
      BOOST_CHECK(!ec);

      boost::singularity<application::context>::destroy();
   }




   return 0;
}



