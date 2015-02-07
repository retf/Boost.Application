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

#include <cstdlib>
#include <shlobj.h>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace application { namespace detail {

    class default_path_impl
    {
        filesystem::path full_path_;
        
        boost::filesystem::path path_from_me(boost::system::error_code &ec) {
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

        boost::filesystem::path getenv(const char* env_name)
        {
    #if defined(_MSC_VER) && _MSC_VER >= 14
            std::vector<char> buf;
            std::size_t req_size = 0;

            ::getenv_s(&req_size, NULL, 0, env_name);
            if(req_size == 0)
                return boost::filesystem::path();

            if(buf.size() < req_size)
                buf.resize(req_size);

            ::getenv_s(&req_size, buf.data(), buf.size(), env_name);
            if(req_size < buf.size())
                buf.resize(req_size);

            return buf.data();
    #else
            const char* res = ::getenv(env_name);
            return res ? res : boost::filesystem::path();
    #endif
        }

    public:

        filesystem::path current_path(void)
        {
           return filesystem::current_path();
        }

        const filesystem::path& location(boost::system::error_code &ec)
        {
            if(!full_path_.empty())
                return full_path_;

            boost::filesystem::path full_path
                = path_from_me(ec);
            if(ec)
                full_path_ = boost::filesystem::path();
            
            full_path_ = full_path;

            return full_path_;
        }

        const filesystem::path& location()
        {
            if(!full_path_.empty())
                return full_path_;

            boost::system::error_code ec;

            full_path_ = location(ec);

            if (ec) {
                boost::throw_exception(
                    boost::system::system_error(
                    ec, "location() failed"
                    ));
            }

            return full_path_;
        }
    
        boost::filesystem::path home_path()
        {
    #if NTDDI_VERSION >= 0x06010000
            wchar_t* res = NULL;
            if(SUCCEEDED(::SHGetKnownFolderPath(
                             FOLDERID_Profile, 0, NULL, (PWSTR*)res))) {
                boost::filesystem::path p(res);
                CoTaskMemFree(static_cast<void*>(res));
                return p;
            }
    #else
            TCHAR res[MAX_PATH];
            if(SUCCEEDED(::SHGetFolderPath(
                             NULL, CSIDL_PROFILE, NULL, 0, res)))
                return res;
    #endif
    
            boost::filesystem::path ret = getenv("USERPROFILE");
            if(ret.empty()) {
                ret = getenv("HOMEDRIVE") / getenv("HOMEPATH");
                if(ret.empty()) {
                    ret = getenv("HOME");
                    if(ret.empty())
                        return boost::filesystem::path(".");
                }
            }
    
            return ret;
        }
    
        boost::filesystem::path app_data_path()
        {
    #if NTDDI_VERSION >= 0x06010000
            wchar_t* ret = NULL;
            if(SUCCEEDED(::SHGetKnownFolderPath(
                             FOLDERID_RoamingAppData, 0, NULL, (PWSTR*)ret))) {
                boost::filesystem::path p(ret);
                CoTaskMemFree(static_cast<void*>(ret));
                return p;
            }
    #else
            TCHAR ret[MAX_PATH];
            if(SUCCEEDED(::SHGetFolderPath(
                             NULL, CSIDL_APPDATA, NULL, 0, ret)))
                return ret;
    #endif
            return boost::filesystem::path(".");
        }
    
        boost::filesystem::path config_path()
        {
            return app_data_path();
        }
    
        boost::filesystem::path temp_path()
        {
            // taken from msdn example for GetTempPath()
            DWORD ret = ::GetTempPath(0, (char*)"");
    
            if(ret == 0)
                return boost::filesystem::path(".");
    
            std::vector<TCHAR> res(ret);
    
            ret = ::GetTempPath(static_cast<DWORD>(res.size()), res.data());
            if(ret == 0 || ret > res.size())
                return boost::filesystem::path(".");
    
            return res.data();
        }
    public:
    };

    

}}} // namespace boost::dll::detail

#endif // BOOST_APPLICATION_DETAIL_WINDOWS_PATH_FROM_ME_HPP

