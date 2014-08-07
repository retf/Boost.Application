// application_mode_register.hpp ---------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2014 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See http://www.boost.org/libs/application for documentation.

// -----------------------------------------------------------------------------

// Revision History
// 06-12-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_MODES_REGISTER_HPP
#define BOOST_APPLICATION_MODES_REGISTER_HPP

#include <boost/atomic.hpp>

namespace boost { namespace application {

   // application_modes_register

   // run_mode unique identifier generator for each mode, like 
   // common, server and so on
   template <typename T>
   T new_run_mode()
   {
      static boost::atomic<T> id(-1);
      return ++id;
   }

}} // boost::application

#endif // BOOST_APPLICATION_MODES_REGISTER_HPP
