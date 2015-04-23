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
   /*<< Application Context >>*/
   application::context app_context;   
   
   /*<< Tie stop, pause, resume handlers using default behaviour >>*/
   application::auto_handler<myapp> app(app_context);
   
   /*<< Path manipulation aspect, to be used to get executable module path to use in log >>*/
   app_context.insert<application::path>(
      boost::make_shared<application::path>());

   /*<< Arg manipulation aspect >>*/
   app_context.insert<application::args>(
      boost::make_shared<application::args>(argc, argv));

   /*<< Note that now we are using 'application::server' as template param >>*/
   return application::launch<application::server>(app, app_context);
}

//]

