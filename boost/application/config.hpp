// config.hpp  --------------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See http://www.boost.org/libs/application for documentation.

// -----------------------------------------------------------------------------

// Revision History
// 01-04-2012 dd-mm-yyyy - Initial Release
// 10-07-2013 dd-mm-yyyy - Refatored

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_CONFIG_HPP                  
#define BOOST_APPLICATION_CONFIG_HPP

#include <boost/config.hpp> 
#include <boost/system/config.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/application/version.hpp>

#ifndef BOOST_NO_CXX11_SMART_PTR
#include <memory>
#include <unordered_map>
#include <typeindex>
#else
#include <boost/smart_ptr.hpp>
#include <boost/unordered/unordered_map.hpp>
#include <boost/typeindex/typeindex.hpp>
#endif

#if defined(BOOST_POSIX_API)
#   include <errno.h>
#   define BOOST_APPLICATION_LAST_ERROR errno
#elif defined(BOOST_WINDOWS_API)
#   include <Windows.h>
#   define BOOST_APPLICATION_LAST_ERROR GetLastError()
#   if defined(_UNICODE) || defined(UNICODE)
#   define BOOST_APPLICATION_STD_WSTRING
#   endif
#endif

// check if compiler provide some STL features of c++11 
// that we use by the library, else use boost.
// we need think in a good way to do this. any idea?
#ifndef BOOST_NO_CXX11_SMART_PTR 
#define BOOST_APPLICATION_FEATURE_SELECT \
   using std::make_shared;               \
   using std::unique_ptr;                \
   using std::shared_ptr; 
#else
#define BOOST_APPLICATION_FEATURE_SELECT \
   using boost::make_shared;             \
   using boost::shared_ptr;              \
   using boost::unique_ptr; 
#endif
BOOST_APPLICATION_FEATURE_SELECT

// error handle for Boost.Application lib, based on Boost.System.
// user can set this macro for example to BOOST_THROW_EXCEPTION 
// if they whant to use Boost.Exception.

#define BOOST_APPLICATION_THROW(EX) throw EX

// error ctrl

#define BOOST_APPLICATION_SOURCE_LOCATION \
   "in file '" __FILE__ "', line " BOOST_STRINGIZE(__LINE__) ": "

#if defined(BOOST_APPLICATION_TROWN_MY_OWN_EXCEPTION)
#define BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR(what) \
   BOOST_APPLICATION_TROWN_MY_OWN_EXCEPTION( \
      what " : " + boost::system::error_code( \
         BOOST_APPLICATION_LAST_ERROR,  \
            boost::system::system_category()).message(),  \
               BOOST_APPLICATION_LAST_ERROR)
#else
#define BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR(what) \
   BOOST_APPLICATION_THROW(boost::system::system_error( \
      boost::system::error_code(BOOST_APPLICATION_LAST_ERROR, \
         boost::system::system_category()), \
            BOOST_APPLICATION_SOURCE_LOCATION what))
#endif

#define BOOST_APPLICATION_SET_LAST_SYSTEM_ERROR(ec) \
   ec = boost::system::error_code(BOOST_APPLICATION_LAST_ERROR, \
      boost::system::system_category())

#endif // BOOST_APPLICATION_CONFIG_HPP
