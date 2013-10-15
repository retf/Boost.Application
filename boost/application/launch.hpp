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
#include <boost/application/limit_single_instance_aspect.hpp>
#include <boost/application/detail/ensure_single_instance.hpp>

// Note that singularity is in approval process, 
// refer to the above link to know more:
// http://www.boost.org/community/review_schedule.html
#include <boost/singularity/singularity.hpp>

namespace boost { namespace application {

   // this class/function (we need to define better opetion)
   // launch an application as common(terminal/interactive/console) 
   // or server (Daemon/Service) application.

   // receive a boost::system::error_code variable 'ec' launch versions

   // param version, the ec ( boost::system::error_code& ec) will be set to 
   // the result of the operation, they should be tested for errors.
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
   template <typename ApplicationMode, typename Application>
   inline int launch(Application& app, application::context &context)
   {
      boost::system::error_code ec; int ret = 0;
      ret = launch<ApplicationMode>(app, context, ec);

      if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR("launch() failed");
         
      return ret;
   }
   
   // singularity version, throws an exception of type 
   // boost::system::system_error on error.
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
 