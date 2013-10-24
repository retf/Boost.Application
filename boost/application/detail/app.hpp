// app.hpp -----------------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2012 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 22-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_APP_HPP
#define BOOST_APPLICATION_APP_HPP

// application
#include <boost/application/config.hpp>
#include <boost/application/context.hpp>
#include <boost/application/detail/signal_manager.hpp>
#include <boost/application/application_types.hpp>
#include <boost/application/application_initializers.hpp>
// internal aspects
#include <boost/application/aspects/status.hpp>
#include <boost/application/aspects/run_mode.hpp>
// aspects
#include <boost/application/ready_to_use_aspects.hpp>

// Note that singularity is in approval process, 
// refer to the above link to know more:
// http://www.boost.org/community/review_schedule.html
#include <boost/singularity/singularity.hpp>

namespace boost { namespace application {

   /*!
    * \brief This class hold a basic behaviour that can
    *        be shared between application modes.
    * 
    */
   class app 
      : public signal_manager<app>
   {
      friend class signal_manager<app>;

   public:
   
      app(application::context &context, boost::system::error_code& ec) 
         : context_(context), result_(0) 
      {
         BOOST_APPLICATION_FEATURE_SELECT

         context.add_aspect_if_not_exists<status>(
            make_shared<status>(status::running));

         context.add_aspect_if_not_exists<wait_for_termination_request>(
            shared_ptr<wait_for_termination_request>(
               new wait_for_termination_request_default_behaviour));

         if(context_.has_aspect<termination_handler>())
         {
            shared_ptr<termination_handler> th =
               context_.get_aspect<termination_handler>();

            handler::parameter_callback callback 
               = boost::bind<bool>(&app::termination_signal_handler, this, _1);

            tie_signal(SIGINT,  th->get_handler(), callback, ec);
            if(ec) return;

            tie_signal(SIGTERM, th->get_handler(), callback, ec);
            if(ec) return;

            tie_signal(SIGABRT, th->get_handler(), callback, ec);
            if(ec) return;
         }
      }

      virtual ~app()
      {
         context_.use_aspect<status>().state(status::stoped);
      }

      virtual int result()
      {
         // todo
         // we need get this from impl
         return result_;
      }

   protected:

      virtual bool termination_signal_handler(application::context &context)
      { 
         // we need set application_state to stop
         context_.use_aspect<status>().state(status::stoped);

         // and signalize wait_for_termination_request
         context_.use_aspect<wait_for_termination_request>().proceed();

         // this is not used
         return false;
      }

   protected:

      // application context to be used by all derivated classes
      application::context &context_;

      // store result code returned by
      // user code (functor application operator)
      int result_;
   };

}} // boost::application 

#endif // BOOST_APPLICATION_APP_HPP
       