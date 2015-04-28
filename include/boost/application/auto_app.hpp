// auto_handler.hpp  ---------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2014 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 05-06-2014 dd-mm-yyyy - Initial Release
//
//
// -----------------------------------------------------------------------------

//
// *****************************************************************************
// note that this is a experimental module 
// *****************************************************************************
//

#ifndef BOOST_APPLICATION_AUTO_APP_HPP
#define BOOST_APPLICATION_AUTO_APP_HPP

// application
#include <boost/application/config.hpp>
#include <boost/application/detail/csbl.hpp>
#include <boost/application/context.hpp>
#include <boost/application/launch.hpp>
#include <boost/application/auto_handler.hpp>

namespace boost { namespace application { 

   /*!
    * \brief This class Tie a application and context, and simplifies the 
    *        creation of an application.
    *
    *        Using it you can start application with one line.
    *
    * \b Examples:
    * \code
    * launch<common>(auto_app<myapp1>());
    * \endcode
    *        
    */
   template <typename App, typename Cxt = context>
   struct auto_app : noncopyable {
      typedef App app_t;
      typedef Cxt cxt_t;
      typedef auto_app<app_t, cxt_t> this_type_t;
   };

   /*!
    * Creates a application using auto_app, the ec 
    * ( boost::system::error_code& ec) will be set to the result 
    * of the operation, they should be tested for errors.
    *
    * \param app User application functor instance.
    *
    * \param ec Variable (boost::system::error_code) that will be
    *        set to the result of the operation.
    *
    * \return the return of application operator of user functor
    *         class that usually need be returned by main function
    *         to O.S.
    *
    */
   template <typename ApplicationMode, typename Application>
   inline int launch(Application& app,  system::error_code& ec) {

      if(!boost::is_same<Application, typename Application::this_type_t>::value) {
           ec = boost::system::error_code(
                 boost::system::errc::invalid_argument,
                 boost::system::generic_category()
                 );
           return 1;
      }

      if(boost::is_same<typename Application::cxt_t, global_context>::value) {

         global_context_ptr cxt = global_context::create(ec);

         if(ec) return 1;

         auto_handler<typename Application::app_t> app(cxt);
         int ret = launch<ApplicationMode>(app, cxt, ec);

         global_context::destroy();
         return ret;
      }

      context context;
      auto_handler<typename Application::app_t> dapp(context);
      return launch<ApplicationMode>(dapp, context, ec);
   }

   // application::launch<application::common>(application::auto_app<myapp2, application::global_context>(), ec);
   // application::launch<application::common>(application::auto_app<myapp1, application::context>(), ec);
   // application::launch<application::common>(application::auto_app<myapp1>(), ec);

   template <typename ApplicationMode, typename Application>
   inline int launch(Application& app) {

      system::error_code ec; int ret = 0;
      ret = launch<ApplicationMode>(app, ec);

      if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
	       "launch() failed", ec);

      return ret;
   }
 
   // application::launch<application::common>(application::auto_app<myapp2, application::global_context>());
   // application::launch<application::common>(application::auto_app<myapp1, application::context>());
   // application::launch<application::common>(application::auto_app<myapp1>());
   /*
   template <typename ApplicationMode, typename Application>
   inline int launch(Application& app, uuids::uuid& appid, system::error_code& ec) {

      if(boost::is_same<Application::cxt_t, global_context>::value) {

         global_context_ptr cxt = global_context::create(ec);

         if(ec) return 1;

         auto_handler<Application::app_t> app(cxt, appid);
         int ret = launch<ApplicationMode>(app, cxt, ec);

         global_context::destroy();
         return ret;
      }

      context context;
      auto_handler<Application::app_t> dapp(context, appid);
      return launch<ApplicationMode>(dapp, context, ec);
   }
 
   // application::launch<application::common>(application::auto_app<myapp2, application::global_context>(), boost::uuids::string_generator()("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}"), ec);
   // application::launch<application::common>(application::auto_app<myapp1, application::context>(),boost::uuids::string_generator()("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}"), ec);
   // application::launch<application::common>(application::auto_app<myapp1>(), boost::uuids::string_generator()("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}"), ec);

   template <typename ApplicationMode, typename Application>
   inline int launch(Application& app, uuids::uuid& appid) {

      system::error_code ec; int ret = 0;
      ret = launch<ApplicationMode>(app, appid, ec);

      if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
	       "launch() failed", ec);

      return ret;
   }

   // application::launch<application::common>(application::auto_app<myapp2, application::global_context>(), boost::uuids::string_generator()("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}"));
   // application::launch<application::common>(application::auto_app<myapp1, application::context>(),boost::uuids::string_generator()("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}"));
   // application::launch<application::common>(application::auto_app<myapp1>(), boost::uuids::string_generator()("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}"));
 
   template <typename ApplicationMode, typename Application>
   inline int launch(Application& app, int argc, character_types::char_type *argv[], system::error_code& ec) {

      if(boost::is_same<Application::cxt_t, global_context>::value) {

         global_context_ptr cxt = global_context::create(ec);

         if(ec) return 1;

         cxt->insert<args>(
            csbl::make_shared<args>(argc, argv));  

         auto_handler<Application::app_t> app(cxt);
         int ret = launch<ApplicationMode>(app, cxt, ec);

         global_context::destroy();
         return ret;
      }
 
      context cxt;

      cxt.insert<args>(
            csbl::make_shared<args>(argc, argv));  
 
      auto_handler<Application::app_t> dapp(cxt);
      return launch<ApplicationMode>(dapp, cxt, ec);
   }

   template <typename ApplicationMode, typename Application>
   inline int launch(Application& app, int argc, character_types::char_type *argv[]) {

      system::error_code ec; int ret = 0;
      ret = launch<ApplicationMode>(app, argc, argv, ec);

      if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
	       "launch() failed", ec);

      return ret;
   }
 
   template <typename ApplicationMode, typename Application>
   inline int launch(Application& app, int argc, character_types::char_type *argv[], uuids::uuid& appid, system::error_code& ec) {

      if(boost::is_same<Application::cxt_t, global_context>::value) {

         global_context_ptr cxt = global_context::create(ec);

         if(ec) return 1;

         cxt->insert<args>(
            csbl::make_shared<args>(argc, argv));  

         auto_handler<Application::app_t> app(cxt, appid);
         int ret = launch<ApplicationMode>(app, cxt, ec);

         global_context::destroy();
         return ret;
      }
 
      context cxt;

      cxt.insert<args>(
            csbl::make_shared<args>(argc, argv));  
 
      auto_handler<Application::app_t> dapp(cxt, appid);
      return launch<ApplicationMode>(dapp, cxt, ec);
   }

   template <typename ApplicationMode, typename Application>
   inline int launch(Application& app, int argc, character_types::char_type *argv[], uuids::uuid& appid) {

      system::error_code ec; int ret = 0;
      ret = launch<ApplicationMode>(app, argc, argv, appid, ec);

      if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
	       "launch() failed", ec);

      return ret;
   }
   */
}} // boost::application


#endif // BOOST_APPLICATION_AUTO_HANDLER_HPP
