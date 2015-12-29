// Copyright 2011-2014 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <boost/application.hpp>
#include <boost/test/minimal.hpp>
#include <boost/uuid/string_generator.hpp>

using namespace boost;

inline application::global_context_ptr this_application() {
   return application::global_context::get();
}

class myapp
{
public:

   bool instace_aready_running_false()
   {
      return false;
   }

   bool instace_aready_running_true()
   {
      return true;
   }
   
};

class myapp2
{
public:

   myapp2(application::context& context)
      : context_(context)
   {
   }
   
   bool instace_aready_running_false()
   {
      return false;
   }

   bool instace_aready_running_true()
   {
      return true;
   }
   
private:
   application::context& context_;
   
};

int test_main(int argc, char** argv)
{   
   // test no ensure_single_instance
   {
      application::context cxt; boost::system::error_code ec;

      BOOST_CHECK(!application::detail::ensure_single_instance<application::context>()(cxt, ec)); 
      BOOST_CHECK(!ec);
   }
	
   // test ensure_single_instance instace_aready_running_param_false
   {
      application::context cxt; boost::system::error_code ec;
      myapp2 app(cxt); 

      boost::uuids::string_generator gen;
      boost::uuids::uuid appuuid = gen("{8F66E4AD-ECA5-475D-8784-4BAA329EF9F2}");

      application::handler<>::callback cb 
         = boost::bind(&myapp2::instace_aready_running_false, &app);

      cxt.insert<application::limit_single_instance>(
         boost::make_shared<application::limit_single_instance_default_behaviour>(appuuid, cb));

      BOOST_CHECK(!application::detail::ensure_single_instance<application::context>()(cxt, ec)); 
      BOOST_CHECK(!ec);
      BOOST_CHECK(application::detail::ensure_single_instance<application::context>()(cxt, ec)); 
      BOOST_CHECK(!ec);
   }
	
   // test ensure_single_instance instace_aready_running_param_true
   {
      application::context cxt; boost::system::error_code ec;
      myapp2 app(cxt); 

      boost::uuids::string_generator gen;
      boost::uuids::uuid appuuid = gen("{7F66E4AD-ECA5-475D-8784-4BAA329EF9F2}");

      application::handler<>::callback cb 
         = boost::bind(&myapp2::instace_aready_running_true, &app);

      cxt.insert<application::limit_single_instance>(
         boost::make_shared<application::limit_single_instance_default_behaviour>(appuuid, cb));

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

      application::global_context::create();

      application::handler<>::callback cb 
         = boost::bind(&myapp::instace_aready_running_false, &app);

      this_application()->insert<application::limit_single_instance>(
         boost::make_shared<application::limit_single_instance_default_behaviour>(appuuid, cb));

      BOOST_CHECK(!application::detail::ensure_single_instance<application::global_context_ptr>()(this_application(), ec));
      BOOST_CHECK(!ec);
      BOOST_CHECK(application::detail::ensure_single_instance<application::global_context_ptr>()(this_application(), ec));
      BOOST_CHECK(!ec);

      application::global_context::destroy();
   }

   // test ensure_single_instance instace_aready_running_true
   {
      myapp app; boost::system::error_code ec;

      boost::uuids::string_generator gen;
      boost::uuids::uuid appuuid = gen("{5F66E4AD-ECA5-475D-8784-4BAA329EF9F2}");

      application::global_context::create();

      application::handler<>::callback cb 
         = boost::bind(&myapp::instace_aready_running_true, &app);

      this_application()->insert<application::limit_single_instance>(
         boost::make_shared<application::limit_single_instance_default_behaviour>(appuuid, cb));

      BOOST_CHECK(!application::detail::ensure_single_instance<application::global_context_ptr>()(this_application(), ec));
      BOOST_CHECK(!ec);
      BOOST_CHECK(!application::detail::ensure_single_instance<application::global_context_ptr>()(this_application(), ec));
      BOOST_CHECK(!ec);

      application::global_context::destroy();
   }

   return 0;
}



