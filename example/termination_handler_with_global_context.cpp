// -----------------------------------------------------------------------------
// limit_single_instance_callback.cpp : examples that show how use
// Boost.Application to make a simplest interactive (terminal) application
//
// Note 1: The Boost.Application (Aspects v4) and this sample are in
//         development process.
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// -----------------------------------------------------------------------------

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <boost/application.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

using namespace boost;


// singleton access

inline application::global_context_ptr this_application() {
   return application::global_context::get();
}


class myapp
{
public:

   ~myapp()
   {
      std::cout << "~myapp()" << std::endl;
   }

   void work_thread()
   {
      while(1)
      {
         boost::this_thread::sleep(boost::posix_time::seconds(2));
         std::cout << "running" << std::endl;
      }
   }

   // param
   int operator()()
   {
      std::cout << "operator()" << std::endl;

	  // using [state]

      /*
      std::shared_ptr<status> state =
         context.get_aspect<status>();

      while(state->state() != status::stopped)
      {
         boost::this_thread::sleep(boost::posix_time::seconds(2));
         std::cout << "running" << std::endl;
      }
      */

      // or using [wait_for_termination_request]

      // launch a work thread
      boost::thread thread(boost::bind(&myapp::work_thread, this));

      this_application()->find<application::wait_for_termination_request>()->wait();

      return 0;
   }

   bool stop()
   {
      char type;
      do
      {
         std::cout << "Do you want to exit? [y/n]" << std::endl;
         std::cin >> type;
      }
      while( !std::cin.fail() && type!='y' && type!='n' );

      if(type == 'y')
          // tell to app to continue.
         return true;

      // tell to app to exit.
      return false;
   }

};

// main

int main(int argc, char *argv[])
{
   myapp app;

   application::global_context_ptr ctx = application::global_context::create();

   application::handler<>::callback callback
      = boost::bind(&myapp::stop, &app);

   this_application()->insert<application::termination_handler>(
      boost::make_shared<application::termination_handler_default_behaviour>(callback));

   int ret =  application::launch<application::common>(app, ctx);

   application::global_context::destroy();

   return ret;
}
