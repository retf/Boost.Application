// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// This file is part of tutorial, this is the 'last stage'.
//

#include "myfunctor_stage4.hpp"

//[tutorialcpps4

int main(int argc, char *argv[])
{   
   /*<< Functor user Class >>*/
   myapp app; 

   /*<< Application Context >>*/
   application::context app_context;
   
   /*<< Path manipulation aspect, to be used to get executable module path to use in log >>*/
   app_context.insert<application::path>(
      boost::make_shared<application::path_default_behaviour>(argc, argv));

   /*<< Arg manipulation aspect >>*/
   app_context.insert<application::args>(
      boost::make_shared<application::args>(argc, argv));

   /*<< Termination handler tie >>*/
   application::handler<>::parameter_callback termination_callback 
      = boost::bind<bool>(&myapp::stop, &app, _1);

   app_context.insert<application::termination_handler>(
      boost::make_shared<application::termination_handler_default_behaviour>(termination_callback));

   /*<< Pause handler tie  >>*/
   application::handler<>::parameter_callback pause_callback 
      = boost::bind<bool>(&myapp::pause, &app, _1);

   app_context.insert<application::pause_handler>(
      boost::make_shared<application::pause_handler_default_behaviour>(pause_callback));

   /*<< Resume handler tie  >>*/
   application::handler<>::parameter_callback resume_callback 
      = boost::bind<bool>(&myapp::resume, &app, _1);

   app_context.insert<application::resume_handler>(
      boost::make_shared<application::resume_handler_default_behaviour>(resume_callback));

   /*<< Note that now we are using 'application::server' as template param >>*/
   return application::launch<application::server>(app, app_context);
}

//]

