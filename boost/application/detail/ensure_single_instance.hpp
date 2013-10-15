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

#ifndef BOOST_APPLICATION_DETAIL_ENSURE_SINGLE_INSTANCE_HPP
#define BOOST_APPLICATION_DETAIL_ENSURE_SINGLE_INSTANCE_HPP

// application
#include <boost/application/config.hpp>
#include <boost/application/limit_single_instance_aspect.hpp>

namespace boost { namespace application { namespace detail {

   // check single_instance aspect and runs the requested behavior,
   // returns true to indicate that application needs exit.
   inline bool ensure_single_instance(context &cxt, 
      boost::system::error_code& ec)
   {
      if(cxt.has_aspect<limit_single_instance>())
      {
         bool is_another_instance_running = 
            cxt.use_aspect<limit_single_instance>().lock(ec);

         if(ec) return false; // user need check by error

         if(!is_another_instance_running)
            return false; // continue, no other instance running

         // check if we have any callback to call

         limit_single_instance::cxparameter_cb* cxparameter =
            cxt.use_aspect<limit_single_instance>().get_cxparameter_cb();

         if(cxparameter)
         {
            if((*cxparameter)(cxt))
            {
               // user tell us to continue
               return false;
            }

            return true;
         }

         limit_single_instance::singularity_cb* singularity =
            cxt.use_aspect<limit_single_instance>().get_singularity_cb();

         if(singularity)
         {
            if((*singularity)())
            {
               // user tell us to continue
               return false;
            }

            return true;
         }
      }

      return true;
   }

}}} // boost::application 

#endif // BOOST_APPLICATION_DETAIL_ENSURE_SINGLE_INSTANCE_HPP
 