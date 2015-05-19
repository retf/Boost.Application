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

#if NTDDI_VERSION < NTDDI_VISTA
#error at least the windows vista feature level of the windows sdk is required, e.g. include https://gist.github.com/BurningEnlightenment/05eef0ba2b569d5e2828 at the beginning of your prefix header
#endif

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace application { namespace detail {

    class default_path_impl
    {
        filesystem::path full_path_;
        
        static boost::filesystem::path path_from_me(boost::system::error_code &ec) {
            boost::filesystem::path ret;
            
            // A handle to the loaded module whose path is being requested.
            // If this parameter is NULL, GetModuleFileName retrieves the path of the
            // executable file of the current process.
            WCHAR path_hldr[MAX_PATH];
            LPWSTR path = path_hldr;
            ::GetModuleFileNameW(NULL, path, MAX_PATH);
            ec = last_error_code();
    
            // In case of ERROR_INSUFFICIENT_BUFFER_ trying to get buffer big enough to store the whole path
            for (unsigned i = 2; i < 129 && ec.value() == ERROR_INSUFFICIENT_BUFFER; i *= 2) {
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

        static boost::filesystem::path getenv(const char* env_name)
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

        static bool KnownFolderPath( REFKNOWNFOLDERID _Id, filesystem::path &out )
        {
            PWSTR res = NULL;
            if ( SUCCEEDED( ::SHGetKnownFolderPath( _Id, KF_FLAG_CREATE, NULL, &res ) ) )
            {
                out = res;
                CoTaskMemFree( static_cast<void*>( res ) );
                return true;
            }
            return false;
        }

    public:

        filesystem::path current_path(void)
        {
           return filesystem::current_path();
        }

        const filesystem::path& location(boost::system::error_code &ec)
        {
            if ( full_path_.empty( ) )
            {
                full_path_ = path_from_me(ec);
            }
            return full_path_;
        }

        const filesystem::path& location()
        {
            if ( full_path_.empty( ) )
            {
                boost::system::error_code ec;

                full_path_ = location( ec );

                if ( ec )
                {
                    boost::throw_exception(
                        boost::system::system_error(
                        ec, "location() failed"
                        ) );
                }
            }
            return full_path_;
        }
    
        boost::filesystem::path home_path()
        {
            boost::filesystem::path home( L"." );
            KnownFolderPath( FOLDERID_Profile, home );
            return home;
        }
    
        boost::filesystem::path app_data_path()
        {
            boost::filesystem::path p( L"." );
            KnownFolderPath( FOLDERID_LocalAppData, p );
            return p;
        }
    
        boost::filesystem::path config_path()
        {
            boost::filesystem::path p( L"." );
            KnownFolderPath( FOLDERID_RoamingAppData, p );
            return p;
        }
    
        boost::filesystem::path temp_path()
        {
            static const DWORD max_path_len = MAX_PATH + 1;
            wchar_t path[max_path_len] = L"";

            DWORD path_len = ::GetTempPathW( max_path_len, path );

            return boost::filesystem::path( path_len ? path : L"." );
        }
    };

    

}}} // namespace boost::dll::detail

#endif // BOOST_APPLICATION_DETAIL_WINDOWS_PATH_FROM_ME_HPP

