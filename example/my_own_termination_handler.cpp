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

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <fstream>

#include <boost/application.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

using namespace boost::application;

//[myownsig
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
	  
      context.find<wait_for_termination_request>()->wait();

      return 0;
   }

};

/*<< Inheriting of signal_manager >>*/
class my_signal_manager : public signal_manager
{
public:

   /*<< Customize SIGNALS bind >>*/
   my_signal_manager(context &context)
      : signal_manager(context)
   {
      handler<>::parameter_callback callback 
         = boost::bind<bool>(&my_signal_manager::stop, this, _1);

      // define my own signal / handler
#if defined( BOOST_WINDOWS_API )
      bind(SIGINT,  callback); // CTRL-C (2)
#elif defined( BOOST_POSIX_API )      
      /*<< Define signal bind >>*/
      bind(SIGUSR2, callback); 
#endif

   }

   /*<< Define signal callback >>*/
   bool stop(context &context)
   {
      BOOST_APPLICATION_FEATURE_SELECT

#if defined( BOOST_WINDOWS_API )
      std::cout << "exiting..." << std::endl;
#elif defined( BOOST_POSIX_API )
      std::ofstream my_log_file;
      my_log_file.open((context.find<
         path>()->executable_path().string() + "/log_stop.txt").c_str());
      my_log_file << ":0)-" << std::endl;
      my_log_file.close();
#endif

      shared_ptr<wait_for_termination_request> th 
         = context.find<wait_for_termination_request>();

      th->proceed();

      return false;
   }
};

// main

int main(int argc, char *argv[])
{   
   myapp app;
   context app_context;

   app_context.insert<path>(
      boost::make_shared<path_default_behaviour>(argc, argv));

   // we will customize our signals behaviour
   /*<< Instantiate your custon signal manager. >>*/
   my_signal_manager sm(app_context);
   
#if defined( BOOST_WINDOWS_API )
   /*<< Pass 'custon signal manager (sm)' to launch function. >>*/
   return launch<common>(app, sm, app_context);
#elif defined( BOOST_POSIX_API )
   return launch<server>(app, sm, app_context);
#endif

}
//]
