// -----------------------------------------------------------------------------
// my_own_wait_for_termination_request.cpp : examples that show how use 
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

using namespace boost;

class myapp
{
public:

   // param
   int operator()(application::context& context)
   {
      std::cout << "Test" << std::endl;
	  
	   context.use_aspect<application::wait_for_termination_request>().wait();

      return 0;
   }
};

// my made by hand behaviour
class wait_for_termination_request_my_behaviour 
   : public application::wait_for_termination_request
{
public:

   void wait(){
      char type;
      do
      {
         std::cout << "Exit? [y/n]" << std::endl;
         std::cin >> type;
      }
      while( !std::cin.fail() && type!='y' );
   }

   void proceed(){
      // do nothing
   }
};

// main

int main(int argc, char *argv[])
{   
   myapp app;
   application::context app_context;

   app_context.add_aspect<application::args>(
      std::make_shared<application::args>(argc, argv));
 
   // if user do this, the default behavoiur will be ignored, 
   // and the user behaviour will be executed by application::server
   app_context.add_aspect< application::wait_for_termination_request>(
      std::shared_ptr<application::wait_for_termination_request>(
         new wait_for_termination_request_my_behaviour));

   return application::launch<application::common>(app, app_context);
}
