// -----------------------------------------------------------------------------
// handling_errors_ex_version.cpp : examples that show how use 
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


#include <iostream>
#include <boost/application.hpp>

using namespace boost;

class myapp
{
public:

   // param
   int operator()(application::context& context)
   {
      return 0;
   }
};

// main

//[exver
// ...
int main(int argc, char *argv[])
{   
   try
   {
      myapp app;
      application::context app_context;

      return application::launch<application::common>(app, app_context);
   }
   catch(boost::system::system_error& se)
   {
      // error
      std::cerr << se.what() << std::endl;

      return 1;
   }
}
//]

