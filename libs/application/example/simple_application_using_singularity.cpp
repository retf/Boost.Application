// -----------------------------------------------------------------------------
// simple_application_using_singularity.cpp : examples that show how use 
// Boost.Application to make a simplest interactive (terminal) application 
// using Boost.singularity
//
// Note 1: The Boost.Application (Aspects v4) and this sample are in 
//         development process.
//
// Note 2: The Boost.Singularity is in approval process, 
//         refer boost.org to know more.
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// -----------------------------------------------------------------------------

#define BOOST_ALL_DYN_LINK
#define BOOST_LIB_DIAGNOSTIC

#include <iostream>
#include <boost/application.hpp>

using namespace boost;

// singularity access 

boost::singularity<application::context> global_context;
inline application::context& this_application() {
   return global_context.get_global();
}

// my functor application

class myapp
{
public:

   // singularity, no param
   int operator()()
   {
      std::cout << "Test" << std::endl;
      std::shared_ptr<application::args> myargs 
         = this_application().get_aspect<application::args>();

      if (myargs)
      {
         std::vector<std::string> arg_vector = myargs->arg_vector();

         // only print args on screen
         for(std::vector<std::string>::iterator it = arg_vector.begin(); 
            it != arg_vector.end(); ++it) {
            std::cout << *it << std::endl;
         }
      }

      return 0;
   }

};

// main

int main(int argc, char *argv[])
{   
   myapp app;
   int ret =  0
 
   boost::singularity<application::context>::create_global();

   this_application().add_aspect<application::args>(
      std::make_shared<application::args>(argc, argv));

   ret = application::launch<application::server>(app, global_context);

   boost::singularity<application::context>::destroy();

   return ret;
}
