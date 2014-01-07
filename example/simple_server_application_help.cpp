// -----------------------------------------------------------------------------
// simple_server_application_help.cpp : help only 
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

//[simplesrvrhelp
/*<< Define a 'functor' myapp class for our application >>*/
class myapp // [[a]]
{   
public:

   // param version
   /*<< Define a application operator using 'param' signature >>*/
   int operator()(application::context& context) // [[a]]
   {
      // Do some thing

      // [[k]]
      context.find<application::wait_for_termination_request>()->wait();

      return 0;
   }

   // [[d]]
   // param version
   /*<< Optionally, define a 'stop callback' handler, using 'param' signature >>*/
   bool stop(application::context& context)
   {
      // Do some thing
      return true; // return true to stop, false to ignore
   }
};

int main(int argc, char *argv[])
{   

   /*<< Instatntiate our application >>*/
   myapp app; // [[a]]
   /*<< Create a 'context' for application that will hold our aspects >>*/
   application::context app_context; // [[b]]

   // my server aspects

   // [[c]]
   /*<< Add 'path aspect' to application context >>*/
   app_context.insert<application::path>(
      boost::make_shared<application::path_default_behaviour>(argc, argv));

   // [[c]]
   /*<< Add 'args aspect' to application context >>*/
   app_context.insert<application::args>(
      boost::make_shared<application::args>(argc, argv));

   // [[d]]
   /*<< Define a 'termination_callback' to our application >>*/
   application::handler::parameter_callback termination_callback 
      = boost::bind<bool>(&myapp::stop, &app, _1);

   // [[d]]
   /*<< Add a 'termination_callback' to our application context >>*/
   app_context.insert<application::termination_handler>(
      boost::make_shared<application::termination_handler_default_behaviour>(termination_callback));

   // [[h]]
   /*<< Launch an application using server application mode >>*/
   return application::launch<application::server>(app, app_context);
}
//]

