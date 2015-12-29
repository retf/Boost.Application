// -----------------------------------------------------------------------------
// handling_errors_ec_version.cpp : examples that show how use 
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

//[ecver
// ...
int main(int argc, char *argv[])
{   
   myapp app;
   application::context app_context;

   boost::system::error_code ec;
   int result = application::launch<application::common>(app, app_context, ec);

   if(ec)
   {
      std::cerr << "[E] " << ec.message() 
         << " <" << ec.value() << "> " << std::cout;
   }

   return result;
}
//]

