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
// internal aspects
#include <boost/application/aspects/status.hpp>
#include <boost/application/aspects/run_mode.hpp>
// public aspects
#include <boost/application/ready_to_use_aspects.hpp>

namespace boost { namespace application {

   /*!
    * \brief This class hold a application mode system. 
    *  
    * common : Can be seen as Terminal/Console/Interactive 
    *          aplication type. 
    *  
    * An application mode is a collection of aspects that 
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
       * Creates a common application.
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
      common(application::context &context, boost::system::error_code& ec)
         : context_ (context)
         , thread_(0)
      {
         BOOST_APPLICATION_FEATURE_SELECT

         // default aspects patterns added to this kind of application

         context.add_aspect_if_not_exists<run_mode>(
            make_shared<run_mode>(run_mode::common));

         context.add_aspect_if_not_exists<status>(
            make_shared<status>(status::running));

         context.add_aspect_if_not_exists<wait_for_termination_request>(
            shared_ptr<wait_for_termination_request>(
               new wait_for_termination_request_default_behaviour));

         // launch a signal engine on new thread
         thread_ = new boost::thread(
            boost::bind(&common::termination_handler_signal_setup, this));
      }

      /*!
       * Destruct an common application.
       *
       */
      virtual ~common() {
         if(thread_) delete thread_;
      }

   protected:

      // user can override this, and configure your own signal handler
      virtual void termination_handler_signal_setup()
      {
         BOOST_APPLICATION_FEATURE_SELECT

         if(context_.has_aspect<termination_handler>())
         {
            asio::signal_set signals(io_service_, SIGINT, SIGTERM, SIGABRT);

            signals.async_wait(
               boost::bind(&common::termination_signal_handler, this, 
                  boost::asio::placeholders::error, 
                  boost::asio::placeholders::signal_number));

             io_service_.run();
         }
      }

      virtual void termination_signal_handler(
         const boost::system::error_code& ec, int signal_number)
      {
         BOOST_APPLICATION_FEATURE_SELECT

          std::cout <<  ec.message();

         if (!ec)
         {
            if((signal_number == SIGINT)  ||
               (signal_number == SIGTERM) ||
               (signal_number == SIGABRT))
            {
               shared_ptr<termination_handler> th =
                  context_.get_aspect<termination_handler>();

               handler::parameter_callback* parameter = 0;

               if(th->callback(parameter))
               {
                  if((*parameter)(context_))
                  {
                     // we need continue
                     return;
                  }

                  // we need set application_state to stop
                  // and exit, signalize wait_for_termination_request
                  context_.use_aspect<status>().state(status::stoped);
                  context_.use_aspect<wait_for_termination_request>().proceed();
               }

               handler::singleton_callback* singleton = 0;

               if(th->callback(singleton))
               {
                  if((*singleton)())
                  {
                     // we need continue
                     return;
                  }

                  // we need set application_state to stop
                  // and exit, signalize wait_for_termination_request
                  context_.use_aspect<status>().state(status::stoped);
                  context_.use_aspect<wait_for_termination_request>().proceed();
               }
            }
         }
      }

   private:

      application::context &context_;

      boost::thread *thread_; 
      asio::io_service io_service_;
   };

}} // boost::application 

#endif // BOOST_APPLICATION_COMMON_APPLICATION_HPP
       