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

#define BOOST_ALL_DYN_LINK
#define BOOST_LIB_DIAGNOSTIC

#include <iostream>
#include <boost/application.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

using namespace boost::application;

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
   int operator()(context& context)
   {
      std::cout << "operator()" << std::endl;
	  
      // launch a work thread
      boost::thread thread(boost::bind(&myapp::work_thread, this));
	  
      context.use_aspect<wait_for_termination_request>().wait();

      return 0;
   }

};

// my common class

class mycommon : public common
{
public:

   template <typename Application>
   mycommon(Application& app, context &context, boost::system::error_code& ec)
      : common(app, context, ec)
   {
      handler::parameter_callback callback 
               = boost::bind<bool>(&mycommon::stop, this, _1);

      // define my own signal / handler

      tie_signal(SIGINT,  callback);
      tie_signal(SIGTERM, callback);
   }

   bool stop(context &context)
   {
      std::cout << "exiting..." << std::endl;

      std::shared_ptr<wait_for_termination_request> th 
         = context.get_aspect<wait_for_termination_request>();

      th->proceed();

      return false;
   }

};

// main

int main(int argc, char *argv[])
{   
   myapp app;
   context app_context;

   return launch<mycommon>(app, app_context);
}

