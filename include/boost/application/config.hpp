// config.hpp ----------------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See http://www.boost.org/libs/application for documentation.

// -----------------------------------------------------------------------------

// Revision History
// 18-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

/*!
 * Configuration file used by Boost.Application.
 *
 * Unicode setup for Windows, System Error Handle
 * and C++11/Boost feature Select area handled here.
 *
 */

#ifndef BOOST_APPLICATION_CONFIG_HPP
#define BOOST_APPLICATION_CONFIG_HPP

#include <boost/config.hpp>
#include <boost/application/system_error.hpp>
#include <boost/application/version.hpp>
#include <boost/application/detail/typeindex.hpp>

// other header selection
//
// the user can force one selection defining
//
// BOOST_APPLICATION_FEATURE_NS_SELECT_STD or
// BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST
//
// e.g. on bjam:
// > sudo bjam define=BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST
//

// force selection
#if defined( BOOST_APPLICATION_FEATURE_NS_SELECT_STD )
#   include <memory>
#   include <unordered_map>
#elif defined( BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST )
#   include <boost/shared_ptr.hpp>
#   include <boost/make_shared.hpp>
#   include <boost/unordered_map.hpp>
#else // auto detect
//  The standard library header <memory> has no shared_ptr and unique_ptr.
#   ifndef BOOST_NO_CXX11_SMART_PTR
#      include <memory>
#   else
#      include <boost/shared_ptr.hpp>
#      include <boost/make_shared.hpp>
#   endif
//  The standard library does not provide header <unordered_map>.
#   ifndef BOOST_NO_CXX11_HDR_UNORDERED_MAP
#      include <unordered_map>
#   else
#      include <boost/unordered_map.hpp>
#   endif
#endif

#if defined(BOOST_WINDOWS_API)
#   if defined(_UNICODE) || defined(UNICODE)
#      define BOOST_APPLICATION_STD_WSTRING
#   endif
#endif

// check if compiler provide some STL features of c++11
// that we use by the library, else use boost.

// force selection
#if defined( BOOST_APPLICATION_FEATURE_NS_SELECT_STD )
#   define BOOST_APPLICATION_FEATURE_NS_SELECT std
#   define BOOST_APPLICATION_TYPE_INDEX_SELECT type_index
#elif defined( BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST )
#   define BOOST_APPLICATION_FEATURE_NS_SELECT boost
//  the last type_index uses boost::typeindex insted boost::typeind 
//  (https://github.com/boostorg/type_index)
#   define BOOST_APPLICATION_TYPE_INDEX_SELECT typeindex::type_index 
#else // auto detect
// BOOST_APPLICATION_FEATURE_NS_SELECT is used to select correct ns
// on class members.
#   ifndef BOOST_NO_CXX11_SMART_PTR
#      define BOOST_APPLICATION_FEATURE_NS_SELECT std
#      define BOOST_APPLICATION_TYPE_INDEX_SELECT type_index
#   else
#      define BOOST_APPLICATION_FEATURE_NS_SELECT boost
#      define BOOST_APPLICATION_TYPE_INDEX_SELECT typeindex::type_index
#   endif
#endif

// BOOST_APPLICATION_FEATURE_SELECT is used to select correct ns
// on functin/method scope.
#define BOOST_APPLICATION_FEATURE_SELECT                                       \
   using BOOST_APPLICATION_FEATURE_NS_SELECT::make_shared;                     \
   using BOOST_APPLICATION_FEATURE_NS_SELECT::shared_ptr;                      \
   using BOOST_APPLICATION_FEATURE_NS_SELECT::BOOST_APPLICATION_TYPE_INDEX_SELECT; \
   using BOOST_APPLICATION_FEATURE_NS_SELECT::unordered_map;                   \
   using BOOST_APPLICATION_FEATURE_NS_SELECT::static_pointer_cast;

// error handle for Boost.Application lib, based on Boost.System.
// user can set this macro for example to BOOST_THROW_EXCEPTION
// if they whant to use Boost.Exception.

#define BOOST_APPLICATION_THROW(EX) throw EX

// error ctrl

// report location of error.
#define BOOST_APPLICATION_SOURCE_LOCATION                                      \
   "in file '" __FILE__ "', line " BOOST_STRINGIZE(__LINE__) ": "

// define BOOST_APPLICATION_TROWN_MY_OWN_EXCEPTION if you want
// THROW your own EXCEPTION
#if defined(BOOST_APPLICATION_TROWN_MY_OWN_EXCEPTION)

#define BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR(what)                        \
   BOOST_APPLICATION_TROWN_MY_OWN_EXCEPTION(                                   \
      what " : " + boost::system::error_code(                                  \
         last_error(),                                                         \
            boost::system::system_category()).message(),                       \
               last_error())

#define BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(what, ec)        \
   BOOST_APPLICATION_TROWN_MY_OWN_EXCEPTION(                                   \
      what " : " + ec.message(),                                               \
         ec.value())

#else

// use this version if you have not recovered the 'c' ie right after the error.
// this version recovery 'ec' internaly.
#define BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR(what)                        \
   BOOST_APPLICATION_THROW(boost::system::system_error(                        \
      boost::system::error_code(last_error(),                                  \
         boost::system::system_category()),                                    \
            BOOST_APPLICATION_SOURCE_LOCATION what))

// use this version when you already have the value of 'ec', ie you already
// called 'last_error'.
#define BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(what, ec)        \
   BOOST_APPLICATION_THROW(boost::system::system_error(                        \
      ec, BOOST_APPLICATION_SOURCE_LOCATION what))

#endif

#endif // BOOST_APPLICATION_CONFIG_HPP
