// wait_for_termination_request_impl.hpp  ------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 15-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_WAIT_FOR_TERMINATION_REQUEST_IMPL_HPP
#define BOOST_APPLICATION_WAIT_FOR_TERMINATION_REQUEST_IMPL_HPP

namespace boost { namespace application {

   class wait_for_termination_request_impl : noncopyable
   {
   public:

      wait_for_termination_request_impl()
         : terminate_event_(NULL)
      {
         // create the termination event
         terminate_event_ = CreateEvent (0, TRUE, FALSE, 0);
      }

      virtual ~wait_for_termination_request_impl()
      {
         if (terminate_event_)
            CloseHandle(terminate_event_);
      }

      void wait()
      {
         // Wait for stop signal, and then terminate
         WaitForSingleObject(terminate_event_, INFINITE);
      }

      void proceed()
      {
         if (terminate_event_)
            // set the event that is holding main
            // so that app can continue
            SetEvent(terminate_event_);
      }

   private:

      // windowns terminate event ctrl
      HANDLE terminate_event_;

   };

}} // boost::application

#endif // BOOST_APPLICATION_WAIT_FOR_TERMINATION_REQUEST_IMPL_HPP

