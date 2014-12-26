// Copyright 2014 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_APPLICATION_SYSTEM_ERROR_HPP
#define BOOST_APPLICATION_SYSTEM_ERROR_HPP

#include <boost/system/config.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/asio.hpp>

#if defined(BOOST_POSIX_API)
#   include <errno.h>
#elif defined(BOOST_WINDOWS_API)
#   include <windows.h>
#endif

#ifdef BOOST_HAS_PRAGMA_ONCE
#   pragma once
#endif

namespace boost { namespace application {

   // BOOST_APPLICATION_LAST_ERROR

   /*!
    * Retrieves the last error code of system.
    *
    * \return the last error code of system.
    *
    * [Windows]
    * GetLastError
    *
    * [POSIX]
    * errno
    *
    */
   inline int last_error(void) BOOST_NOEXCEPT
   {
#if defined(BOOST_POSIX_API)
      return errno;
#elif defined(BOOST_WINDOWS_API)
      return GetLastError();
#endif
   }

   // BOOST_APPLICATION_SET_LAST_SYSTEM_ERROR

   /*!
    * Generate a system::error_code form last_error function.
    *
    * \return the system::error_code based on last_error
    *         function (System Error).
    *
    */
   inline system::error_code last_error_code() BOOST_NOEXCEPT
   {
      return boost::system::error_code(last_error(),
         boost::system::system_category());
   }

}}// boost::application

#endif // BOOST_APPLICATION_SYSTEM_ERROR_HPP

