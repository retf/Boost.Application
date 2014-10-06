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

//[tutorials2

class myapp
{
public:

   myapp(application::context& context)
      : context_(context)
   {
   }

   int operator()()
   {
      /*<< Retrieves 'status' aspect from your context >>*/
      boost::shared_ptr<application::status> st =
         context_.find<application::status>();

      /*<< Check 'aspect' status 'state' >>*/
      while(st->state() != application::status::stopped)
      {
	     /*<< Your application loop body >>*/
         boost::this_thread::sleep(boost::posix_time::seconds(1));
         // your application logic here!
      }

      return 0;
   }

   // check in next stage
   bool stop()
   {
      return true;
   }

private:
   application::context& context_;

};

//]

