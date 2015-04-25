// server_application_impl.hpp -----------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2014 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 22-10-2013 dd-mm-yyyy - Initial Release

/*
---  0---|--- 10---|--- 20---|--- 30---|--- 40---|--- 50---|--- 60---|--- 70---|--- 80---|--- 90---|
123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|
*/

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

   /*
      Note about debug code on GDB
      
      # Debug fork
      
      set follow-fork-mode <mode>
      
      Set the debugger response to a program call of fork or vfork. A call to fork or vfork creates 
      a new process. The <mode> argument can be:

      parent: The original process is debugged after a fork. The child process runs unimpeded. 
      This is the default.

      child: The new process is debugged after a fork. The parent process runs unimpeded.

      E.G.: 
      
      set follow-fork-mode child
      
      # Debug signal
      
      handle <SIGNAL> nostop noprint pass   
      
      E.G.: 
      
      handle SIGINT nostop noprint pass   
   */

   template <typename CharType>
   class server_application_impl_ : public application_impl
   {
   public:

      // callback for app code
      typedef csbl::function< int (void) > mainop;

      // string types to be used internaly to handle unicode on windows
      typedef CharType char_type;
      typedef std::basic_string<char_type> string_type;

      server_application_impl_(const mainop &main, signal_binder &sb,
                               application::context &context, boost::system::error_code& ec)
         : application_impl(context)
         , main_(main)
      {
         // ver 1
#if defined( USE_DAEMONIZE_VER_1 )
         process_id_ = daemonize(ec);
#else        
         if(daemon(0, 0, ec) < 0)
         {
            ec = last_error_code();
            return;
         }
         
         process_id_ = getpid();
#endif
         
         sb.start(); // need be started after daemonize
      }

      int run()
      {
         return main_();
      }

   protected:
   
      //
      // ver 2
      
      // the ver 2 contains a version of a BSD-style daemon(3)
      // function, a function to "daemonize" the calling process.  This
      // implementation is based both on the generic daemon logic defined in
      // the Unix Programmer's FAQ and on the daemon_start() function in
      // W. Richard Stevens' _Unix_Network_Programming_ book (Prentice-Hall,
      // 1990).  At the time of this writing, the Unix Programmer's FAQ is
      // located at `http://www.whitefang.com/unix/faq_toc.html' (among
      // other places).
   
      // redirect_fds(): redirect stdin, stdout, and stderr to /dev/NULL */

      void redirect_fds(boost::system::error_code &ec)
      {
         (void) close(0);
         (void) close(1);
         (void) close(2);

         if (open("/dev/null", O_RDWR) != 0)
         {
            //syslog(LOG_ERR, "Unable to open /dev/null: %s", strerror(errno));
            //exit(1);
            ec = boost::application::last_error_code();
            return;
         }

         (void) dup(0);
         (void) dup(0);
      }

      int do_fork()
      {
         int status = 0;

         switch(fork())
         {
            case 0:
               // this is the child that will become the daemon. 
            break;

            case -1:
               // fork failure.             
               status = -1;
            break;

            default:
               // parent: Exit. 
               _exit(0);
         }

         return status;
      }
      
      int daemon(int nochdir, int noclose, boost::system::error_code &ec)
      {
         int status = 0;

         //openlog("daemonize", LOG_PID, LOG_DAEMON);

         // fork once to go into the background.
         if((status = do_fork()) < 0 )
            ;
         // create new session
         else if(setsid() < 0)              // shouldn't fail 
            status = -1;
         // fork again to ensure that daemon never reacquires a control terminal. 
         else if((status = do_fork()) < 0 )
            ;
         else
         {
            // clear any inherited umask(2) value 

            umask(0);

            // we're there.

            if(!nochdir)
            {
               // go to a neutral corner. 
               chdir("/");
            }

            if(!noclose)
            {
               redirect_fds(ec);
            }
         }

         return status;
      }
      
      //
      // ver 1

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
         setpgid(0, 0);

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

      // app code
      mainop main_;
   };

   /////////////////////////////////////////////////////////////////////////////
   // server_application_impl
   //

   // args versions for Multi-Byte string and Unicode string
   typedef server_application_impl_<character_types::char_type> server_application_impl;
   // wchar_t / char

}} // boost::application

#endif // BOOST_APPLICATION_SERVER_APPLICATION_IMPL_HPP

