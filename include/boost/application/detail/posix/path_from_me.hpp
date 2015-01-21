// Copyright 2014 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_APPLICATION_DETAIL_POSIX_PATH_FROM_ME_HPP
#define BOOST_APPLICATION_DETAIL_POSIX_PATH_FROM_ME_HPP

#include <boost/application/config.hpp>
#include <boost/filesystem/path.hpp>

#include <cstdlib>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace application { namespace detail {

    inline boost::filesystem::path path_from_me(boost::system::error_code &ec)  {
        return boost::filesystem::read_symlink("/proc/self/exe", ec);  
    }

    inline std::string getenv(const char* env_name)
    {
        char * r = ::getenv(env_name);
        return r ? r : "";
    }

    inline std::string home_path()
    {
        return getenv("HOME");
    }

    inline std::string app_data_path()
    {
        std::string ret = getenv("XDG_DATA_HOME");
        if(ret.empty())
            return home_path() + "/.local/share"; // Fallback
        return ret;
    }

    inline std::string config_path()
    {
        std::string ret = getenv("XDG_CONFIG_HOME");
        if(ret.empty())
            return home_path() + "/.config"; // Fallback
        return ret;
    }

    inline std::string temp_path()
    {
        std::string ret = getenv("TMPDIR");
        if(ret.empty())
            return "/tmp"; // Fallback if TMPDIR not available
        return ret;
    }

}}} // namespace boost::dll::detail

#endif // BOOST_APPLICATION_DETAIL_POSIX_PATH_FROM_ME_HPP
