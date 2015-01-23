// Copyright 2014 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_APPLICATION_DETAIL_POSIX_PATH_FROM_ME_HPP
#define BOOST_APPLICATION_DETAIL_POSIX_PATH_FROM_ME_HPP

#include <boost/application/config.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/predef/os.h>

#include <cstdlib>
#include <pwd.h>
#include <unistd.h>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace application { namespace detail {

    inline boost::filesystem::path path_from_me(boost::system::error_code &ec)  {
        return boost::filesystem::read_symlink("/proc/self/exe", ec);  
    }

    inline boost::filesystem::path getenv(const char* env_name)
    {
        const char* res = ::getenv(env_name);
        return res ? res : boost::filesystem::path();
    }

    inline boost::filesystem::path home_path()
    {
        boost::filesystem::path path = getenv("HOME");
        if(path.empty())
        {
            struct passwd* pwd = getpwuid(getuid());
            if(pwd)
                return pwd->pw_dir;
            return ".";
        }
        return path;
    }

    inline boost::filesystem::path app_data_path()
    {
        boost::filesystem::path path = getenv("XDG_DATA_HOME");
        if(path.empty()) {
#if BOOST_OS_MACOS
            return home_path() / "Library/Preferences/";
#else
            return home_path() / ".local/share";
#endif
        }
        return path;
    }

    inline boost::filesystem::path config_path()
    {

        boost::filesystem::path path = getenv("XDG_CONFIG_HOME");
        if(path.empty()) {
#if BOOST_OS_MACOS
            return home_path() / "Library/Preferences/";
#else
            return home_path() / ".config";
#endif
        }
        return path;
    }

    inline boost::filesystem::path temp_path()
    {
        boost::filesystem::path path = getenv("TMPDIR");
        if(path.empty())
            return "/tmp"; // Fallback if TMPDIR not available
        return path;
    }

}}} // namespace boost::dll::detail

#endif // BOOST_APPLICATION_DETAIL_POSIX_PATH_FROM_ME_HPP
