// Copyright 2011-2013 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_APPLICATION_MY_APPLICATION_MODE_HPP
#define BOOST_APPLICATION_MY_APPLICATION_MODE_HPP

#include <boost/application/config.hpp>
#include <boost/application/context.hpp>
//#include <boost/application/detail/app.hpp>
#include <boost/application/detail/csbl.hpp>
#include <boost/application/application_initializers.hpp>
#include <boost/application/application_mode_register.hpp>
// internal aspects
#include <boost/application/aspects/status.hpp>
#include <boost/application/aspects/run_mode.hpp>

// Apache httpd API
#include <httpd.h>
#include <http_config.h>
#include <http_request.h>
#include <http_protocol.h>
#include <http_log.h>
#include <ap_config.h>

// Apache APR
#include <apr_dbd.h>
#include <mod_dbd.h> 
#include <apr_strings.h>
#include <apr_hash.h>  

using namespace boost::application;

class apache2_httpd_mod;

//
// apache2_httpd_mod aspects
//

class http_get_verb_handler : public handler<std::string>
{
public:
   http_get_verb_handler(const parameter_callback& callback)
      : handler<std::string>(callback) {}

   http_get_verb_handler(const singleton_callback& callback)
      : handler<std::string>(callback) {}
};

class content_type
{
   friend class apache2_httpd_mod;

public:
   content_type(const std::string& my_content_type)
      : content_type_ (my_content_type)
   {}

private:
   std::string content_type_;
};

//
// apache2_httpd_mod application mode
//

class apache2_httpd_mod
{

public:

   static int mode()
   {
      static int id = new_run_mode<int>();
      return id;
   }

   template <typename Application, typename CustomType>
   apache2_httpd_mod(Application& myapp, CustomType &ct, context &cxt, boost::system::error_code& ec)
    
   {
      handle_request(myapp, ct, cxt);
   }

   template <typename Application, typename CustomType>
   apache2_httpd_mod(Application& myapp, CustomType &ct, boost::singularity<context> &cxt, boost::system::error_code& ec)
   {
      handle_request(myapp, ct, cxt.get_global());
   }

   int run() { return OK; }

protected:

   template <typename Application, typename CustomType>
   void handle_request(Application& myapp, CustomType &ct, context &cxt)
   {
      // default impl aspects

      if(!cxt.find<run_mode>())
      {
         cxt.insert<run_mode>(
            csbl::make_shared<run_mode>(mode()));
      }

      if(!cxt.find<status>())
      {
         cxt.insert<status>(
            csbl::make_shared<status>(status::running));
      }

      // GET

      csbl::shared_ptr<http_get_verb_handler> http_get_verb =
         cxt.find<http_get_verb_handler>();
      
      if(http_get_verb)
      {
         csbl::shared_ptr<content_type> contenttype = 
            cxt.find<content_type>();

         if(contenttype)
            ap_set_content_type(&ct, contenttype->content_type_.c_str());
         else
            ap_set_content_type(&ct, "text/html;charset=ascii");

         // check if we have any callback to call

         handler<std::string>::parameter_callback* parameter = 0;

         if(http_get_verb->callback(parameter))
         {
            ap_rputs((*parameter)(cxt).c_str(), &ct);
         }

         handler<std::string>::singleton_callback* singleton = 0;

         if(http_get_verb->callback(singleton))
         {
            ap_rputs((*singleton)().c_str(), &ct);
         }
      }

      // Add other http verbs 
      // TODO...
   }

};

#define BOOST_APPLICATION_APACHE_REGISTER_TEST_MY_MODE(h)                      \
extern "C"                                                                     \
void boost_application_register_hooks(apr_pool_t *p)                           \
{	                                                                            \
   ap_hook_handler(h, NULL, NULL, APR_HOOK_MIDDLE);                            \
}                                                                              \
                                                                               \
module AP_MODULE_DECLARE_DATA md = {                                           \
    STANDARD20_MODULE_STUFF,                                                   \
    NULL,                                                                      \
    NULL,                                                                      \
    NULL,                                                                      \
    NULL,                                                                      \
    NULL,                                                                      \
    boost_application_register_hooks                                           \
};

#endif // BOOST_APPLICATION_MY_APPLICATION_MODE_HPP
