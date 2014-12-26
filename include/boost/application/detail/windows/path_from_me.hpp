// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_APPLICATION_DETAIL_WINDOWS_PATH_FROM_ME_HPP
#define BOOST_APPLICATION_DETAIL_WINDOWS_PATH_FROM_ME_HPP

#include <boost/application/config.hpp>
#include <boost/application/system_error.hpp>
#include <boost/filesystem/path.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace application { namespace detail {

    inline boost::filesystem::path path_from_me(boost::system::error_code &ec) {
        boost::filesystem::path ret;
        
        // A handle to the loaded module whose path is being requested.
        // If this parameter is NULL, GetModuleFileName retrieves the path of the
        // executable file of the current process.
        WCHAR path_hldr[MAX_PATH];
        LPWSTR path = path_hldr;
        ::GetModuleFileNameW(NULL, path, MAX_PATH);
        ec = last_error_code();

        // In case of ERROR_INSUFFICIENT_BUFFER_ trying to get buffer big enough to store the whole path
        for (unsigned i = 2; i < 1025 && ec.value() == ERROR_INSUFFICIENT_BUFFER; i *= 2) {
            path = new WCHAR[MAX_PATH * i];

            ::GetModuleFileNameW(NULL, path, MAX_PATH * i);
            ec = last_error_code();

            if (ec) {
                delete[] path;
            }
        }
        
        if (ec) {
            // Error other than ERROR_INSUFFICIENT_BUFFER_ occured or failed to allocate buffer big enough
            return boost::filesystem::path();
        }
        
        ret = path;
        if (path != path_hldr) {
            delete[] path;
        }
        
        return ret;
    }

}}} // namespace boost::dll::detail

#endif // BOOST_APPLICATION_DETAIL_WINDOWS_PATH_FROM_ME_HPP

