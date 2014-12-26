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
#include <boost/application/detail/windows/path_from_me.hpp>
#elif defined( BOOST_POSIX_API )
#include <boost/application/detail/posix/path_from_me.hpp>
#else
#error "Sorry, no boost application are available for this platform."
#endif

namespace boost { namespace application {

    class path
    {
        filesystem::path extract_path(boost::system::error_code &ec) {

            boost::filesystem::path full_path 
                = boost::application::detail::path_from_me(ec);

            if(ec)
                return boost::filesystem::path();

            return full_path;
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

            full_path_ = extract_path(ec);

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

        const filesystem::path& executable_path_name(boost::system::error_code &ec)
        {
           return location(ec);
        }

        const filesystem::path& executable_path_name()
        {
            return location();
        }

        filesystem::path executable_path(boost::system::error_code &ec)
        {
            return location(ec).parent_path();
        }

        filesystem::path executable_path()
        {
            return location().parent_path();
        }

        filesystem::path executable_full_name(boost::system::error_code &ec)
        {
            return location(ec).filename();
        }

        filesystem::path executable_full_name(void)
        {
            return location().filename();
        }

        filesystem::path executable_name(boost::system::error_code &ec)
        {
            return location(ec).stem();
        }

        filesystem::path executable_name(void)
        {
            return location().stem();
        }

    private:

        filesystem::path full_path_;
    };

}} // boost::application

#endif // BOOST_APPLICATION_PATH_ASPECT_HPP

