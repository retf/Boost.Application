//
// Copyright (c) Antony Polukhin, 2012-2013.
//
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_TYPE_INDEX_TEMPLATE_INDEX_HPP
#define BOOST_TYPE_INDEX_TEMPLATE_INDEX_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif


/// \file template_index.hpp
/// \brief Contains implementation of boost::template_index class.
///
/// boost::template_index is just a typedef of boost::template_info, that combines functionality
/// of boost::type_info and boost::type_index but can work with RTTI disabled.

#include "template_info.hpp"

namespace boost {

/// boost::template_index is just a typedef of boost::template_info, that combines functionality
/// of boost::type_info and boost::type_index but can work with RTTI disabled.
typedef template_info template_index;

} // namespace boost

#endif // BOOST_TYPE_INDEX_TEMPLATE_INDEX_HPP

