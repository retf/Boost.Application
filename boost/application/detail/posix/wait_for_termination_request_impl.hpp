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
      // http://www.cs.kent.edu/~farrell/sp/lectures/signals.html      
   {
   public:

      // will wait for termination request
      void wait()
      {
         sigset_t sset;

         sigemptyset(&sset);

         sigaddset(&sset, SIGINT);
         sigaddset(&sset, SIGQUIT);
         sigaddset(&sset, SIGTERM);

         sigprocmask(SIG_BLOCK, &sset, NULL);

         int sig;
         sigwait(&sset, &sig);
      }

      void proceed() {
      }

   private:

   };
      
}} // boost::application

#endif // BOOST_APPLICATION_WAIT_FOR_TERMINATION_REQUEST_IMPL_HPP

