// launch.hpp  ---------------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 14-10-2013 dd-mm-yyyy - Initial Release
// 
// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_LAUNCH_HPP
#define BOOST_APPLICATION_LAUNCH_HPP

// application
#include <boost/application/config.hpp>
// modes
#include <boost/application/common_application.hpp>
#include <boost/application/server_application.hpp>
// aspects
#include <boost/application/aspects/limit_single_instance.hpp>
#include <boost/application/detail/ensure_single_instance.hpp>

// Note that singularity is in approval process, 
// refer to the above link to know more:
// http://www.boost.org/community/review_schedule.html
#include <boost/singularity/singularity.hpp>

namespace boost { namespace application {

   /*!
    * This file has a list of free function to launch an aplication using
    * a specifyc mode. 
    *  
    * In this version 2 flavors of application are supported: 
    *  
    * - common_application 
    * - server_application 
    *  
    * The "launch" function can create any of this files, and launch 
    * will aready setup/add default control aspects to application context. 
    *  
    * Two version of "launch" are available, the first receive a 
    * boost::system::error_code variable 'ec' that would be set 
    * to the result of the operation, and no thrown an exception of 
    * boost::system::system_error. 
    *  
    */

   // receive a boost::system::error_code variable 'ec' launch versions

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
   template <typename ApplicationMode, typename Application>
   inline int launch(Application& app, application::context &context, 
      boost::system::error_code& ec)
   {
      // the ensure_single_instance tell us to exit?
      bool we_need_exit = detail::ensure_single_instance(context, ec);

      if(ec) return 0; 
      if(we_need_exit) return 0; 

      // all ok, istantiate application mode and start user code.
      ApplicationMode run_as(context, ec);
      return app(context);
   }
   
   // singularity version, the ec ( boost::system::error_code& ec) will be  
   // set to the result of the operation, they should be tested for errors.

   /*!
    * Creates a application, the ec ( boost::system::error_code& ec)
    * will be set to the result of the operation, they should be
    * tested for errors. 
    * 
    * \param app User application functor instance.
    *  
    * \param context The "singularity" context of application, 
    *        that will be passed as paramenter to application operator
    *        and populated with defaul aspects, depending on type of
    *        application that are passed as ApplicationMode.
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
   inline int launch(Application& app, 
      singularity<application::context> &context, boost::system::error_code& ec)
   {
      // the ensure_single_instance tell us to exit?
      bool we_need_exit = detail::ensure_single_instance(context.get_global(), ec);

      if(ec) return 0; 
      if(we_need_exit) return 0; 

      ApplicationMode run_as(context.get_global(), ec);
      return app();
   }

   // throws an exception of type boost::system::system_error launch versions

   // param version, throws an exception of type 
   // boost::system::system_error on error.

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
   template <typename ApplicationMode, typename Application>
   inline int launch(Application& app, application::context &context)
   {
      boost::system::error_code ec; int ret = 0;
      ret = launch<ApplicationMode>(app, context, ec);

      if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR("launch() failed");
         
      return ret;
   }
   
   /*!
    * Creates a application, throws an exception of type
    * boost::system::system_error on error.
    * 
    * \param app User application functor instance.
    *  
    * \param context The "singularity" context of application, 
    *        that will be passed as paramenter to application operator
    *        and populated with defaul aspects, depending on type of
    *        application that are passed as ApplicationMode.
    *  
    * \return the return of application operator of user functor 
    *         class that usually need be returned by main function
    *         to O.S.
    *      
    */
   template <typename ApplicationMode, typename Application>
   inline int launch(Application& app, singularity<application::context> &context)
   {
      boost::system::error_code ec; int ret = 0;
      ret = launch<ApplicationMode>(app, context, ec);

      if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR("launch() failed");
         
      return ret;
   }

}} // boost::application 

#endif // BOOST_APPLICATION_LAUNCH_HPP
 