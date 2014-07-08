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

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <boost/application.hpp>

using namespace boost;

//[mownwfr
class myapp
{
public:

   myapp(application::context& context)
      : context_(context)
   {
   }

   // param
   int operator()()
   {
      std::cout << "Test" << std::endl;
	  
	  /*<< Use your custon handler >>*/
      context_.find<application::wait_for_termination_request>()->wait();

      return 0;
   }

private:

   application::context& context_;
};

// my made by hand behaviour
class wait_for_termination_request_my_behaviour 
   /*<< Inheriting of 'wait_for_termination_request' handler contract abstract class >>*/ 
   : public application::wait_for_termination_request
{
public:

   /*<< Define your desired operation for 'wait' method >>*/ 
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
   application::context app_context;   
   myapp app(app_context);

   app_context.insert<application::args>(
      boost::make_shared<application::args>(argc, argv));
 
   // if user do this, the default behavoiur will be ignored, 
   // and the user behaviour will be executed by application::server
   /*<< Add your custon handler to context aspect pool of application >>*/ 
   app_context.insert< application::wait_for_termination_request>(
      shared_ptr<application::wait_for_termination_request>(
         new wait_for_termination_request_my_behaviour));

   return application::launch<application::common>(app, app_context);
}
//]