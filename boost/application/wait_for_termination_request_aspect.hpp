// args_aspect.hpp  ----------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 14-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_WAIT_FOR_TERMINATION_REQUEST_ASPECT_HPP
#define BOOST_APPLICATION_WAIT_FOR_TERMINATION_REQUEST_ASPECT_HPP

#include <boost/application/config.hpp>
#if defined( BOOST_WINDOWS_API )
#include <boost/application/impl/windows/wait_for_termination_request_impl.hpp>
#elif defined( BOOST_POSIX_API )
#include <boost/application/impl/posix/wait_for_termination_request_impl.hpp>
#else
#error "Sorry, no boost application are available for this platform."
#endif

namespace boost { namespace application {

   class wait_for_termination_request : noncopyable          
   {
   public:
      wait_for_termination_request()
         : impl_(new wait_for_termination_request_impl()) {}

      // wait for termination request
      void wait() {
         impl_->wait();
      }

      // continue, cause wait to be relesed
      void proceed() {
         impl_->proceed();
      }

   private:
      unique_ptr<wait_for_termination_request_impl> impl_;
   };
      
}} // boost::application

#endif // BOOST_APPLICATION_WAIT_FOR_TERMINATION_REQUEST_ASPECT_HPP

