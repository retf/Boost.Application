// shared_library.hpp ---------------------------------------------------------//
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
   class shared_library_impl : public noncopyable 
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
      shared_library_impl(const library_type<T> &sl)
         : handle_(NULL)
      {
         load(sl);
      }

      template <typename T>
      shared_library_impl(const library_type<T> &sl, boost::system::error_code &ec)
         : handle_(NULL)
      {
         load(sl, ec);
      }

      template <typename T>
      shared_library_impl(const library_type<T> &sl, unsigned long mode)
         : handle_(NULL)
      {
         load(sl, mode);
      }

      template <typename T>
      shared_library_impl(const library_type<T> &sl, unsigned long mode, 
                          boost::system::error_code &ec)
         : handle_(NULL)
      {
         load(sl, mode, ec);
      }

      template <typename T>
      void load(const library_type<T> &sl)
      {
         if (handle_) 
            unload();

         path_ = sl.get().c_str();

         boost::system::error_code ec;
         if(!load(RTLD_LAZY | RTLD_GLOBAL, ec))
            BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR("dlopen() failed");
      }

      template <typename T>
      void load(const library_type<T> &sl, 
                boost::system::error_code &ec)
      {
         if (handle_) 
            unload();

         path_ = sl.get().c_str();

         load(RTLD_LAZY | RTLD_GLOBAL, ec);// usual mode, generic
      }

      template <typename T>
      void load(const library_type<T> &sl, unsigned long mode)
      {
         path_ = sl.get().c_str();

         boost::system::error_code ec;
         if(!load(static_cast<unsigned long>(mode), ec))
            BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR("dlopen() failed");
      }

      template <typename T>
      void load(const library_type<T> &sl, unsigned long mode, 
                boost::system::error_code &ec)
      {
         path_ = sl.get().c_str();
         load(static_cast<unsigned long>(mode), ec);
      }

      void unload()
      {
         boost::lock_guard<boost::mutex> lock(mutex);

         if (handle_)
         {
            dlclose(handle_);
            handle_ = 0;
         }

         path_.clear();
      }

      bool is_loaded() const
      {
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
      void* get_symbol(const symbol_type<T> &sb) 
      {
         boost::system::error_code ec; 

         void* symbol = symbol_addr(sb, ec);
         if(symbol == NULL)
            BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR("dlsym() failed");

         return symbol;
      }

      template <typename T>
      void* get_symbol(const symbol_type<T> &sb, boost::system::error_code &ec)
      {
         return symbol_addr(sb, ec);
      }

      template <typename T>
      void* operator()(const symbol_type<T> &sb)
      {
         boost::system::error_code ec;

         void* symbol = symbol_addr(sb, ec);
         if(symbol ==  NULL)
            BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR("dlsym() failed");

         return symbol;
      }

      template <typename T>
      void* operator()(const symbol_type<T> &sb, boost::system::error_code &ec)
      {
         return symbol_addr(sb, ec);
      }

      const boost::filesystem::path& get_path() const
      {
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
         boost::lock_guard<boost::mutex> lock(mutex);

         void* symbol = 0;

         if (handle_)
         {
            // dlsym - obtain the address of a symbol from a dlopen object
            symbol = dlsym(handle_, sb.get().c_str());
         }

         if(symbol == NULL)
            BOOST_APPLICATION_SET_LAST_SYSTEM_ERROR(ec);

         // If handle does not refer to a valid object opened by dlopen(), 
         // or if the named symbol cannot be found within any of the objects 
         // associated with handle, dlsym() shall return NULL. 
         // More detailed diagnostic information shall be available through dlerror().

         return symbol;
      }
	  
      bool load(unsigned long mode, boost::system::error_code &ec)
      {
         boost::lock_guard<boost::mutex> lock(mutex);

         handle_ = dlopen(path_.string().c_str(), static_cast<int>(mode));

         if (!handle_) 
         {
            BOOST_APPLICATION_SET_LAST_SYSTEM_ERROR(ec);
            return false;
         }

         return true;
      }

	  private:
      
      boost::mutex mutex;  
      boost::filesystem::path path_;
      void* handle_;

   };

}} // boost::application

#endif // BOOST_APPLICATION_SHARED_LIBRARY_IMPL_HPP

