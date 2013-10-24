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
   class server_application_impl_ : noncopyable
   {
   public:

      typedef boost::function< int (application::context&) > main_parameter;
      typedef boost::function< int (void) >                  main_singleton;

      // string types to be used internaly to handle unicode on windows
      typedef CharType char_type;
      typedef std::basic_string<char_type> string_type;

      server_application_impl_(const main_parameter &main, 
                               application::context &context, 
                               boost::system::error_code& ec)
         : context_(context)
         , main_parameter_(main)
      {      
         type_ = parameter;
         
         // need check this
         process_id_ = daemonize(ec);
      }

      server_application_impl_(const main_singleton &main, 
                               singularity<application::context> &context, 
                               boost::system::error_code& ec)
         : context_(context.get_global())
         , main_singleton(main)
      {       
         type_ = singleton;

         // need check this
         process_id_ = daemonize(ec);
      }

      int run()
      {
         if(type_ == parameter)
         {
            return main_parameter_(context_); 
         }
         else if(type_ == singleton)
         {
            return main_singleton_();
         }
      }

   protected:

      pid_t daemonize(boost::system::error_code &ec)
      {
         pid_t pid;

         struct rlimit rl;
         struct sigaction sa;

         // Clear file creation mask.
         umask(0);

         // Get maximum number of file descriptors.
         if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
         {
            // can't getrlimit 
            ec = last_error_code();
         }

         // Become a session leader to lose controlling TTY.

         if ((pid = fork()) < 0)
         {
            // can't fork
            ec = last_error_code();
         }
         else if (pid != 0) 
         {
            exit(0); // parent exits
         }

         // Chield continues 

         // Obtain a new process group
         setsid(); 

         // Ensure future opens won't allocate controlling TTYs.

         sa.sa_handler = SIG_IGN;
         sigemptyset(&sa.sa_mask);
         sa.sa_flags = 0;

         if (sigaction(SIGHUP, &sa, NULL) < 0)
         {
            // can't ignore SIGHUP
            ec = last_error_code();
         }

         if ((pid = fork()) < 0)
         {
            // can't fork
            ec = last_error_code();
         }
         else if (pid != 0) // parent 
         {
            exit(0);
         }

         //
         // Change the current working directory to the root so
         // we won't prevent file systems from being unmounted.
         if (chdir("/") < 0)
         {
            // can't change directory to /
            ec = last_error_code();
         }

         // Close all open file descriptors.
         if (rl.rlim_max == RLIM_INFINITY)
         {
            rl.rlim_max = 1024;
         }

         for (int i = 0; i < (int)rl.rlim_max; i++)
         {
            close(i);
         }

         // Attach file descriptors 0, 1, and 2 to /dev/null.

         int fd0, fd1, fd2;

         fd0 = open("/dev/null", O_RDWR);
         fd1 = dup(0);
         fd2 = dup(0);

         return pid;
      }

   private:

      pid_t process_id_;

      main_parameter main_parameter_;
      main_singleton main_singleton_;

      enum instantiation_type
      {
         parameter,
         singleton

      } type_;

      application::context &context_;

   };
	
   /////////////////////////////////////////////////////////////////////////////
   // server_application_impl
   //

   // args versions for Multi-Byte string and Unicode string
   typedef server_application_impl_<character_types::char_type> server_application_impl;
   // wchar_t / char
   
}} // boost::application 

#endif // BOOST_APPLICATION_SERVER_APPLICATION_IMPL_HPP

