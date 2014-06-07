// ensure_single_instance.hpp  ------------------------------------------------//
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
// note that this module is a test (in development)
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

   namespace detail {

      // handler detector
      class handler_detector : noncopyable {
      protected:     
         // handlers that we will check
         // (look for, rule)
         MEMBER_HANDLER_EXIST(stop, has_stop); 
         MEMBER_HANDLER_EXIST(pause, has_pause); 
         MEMBER_HANDLER_EXIST(resume, has_resume); 
         MEMBER_HANDLER_EXIST(instace_aready_running, has_single_instance); 
      };

      // for param context

      template <typename Application>
      class parameter_handler_auto_set : public Application, handler_detector 
      {
      public:

         template <typename AppContext>
         parameter_handler_auto_set(AppContext &cxt)
         {
            setup(cxt);
         }

         template <typename AppContext>
         parameter_handler_auto_set(AppContext &cxt, boost::uuids::uuid& appid)
         {
            setup(cxt);
            setup(cxt, appid);
         }

      private:

         // setup context fro common handlers

         template <typename AppContext>
         void setup(AppContext &cxt)
         {
            if(has_stop<Application, bool(Application::*)(context&)>::value)
            {   
               cxt.insert<termination_handler>(
                  boost::make_shared<termination_handler_default_behaviour>(
                     handler<bool>::make_parameter_callback(*this, 
                        &parameter_handler_auto_set::stop_handler_<
                           has_stop<Application, bool(Application::*)(context&)>::value
                              > )));
            }
   
            if(has_pause<Application, bool(Application::*)(context&)>::value)
            {
               cxt.insert<pause_handler>(
                  boost::make_shared<pause_handler_default_behaviour>(
                     handler<bool>::make_parameter_callback(*this, 
                        &parameter_handler_auto_set::pause_handler_<
                           has_pause<Application, bool(Application::*)(context&)>::value
                              > )));
            }

            if(has_resume<Application, bool(Application::*)(context&)>::value)
            {
               cxt.insert<resume_handler>(
                  boost::make_shared<resume_handler_default_behaviour>(
                     handler<bool>::make_parameter_callback(*this, 
                        &parameter_handler_auto_set::resume_handler_<
                           has_resume<Application, bool(Application::*)(context&)>::value
                              > )));
            }

         }

         template <typename AppContext>
         void setup(AppContext &cxt, boost::uuids::uuid& appid)
         {
            if(has_single_instance<Application, bool(Application::*)(context&)>::value)
            {
               cxt.insert<limit_single_instance>(
                  boost::make_shared<limit_single_instance_default_behaviour>(appid,
                     handler<bool>::make_parameter_callback(*this, 
                        &parameter_handler_auto_set::single_instance_handler_<
                           has_single_instance<Application, bool(Application::*)(context&)>::value
                              > )));
            }
         }

         // stop

         template<bool Enable>  
         typename boost::enable_if_c< Enable, bool>::type 
            stop_handler_(context & c) 
         { 
            return stop(c); 
         } 

         template<bool Enable> // never called
         typename boost::enable_if_c<!Enable, bool>::type 
            stop_handler_(context & c) 
         {
            return false; 
         } 

         // pause

         template<bool Enable>  
         typename boost::enable_if_c< Enable, bool>::type 
            pause_handler_(context & c) 
         { 
            return pause(c); 
         } 

         template<bool Enable> // never called
         typename boost::enable_if_c<!Enable, bool>::type 
            pause_handler_(context & c) 
         {
            return false; 
         }
 
         // resume

         template<bool Enable>  
         typename boost::enable_if_c< Enable, bool>::type 
            resume_handler_(context & c) 
         { 
            return resume(c); 
         } 

         template<bool Enable> // never called
         typename boost::enable_if_c<!Enable, bool>::type 
            resume_handler_(context & c) 
         {
            return false; 
         }

         // single_instance

         template<bool Enable>  
         typename boost::enable_if_c< Enable, bool>::type 
            single_instance_handler_(context & c) 
         { 
            return instace_aready_running(c); 
         } 

         template<bool Enable> // never called
         typename boost::enable_if_c<!Enable, bool>::type 
            single_instance_handler_(context & c) 
         {
            return false; 
         }
      };

      // for global context

      template <typename Application>
      class global_handler_auto_set : public Application, handler_detector 
      {
      public:

         template <typename AppContext>
         global_handler_auto_set(AppContext &cxt)
         {
            setup(cxt);
         }

         template <typename AppContext>
         global_handler_auto_set(AppContext &cxt, boost::uuids::uuid& appid)
         {
            setup(cxt);
            setup(cxt, appid);
         }

      private:

         // setup context fro common handlers

         template <typename AppContext>
         void setup(AppContext &cxt)
         {
            if(has_stop<Application, bool(Application::*)()>::value)
            {   
               cxt.insert<termination_handler>(
                  boost::make_shared<termination_handler_default_behaviour>(
                     handler<bool>::make_global_callback(*this, 
                        &global_handler_auto_set::stop_handler_<
                           has_stop<Application, bool(Application::*)()>::value
                              > )));
            }
   
            if(has_pause<Application, bool(Application::*)()>::value)
            {
               cxt.insert<pause_handler>(
                  boost::make_shared<pause_handler_default_behaviour>(
                     handler<bool>::make_global_callback(*this, 
                        &global_handler_auto_set::pause_handler_<
                           has_pause<Application, bool(Application::*)()>::value
                              > )));
            }

            if(has_resume<Application, bool(Application::*)()>::value)
            {
               cxt.insert<resume_handler>(
                  boost::make_shared<resume_handler_default_behaviour>(
                     handler<bool>::make_global_callback(*this, 
                        &global_handler_auto_set::resume_handler_<
                           has_resume<Application, bool(Application::*)()>::value
                              > )));
            }

         }

         template <typename AppContext>
         void setup(AppContext &cxt, boost::uuids::uuid& appid)
         {
            if(has_single_instance<Application, bool(Application::*)()>::value)
            {
               cxt.insert<limit_single_instance>(
                  boost::make_shared<limit_single_instance_default_behaviour>(appid,
                     handler<bool>::make_global_callback(*this, 
                        &global_handler_auto_set::single_instance_handler_<
                           has_single_instance<Application, bool(Application::*)()>::value
                              > )));
            }
         }

         // stop

         template<bool Enable>  
         typename boost::enable_if_c< Enable, bool>::type 
            stop_handler_() 
         { 
            return stop(); 
         } 

         template<bool Enable> // never called
         typename boost::enable_if_c<!Enable, bool>::type 
            stop_handler_() 
         {
            return false; 
         } 

         // pause

         template<bool Enable>  
         typename boost::enable_if_c< Enable, bool>::type 
            pause_handler_() 
         { 
            return pause(); 
         } 

         template<bool Enable> // never called
         typename boost::enable_if_c<!Enable, bool>::type 
            pause_handler_() 
         {
            return false; 
         }
 
         // resume

         template<bool Enable>  
         typename boost::enable_if_c< Enable, bool>::type 
            resume_handler_() 
         { 
            return resume(); 
         } 

         template<bool Enable> // never called
         typename boost::enable_if_c<!Enable, bool>::type 
            resume_handler_() 
         {
            return false; 
         }

         // single_instance

         template<bool Enable>  
         typename boost::enable_if_c< Enable, bool>::type 
            single_instance_handler_() 
         { 
            return instace_aready_running(); 
         } 

         template<bool Enable> // never called
         typename boost::enable_if_c<!Enable, bool>::type 
            single_instance_handler_() 
         {
            return false; 
         }
      };  

      // to detect param or global context using operator
      class app_type_identification {
      protected:     
         // handlers that we will check
         // (look for, rule)
         MEMBER_HANDLER_EXIST(operator() , has_operator); 
      };

      template <typename Application>
      class app_type_detector : public app_type_identification 
      {
      public:
         typedef typename boost::conditional< 
            has_operator<Application, int(Application::*)(context&)>::value, 
               parameter_handler_auto_set<Application>, 
               global_handler_auto_set<Application> 
         >::type type;
      };

   } // detail

   template <typename Application>
   class auto_handler : public detail::app_type_detector<Application>::type 
   {
   public:
      template <typename AppContext>
      auto_handler(AppContext &cxt) 
         : detail::app_type_detector<Application>::type(cxt) {}

      template <typename AppContext>
      auto_handler(AppContext &cxt, boost::uuids::uuid& appid) 
         : detail::app_type_detector<Application>::type(cxt, appid) {}
   };

}} // boost::application


/*

--------------------------------------------------------------------------------

Use with context as parameter:

--------------------------------------------------------------------------------

class myapp
{
public:

   int operator()(boost::application::context& context)
   {
      context.find<boost::application::wait_for_termination_request>()->wait();
      return 0;
   }

   bool instace_aready_running(boost::application::context& context) 
   { 
      std::cout << "instace aready running!" << std::endl; 
      return false;
   } 

   bool stop(boost::application::context& context)
   {
      std::cout << "stop app!" << std::endl; 
      return true;
   }
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
      = boost::bind<bool>(&myapp::instace_aready_running, &app, _1);

   app_context.insert<application::limit_single_instance>(
      boost::make_shared<application::limit_single_instance_default_behaviour>(appuuid, callback));

   // add termination handler
  
   application::handler<>::parameter_callback termination_callback 
      = boost::bind<bool>(&myapp::stop, &app, _1);

   app_context.insert<application::termination_handler>(
      boost::make_shared<application::termination_handler_default_behaviour>(termination_callback));

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
      = boost::bind<bool>(&myapp::instace_aready_running, &app);

   this_application()->insert<limit_single_instance>(
      boost::make_shared<limit_single_instance_default_behaviour>(appuuid, limit_single_instance_callback));

   handler<>::global_context_callback termination_callback 
      = boost::bind<bool>(&myapp::stop, &app);

   this_application()->insert<termination_handler>(
      boost::make_shared<termination_handler_default_behaviour>(termination_callback));

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
