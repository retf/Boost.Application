// Copyright 2014 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_APPLICATION_PATH_ASPECT_HPP
#define BOOST_APPLICATION_PATH_ASPECT_HPP

#include <boost/application/config.hpp>
#include <boost/application/detail/csbl.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#if defined( BOOST_WINDOWS_API )
#include <boost/application/detail/windows/path_impl.hpp>
#elif defined( BOOST_POSIX_API )
#include <boost/application/detail/posix/path_impl.hpp>
#else
#error "Sorry, no boost application are available for this platform."
#endif

namespace boost { namespace application {


    class path 
    {
    public:
        path() : impl_(new detail::default_path_impl) {}

        fs::path current_path(void)
        {
            return impl_->current_path();
        }
      
        fs::path location(error_code_t& ec)
        {
            return impl_->location(ec);
        }

        fs::path location()
        {
            return impl_->location();
        }

        fs::path executable_path_name(error_code_t& ec)
        {
           return impl_->location(ec);
        }

        fs::path executable_path_name()
        {
            return impl_->location();
        }

        fs::path executable_path(error_code_t& ec)
        {
            return location(ec).parent_path();
        }

        fs::path executable_path()
        {
            return impl_->location().parent_path();
        }

        fs::path executable_full_name(error_code_t& ec)
        {
            return impl_->location(ec).filename();
        }

        fs::path executable_full_name(void)
        {
            return impl_->location().filename();
        }

        fs::path executable_name(error_code_t& ec)
        {
            return impl_->location(ec).stem();
        }

        fs::path executable_name(void)
        {
            return impl_->location().stem();
        }

        fs::path home_path(void)
        {
            return impl_->home_path();
        }

        fs::path config_path(void)
        {
            return impl_->config_path();
        }

        fs::path app_data_path(void)
        {
            return impl_->app_data_path();
        }

        fs::path temp_path(void)
        {
            return impl_->temp_path();
        }

    private:
        csbl::shared_ptr<detail::default_path_impl> impl_;
    };

}} // boost::application

#endif // BOOST_APPLICATION_PATH_ASPECT_HPP

