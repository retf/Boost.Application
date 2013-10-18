// ensure_single_instance.hpp  ------------------------------------------------//
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
#include <boost/application/aspects/limit_single_instance.hpp>

namespace boost { namespace application { namespace detail {

   // check single_instance aspect and runs the requested behavior,
   // returns true to indicate that application needs exit.
   inline bool ensure_single_instance(context &cxt, 
      boost::system::error_code& ec)
   {
      if(cxt.has_aspect<limit_single_instance>())
      {    
         std::shared_ptr<limit_single_instance> ol =          
            cxt.get_aspect<limit_single_instance>();

         bool is_another_instance_running = ol->lock(ec);

         if(ec) return false; // user need check by error

         if(!is_another_instance_running)
            return false; // continue, no other instance running

         // check if we have any callback to call

         handler::parameter_callback* parameter;

         if(ol->callback(parameter))
         {
            if((*parameter)(cxt))
            {
               // user tell us to continue
               return false;
            }

            return true;
         }

         handler::singleton_callback* singleton = 0;

         if(ol->callback(singleton))
         {
            if((*singleton)())
            {
               // user tell us to continue
               return false;
            }

            return true;
         }

      }

      // continue / no restriction
      return false;
   }

}}} // boost::application 

#endif // BOOST_APPLICATION_DETAIL_ENSURE_SINGLE_INSTANCE_HPP
 