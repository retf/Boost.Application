// limit_single_instance.hpp ------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2014 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 06-01-2012 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_LIMIT_SINGLE_INSTANCE_ASPECT_HPP
#define BOOST_APPLICATION_LIMIT_SINGLE_INSTANCE_ASPECT_HPP

#include <boost/noncopyable.hpp>

#include <boost/application/config.hpp>
#include <boost/application/detail/csbl.hpp>
#if defined( ENABLE_BOOST_INTERPROCESS_NAMED_MUTEX )
#include <boost/interprocess/sync/named_mutex.hpp>
#endif
#if defined( BOOST_WINDOWS_API )
#include <boost/application/detail/windows/limit_single_instance_impl.hpp>
#elif defined( BOOST_POSIX_API )
#include <boost/application/detail/posix/limit_single_instance_impl.hpp>
#else
#error "Sorry, no boost application are available for this platform."
#endif
#include <boost/application/handler.hpp>

namespace boost { namespace application {

   /*!
    * \brief A contract class to be used by the user on your own
    *        class implementation of wait_for_termination_request aspect.
    *
    */
   class limit_single_instance : public handler<>
   {
   public:

      limit_single_instance() {}

      limit_single_instance(const callback& cb)
         : handler<>(cb) {}

      virtual ~limit_single_instance() {}

      /*!
       * User need implement this to return true
       * to indicate that we already have another instance
       * running on current operating system.
       *
       */
      virtual bool lock(boost::system::error_code &ec) = 0;
      virtual bool lock() = 0;

      virtual bool is_another_instance_running() = 0;
      virtual void release(bool force = false) = 0;
   };

   /*!
    * \brief This aspect class ensure that only a single instance of
    *        application (process) is running on a given operating system.
    *
    * Note that the automatic behavior of limit single instance is just
    * evaluated in automatically way when the application is
    * started by the launcher.
    *
    * Note that user must call lock() to know if a istance of
    * application (process) is running,
    * and only after that the call of is_another_instance_running will
    * return correct value. Call to is_another_instance_running,
    * before call of lock, cause is_another_instance_running to
    * return always false.
    */
  

// this is a not official version
#if defined( ENABLE_BOOST_INTERPROCESS_NAMED_MUTEX )

   /*!
    * \brief This aspect class ensure that only a single instance of
    *        application (process) is running on a given operating system.
    *
    * Note that this inplementation use Boost.Interprocess to create a
    * named_mutex.
    *
    */
   class limit_single_instance_default_behaviour
      : public limit_single_instance
   {
   public:
      /*!
       * Constructs an limit_single_instance.
       *
       * \param app_uuid An Uuid (Boost.Uuid) that identify a application.
       *        The mutex will be created using this value.
       *        In case of positive evaluation of single istance application,
       *        will exit.
       */
      limit_single_instance_default_behaviour(const uuids::uuid& app_uuid)
         : uuid_(app_uuid)
         , owns_lock_(false)
      {}

      /*!
       * Constructs an limit_single_instance.
       *
       * \param app_uuid An Uuid that identify a application.
       *        The mutex will be created using this value.
       *
       * \param callback An callback that receive a application context.
       *        User must return true on callback to continue
       *        with application execution, and false to terminate application
       *        execution in case of positive evaluation of single istance.
       */
      limit_single_instance_default_behaviour(const uuids::uuid& app_uuid,
         const callback& cb)
         : limit_single_instance(cb)
         , uuid_(app_uuid)
         , owns_lock_(false)
      {}

      ~limit_single_instance_default_behaviour()
      {
         release();
      }

      /*!
       * Creates a system mutex, the ec ( boost::system::error_code& ec)
       * will be set to the result of the operation, they should be
       * tested for errors.
       *
       * \param ec Variable (boost::system::error_code) that will be
       *        set to the result of the operation.
       *
       * \return if system mutex is already created, return true
       *         to indicate that we already have another instance
       *         running on current operating system.
       *
       */
      bool lock(boost::system::error_code &ec)
      {
         std::string instance_id =
            to_upper_copy(boost::lexical_cast<std::string>(uuid_));

         if(named_mutex_ == 0)
         {
            named_mutex_.reset(new interprocess::named_mutex(
               interprocess::open_or_create, instance_id.c_str()));
         }

         // Tries to lock the interprocess_mutex, returns false
         // when interprocess_mutex is already locked.
         owns_lock_ = named_mutex_->try_lock();

         if(owns_lock_ == true)
         {
            // we just create the lock
            return false;
         }

         return true;
      }

      /*!
       * Creates a system mutex, throws an exception of type
       * boost::system::system_error on error.
       *
       * \return if system mutex is already created, return true
       *         to indicate that we already have another instance
       *         running on current operating system.
       *
       */
      bool lock() {
         boost::system::error_code ec;

         bool result = lock(ec);
         if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
           "lock() failed", ec);

         return result;
      }

      /*!
       * Indicate that we already have another instance of application
       * running on current operating system.
       *
       * \pre the lock() method need be called first.
       * \return true if a instance of application (process) is
       *         already running on current system.
       *
       */
      bool is_another_instance_running() {
         return owns_lock_;
      }

      /*!
       * Release system mutex.
       *
       */
      void release(bool force = false)
      {
         if(force)
         {
            named_mutex_->unlock(); owns_lock_ = false; return;
         }

         if(owns_lock_)
         {
            named_mutex_->unlock(); owns_lock_ = false;
         }
      }

   private:
      bool owns_lock_;
      uuids::uuid uuid_;

      csbl::shared_ptr<interprocess::named_mutex> named_mutex_;
   };
#else
   class limit_single_instance_default_behaviour : public limit_single_instance
   {
   public:

      /*!
       * Constructs an limit_single_instance.
       *
       * \param app_uuid An Uuid (Boost.Uuid) that identify a application.
       *        The mutex will be created using this value.
       *        In case of positive evaluation of single istance application,
       *        will exit.
       */
      limit_single_instance_default_behaviour(const uuids::uuid& app_uuid)
         : impl_(new limit_single_instance_impl())
         , uuid_(app_uuid)
      {}

      /*!
       * Constructs an limit_single_instance.
       *
       * \param app_uuid An Uuid that identify a application.
       *        The mutex will be created using this value.
       *
       * \param callback An callback that receive a application context.
       *        User must return true on callback to continue
       *        with application execution, and false to terminate application
       *        execution in case of positive evaluation of single istance.
       */
      limit_single_instance_default_behaviour(const uuids::uuid& app_uuid,
         const callback& cb)
         : limit_single_instance(cb)
         , impl_(new limit_single_instance_impl())
         , uuid_(app_uuid)
      {}

      /*!
       * Creates a system mutex, the ec ( boost::system::error_code& ec)
       * will be set to the result of the operation, they should be
       * tested for errors.
       *
       * \param ec Variable (boost::system::error_code) that will be
       *        set to the result of the operation.
       *
       * \return if system mutex is already created, return true
       *         to indicate that we already have another instance
       *         running on current operating system.
       *
       */
      bool lock(boost::system::error_code &ec) {
         return impl_->lock(uuid_, ec);
      }

      /*!
       * Creates a system mutex, throws an exception of type
       * boost::system::system_error on error.
       *
       * \return if system mutex is already created, return true
       *         to indicate that we already have another instance
       *         running on current operating system.
       *
       */
      bool lock() {
         boost::system::error_code ec;

         bool result = lock(ec);
         if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
            "lock() failed", ec);

         return result;
      }

      /*!
       * Indicate that we already have another instance of application
       * running on current operating system.
       *
       * \pre the lock() method need be called first.
       * \return true if a instance of application (process) is
       *         already running on current system.
       *
       */
      bool is_another_instance_running() {
         return impl_->is_another_instance_running();
      }

      /*!
       * Release system mutex.
       *
       */
      void release(bool force = false) {
         impl_->release(force);
      }

   private:

      csbl::shared_ptr<limit_single_instance_impl> impl_;
      uuids::uuid uuid_;

   };
#endif

}}  // boost::application

#endif // BOOST_APPLICATION_LIMIT_SINGLE_INSTANCE_ASPECT_HPP

