// status_aspect.hpp  ---------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 14-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_STATUS_ASPECT_HPP
#define BOOST_APPLICATION_STATUS_ASPECT_HPP

// appication
#include <boost/application/config.hpp>

namespace boost { namespace application {

   // status aspect
   //
   // indicates the current state of application, that can be:
   //
   // * application_stoped 
   // * application_running
   // * application_paused (used only on server appliation on windows side)
   class status 
   {
      friend class server;
      friend class common;

   public:  
      // application current state
      enum application_state {
         stoped = 0,
         running,
         paused // Windows Service
      };

      status(application_state state) 
         :  application_state_(state) {}

      application_state state() {
         return application_state_;
      }

      void state(application_state state) {
         application_state_ = state;
      }

      bool operator==(application_state state) const {
         return state == application_state_;
      }

   private:

      application_state application_state_;

   };
   
}} // boost::application

#endif // BOOST_APPLICATION_STATUS_ASPECT_HPP

