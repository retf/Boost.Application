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

   my_apache2_httpd_content_generator_mod(application::context& context)
      : context_(context) { }

   int operator()() 
   {
      return 0; 
   }

   std::string get()
   {
      context_.insert<content_type>(
         boost::make_shared<content_type>("text/html;charset=ascii"));

      std::stringstream htm;

      htm.str("");
      htm << "<html>"
          << "   <head>"
          << "      <title>Boost.Application Test</title>"
          << "   </head>"
          << ""
          << "   <body>"
          << "      <h1> Hello Boost.Application Version " 
          << application::library_version_string() << "!"
          << "      </h1>"
          << "      <br/>"
          << "      Apache HTTPd Mod."
          << "      <br/>"
          << "   </body>"
          << "</html>"
          ;

      boost::shared_ptr<apache_log> apachelog = context_.find<apache_log>();
      if(apachelog)
      {
         // log something on apache log file
         apachelog->information("Page requested!");
      }

      return htm.str();
   }

private:

   application::context& context_;

};

// an application will be launched to handle each request that arrives.
extern "C" int myhandle(request_rec *r)
{     
   application::context app_context;

   my_apache2_httpd_content_generator_mod app(app_context);

   app_context.insert<web_app_name>(
      boost::make_shared<web_app_name>("boostapp"));

   application::handler<std::string>::callback my_http_get_verb
      = boost::bind(
         &my_apache2_httpd_content_generator_mod::get, &app);

   app_context.insert<http_get_verb_handler>(
      boost::make_shared<
         http_get_verb_handler>(my_http_get_verb));

   return application::launch<apache2_httpd_mod>(app, *r, app_context);
}

// register request function and mod on apache server
BOOST_APPLICATION_APACHE_REGISTER_TEST_MY_MODE(myhandle, my_boost_app_mod)

/*
   On httpd.conf, add:

LoadModule my_boost_app_mod path_of_my_mod\application_mode_apache_httpd_mod.dll
<Location /boostapp>
  SetHandler boostapp
</Location>

   And start apache httpd and on your favorite navegator, access:
   http://localhost:8080/boostapp

   You will see: Hello! 

   <html>
    <head>
        <title>Boost.Application Test</title>
    </head>
    <body>
        <h1> Hello! </h1>
    </body>
   </html>

   on it!

*/
