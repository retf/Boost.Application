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

   /*<<Define the constructor that will receive a application context>>*/
   myapp(application::context& context)
      : context_(context)
   {
   }
   
   /*<< Define a application operator using 'param' signature >>*/
   int operator()() // [[a]]
   {
      // Do some thing

      // [[k]]
      context_.find<application::wait_for_termination_request>()->wait();

      return 0;
   }

   // [[d]]
   /*<< Optionally, define a 'stop callback' handler, using 'param' signature >>*/
   bool stop()
   {
      // Do some thing
      return true; // return true to stop, false to ignore
   }
   
private:
   
   /*<<Application context to hold aspects>>*/
   application::context& context_;
};

int main(int argc, char *argv[])
{   

   /*<< Create a local 'context' for application that will hold our aspects >>*/
   application::context app_context; // [[b]]
   
   /*<< Instatntiate our application using auto_handler, the 'stop' method will be automatically handled >>*/
   application::auto_handler<myapp> app(app_context); // [[a]]

   // my server aspects

   // [[c]]
   /*<< Add 'args aspect' to application context >>*/
   app_context.insert<application::args>(
      boost::make_shared<application::args>(argc, argv));

   // [[h]]
   /*<< Launch an application using server application mode >>*/
   return application::launch<application::server>(app, app_context);
}
//]

