// Copyright 2014 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_APPLICATION_DETAIL_POSIX_PATH_FROM_ME_HPP
#define BOOST_APPLICATION_DETAIL_POSIX_PATH_FROM_ME_HPP

#include <boost/application/config.hpp>
#ifdef ASIO_STANDALONE
#   include <filesystem>
#else
#   include <boost/filesystem/path.hpp>
#endif
#include <boost/predef/os.h>

#include <cstdlib>
#include <pwd.h>
#include <unistd.h>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace application { namespace detail {

    class default_path_impl
    {
        fs::path full_path_;
        
        fs::path path_from_me(error_code_t& ec)  {
            return fs::read_symlink("/proc/self/exe", ec);
        }

        fs::path getenv(const char* env_name)
        {
            const char* res = ::getenv(env_name);
            return res ? res : fs::path();
        }

    public:

        fs::path current_path(void)
        {
           return fs::current_path();
        }

        const fs::path& location(error_code_t& ec)
        {
            if(!full_path_.empty())
                return full_path_;

            fs::path full_path
                = path_from_me(ec);
            if(ec)
                full_path_ = fs::path();

            full_path_ = full_path;

            return full_path_;
        }

        const fs::path& location()
        {
            if(!full_path_.empty())
                return full_path_;

            error_code_t ec;

            full_path_ = location(ec);

            if (ec) {
#ifdef ASIO_STANDALONE
                boost::throw_exception(
                    std::system_error(
                    ec, "location() failed"
                    ));
#else
                boost::throw_exception(
                    boost::system::system_error(
                    ec, "location() failed"
                    ));
#endif
            }

            return full_path_;
        }

        inline fs::path home_path()
        {
            fs::path path = getenv("HOME");
            if(path.empty())
            {
                struct passwd* pwd = getpwuid(getuid());
                if(pwd)
                    return pwd->pw_dir;
                return ".";
            }
            return path;
        }

        inline fs::path app_data_path()
        {
            fs::path path = getenv("XDG_DATA_HOME");
            if(path.empty()) {
    #if BOOST_OS_MACOS
                return home_path() / "Library/Preferences/";
    #else
                return home_path() / ".local/share";
    #endif
            }
            return path;
        }

        inline fs::path config_path()
        {

            fs::path path = getenv("XDG_CONFIG_HOME");
            if(path.empty()) {
    #if BOOST_OS_MACOS
                return home_path() / "Library/Preferences/";
    #else
                return home_path() / ".config";
    #endif
            }
            return path;
        }

        inline fs::path temp_path()
        {
            fs::path path = getenv("TMPDIR");
            if(path.empty())
                return "/tmp"; // Fallback if TMPDIR not available
            return path;
        }
    };

}}} // namespace boost::dll::detail

#endif // BOOST_APPLICATION_DETAIL_POSIX_PATH_FROM_ME_HPP
