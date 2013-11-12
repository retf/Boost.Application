// selfpipe.hpp  --------------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 08-11-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

// test aspet (indev)

#ifndef BOOST_APPLICATION_SELFPIPE_ASPECT_HPP
#define BOOST_APPLICATION_SELFPIPE_ASPECT_HPP

#include <boost/application/config.hpp>
#include <boost/application/base_type.hpp>

#include <unistd.h>
#include <fcntl.h>

namespace boost { namespace application {

   // If an application has selfpipe aspect, the selfpipe::setup() method must be
   // called during application's initialization, and selfpipe::teardown() during
   // termination.

   class selfpipe : noncopyable
   {
   
   public:
   
   selfpipe()
   {
      boost::system::error_code ec;
	  
      setup(ec);
	  if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR("selfpipe() failed");
   }
   
   selfpipe(boost::system::error_code &ec)
   {
      setup(ec);
   }
   
   virtual ~selfpipe()
   {
      teardown();
   }
   
   protected:

      void setup(boost::system::error_code &ec) 
      {
	     if (pipe(fd_) == -1)
		 {
            ec = last_error_code(); return 0;
		 }

         fcntl(fd_[readfd], F_SETFL, 
            fcntl(fd_[readfd], F_GETFL) | O_NONBLOCK);
		 
         fcntl(fd_[writefd], F_SETFL, 
		    fcntl(fd_[writefd], F_GETFL) | O_NONBLOCK);
      }

      void teardown() 
      {
         close(fd_[readfd]);
         close(fd_[writefd]);
      }

   public:

      int read_fd() const 
      { 
         return fd_[readfd]; 
      }
	  
	  int write_fd() const 
	  { 
		 return fd_[writefd]; 
	  }
	  
      void poke() 
      { 
         write(fd_[writefd], "", 1); 
      }

   private:
   
      enum { readfd = 0, writefd = 1 };
	  
      int fd_[2];
	  
   };

}} // boost::application

#endif // BOOST_APPLICATION_SELFPIPE_ASPECT_HPP


