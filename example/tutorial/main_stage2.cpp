// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// This file is part of tutorial, refer to 'last stage' to see a code that 
// can be compiled.
//

#include "myfunctor_stage2.hpp"

//[tutorialcpps2

int main(int argc, char *argv[])
{   
   application::context app_context;
   
   /*<< Tie stop to termination_handler using default behaviour >>*/
   application::auto_handler<myapp> app(app_context);

   /*<< Starts the application as a common application type. >>*/
   return application::launch<application::common>(app, app_context);
}

//]

