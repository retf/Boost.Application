// -----------------------------------------------------------------------------
// limit_single_instance_callback_with_global_context.cpp :
//examples that show how use Boost.Application to make a simplest interactive 
// (terminal) application 
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

using namespace boost::application;

// singleton access

inline global_context_ptr this_application() {
   return global_context::get();
}

// my functor application

class myapp
{
public:

   // singleton, no param
   int operator()()
   {
      std::cout << "Test" << std::endl;
      boost::shared_ptr<args> myargs 
         = this_application()->find<args>();

      if (myargs)
      {
         const std::vector<std::string>& arg_vector = myargs->arg_vector();

         // only print args on screen
         for(std::vector<std::string>::const_iterator it = arg_vector.begin(); 
            it != arg_vector.end(); ++it) {
            std::cout << *it << std::endl;
         }
      }

      this_application()->find<wait_for_termination_request>()->wait();

      return 0;
   }

   bool instace_aready_running(void)
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
   
   boost::uuids::string_generator gen;
   boost::uuids::uuid appuuid = gen("{9F63E4AD-ECA5-475D-8784-4BAA329EF9F3}");
 
   global_context_ptr ctx = global_context::create();
   
   // way 1
   /*
   handler<>::callback cb 
      = boost::bind(&myapp::instace_aready_running, &app);
   
   // use aspects

   this_application()->insert<args>(
      boost::make_shared<args>(argc, argv));

   this_application()->insert<limit_single_instance>(
      boost::make_shared<limit_single_instance_default_behaviour>(appuuid, cb));
   */

   // way 2
   this_application()->insert<limit_single_instance>(
      boost::make_shared<limit_single_instance_default_behaviour>(appuuid, 
         handler<bool>::make_callback(app, &myapp::instace_aready_running)));

   int ret = launch<common>(app, ctx);

   global_context::destroy();

   return ret;
}

