// signal_binder.hpp ---------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2014 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 26-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_SIGNAL_MANAGER_HPP
#define BOOST_APPLICATION_SIGNAL_MANAGER_HPP

#include <boost/application/config.hpp>
#include <boost/application/detail/csbl.hpp>

#include <boost/application/context.hpp>
#include <boost/application/handler.hpp>

// #include <boost/bind.hpp>
// #include <boost/thread/thread.hpp>

#include <boost/application/aspects/termination_handler.hpp>
#include <boost/application/aspects/limit_single_instance.hpp>
#include <boost/application/aspects/wait_for_termination_request.hpp>

namespace boost { namespace application {

   // This is an attempt to make things more flexible,
   // this allow user to define your own sinal -> handler map

   // forward declaration.
   template <class T> class common_application_impl_;
   template <class T> class server_application_impl_;

   /*!
    * \brief This class is the base of signal_manager class.
    *        User can extend of signal_manager to customize SIGANAL/Handlers
    *        of your application.
    *
    */
   class signal_binder
   {
      template<class> friend class common_application_impl_;
      template<class> friend class server_application_impl_;

   public:
      explicit signal_binder(context &cxt)
         : signals_(io_service_)
         , context_(cxt) {
         signals_.async_wait(
            boost::bind(&signal_binder::signal_handler, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::signal_number));
      }

      explicit signal_binder(global_context_ptr cxt)
         : signals_(io_service_)
         , context_(*cxt.get()) {
         signals_.async_wait(
            boost::bind(&signal_binder::signal_handler, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::signal_number));
      }

      virtual ~signal_binder() {
         if(io_service_thread_) {
            io_service_.stop();
            if (io_service_thread_->joinable()) {
               io_service_thread_->join();
            }
         }
      }

      /*!
       * Bind/tie a standard SIGNAL to a handler callback.
       *
       * \param signal_number The signal constant, e.g.: SIGUSR2, SIGINT.
       *
       * \param h An handler that hold a callback that will called when signal
       *          arrives.
       *
       */
      void bind(int signal_number, const handler<>& h) {
         boost::system::error_code ec;
         bind(signal_number, h, handler<>(), ec);

         if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
            "bind() failed", ec);
      }

      /*!
       * Bind/tie a standard SIGNAL to a handler callback, and define
       * handler to be executed on sucess of first handler.
       *
       * \param signal_number The signal constant, e.g.: SIGUSR2, SIGINT.
       *
       * \param h1 An handler that hold a callback that will called when signal
       *          arrives.
       *
       * \param h2 An handler that hold a callback that will called when the
       *           first handler return true;
       *
       */
      void bind(int signal_number, const handler<>& h1, const handler<>& h2) {
         boost::system::error_code ec;
         bind(signal_number, h1, h2, ec);

         if(ec)
            BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
            "bind() failed", ec);
      }

       /*!
       * Bind/tie a standard SIGNAL to a handler callback. 'ec' version.
       *
       * \param signal_number The signal constant, e.g.: SIGUSR2, SIGINT.
       *
       * \param h An handler that hold a callback that will called when signal
       *          arrives.
       *
       */
      void bind(int signal_number, const handler<>& h,
         boost::system::error_code& ec) {
         bind(signal_number, h, handler<>(), ec);
      }

      /*!
       * Bind/tie a standard SIGNAL to a handler callback, and define
       * handler to be executed on sucess of first handler. 'ec' version.
       *
       * \param signal_number The signal constant, e.g.: SIGUSR2, SIGINT.
       *
       * \param h1 An handler that hold a callback that will called when signal
       *          arrives.
       *
       * \param h2 An handler that hold a callback that will called when the
       *           first handler return true;
       *
       */
      void bind(int signal_number, const handler<>& h1, const handler<>& h2,
         boost::system::error_code& ec) {
         signals_.add(signal_number, ec);
         handler_map_[signal_number] = std::make_pair(h1, h2);
      }

      /*!
       * Unbind/untie a standard SIGNAL.
       *
       * \param signal_number The signal constant, e.g.: SIGUSR2, SIGINT.
       *
       */
      void unbind(int signal_number) {
         boost::system::error_code ec;
         unbind(signal_number, ec);

         if(ec)
            BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
               "unbind() failed", ec);
      }

      /*!
       * Unbind/untie a standard SIGNAL. 'ec' version.
       *
       * \param signal_number The signal constant, e.g.: SIGUSR2, SIGINT.
       *
       */
      void unbind(int signal_number, boost::system::error_code& ec) {
         if(handler_map_.cend() != handler_map_.find(signal_number))
         {
            // replace
            signals_.remove(signal_number, ec);
            handler_map_.erase(signal_number);
         }
      }

      /*!
       * Check if a standard SIGNAL is tied to a handler;
       *
       * \param signal_number The signal constant, e.g.: SIGUSR2, SIGINT.
       *
       */
      bool is_bound(int signal_number) {
         return (handler_map_.cend() != handler_map_.find(signal_number));
      }

    protected:

      void start() {
         io_service_thread_.reset(new csbl::thread(
            boost::bind(&signal_binder::run_io_service, this)));
      }

      void run_io_service() {
         io_service_.run();
      }

      void signal_handler(const boost::system::error_code& ec,
         int signal_number) {
         spawn(ec, signal_number);

         // triggers again
         signals_.async_wait(
            boost::bind(&signal_binder::signal_handler, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::signal_number));
      }

   protected:

      void spawn(const boost::system::error_code& ec, int signal_number) {
         if (ec)
            return;

         if(handler_map_[signal_number].first.is_valid()) {
            handler<>::callback* cb = 0;

            if(handler_map_[signal_number].first.get(cb)) {
               if((*cb)()) {
                  // user tell us to call second callback
                  if(handler_map_[signal_number].second.get(cb))
                     (*cb)();
               }

               return;
            }
         }
      }

   private:

      // signal < handler / handler>
      // if first handler returns true, the second handler are called
      csbl::unordered_map<int, std::pair< handler<>, handler<> > > handler_map_;

      asio::io_service io_service_;
      asio::signal_set signals_;

      csbl::shared_ptr<csbl::thread> io_service_thread_;

   protected:

      // for signal_manager access
      application::context &context_;

   };

   /*!
    * \brief This class hold the default behaviour of SIGNALS
    *        used by application.
    *
    */
   class signal_manager
      : public signal_binder
   {
   public:

      signal_manager(application::context &context,
         boost::system::error_code& ec)
         : signal_binder(context)
      {
         register_signals(ec);
      }

      signal_manager(application::context &context)
         : signal_binder(context)
      {
         boost::system::error_code ec;

         register_signals(ec);

         if(ec)
            BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
               "signal_manager() failed", ec);
      }

   protected:

      virtual csbl::shared_ptr<termination_handler>
         setup_termination_behaviour()
      {
         strict_lock<application::aspect_map> guard(context_);

         if(!context_.find<wait_for_termination_request>(guard))
         {
            context_.insert<wait_for_termination_request>(
               csbl::shared_ptr<wait_for_termination_request>(
                  new wait_for_termination_request_default_behaviour), guard);
         }

         return context_.find<termination_handler>(guard);
      }

      // parameter context version

      virtual void register_signals(boost::system::error_code& ec)
      {
         csbl::shared_ptr<termination_handler> th
            = setup_termination_behaviour();

         if(th)
         {
            handler<>::callback cb
               = boost::bind(
               &signal_manager::termination_signal_handler, this);

            bind(SIGINT,  th->get_handler(), cb, ec);
            if(ec) return;

            bind(SIGTERM, th->get_handler(), cb, ec);
            if(ec) return;

            bind(SIGABRT, th->get_handler(), cb, ec);
            if(ec) return;
         }
      }

      virtual bool termination_signal_handler(void)
      {
         // we need set application_state to stop
         context_.find<status>()->state(status::stopped);

         // remove process lock
         csbl::shared_ptr<limit_single_instance> si
            = context_.find<limit_single_instance>();

         if(si)
            si->release(true);

         // and signalize wait_for_termination_request
         context_.find<wait_for_termination_request>()->proceed();

         // this is not used
         return false;
      }

   };

}} // boost::application

#endif // BOOST_APPLICATION_SIGNAL_MANAGER_HPP

