//  application_impl.hpp  ---------------------------------------------------//
// -----------------------------------------------------------------------------

//  Copyright 2011-2012 Renato Tegon Forti

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 06-01-2012 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_LIMIT_SINGLE_INSTANCE_ASPECT_HPP
#define BOOST_APPLICATION_LIMIT_SINGLE_INSTANCE_ASPECT_HPP

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

#include <boost/application/config.hpp>
#if defined( BOOST_WINDOWS_API )
#include <boost/application/impl/windows/limit_single_instance_impl.hpp>
#elif defined( BOOST_POSIX_API )
#include <boost/application/impl/posix/limit_single_instance_impl.hpp>
#else
#error "Sorry, no boost application are available for this platform."
#endif

namespace boost { namespace application { 

   // this aspect ensure that only a single instance of application (process) 
   // is running on a given operating system.
   // Note that the automatic behavior of limit_single_instance is just 
   // evaluated in automatically way when the application is 
   // started by the launcher.
   class limit_single_instance : noncopyable
   {
   public:

      // callback methods   
      typedef boost::function< bool (context&) > cxparameter_cb; 
      typedef boost::function< bool (void)     > singularity_cb; 

      // Boost.Uuid as unique application id.
      // in case of positive evaluation of single istance application, 
      // will exit.
      limit_single_instance(const uuids::uuid& app_uuid)
         : impl_(new limit_single_instance_impl()) 
         , uuid_(app_uuid)
      {}

      // uuid as unique application id, and a callback that receive a 
      // application context as param, return true on callback to continue 
      // and false to exit in case of positive evaluation of single istance.
      limit_single_instance(const uuids::uuid& app_uuid, 
         const cxparameter_cb& callback)
         : impl_(new limit_single_instance_impl()) 
         , uuid_(app_uuid)
         , callback_cxparameter_(callback) 
      {}

      // uuid as unique application id, and a callback.
      // user must return true on callback to continue with application
      // and false to exit in case of positive evaluation of single istance.
      limit_single_instance(const uuids::uuid& app_uuid, 
         const singularity_cb& callback)
         : impl_(new limit_single_instance_impl()) 
         , uuid_(app_uuid)
         , callback_singularity_(callback)  
      {}

      // creates a system mutex, the ec ( boost::system::error_code& ec) will be  
      // set to the result of the operation, they should be tested for errors. 
      // if system mutex is already created, return true to indicate that we 
      // already have another instance running on current operating system.
      bool lock(boost::system::error_code &ec) {
         return impl_->lock(uuid_, ec);
      }

      // creates a system mutex, throws an exception of type 
      // boost::system::system_error on error. 
      // if system mutex is already created, return true to indicate that we 
      // already have another instance running on current operating system.
      bool lock() {
         boost::system::error_code ec;

         bool result = lock(ec);
         if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR("lock() failed");

         return result;
      }

      // return true to indicate that we already have another instance 
      // running on current operating system.
      bool is_another_instance_running() {
         return impl_->is_another_instance_running();
      }

      // release system mutex.
      void release(void) {
         impl_->release();
      }

      cxparameter_cb* get_cxparameter_cb()
      {
         if(callback_cxparameter_.empty())
            return NULL;

         return &callback_cxparameter_;
      }

      singularity_cb* get_singularity_cb()
      {
         if(callback_singularity_.empty())
            return NULL;

         return &callback_singularity_;
      }

   private:
      unique_ptr<limit_single_instance_impl> impl_;
      uuids::uuid uuid_;

      cxparameter_cb callback_cxparameter_; 
      singularity_cb callback_singularity_; 
   };

} }  // boost::application

#endif // BOOST_APPLICATION_IMPL_LIMIT_SINGLE_INSTANCE_HPP
		