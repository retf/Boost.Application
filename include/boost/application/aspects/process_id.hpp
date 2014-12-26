// pid.hpp  ------------------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2014 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 14-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_PID_ASPECT_HPP
#define BOOST_APPLICATION_PID_ASPECT_HPP

#include <boost/application/config.hpp>
#include <boost/application/detail/csbl.hpp>
/*
#if defined( BOOST_WINDOWS_API )
#include <boost/application/detail/windows/pid_impl.hpp>
#elif defined( BOOST_POSIX_API )
#include <boost/application/detail/posix/pid_impl.hpp>
#else
#error "Sorry, no boost application are available for this platform."
#endif
*/

// in dev

namespace boost { namespace application {

#if defined( BOOST_WINDOWS_API )

   class  process_id_impl {
      
   public:
      typedef DWORD native_pid_t;
      
      process_id_impl()
         : pid_ (0)  
      {}
      
      process_id_impl(native_pid_t pid)
         : pid_ (pid)  
      {}
      
      native_pid_t pid() BOOST_NOEXCEPT {
         if(pid_)
            return pid_;
            
         pid_ = ::GetCurrentProcessId();
         return pid_;
      }
        
   private:
      native_pid_t pid_;
   };

#elif defined( BOOST_POSIX_API )

   class  process_id_impl {
      
   public:
      typedef pid_t native_pid_t;
      
      process_id_impl()
         : pid_ (0)  
      {}
      
      process_id_impl(native_pid_t pid)
         : pid_ (pid)  
      {}
      
      native_pid_t pid() BOOST_NOEXCEPT {
         if(pid_)
            return pid_;
            
         pid_ = getpid();
         return pid_;
      }
        
   private:
      native_pid_t pid_;
   };

#else
#error "Sorry, no boost application are available for this platform."
#endif

   /*!
    * \brief A class aspect to be used to obtain a PID of proccess.
    *
    */
   class process_id 
      : public process_id_impl {
      typedef process_id_impl base_t;   
      
   public:
   
      typedef process_id_impl::native_pid_t native_pid_t;
   
      process_id() 
      {}
      
      process_id(native_pid_t p) 
         : base_t(p)
      {}
      
      virtual ~process_id() 
      {}

      /*!
       * Obtain a PID of proccess.
       *
       */
      native_pid_t pid() BOOST_NOEXCEPT {
         return base_t::pid();
      }
   };

}} // boost::application

#endif // BOOST_APPLICATION_PID_ASPECT_HPP

