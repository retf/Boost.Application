// ensure_single_instance.hpp  ----------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2014 Renato Tegon Forti

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
#include <boost/application/detail/csbl.hpp>
#include <boost/application/aspects/limit_single_instance.hpp>

namespace boost { namespace application { namespace detail {

   // check single_instance aspect and runs the requested behavior,
   // returns true to indicate that application needs exit.

   inline bool check(context &cxt,
      boost::system::error_code& ec)
   { 
      csbl::shared_ptr<limit_single_instance> ol =
         cxt.find<limit_single_instance>();
      
      if(ol)
      {
         bool is_another_instance_running = ol->lock(ec);

         if(ec) return false; // user need check by error

         if(!is_another_instance_running)
            return false; // continue, no other instance running

         // check if we have any callback to call

         handler<>::callback* cb = 0;

         if(ol->get(cb))
         {
            if((*cb)())
            {
               // user tell us to continue
               return false;
            }

            return true;
         }

         // default behaviour
         return true;
      }

      // continue / no restriction
      return false;
   }

   template <class T> struct ensure_single_instance {
      bool operator()(T &cxt, boost::system::error_code& ec)
      {
         return check(cxt, ec);
      }
   };

   template <> struct ensure_single_instance< 
         global_context_ptr > {
      bool operator()(global_context_ptr cxt,
         boost::system::error_code& ec)
      {
         return check(*cxt.get(), ec);
      }
   };

}}} // boost::application

#endif // BOOST_APPLICATION_DETAIL_ENSURE_SINGLE_INSTANCE_HPP

