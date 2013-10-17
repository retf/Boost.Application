// run_mode.hpp  -------------------------------------------------------------//
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

   /*!
    * \brief This aspect class is used internaly by applications types (modes)
    * 
    * Indicates how the application was created,
    * 3 ways are possible:
    *
    * - created by the launcher as a Server (application_server)
    * - created by an app launcher as Common/Interactive (application_common)
    * - created directly by User (application_direct)
    * 
    */
   class run_mode
   {
      friend class server;
      friend class common;

   public:

      // application types / modes
      enum application_run_mode {
         common = 0,
         server,
         direct // not created by the launcher
      };

      /*!
       * Constructs an run_mode aspect.
       *
       * \param run_mode The mode of application.
       */
      run_mode(application_run_mode run_mode) 
         : application_run_mode_(run_mode) {}

      /*!
       * Retreaves current mode of application.
       * 
       * \return the mode of application.
       *      
       */
      application_run_mode mode() {
         return application_run_mode_;
      }

      /*!
       * Sets the current mode of application.
       * 
       * \param mode The mode of application.
       *      
       */
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

