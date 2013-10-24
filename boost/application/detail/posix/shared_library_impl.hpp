// shared_library_impl.hpp ---------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2012 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 05-04-2012 dd-mm-yyyy - Initial Release
// 16-09-2013 dd-mm-yyyy - Refatored
// 
// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_SHARED_LIBRARY_IMPL_HPP
#define BOOST_APPLICATION_SHARED_LIBRARY_IMPL_HPP

#include <iostream>

#include <boost/application/config.hpp>
#include <boost/application/shared_library_initializers.hpp>
#include <boost/application/shared_library_load_mode.hpp>

#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/filesystem/path.hpp>

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>

namespace boost { namespace application {

   class shared_library;
   class shared_library_impl : noncopyable 
   {
      friend class shared_library;

   public:
      shared_library_impl()
         : handle_(NULL)
      {
      }
   		
      virtual ~shared_library_impl()
      {
         unload();
      }
    
      template <typename T>
      shared_library_impl(const library_type<T> &sl, boost::system::error_code &ec)
         : handle_(NULL)
      {
         load(sl, ec);
      }

      template <typename T>
      shared_library_impl(const library_type<T> &sl, shared_library_load_mode mode, 
                          boost::system::error_code &ec)
         : handle_(NULL)
      {
         load(sl, mode, ec);
      }

      template <typename T>
      void load(const library_type<T> &sl, 
                boost::system::error_code &ec)
      {
         boost::lock_guard<boost::mutex> lock(mutex_);

         if (handle_) 
            unload(lock);

         path_ = sl.get().c_str();

         load(RTLD_LAZY | RTLD_GLOBAL, ec, lock);// usual mode, generic
      }

      template <typename T>
      void load(const library_type<T> &sl, shared_library_load_mode mode, 
                boost::system::error_code &ec)
      {
         boost::lock_guard<boost::mutex> lock(mutex_);

         path_ = sl.get().c_str();
         load(static_cast<unsigned long>(mode), ec, lock);
      }

      void unload()
      {
         boost::lock_guard<boost::mutex> lock(mutex_);
         unload(lock);
      }

      bool is_loaded() 
      {
         boost::lock_guard<boost::mutex> lock(mutex_);
         return (handle_ != 0); 
      }

      template <typename T>
      bool search_symbol(const symbol_type<T> &sb) 
      {
         boost::system::error_code ec;
         if(get_symbol(sb, ec) == NULL)
            return false;

         return true;
      }

      template <typename T>
      void* get_symbol(const symbol_type<T> &sb, boost::system::error_code &ec)
      {
         return symbol_addr(sb, ec);
      }

      const boost::filesystem::path get_path()
      {
         boost::lock_guard<boost::mutex> lock(mutex_);
         return path_;
      }

      static character_types::string_type suffix()
      {
// https://sourceforge.net/p/predef/wiki/OperatingSystems/
#if defined(__APPLE__)
         return character_types::string_type(".dylib");
#else
         return character_types::string_type(".so");
#endif
      }
	  
   protected:
	  
      template <typename T>
      void* symbol_addr(const symbol_type<T> &sb, boost::system::error_code &ec)
      {
         boost::lock_guard<boost::mutex> lock(mutex_);

         void* symbol = 0;

         if (handle_)
         {
            // dlsym - obtain the address of a symbol from a dlopen object
            symbol = dlsym(handle_, sb.get().c_str());
         }

         if(symbol == NULL)
            ec = boost::application::last_error_code();

         // If handle does not refer to a valid object opened by dlopen(), 
         // or if the named symbol cannot be found within any of the objects 
         // associated with handle, dlsym() shall return NULL. 
         // More detailed diagnostic information shall be available through dlerror().

         return symbol;
      }
	  
      bool load(unsigned long mode, boost::system::error_code &ec, boost::lock_guard<boost::mutex> &lock)
      {
         handle_ = dlopen(path_.string().c_str(), static_cast<int>(mode));

         if (!handle_) 
         {
            ec = boost::application::last_error_code();
            return false;
         }

         return true;
      }

      void unload(boost::lock_guard<boost::mutex> &lock)
      {
         if (handle_)
         {
            dlclose(handle_);
            handle_ = 0;
         }

         path_.clear();
      }

	  private:
      
      boost::mutex mutex_;  
      boost::filesystem::path path_;
      void* handle_;

   };

}} // boost::application

#endif // BOOST_APPLICATION_SHARED_LIBRARY_IMPL_HPP

