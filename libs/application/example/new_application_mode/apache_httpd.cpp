// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_ALL_DYN_LINK
#define BOOST_LIB_DIAGNOSTIC

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <boost/application.hpp>
#include "apache_httpd_mode.hpp"

using namespace boost;

class my_apache2_httpd_content_generator_mod
{
public:

   int operator()(application::context& context) 
   {
      return 0; 
   }

   std::string get(application::context& context)
   {
      context.insert<content_type>(
         boost::make_shared<content_type>("text/html;charset=ascii"));

      std::stringstream htm;

      htm.str("");
      htm << "<html>"
          << "   <head>"
          << "      <title>Boost.Application Test</title>"
          << "   </head>"
          << ""
          << "   <body>"
          << "      <h1> Hello! </h1>"
          << "   </body>"
          << "</html>"
          ;

      return htm.str();
   }
};

// an application will be launched to handle each request that arrives.
extern "C" int myhandle(request_rec *r)
{   
   my_apache2_httpd_content_generator_mod app;

   application::context app_context;

   handler<std::string>::parameter_callback my_http_get_verb
      = boost::bind<std::string>(&my_apache2_httpd_content_generator_mod::get, &app, _1);

   app_context.insert<http_get_verb_handler>(
      boost::make_shared<
         http_get_verb_handler>(my_http_get_verb));

   return application::launch<apache2_httpd_mod>(app, *r, app_context);
}
BOOST_APPLICATION_APACHE_REGISTER_TEST_MY_MODE(myhandle)
