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

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <boost/application.hpp>
#include <boost/uuid/string_generator.hpp>

using namespace boost;

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
      boost::shared_ptr<application::args> args =
         context_.find<application::args>();

      if(args)
      {
         const std::vector<std::string> &arg_vector = args->arg_vector();

         // only print args on screen
         for(std::vector<std::string>::const_iterator it = arg_vector.begin(); 
            it != arg_vector.end(); ++it) {
            std::cout << *it << std::endl;
         }
      }

      context_.find<application::wait_for_termination_request>()->wait();

      return 0;
   }

private:

   application::context& context_;
};

// main

int main(int argc, char *argv[])
{   
   application::context app_context;
   myapp app(app_context);

   boost::uuids::string_generator gen;

   app_context.insert<application::limit_single_instance>(
      boost::make_shared<application::limit_single_instance_default_behaviour>(
         gen("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}")));

   return application::launch<application::common>(app, app_context);
}
