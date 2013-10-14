// run_mode_aspect.hpp  ------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 14-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_RUN_MODE_ASPECT_HPP
#define BOOST_APPLICATION_RUN_MODE_ASPECT_HPP

// appication
#include <boost/application/config.hpp>

namespace boost { namespace application {

 // run_mode aspect
   //
   // indicates how the application was created, in this version 3 ways are possible:
   //
   // * created by the launcher as a Server (application_server)
   // * created by an app launcher as Common/Interactive (application_common)
   // * created directly by User (application_direct)
   class run_mode
   {
      friend class server;
      friend class common;

   public:
      // application type
      enum application_run_mode {
         common = 0,
         server,
         direct // not created by the launcher
      };

      run_mode(application_run_mode run_mode) 
         : application_run_mode_(run_mode) {}

      application_run_mode mode() {
         return application_run_mode_;
      }

      void mode(application_run_mode mode) {
         application_run_mode_ = mode;
      }

      bool operator==(application_run_mode mode) const {
         return mode == application_run_mode_;
      }

   private:

      application_run_mode application_run_mode_;

   };
   
}} // boost::application

#endif // BOOST_APPLICATION_RUN_MODE_ASPECT_HPP

