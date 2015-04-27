// -----------------------------------------------------------------------------

// simple_application.cpp : examples that show how use 
// Boost.Application to make a simplest interactive (terminal) application 
//

// -----------------------------------------------------------------------------

// Copyright 2011-2014 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// -----------------------------------------------------------------------------

#define BOOST_ALL_DYN_LINK
#define BOOST_LIB_DIAGNOSTIC

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

//[intro
#include <iostream>
#include <boost/application.hpp>

using namespace boost;

/*<<Define application functor class>>*/
class myapp
{
public:

   /*<<Define the constructor that will receive a application context>>*/
   myapp(application::context& context)
      : context_(context)
   {
   }

   /*<<Define the application operator>>*/
   int operator()()
   {

      /*<<Make use of an'aspect'>>*/
      application::csbl::shared_ptr<application::args> myargs 
         = context_.find<application::args>();

      if (myargs)
      {
         const std::vector<std::string> &arg_vector = myargs->arg_vector();

         // only print args on screen
         for(std::vector<std::string>::const_iterator it = arg_vector.begin(); 
            it != arg_vector.end(); ++it) {
            std::cout << *it << std::endl;
         }
      }

      /*<<Add other application logic>>*/
      // code your application

      return 0;
   }

private:
   
   /*<<Application context to hold aspects>>*/
   application::context& context_;

};

// main

int main(int argc, char *argv[])
{  

   /*<<Create a context application aspect pool>>*/   
   application::context app_context;

   /*<<Instantiate your application>>*/    
   myapp app(app_context);

   /*<<Add an aspect for future use. An 'aspect' can be customized, or new aspects can be created>>*/  
   app_context.insert<application::args>(
      application::csbl::make_shared<application::args>(argc, argv));

   /*<<Start the application on the desired mode (common, server)>>*/  
   return application::launch<application::common>(app, app_context);
}
//]


