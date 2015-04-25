// common_application.hpp ----------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 10-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_COMMON_APPLICATION_HPP
#define BOOST_APPLICATION_COMMON_APPLICATION_HPP

#include <boost/application/config.hpp>
#include <boost/application/context.hpp>
#include <boost/application/detail/csbl.hpp>
#include <boost/application/application_mode_register.hpp>

// internal aspects
#include <boost/application/aspects/status.hpp>
#include <boost/application/aspects/run_mode.hpp>
#include <boost/application/aspects/path.hpp>
#include <boost/application/aspects/process_id.hpp>

// platform dependent
#if defined( BOOST_WINDOWS_API )
#   include <boost/application/detail/windows/common_application_impl.hpp>
#elif defined( BOOST_POSIX_API )
#   include <boost/application/detail/posix/common_application_impl.hpp>
#else
#   error "Sorry, no boost application are available for this platform."
#endif

namespace boost { namespace application {

   /*!
    * \brief This class hold a 'common' application mode system.
    *
    * common : Can be seen as Terminal/Console/Interactive
    *          aplication type.
    *
    * An application mode is a collection of aspects and a class 'mode' that
    * define application instantiation and behaviour.
    *
    * The usual use of this class is to pass it as
    * template param on launch free function.
    *
    */

   class common
   {

   public:

      /*!
       * Retrieves a id that identify application run mode.
       *
       */
      static int mode() {
         static int id = new_run_mode<int>();
         return id;
      }

      /*!
       * Creates a common application.
       *
       * \param myapp An user application functor class.
       *
       * \param context An context of application, that hold all
       *        aspects.
       *
       * \param sm The signal manager of application, that will be used
       *           internaly by application type.
       *           User can customize this instance.
       *
       * \param ec Variable (boost::system::error_code) that will be
       *        set to the result of the operation.
       *
       * Check ec for errors.
       *
       */
      template <typename Application, typename SignalManager>
      common(Application& myapp, SignalManager &sm,
         application::context &context,
         boost::system::error_code& ec)
         : impl_(new common_application_impl(
                 boost::bind(&Application::operator(), &myapp), sm,
                 context, ec)) {
         if(ec) return;

         // default aspects patterns added to this kind of application

         if(!impl_->get_context().find<run_mode>())
             impl_->get_context().insert<run_mode>(
               csbl::make_shared<run_mode>(mode()));

         if(!impl_->get_context().find<status>())
             impl_->get_context().insert<status>(
               csbl::make_shared<status>(status::running));

         if(!impl_->get_context().find<process_id>())
              impl_->get_context().insert<process_id>(
               csbl::make_shared<process_id>());
               
         if(!impl_->get_context().find<path>())
              impl_->get_context().insert<path>(
               csbl::make_shared<path>());
      }

      /*!
       * Prepare application and run user functor operator.
       *
       */
      int run() {
         return impl_->run();
      }

      /*!
       * Destruct an common application.
       *
       */
      virtual ~common() {
         impl_->get_context().find<status>()->state(status::stopped);
      }

   private:

      csbl::shared_ptr<common_application_impl> impl_;
   };

}} // boost::application

#endif // BOOST_APPLICATION_COMMON_APPLICATION_HPP

