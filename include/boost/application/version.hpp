// Copyright 2014 Renato Tegon Forti.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_APPLICATION_APPLICATION_VERSION_HPP
#define BOOST_APPLICATION_APPLICATION_VERSION_HPP

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

/// \file boost/application/version.hpp
/// \brief Hold a set of functions to inspect the version of library.
/// The BOOST_APPLICATION_VERSION_MAJOR value will be incremented whenever a 
/// change is made which might cause compilation errors for existing client code.

#define BOOST_APPLICATION_VERSION_MAJOR 0
#define BOOST_APPLICATION_VERSION_MINOR 4
#define BOOST_APPLICATION_VERSION_BUILD 12

// major.minor.build

#ifndef BOOST_APPLICATION_VERSION
#define BOOST_APPLICATION_VERSION \
    BOOST_STRINGIZE(BOOST_APPLICATION_VERSION_MAJOR) "." \
    BOOST_STRINGIZE(BOOST_APPLICATION_VERSION_MINOR) "." \
    BOOST_STRINGIZE(BOOST_APPLICATION_VERSION_BUILD)
#endif // BOOST_APPLICATION_VERSION

namespace boost { namespace application {

   inline const char* library_version_string() {
      return BOOST_APPLICATION_VERSION;
   }

   inline unsigned int library_major_version() {
      return BOOST_APPLICATION_VERSION_MAJOR;
   }

   inline unsigned int library_minor_version() {
      return BOOST_APPLICATION_VERSION_MINOR;
   }

   inline unsigned int library_build_version() {
      return BOOST_APPLICATION_VERSION_BUILD;
   }

}}// boost::application

#endif // BOOST_APPLICATION_APPLICATION_VERSION_HPP
