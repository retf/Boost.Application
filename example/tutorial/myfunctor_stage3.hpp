// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// This file is part of tutorial, refer to 'last stage' to see a code that 
// can be compiled.
//

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <boost/application.hpp>

using namespace boost;

//[tutorials3

class myapp
{
public:
   
   myapp(application::context& context)
      : context_(context)
   {
   }

   int operator()()
   {
      context_.find<application::wait_for_termination_request>()->wait();
      return 0;
   }

   /*<< Define your 'stop' handler that will be called when the 'stop' event will be fired. >>*/
   bool stop()
   {
      std::cout << "Stoping my application..." << std::endl;
	  
      /*<< return true to stop, false to ignore >>*/
      return true; 
   }

private:
   application::context& context_;

};

//]

