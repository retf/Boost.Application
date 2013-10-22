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

using namespace boost;

class myapp
{
public:

   // param
   int operator()(application::context& context)
   {
      
      if(context.has_aspect<application::args>())
      {
         std::vector<std::string> arg_vector = 
            context.use_aspect<application::args>().arg_vector();

         // only print args on screen
         for(std::vector<std::string>::iterator it = arg_vector.begin(); 
            it != arg_vector.end(); ++it) {
            std::cout << *it << std::endl;
         }
      }

      context.use_aspect<application::wait_for_termination_request>().wait();

      return 0;
   }
};

// main

int main(int argc, char *argv[])
{   
   myapp app;
   application::context app_context;

   boost::uuids::string_generator gen;

   app_context.add_aspect<application::limit_single_instance>(
      std::make_shared<application::limit_single_instance_default_behaviour>(
         gen("{9F66E4AD-ECA5-475D-8784-4BAA329EF9F2}")));

   return application::launch<application::common>(app, app_context);
}
