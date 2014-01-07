// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// This file is part of tutorial, refer to 'last stage' to see a code that 
// can be compiled.
//

#include "myfunctor_stage3.hpp"

//[tutorialcpps3

int main(int argc, char *argv[])
{   
   myapp app; 
   application::context app_context;

   /*<< create bind to stop handler >>*/
   application::handler<>::parameter_callback termination_callback 
      = boost::bind<bool>(&myapp::stop, &app, _1);

   /*<< tie stop to termination_handler using default behaviour >>*/
   app_context.insert<application::termination_handler>(
      boost::make_shared<application::termination_handler_default_behaviour>(termination_callback));

   /*<< Note that now we are using 'application::server' as template param >>*/
   return application::launch<application::server>(app, app_context);
}

//]

