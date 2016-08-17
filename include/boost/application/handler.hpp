//  handler.hpp --------------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 18-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_HANDLER_HPP
#define BOOST_APPLICATION_HANDLER_HPP

#include <boost/application/config.hpp>
#include <boost/application/context.hpp>

namespace boost { namespace application {

    /*!
     * \brief A class to be used by the classes that need execulte callback.
     *
     * This callback is used on all handlers that application can define.
     *
     * E.g.:
     *
     * bool instace_aready_running();
     *
     * All callbacks are executed by user selected "application mode"
     * the return is a template that can be defined be the user.
     *
     * In case of our sample, if your return true from
     * instace_aready_running the application mechanism will continue,
     * if your return false the application mechanism will exit.
     *
     *
     */
   
   template<class HandlerReturnType = bool>
   class handler
   {
   public:

      typedef csbl::function< HandlerReturnType (void) > callback;

      /*!
       * Constructs an void handler.
       *
       */
      handler() {}
      
      /*!
       * Constructs an handler.
       *
       * \param callback An member callback. User must return desired behaviour.
       */
      handler(const callback& cb)
         : callback_(cb) {}

      /*!
       * Destruct an handler.
       *
       */
      virtual ~handler() {}

      /*!
       * Set a callback to handler.
       *
       * \param callback An callback method as param.
       */
      void set(const callback& cb) {
         callback_ = cb;
      }

      /*!
       * Get a callback pointer to futher execution.
       * This method is used internaly by launch/application mode.
       *
       * \param callback An callback to execute.
       *
       * \return true if callback pointer is valid.
       *
       */
      bool get(callback*& cb) {
          if(is_valid()) {
            cb = &callback_;
            return true;
          }

         cb = 0;
         return false;
      }

      /*!
       * Check if a callback pointer to futher execution is valid.
       *
       * \return true if callback pointer is valid.
       *
       */
      bool is_valid() const {
         if(callback_)
            return true;

         return false;
      }

      /*!
       * Retrieves handler.
       *
       * \return the handler instance.
       *
       */
      handler &get_handler() {
         return *this;
      }

      // util

      /*!
       * Convenient static method to create a callback
       * to be used by handler.
       *
       * e.g. to make a callback of method like this:
       * 
       * struct myapp {
       *    bool instace_aready_running();
       * }; 
       *
       * myapp app;
       *
       * handler<bool>::make_callback(app, &myapp::instace_aready_running);
       *
       * \return a boost::function<HandlerReturnType (context&) > callback 
       *         to be used on handler class.
       *
       */
      template< typename App, typename Handler >
      static csbl::function< HandlerReturnType (void) > 
         make_callback(App& app, Handler h) {
         return boost::bind(h, &app);
      }

   private:
      callback callback_;
   };

   // usual handler 
   typedef handler<bool> dafault_handler;

}} // boost::application

#endif // BOOST_APPLICATION_HANDLER_HPP
