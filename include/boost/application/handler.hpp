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

#include <boost/function.hpp>

namespace boost { namespace application {

    /*!
     * \brief A class to be used by the classes that need execulte callback.
     *
     * This callback is used on all handlers that application can define.
     *
     * The application library support 2 versions, the first receive a
     * application context as param, and the signature looks like:
     *
     * bool instace_aready_running(context &context);
     *
     * The second version assumes that you are using Boost.Singularity,
     * then no parameter was passed, and the signature looks like:
     *
     * bool instace_aready_running(void);
     *
     * All callbacks are executed by user selected "application mode"
     * the return is a Template that can be defined be the user.
     *
     * In case of our sample, if your return true from
     * instace_aready_running the application mechanism will continue,
     * if your return false the application mechanism will exit.
     *
     */
   
   template<class HandlerReturnType = bool>
   class handler
   {
   public:

      typedef boost::function< HandlerReturnType (context&) >
         parameter_callback;

      typedef boost::function< HandlerReturnType (void)     >
         singleton_callback;

      /*!
       * Constructs an void handler.
       *
       */
      handler() {}

      /*!
       * Destruct an handler.
       *
       */
      virtual ~handler() {}

      /*!
       * Constructs an handler.
       *
       * \param callback An callback that receive a application context
       *        as param. User must return desired behaviour.
       */
      handler(const parameter_callback& callback)
         : parameter_callback_(callback) {}

      /*!
       * Constructs an handler.
       *
       * \param callback An callback to control behaviour of execution.
       *        User must return desired behaviour.
       *
       */
      handler(const singleton_callback& callback)
         : singleton_callback_(callback) {}

      /*!
       * Set a callback to handler.
       *
       * \param callback An callback that receive a application context
       *        as param.
       */
      void callback(const parameter_callback& callback)
      {
         parameter_callback_ = callback;
      }

      /*!
       * Set a callback to handler.
       *
       * \param callback An callback.
       *
       */
      void callback(const singleton_callback& callback)
      {
         singleton_callback_ = callback;
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
      bool callback(parameter_callback*& callback)
      {
          if(parameter_callback_is_valid())
          {
            callback = &parameter_callback_;
            return true;
          }

         callback = 0;
         return false;
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
      bool callback(singleton_callback*& callback)
      {
          if(singleton_callback_is_valid())
          {
            callback = &singleton_callback_;
            return true;
          }

         callback = 0;
         return false;
      }

      /*!
       * Check if a callback pointer to futher execution is valid.
       *
       * \return true if callback pointer is valid.
       *
       */
      bool parameter_callback_is_valid() const
      {
          if(parameter_callback_.empty())
            return false;

          return true;
      }

      /*!
       * Check if a callback pointer to futher execution is valid.
       *
       * \return true if callback pointer is valid.
       *
       */
      bool singleton_callback_is_valid() const
      {
          if(singleton_callback_.empty())
            return false;

          return true;
      }

      /*!
       * Retrieves handler.
       *
       * \return the handler instance.
       *
       */
      handler &get_handler()
      {
         return *this;
      }

   private:

      parameter_callback parameter_callback_;
      singleton_callback singleton_callback_;

   };

   typedef handler<bool> dafault_handler;

   /*!
    * Util free function to create a 'parameter' callback
    *
    * \return a boost::function<ReturnType (context&) > callback 
    *         to be used on handler class.
    *
    */
   template< typename HandlerReturnType, typename App, typename Handler >
   boost::function< HandlerReturnType (context&) > 
      make_parameter_callback(App& app, Handler h)
   {
      return boost::bind< HandlerReturnType >(h, &app, _1);
   }

   /*!
    * Util free function to create a 'singleton' callback
    *
    * \return a boost::function<HandlerReturnType (void) > callback 
    *         to be used on handler class.
    *
    */
   template< typename HandlerReturnType, typename App, typename Handler >
   boost::function< HandlerReturnType (void) > 
      make_singleton_callback(App& app, Handler h)
   {
      return boost::bind< HandlerReturnType >(h, &app);
   }

}} // boost::application

#endif // BOOST_APPLICATION_HANDLER_HPP

