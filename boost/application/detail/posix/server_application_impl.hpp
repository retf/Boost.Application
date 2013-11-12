// server_application_impl.hpp -----------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 22-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_SERVER_APPLICATION_IMPL_HPP
#define BOOST_APPLICATION_SERVER_APPLICATION_IMPL_HPP

#include <boost/application/config.hpp>
#include <boost/application/context.hpp>
#include <boost/application/detail/application_impl.hpp>
#include <boost/application/signal_binder.hpp>

#include <boost/assert.hpp>
#include <boost/function.hpp>

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>

#include <sys/stat.h>

namespace boost { namespace application {

   template <typename CharType>
   class server_application_impl_ : public application_impl
   {
   public:

      typedef boost::function< int (application::context&) > main_parameter;
      typedef boost::function< int (void) >                  main_singleton;

      // string types to be used internaly to handle unicode on windows
      typedef CharType char_type;
      typedef std::basic_string<char_type> string_type;

      server_application_impl_(const main_parameter &main,
                               signal_binder &sb,
                               application::context &context,
                               boost::system::error_code& ec)
         : application_impl(parameter, context)
         , main_parameter_(main)
      {

         process_id_ = daemonize(ec);
         sb.start(); // need be started after daemonize
      }

      server_application_impl_(const main_singleton &main,
                               signal_binder &sb,
                               singularity<application::context> &context,
                               boost::system::error_code& ec)
         : application_impl(singleton, context.get_global())
         , main_singleton(main)
      {

         process_id_ = daemonize(ec);
         sb.start(); // need be started after daemonize
      }

      int run()
      {
         switch(type_)
         {
           case parameter: return main_parameter_(context_);
           case singleton: return main_singleton_();
           default: BOOST_ASSERT_MSG(false, "boost::application type is not implemented");
         }
         return 0;
      }

   protected:

      // create a daemon using fork().
      //
      // the fork() creates a new process by duplicating
      // the calling process.
      //
      // here the parent process would exit leaving the the child process behind.
      //
      // this child process detaches from the controlling terminal, reopens all
      // of {stdin, stdout, stderr} to /dev/null, and changes the working directory
      // to the root directory. (based on flags).
      //
      // under linux, fork() is implemented using copy-on-write pages, so the only
      // penalty that it incurs is the time and memory required to duplicate the
      // parent's page tables, and to create a unique task structure for the child.

      pid_t daemonize(boost::system::error_code &ec)
      {
         // ignore terminal stop signals
         #ifdef SIGTTOU
         signal(SIGTTOU, SIG_IGN);
         #endif

         #ifdef SIGTTIN
         signal(SIGTTIN, SIG_IGN);
         #endif

         #ifdef SIGTSTP
         signal(SIGTSTP, SIG_IGN);
         #endif

         // allow parent shell to continue.
         // ensure the process is not a process group leader.

         pid_t fork_pid = fork();

         if(fork_pid < 0) {
            // fork error
            ec = last_error_code();
            return 0;
         } else if (fork_pid != 0){
            exit(0); // parent exits
         }

         // child from here
         //

         // obtain a new process group
         setsid();

         // lose controlling terminal & change process group
         setpgrp();

         // Ensure future opens won't allocate controlling TTYs.

         struct rlimit rl;
         struct sigaction sa;

         sa.sa_handler = SIG_IGN;
         sigemptyset(&sa.sa_mask);
         sa.sa_flags = 0;

         if (sigaction(SIGHUP, &sa, NULL) < 0)
         {
            // can't ignore SIGHUP
            ec = last_error_code(); return 0;
         }

         fork_pid = fork(); // become non-pgrp-leader

         if(fork_pid < 0) {
            // fork error
            ec = last_error_code();
            return 0;
         } else if (fork_pid != 0){
            exit(0); // parent exits again
         }

         // change the current working directory to the root so
         // we won't prevent file systems from being unmounted.
         if (chdir("/") < 0)
         {
            // can't change directory to /
            ec = last_error_code();
         }

         // close all open file descriptors.
         if (rl.rlim_max == RLIM_INFINITY)
         {
            rl.rlim_max = 1024;
         }

         for (int i = 0; i < (int)rl.rlim_max; i++)
         {
            close(i);
         }

         // Attach file descriptors 0, 1, and 2 to /dev/null.

         open("/dev/null", O_RDWR);
         dup(0);
         dup(0);

         // clear any inherited file mode creation mask
         umask(0);

         return fork_pid;
      }

   private:

      pid_t process_id_;

      main_parameter main_parameter_;
      main_singleton main_singleton_;

   };

   /////////////////////////////////////////////////////////////////////////////
   // server_application_impl
   //

   // args versions for Multi-Byte string and Unicode string
   typedef server_application_impl_<character_types::char_type> server_application_impl;
   // wchar_t / char

}} // boost::application

#endif // BOOST_APPLICATION_SERVER_APPLICATION_IMPL_HPP

