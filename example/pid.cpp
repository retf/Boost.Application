// -----------------------------------------------------------------------------
// path.cpp : examples that show how use 
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

using namespace boost;

//[pid
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

      boost::shared_ptr<application::process_id> pid 
         = context_.find<application::process_id>();

      std::cout << "pid : " << pid->pid()      << std::endl;


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

   return application::launch<application::common>(app, app_context);
}

//]
