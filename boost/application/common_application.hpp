// common_application.hpp ----------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 10-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_COMMON_APPLICATION_HPP
#define BOOST_APPLICATION_COMMON_APPLICATION_HPP

#include <boost/application/config.hpp>
#include <boost/application/context.hpp>
// internal aspects
#include <boost/application/status_aspect.hpp>
#include <boost/application/run_mode_aspect.hpp>
// public aspects
#include <boost/application/ready_to_use_aspects.hpp>

namespace boost { namespace application {

   BOOST_APPLICATION_FEATURE_SELECT

   class common
   {
   public:
      common(application::context &context, boost::system::error_code& ec)
      {

         // default aspects patterns added to this kind of application

         context.add_aspect_if_not_exists<run_mode>(
            make_shared<run_mode>(run_mode::common));

         context.add_aspect_if_not_exists<status>(
            make_shared<status>(status::running));

         context.add_aspect_if_not_exists<wait_for_termination_request>(
            shared_ptr<wait_for_termination_request>(new wait_for_termination_request));
      }

   };
	
}} // boost::application 

#endif // BOOST_APPLICATION_COMMON_APPLICATION_HPP
       