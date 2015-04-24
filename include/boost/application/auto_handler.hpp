// auto_handler.hpp  ---------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2014 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 05-06-2014 dd-mm-yyyy - Initial Release
//
//
// -----------------------------------------------------------------------------

//
// *****************************************************************************
// note that this is a experimental module 
// note that construcor can not be explicit
// *****************************************************************************
//

#ifndef BOOST_APPLICATION_AUTO_HANDLER_HPP
#define BOOST_APPLICATION_AUTO_HANDLER_HPP

// application
#include <boost/application/config.hpp>
#include <boost/application/detail/csbl.hpp>
#include <boost/application/context.hpp>

#include <boost/noncopyable.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp> 

// This macro check if a ctrl handler exists in child class 
// Based on:
// http://en.wikibooks.org/w/index.php?title=More_C%2B%2B_Idioms%2FMember_Detector
// 
#define MEMBER_HANDLER_EXIST(func, name)                                   \
template<typename T, typename R>                                           \
struct name                                                                \
{                                                                          \
   typedef char yes[1];                                                    \
   typedef char no [2];                                                    \
                                                                           \
   template <typename U, U>                                                \
   struct type_check;                                                      \
                                                                           \
   template <typename _1> static yes &chk(type_check<R, &_1::func> *);     \
   template <typename   > static no  &chk(...);                            \
                                                                           \
   static bool const value =                                               \
   sizeof(chk<T>(0)) == sizeof(yes);                                       \
}

namespace boost { namespace application { 

   /*!
    * This file has a util class (auto_handler) that can be used to 
    * bind a class methods to handlers in automatic way.
    *
    * The available handlers are:
    *
    * - stop
    * - pause (windows only)
    * - resume (windows only)
    * - instace_aready_running
    *
    * Functor class methods (to handler) signature:
    *
    * - bool stop(void);
    * - bool pause(void);
    * - bool resume(void);
    * - bool instace_aready_running(void);
    *
    */

   namespace detail {

      // handler detector
      class handler_detector : noncopyable {
      public:     
         // handlers that we will check
         // (look for, rule)
         MEMBER_HANDLER_EXIST(stop, has_stop); 
         // platform dependent 
         // pause and resume is only available on windows
#        if defined( BOOST_WINDOWS_API )
         MEMBER_HANDLER_EXIST(pause, has_pause); 
         MEMBER_HANDLER_EXIST(resume, has_resume); 
#        endif
         MEMBER_HANDLER_EXIST(instace_aready_running, has_single_instance); 
      };

      // context constructible 
      template <typename Application, typename Derived>
      struct handler_auto_set_c : public Application {
         handler_auto_set_c(context &cxt)
            : Application (cxt) {
            static_cast<Derived*>(this)->setup(cxt);
         }

         handler_auto_set_c(context &cxt, boost::uuids::uuid& appid)
            : Application (cxt) {
            static_cast<Derived*>(this)->setup(cxt);
            static_cast<Derived*>(this)->setup(cxt, appid);
         }
      };

      // unconstructible (to use with global_context)
      template <typename Application, typename Derived>
      struct handler_auto_set_u : public Application {
         handler_auto_set_u(context &cxt) {
            static_cast<Derived*>(this)->setup(cxt);
         }

         handler_auto_set_u(context &cxt, boost::uuids::uuid& appid) {
            static_cast<Derived*>(this)->setup(cxt);
            static_cast<Derived*>(this)->setup(cxt, appid);
         }
      };

   } // detail
   
   /*!
    * \brief This class hold a 'auto_handler' mechanism allowing
    *        the user make the "bind" to the main handlers automatically.
    *
    * Use:
    * 
    * class myapp
    * {
    * public:
    * 
    *    myapp(boost::application::context& context)
    *       : context_(context) { }
    * 
    *    int operator()()
    *    {
    *        context_.find<boost::application::wait_for_termination_request>()->wait();
    *        return 0;
    *    }
    * 
    *    bool stop()
    *    {
    *        return true;
    *    }
    * 
    * private:
    *    boost::application::context& context_;
    * };
    *
    * int main()
    * {  
    *    boost::application::context app_context;
    *    boost::application::auto_handler<myapp> app(app_context);
    * 
    *    return boost::application::launch<boost::application::common>(app, app_context);
    * }
    * 
    */

   template <typename Application>
   class auto_handler : public detail::handler_detector, public 
      boost::conditional< 
         boost::is_convertible<context&, Application>::value, 
            detail::handler_auto_set_c<Application, auto_handler<Application> >, 
            detail::handler_auto_set_u<Application, auto_handler<Application> > 
      >::type
   {
      
      template <typename , typename >
         friend struct detail::handler_auto_set_u;

      template <typename , typename >
         friend struct detail::handler_auto_set_c;

   public:
 
      typedef typename boost::conditional< 
         boost::is_convertible<context&, Application>::value, 
            detail::handler_auto_set_c<Application, auto_handler<Application> >, 
            detail::handler_auto_set_u<Application, auto_handler<Application> > 
         >::type base_selector;

      auto_handler(context &cxt)
         : base_selector(cxt) { }

      auto_handler(context &cxt, uuids::uuid& appid)
         : base_selector (cxt, appid) { }

      auto_handler(global_context_ptr cxt)
         : base_selector(*cxt.get()) { }

      auto_handler(global_context_ptr cxt, uuids::uuid& appid)
         : base_selector(*cxt.get(), appid) { }

   protected:

      // setup context fro common handlers

      void setup(context &cxt) {
         if(has_stop<Application, bool(Application::*)()>::value) {   
            cxt.insert<termination_handler>(
               csbl::make_shared<termination_handler_default_behaviour>(
                  handler<bool>::make_callback(*this, 
                     &auto_handler::stop_handler_<
                        has_stop<Application, bool(Application::*)()>::value
                           > )));
         }
  
         // platform dependent
         // pause and resume is only available on windows     
#        if defined( BOOST_WINDOWS_API )   
         if(has_pause<Application, bool(Application::*)()>::value) {
            cxt.insert<pause_handler>(
               csbl::make_shared<pause_handler_default_behaviour>(
                  handler<bool>::make_callback(*this, 
                     &auto_handler::pause_handler_<
                        has_pause<Application, bool(Application::*)()>::value
                           > )));
         }

         if(has_resume<Application, bool(Application::*)()>::value) {
            cxt.insert<resume_handler>(
               csbl::make_shared<resume_handler_default_behaviour>(
                  handler<bool>::make_callback(*this, 
                     &auto_handler::resume_handler_<
                        has_resume<Application, bool(Application::*)()>::value
                           > )));
         }
#        endif  

      }

      void setup(context &cxt, boost::uuids::uuid& appid) {
         if(has_single_instance<Application, bool(Application::*)()>::value) {
            cxt.insert<limit_single_instance>(
               csbl::make_shared<limit_single_instance_default_behaviour>(appid,
                  handler<bool>::make_callback(*this, 
                     &auto_handler::single_instance_handler_<
                        has_single_instance<Application, bool(Application::*)()>::value
                           > )));
         }
      }

      // stop

      template<bool Enable>  
      typename boost::enable_if_c< Enable, bool>::type 
         stop_handler_() { 
         return Application::stop(); 
      } 

      template<bool Enable> // never called
      typename boost::enable_if_c<!Enable, bool>::type 
         stop_handler_() {
         return false; 
      } 
    
      // platform dependent
      // pause and resume is only available on windows       
#     if defined( BOOST_WINDOWS_API ) 
      // pause

      template<bool Enable>  
      typename boost::enable_if_c< Enable, bool>::type 
         pause_handler_() { 
         return Application::pause(); 
      } 

      template<bool Enable> // never called
      typename boost::enable_if_c<!Enable, bool>::type 
         pause_handler_() {
         return false; 
      }
 
      // resume

      template<bool Enable>  
      typename boost::enable_if_c< Enable, bool>::type 
         resume_handler_() { 
         return Application::resume(); 
      } 

      template<bool Enable> // never called
      typename boost::enable_if_c<!Enable, bool>::type 
         resume_handler_() {
         return false; 
      }
#     endif  

      // single_instance

      template<bool Enable>  
      typename boost::enable_if_c< Enable, bool>::type 
         single_instance_handler_() { 
         return Application::instace_aready_running(); 
      } 

      template<bool Enable> // never called
      typename boost::enable_if_c<!Enable, bool>::type 
         single_instance_handler_() {
         return false; 
      }
   };  

}} // boost::application

/*

--------------------------------------------------------------------------------

Use with context as parameter:

--------------------------------------------------------------------------------

class myapp
{
public:

   myapp(boost::application::context& context)
      : context_(context)
   {
   }

   int operator()()
   {
      context_.find<boost::application::wait_for_termination_request>()->wait();
      return 0;
   }

   bool instace_aready_running() 
   { 
      std::cout << "instace aready running!" << std::endl; 
      return false;
   } 

   bool stop()
   {
      std::cout << "stop app!" << std::endl; 
      return true;
   }

private:
   boost::application::context& context_;

};

-- Without 'auto_handler' (comparation)
--------------------------------------------------------------------------------

int main()
{  
   myapp app;

   boost::uuids::string_generator gen;
   boost::uuids::uuid appuuid = gen("{9F66E4AD-ECA5-475D-8784-4BAA329EF9F2}");

   boost::application::context app_context;
   
   // add limit_single_instance handler

   application::handler<bool>::parameter_context_callback callback 
      = boost::bind(&myapp::instace_aready_running, &app, _1);

   app_context.insert<application::limit_single_instance>(
      csbl::make_shared<application::limit_single_instance_default_behaviour>(appuuid, callback));

   // add termination handler
  
   application::handler<>::parameter_callback termination_callback 
      = boost::bind(&myapp::stop, &app, _1);

   app_context.insert<application::termination_handler>(
      csbl::make_shared<application::termination_handler_default_behaviour>(termination_callback));

   return boost::application::launch<boost::application::common>(app, app_context);
}

-- Using 'auto_handler'
--------------------------------------------------------------------------------

int main()
{  
   boost::uuids::string_generator gen;
   boost::uuids::uuid appuuid = gen("{9F66E4AD-ECA5-475D-8784-4BAA329EF9F2}");

   boost::application::context app_context;
   boost::application::auto_handler<myapp> app(app_context, appuuid);

   return boost::application::launch<boost::application::common>(app, app_context);
}

--------------------------------------------------------------------------------

Use global context:

--------------------------------------------------------------------------------

inline boost::application::global_context_ptr this_application() {
   return boost::application::global_context::get();
}

class myapp
{
public:

   int operator()()
   {
      this_application()->find<boost::application::wait_for_termination_request>()->wait();
      return 0;
   }

   bool instace_aready_running() 
   { 
      std::cout << "instace aready running!" << std::endl; 
      return false;
   } 

   bool stop()
   {
      std::cout << "stop app!" << std::endl; 
      return true;
   }
};

-- Without 'auto_handler' (comparation) / (global context)
--------------------------------------------------------------------------------

int main()
{  
   application::global_context_ptr ctx = application::global_context::create();

   myapp app;   

   boost::uuids::string_generator gen;
   boost::uuids::uuid appuuid = gen("{9F66E4AD-ECA5-475D-8784-4BAA329EF9F2}");

    handler<>::global_context_callback limit_single_instance_callback 
      = boost::bind(&myapp::instace_aready_running, &app);

   this_application()->insert<limit_single_instance>(
      csbl::make_shared<limit_single_instance_default_behaviour>(appuuid, limit_single_instance_callback));

   handler<>::global_context_callback termination_callback 
      = boost::bind(&myapp::stop, &app);

   this_application()->insert<termination_handler>(
      csbl::make_shared<termination_handler_default_behaviour>(termination_callback));

   int ret = application::launch<application::common>(app, ctx);

   application::global_context::destroy();
}

-- Using 'auto_handler' (global context)
--------------------------------------------------------------------------------

int main()
{  
   application::global_context_ptr ctx = application::global_context::create();

   boost::uuids::string_generator gen;
   boost::uuids::uuid appuuid = gen("{9F66E4AD-ECA5-475D-8784-4BAA329EF9F2}");

   boost::application::auto_handler<myapp> app(*ctx, appuuid);

   int ret = application::launch<application::common>(app, ctx);

   application::global_context::destroy();
}

*/

#endif // BOOST_APPLICATION_AUTO_HANDLER_HPP
