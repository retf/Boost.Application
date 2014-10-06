// Copyright 2011-2013 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_APPLICATION_MY_APPLICATION_MODE_HPP
#define BOOST_APPLICATION_MY_APPLICATION_MODE_HPP

#include <boost/application/config.hpp>
#include <boost/application/context.hpp>
#include <boost/application/detail/csbl.hpp>
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
   http_get_verb_handler(const handler<std::string>::callback& cb)
      : handler<std::string>(cb) {}
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

class web_app_name
{
   friend class apache2_httpd_mod;

public:
   web_app_name(const std::string& web_app_name)
      : web_app_name_ (web_app_name)
   {}

private:
   std::string web_app_name_;
};

class apache_log
{
   friend class apache2_httpd_mod;

public:
   apache_log(request_rec *r)
      : r_(r) { }

   void error(const std::string& msg)
   {
      ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_ERR, 0, r_->server, msg.c_str());
   }

   void information(const std::string& msg)
   {
      ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_INFO, 0, r_->server, msg.c_str());
   }

private:
   request_rec *r_;

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

   template <typename Application, typename RequestRec>
   apache2_httpd_mod(Application& myapp, RequestRec &rr,
      context& cxt, boost::system::error_code& ec)
      : error_(OK)
   {
      handle_request(myapp, rr, cxt);
   }

   int run() { return error_; }

protected:

   template <typename Application, typename RequestRec>
   void handle_request(Application& myapp, RequestRec &rr, context &cxt)
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

      csbl::shared_ptr<web_app_name> appname = cxt.find<web_app_name>();

      if(!appname)
      {
         error_ = DECLINED; return;
      }

      if (strcmp(rr.handler, appname->web_app_name_.c_str()))
      {
         error_ = DECLINED; return;
      }

      // we allow only GET

      // Add other http verbs
      // ...

      if(rr.method_number != M_GET)
      {
         error_ = HTTP_METHOD_NOT_ALLOWED; return;
      }

      // GET

      csbl::shared_ptr<http_get_verb_handler> http_get_verb =
         cxt.find<http_get_verb_handler>();

      if(http_get_verb)
      {
         // apache log
         cxt.insert<apache_log>(csbl::make_shared<apache_log>(&rr));

         csbl::shared_ptr<content_type> contenttype =
            cxt.find<content_type>();

         if(contenttype)
            ap_set_content_type(&rr, contenttype->content_type_.c_str());
         else
            ap_set_content_type(&rr, "text/html;charset=ascii");

         // check if we have any callback to call

         handler<std::string>::callback* cb = 0;

         if(http_get_verb->get(cb))
         {
            ap_rputs((*cb)().c_str(), &rr); return;
         }
      }

      // we need set application_state to stop
      cxt.find<status>()->state(status::stopped);

      // we cant find any handler, generate apache error
      error_ = HTTP_INTERNAL_SERVER_ERROR;
   }

private:

   int error_;

};

#define BOOST_APPLICATION_APACHE_REGISTER_TEST_MY_MODE(h, m)                   \
extern "C" {                                                                   \
void boost_application_register_hooks(apr_pool_t *p)                           \
{                                                                              \
   ap_hook_handler(h, NULL, NULL, APR_HOOK_MIDDLE);                            \
}                                                                              \
                                                                               \
module AP_MODULE_DECLARE_DATA m = {                                            \
    STANDARD20_MODULE_STUFF,                                                   \
    NULL,                                                                      \
    NULL,                                                                      \
    NULL,                                                                      \
    NULL,                                                                      \
    NULL,                                                                      \
    boost_application_register_hooks                                           \
}; }

#endif // BOOST_APPLICATION_MY_APPLICATION_MODE_HPP

