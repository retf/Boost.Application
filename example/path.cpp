// -----------------------------------------------------------------------------
// path.cpp : examples that show how use 
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

#define BOOST_ALL_DYN_LINK
#define BOOST_LIB_DIAGNOSTIC

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <boost/application.hpp>

using namespace boost;

//[path
class myapp
{
public:

   myapp(application::context& context)
      : context_(context)
   {
   }

   // param
   int operator()()
   {
      std::cout << "Test" << std::endl;

	    /*<< Use 'path' aspect on your logic >>*/ 
      boost::shared_ptr<application::path> path 
         = context_.find<application::path>();

      std::cout << "executable_path      : " << path->executable_path()      << std::endl;
      std::cout << "current_path         : " << path->current_path()         << std::endl;
      std::cout << "executable_name      : " << path->executable_name()      << std::endl;
      std::cout << "executable_full_name : " << path->executable_full_name() << std::endl;
      std::cout << "executable_path_name : " << path->executable_path_name() << std::endl;

      // e.g.: "executable_path      : E:\project.boost.app.v4\libs\application\vc11ide\Debug"
      // e.g.: "current_path         : E:\project.boost.app.v4\libs\application\vc11ide"
      // e.g.: "executable_name      : path"
      // e.g.: "executable_full_name : path.exe"
      // e.g.: "executable_path_name : E:\project.boost.app.v4\libs\application\vc11ide\Debug\path.exe"

      return 0;
   }

private:
   application::context& context_;

};

// main

int main(int argc, char *argv[])
{      
   application::context app_context;
   myapp app(app_context);

   /*<< Add 'path' aspect to context pool of application, it will be available for future use >>*/ 
   app_context.insert<application::path>(
     boost::make_shared<application::path_default_behaviour>(argc, argv));

   return application::launch<application::common>(app, app_context);
}

//]
