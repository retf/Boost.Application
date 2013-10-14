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
#include <boost/application/common_application.hpp>
#include <boost/application/server_application.hpp>

// Note that singularity is in approval process, 
// refer to the above link to know more:
// http://www.boost.org/community/review_schedule.html
#include <boost/singularity/singularity.hpp>

namespace boost { namespace application {

   // this class/function (we need to define better opetion)
   // launch an application as common(terminal/interactive/console) 
   // or server (Daemon/Service) application.

   // param version
   template <typename ApplicationMode, typename Application>
   inline int launch(Application& app, application::context &context)
   {
      ApplicationMode run_as(context);
      return app(context);
   }
   
   // singularity version
   template <typename ApplicationMode, typename Application>
   inline int launch(Application& app, singularity<application::context> &context)
   {
      ApplicationMode run_as(context.get_global());
      return app();
   }

}} // boost::application 

#endif // BOOST_APPLICATION_LAUNCH_HPP
 