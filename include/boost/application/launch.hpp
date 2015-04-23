// Copyright 2014 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)--

#ifndef BOOST_APPLICATION_LAUNCH_HPP
#define BOOST_APPLICATION_LAUNCH_HPP

/// \file boost/application/launch.hpp
/// \brief This file has a list of free function to launch an aplication using
/// a specifyc mode.
///
/// In this version 2 flavors of application are supported:
/// 
/// - common
/// - server
/// 
/// The "launch" function can create any of this types, and launch
/// will aready setup/add default control aspects to application context.
/// 
/// Two version of "launch" are available, the first receive a
/// boost::system::error_code variable 'ec' that would be set
/// to the result of the operation, and the other thrown an exception of
/// boost::system::system_error.

// application
#include <boost/application/config.hpp>
// modes
#include <boost/application/common_application.hpp>
#include <boost/application/server_application.hpp>
#include <boost/application/signal_binder.hpp>
// aspects used by launch
#include <boost/application/aspects/limit_single_instance.hpp>
#include <boost/application/detail/ensure_single_instance.hpp>
#include <boost/application/detail/csbl.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace application {

   // receive a boost::system::error_code variable 'ec' launch versions

   /*!
    * Creates a application, the ec ( boost::system::error_code& ec)
    * will be set to the result of the operation, they should be
    * tested for errors.
    *
    * \param app User application functor instance.
    *
    * \param ct The custom type of application, that will be passed
    *        as paramenter to application type istead a default
    *        custom type instance. User can customize this instance.
    *
    * \param context The context of application, that will be passed
    *        as paramenter to application operator and populated
    *        with defaul aspects, depending on type of application
    *        that are passed as ApplicationMode.
    *
    * \param ec Variable (boost::system::error_code) that will be
    *        set to the result of the operation.
    *
    * \return the return of application operator of user functor
    *         class that usually need be returned by main function
    *         to O.S.
    *
    */
   template <typename ApplicationMode, typename Application,
      typename CustomType, typename Context>
   inline int launch(Application& app, CustomType& ct, Context &cxt,
      system::error_code& ec) {
      // the ensure_single_instance tell us to exit?
     
      bool we_need_exit = detail::ensure_single_instance<Context>()(cxt, ec); 
     
      if(ec) return 0;
      if(we_need_exit) return 0;

      // all ok, istantiate application mode and start user code.

      // tie ApplicationMode and User code, and then start it, after that,
      // get result code.

      // we need that Application Mode start function, because in server
      // implementation, we need start it on a new thread.
      return ApplicationMode(app, ct, cxt, ec).run();
   }

   template <typename ApplicationMode, typename Application, 
      typename CustomType>
   inline int launch(Application& app, CustomType& ct, global_context_ptr cxt,
      system::error_code& ec) {
      return launch<ApplicationMode>(app, ct, *cxt.get(), ec);
   }

   /*!
    * Creates a application, the ec ( boost::system::error_code& ec)
    * will be set to the result of the operation, they should be
    * tested for errors.
    *
    * \param app User application functor instance.
    *
    * \param context The context of application, that will be passed
    *        as paramenter to application operator and populated
    *        with defaul aspects, depending on type of application
    *        that are passed as ApplicationMode.
    *
    * \param ec Variable (boost::system::error_code) that will be
    *        set to the result of the operation.
    *
    * \return the return of application operator of user functor
    *         class that usually need be returned by main function
    *         to O.S.
    *
    */
   template <typename ApplicationMode, typename Application, typename Context>
   inline int launch(Application& app, Context &cxt, system::error_code& ec) {
      signal_manager ct(cxt, ec); // our default custom type

      if(ec) return 0;

      return launch<ApplicationMode>(app, ct, cxt, ec);
   }

   template <typename ApplicationMode, typename Application>
   inline int launch(Application& app, global_context_ptr cxt, system::error_code& ec) {
      return launch<ApplicationMode>(app, *cxt.get(), ec);
   }

   // throws an exception of type boost::system::system_error launch versions

   /*!
    * Creates a application, throws an exception of type
    * boost::system::system_error on error.
    *
    * \param app User application functor instance.
    *
    * \param ct The custom type of application, that will be passed
    *        as paramenter to application type istead a default
    *        custom type instance. User can customize this instance.
    *
    * \param context The context of application, that will be passed
    *        as paramenter to application operator and populated
    *        with defaul aspects, depending on type of application
    *        that are passed as ApplicationMode.
    *
    * \return the return of application operator of user functor
    *         class that usually need be returned by main function
    *         to O.S.
    *
    */
   template <typename ApplicationMode, typename Application,
      typename CustomType, typename Context>
   inline int launch(Application& app, CustomType& ct, Context &cxt) {
      system::error_code ec; int ret = 0;
      ret = launch<ApplicationMode>(app, ct, cxt, ec);

      if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
	       "launch() failed", ec);

      return ret;
   }

   template <typename ApplicationMode, typename Application, typename CustomType>
   inline int launch(Application& app, CustomType& ct, global_context_ptr cxt) {
      return launch<ApplicationMode>(app, ct, *cxt.get());
   }

   /*!
    * Creates a application, throws an exception of type
    * boost::system::system_error on error.
    *
    * \param app User application functor instance.
    *
    * \param context The context of application, that will be passed
    *        as paramenter to application operator and populated
    *        with defaul aspects, depending on type of application
    *        that are passed as ApplicationMode.
    *
    * \return the return of application operator of user functor
    *         class that usually need be returned by main function
    *         to O.S.
    *
    */
   template <typename ApplicationMode, typename Application, typename Context>
   inline int launch(Application& app, Context &cxt) {
      system::error_code ec; int ret = 0;
      ret = launch<ApplicationMode>(app, cxt, ec);

      if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
	       "launch() failed", ec);

      return ret;
   }

   template <typename ApplicationMode, typename Application>
   inline int launch(Application& app, global_context_ptr cxt) {
      return launch<ApplicationMode>(app, *cxt.get());
   }

}} // boost::application

#endif // BOOST_APPLICATION_LAUNCH_HPP
