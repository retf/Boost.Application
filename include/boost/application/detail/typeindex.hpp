// app.hpp -----------------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2012 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 25-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_DETAIL_TYPEINDEX_HPP
#define BOOST_APPLICATION_DETAIL_TYPEINDEX_HPP

#include <boost/config.hpp>

#if defined( BOOST_APPLICATION_FEATURE_NS_SELECT_STD )
#   include <typeindex>
#elif defined( BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST )
#   include <boost/type_index.hpp>
#else
#   ifndef BOOST_NO_CXX11_HDR_TYPEINDEX
#      include <typeindex>
#   else
#      include <boost/type_index.hpp>
#   endif
#endif

#endif // BOOST_APPLICATION_DETAIL_TYPEINDEX_HPP
