// app.hpp -----------------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2012 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 25-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_DETAIL_CSBL_HPP
#define BOOST_APPLICATION_DETAIL_CSBL_HPP

#include <boost/application/config.hpp>

// This enable we to use csbl:: and internally the library select correct
// names, e.g. : csbl::unique_ptr will be boost::shared_ptr or std::shared_ptr

namespace boost { namespace application { namespace csbl {
// as common std-boost library

BOOST_APPLICATION_FEATURE_SELECT

}}} // boost::application::csbl

#endif // BOOST_APPLICATION_DETAIL_CSBL_HPP
