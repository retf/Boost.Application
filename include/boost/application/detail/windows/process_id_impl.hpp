// Copyright 2014 Renato Tegon Forti, Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_APPLICATION_DETAIL_PROCESS_ID_HPP
#define BOOST_APPLICATION_DETAIL_PROCESS_ID_HPP

#include <boost/application/config.hpp>
#include <boost/application/detail/csbl.hpp>
#include <boost/application/system_error.hpp>

#include <cstdlib>
#include <shlobj.h>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

namespace boost { namespace application { namespace detail {

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
    
}}} // namespace boost::application::detail

#endif // BOOST_APPLICATION_DETAIL_WINDOWS_PATH_FROM_ME_HPP

