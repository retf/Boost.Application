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

//[tutorials1

/*<< Your application functor class >>*/
class myapp
{
public:

   myapp(application::context& context)
      : context_(context)
   {
   }

   /*<< Application operator, this is like a 'main' function >>*/
   int operator()()
   {
      // your application logic here!
      return 0;
   }

private:
   application::context& context_;

};

//]

