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

#include <stdio.h>
#include <signal.h>

#include <boost/application/aspects/selfpipe.hpp>

namespace boost { namespace application {

#if defined( USE_POSIX_WAIT_FOR_TERMINATION_REQUEST_YELD_BASED )
   class wait_for_termination_request_impl : noncopyable
   {
   public:

      wait_for_termination_request_impl()
         : run_(true)
      {
      }

      // will wait for termination request
      void wait()
      {
         while (run_) { boost::this_thread::yield(); }
      }

      void proceed() 
      {
         run_ = false;
      }

   private:

      bool run_;

   }; 
#elif defined( USE_POSIX_WAIT_FOR_TERMINATION_REQUEST_SIGUSR1_BASED )
   class wait_for_termination_request_impl : noncopyable
      // http://www.cs.kent.edu/~farrell/sp/lectures/signals.html
   {
   public:

      // will wait for termination request
      void wait()
      {
         sigset_t sset;

         sigemptyset(&sset);

         //sigaddset(&sset, SIGINT);
         //sigaddset(&sset, SIGQUIT);
         //sigaddset(&sset, SIGTERM);

         sigaddset(&sset, SIGUSR1);

         sigprocmask(SIG_BLOCK, &sset, NULL);

         int sig;
         sigwait(&sset, &sig);
      }

      void proceed() 
	    {
         raise(SIGUSR1);
      }
   };
#else // DRFAULT WAY
   class wait_for_termination_request_impl : noncopyable
   {
   public:

      // will wait for termination request
      void wait()
      {
         fd_set readfds;
         FD_ZERO(&readfds);
         FD_SET(selfpipe_.read_fd(), &readfds);

         // block and wait
         while(select(selfpipe_.read_fd() + 1, &readfds, 0, 0, 0) == -1 && errno == EINTR)
         {
            // nothing here, restart when signal is catch
         }
      }

      void proceed() 
      {
         selfpipe_.poke();
      }

   private:

      application::selfpipe selfpipe_;

   };
#endif

}} // boost::application

#endif // BOOST_APPLICATION_WAIT_FOR_TERMINATION_REQUEST_IMPL_HPP

