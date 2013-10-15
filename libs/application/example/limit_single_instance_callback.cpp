// -----------------------------------------------------------------------------
// simple_application.cpp : examples that show how use 
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

using namespace boost::application;

class myapp
{
public:

   // param
   int operator()(context& context)
   {
      
      if( context.has_aspect<args>())
      {
         std::vector<std::string> arg_vector = 
            context.use_aspect<args>().arg_vector();

         // only print args on screen
         for(std::vector<std::string>::iterator it = arg_vector.begin(); 
            it != arg_vector.end(); ++it) {
            std::cout << *it << std::endl;
         }
      }

      context.use_aspect<wait_for_termination_request>().wait();

      return 0;
   }

   bool instace_aready_running(context &context)
   {
      char type;
      do
      {
         std::cout << "An instance of this application is already running on "
            "this operating system!" << std::endl;
         std::cout << "Do you want to start another? [y/n]" << std::endl;
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
   context app_context;

   boost::uuids::string_generator gen;
   boost::uuids::uuid appuuid = gen("{9F66E4AD-ECA5-475D-8784-4BAA329EF9F2}");

   limit_single_instance::cxparameter_cb callback 
      = boost::bind<bool>(&myapp::instace_aready_running, &app, _1);

   app_context.add_aspect<limit_single_instance>(
      std::make_shared<limit_single_instance>(appuuid, callback));

   return launch<common>(app, app_context);
}
