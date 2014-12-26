// Copyright 2014 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_APPLICATION_DETAIL_POSIX_PATH_FROM_ME_HPP
#define BOOST_APPLICATION_DETAIL_POSIX_PATH_FROM_ME_HPP

#include <boost/application/config.hpp>
#include <boost/filesystem/path.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace application { namespace detail {

    inline boost::filesystem::path path_from_me(boost::system::error_code &ec)  {
        return boost::filesystem::read_symlink("/proc/self/exe", ec);  
    }

}}} // namespace boost::dll::detail

#endif // BOOST_APPLICATION_DETAIL_POSIX_PATH_FROM_ME_HPP
