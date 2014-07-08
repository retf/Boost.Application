// run_mode.hpp  -------------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2014 Renato Tegon Forti

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
    * Indicates how the application was created.
    *
    */
   class run_mode
   {
   public:

      /*!
       * Constructs an run_mode aspect.
       *
       * \param run_mode The mode of application.
       */
      run_mode(int run_mode)
         : application_run_mode_(run_mode) {}

      /*!
       * Retreaves current mode of application.
       *
       * \return the mode of application.
       *
       */
      int mode() {
         return application_run_mode_;
      }

      /*!
       * Sets the current mode of application.
       *
       * \param mode The mode of application.
       *
       */
      void mode(int mode) {
         application_run_mode_ = mode;
      }

      bool operator==(int mode) const {
         return mode == application_run_mode_;
      }

   private:

      int application_run_mode_;

   };

}} // boost::application

#endif // BOOST_APPLICATION_RUN_MODE_ASPECT_HPP

