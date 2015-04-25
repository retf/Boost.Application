// server_application.hpp ----------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2014 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 10-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_SERVER_APPLICATION_HPP
#define BOOST_APPLICATION_SERVER_APPLICATION_HPP

// application
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
#   include <boost/application/detail/windows/server_application_impl.hpp>
#elif defined( BOOST_POSIX_API )
#   include <boost/application/detail/posix/server_application_impl.hpp>
#else
#   error "Sorry, no boost application are available for this platform."
#endif

namespace boost { namespace application {

   /*!
    * \brief This class hold a 'server' application mode system.
    *
    * server : Can be seen as Server (log-time duration)
    *          aplication type.
    *
    * An application mode is a collection of aspects and a class 'mode' that
    * define application instantiation and behaviour.
    *
    * The usual use of this class is to pass it as
    * template param on launch free function.
    *
    */
   class server
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
       * Creates a server application.
       *
       * \param myapp An user application functor class.
       *
       * \param sm The signal manager of application, that will be used
       *           internaly by application type.
       *           User can customize this instance.
       *
       * \param context An context of application, that hold all
       *        aspects.
       *
       * \param ec Variable (boost::system::error_code) that will be
       *        set to the result of the operation.
       *
       * Check ec for errors.
       *
       */
      template <typename Application, typename SignalManager>
      server(Application& myapp, SignalManager &sm,
             application::context &context, boost::system::error_code& ec) {
         // default aspects patterns added to this kind of application

         if(!context.find<run_mode>())
             context.insert<run_mode>(
               csbl::make_shared<run_mode>(mode()));

         if(!context.find<status>())
             context.insert<status>(
               csbl::make_shared<status>(status::running));

         if(!context.find<process_id>())
              context.insert<process_id>(
               csbl::make_shared<process_id>());
               
         if(!context.find<path>())
              context.insert<path>(
               csbl::make_shared<path>());
               
         // need be created after run_mode, status

         impl_.reset(new server_application_impl(
            boost::bind( &Application::operator(), &myapp), sm,
            context, ec));
      }

      /*!
       * Prepare application and run user functor operator.
       *
       */
      int run() {
         return impl_->run();
      }

      /*!
       * Destruct an server application.
       *
       */
      virtual ~server() {
         impl_->get_context().find<status>()->state(status::stopped);
      }

   private:

      csbl::shared_ptr<server_application_impl> impl_;
   };

}} // boost::application

#endif // BOOST_APPLICATION_SERVER_APPLICATION_HPP

