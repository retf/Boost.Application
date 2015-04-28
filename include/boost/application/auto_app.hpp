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
#include <boost/application/aspects/args.hpp>

namespace boost { namespace application { 

   /*!
    * \brief This class tie a application and context, and simplifies the 
    *        creation of an application.
    *
    *        The default context is not global context, if you desire 
    *        to use global you need specify as second template param,
    *        check example.
    *
    *        Using it you can start application with one line.
    *
    * \b Examples:
    * \code
    * launch< common, auto_app<myapp> >();
    * launch< common, auto_app<myapp, global_context> >();
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
    * Auto creates a application using auto_app, the ec 
    * ( boost::system::error_code& ec) will be set to the result 
    * of the operation, they should be tested for errors.
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
   inline int launch(system::error_code& ec) {

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

      context cxt;
      auto_handler<typename Application::app_t> dapp(cxt);
      return launch<ApplicationMode>(dapp, cxt, ec);
   }

   template <typename ApplicationMode, typename Application>
   inline int launch() {

      system::error_code ec; int ret = 0;
      ret = launch<ApplicationMode, Application>(ec);

      if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
	       "launch() failed", ec);

      return ret;
   }

   template <typename ApplicationMode, typename Application>
   inline int launch(uuids::uuid& appid, system::error_code& ec) {

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

         auto_handler<typename Application::app_t> app(cxt, appid);
         int ret = launch<ApplicationMode>(app, cxt, ec);

         global_context::destroy();
         return ret;
      }

      context cxt;
      auto_handler<typename Application::app_t> dapp(cxt, appid);
      return launch<ApplicationMode>(dapp, cxt, ec);
   }

   template <typename ApplicationMode, typename Application>
   inline int launch(uuids::uuid& appid) {

      system::error_code ec; int ret = 0;
      ret = launch<ApplicationMode, Application>(appid, ec);

      if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
	       "launch() failed", ec);

      return ret;
   } 

   template <typename ApplicationMode, typename Application>
   inline int launch(int argc, character_types::char_type *argv[], system::error_code& ec) {

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

         cxt->insert<args>(
            csbl::make_shared<args>(argc, argv));  

         auto_handler<typename Application::app_t> app(cxt);
         int ret = launch<ApplicationMode>(app, cxt, ec);

         global_context::destroy();
         return ret;
      }

      context cxt;

      cxt.insert<args>(
         csbl::make_shared<args>(argc, argv));  

      auto_handler<typename Application::app_t> dapp(cxt);
      return launch<ApplicationMode>(dapp, cxt, ec);
   }

   template <typename ApplicationMode, typename Application>
   inline int launch(int argc, character_types::char_type *argv[]) {

      system::error_code ec; int ret = 0;
      ret = launch<ApplicationMode, Application>(argc, argv, ec);

      if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
	       "launch() failed", ec);

      return ret;
   }

   template <typename ApplicationMode, typename Application>
   inline int launch(int argc, character_types::char_type *argv[], uuids::uuid& appid, system::error_code& ec) {

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

         cxt->insert<args>(
            csbl::make_shared<args>(argc, argv));  

         auto_handler<typename Application::app_t> app(cxt, appid);
         int ret = launch<ApplicationMode>(app, cxt, ec);

         global_context::destroy();
         return ret;
      }

      context cxt;

      cxt.insert<args>(
         csbl::make_shared<args>(argc, argv));  

      auto_handler<typename Application::app_t> dapp(cxt, appid);
      return launch<ApplicationMode>(dapp, cxt, ec);
   }

   template <typename ApplicationMode, typename Application>
   inline int launch(int argc, character_types::char_type *argv[], uuids::uuid& appid) {

      system::error_code ec; int ret = 0;
      ret = launch<ApplicationMode, Application>(argc, argv, appid, ec);

      if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
	       "launch() failed", ec);

      return ret;
   }
   
}} // boost::application

#endif // BOOST_APPLICATION_AUTO_HANDLER_HPP
