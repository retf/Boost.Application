// server_application_impl.hpp ----------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2014 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 02-02-2012 dd-mm-yyyy - Initial Release
// 13-09-2013 dd-mm-yyyy - Refactoring

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_SERVER_APPLICATION_IMPL_HPP
#define BOOST_APPLICATION_SERVER_APPLICATION_IMPL_HPP

#include <boost/application/config.hpp>
#include <boost/application/detail/csbl.hpp>
#include <boost/application/base_type.hpp>

#include <boost/application/aspects/termination_handler.hpp>
#include <boost/application/aspects/pause_handler.hpp>
#include <boost/application/aspects/resume_handler.hpp>
#include <boost/application/aspects/args.hpp>

#include <boost/application/detail/application_impl.hpp>
#include <boost/application/signal_binder.hpp>

//#include <boost/lambda/lambda.hpp>

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4251 4231 4660)
#endif

#ifdef _MSC_VER
// Advanced Services API - advapi32
// Advanced Services Provide access to functionality additional to the kernel.
// Include things like: Windows registry, shutdown/restart or abort the system,
// start/stop/create a Windows service (that we need), manage user accounts.
#pragma comment(lib, "advapi32.lib")
#endif

namespace boost { namespace application {

   template <typename CharType>
   class server_application_impl_ : public application_impl
   {
   public:

      // callback for app code
      typedef csbl::function< int (void) > mainop;

      // string types to be used internaly to handle unicode on windows
      typedef CharType char_type;
      typedef std::basic_string<char_type> string_type;

      server_application_impl_(const mainop &main_op, signal_binder &sb,
         application::context &context, boost::system::error_code& ec)
         : application_impl(context)
         , main_(main_op)
         , launch_thread_(0)
         , result_code_(0)
         , terminate_event_(0)
         , terminate_code_(0)
      {
         sb.start();
         initialize(ec);
      }

      // do nothing, on service api run(work_thread) method is initiated by SCM
      int run()
      {
         return result_code_;
      }

      virtual ~server_application_impl_()
      {
         if(launch_thread_)
            delete launch_thread_;

         // if terminateEvent has been created, close it.
         if (terminate_event_)
            CloseHandle(terminate_event_);
      }

   protected:

      void initialize(boost::system::error_code& ec)
      {
         if(instance_ == 0)
            instance_ = this;

         string_type void_string;

         // we suppport only one service on process
         SERVICE_TABLE_ENTRY serviceTable[] =
         {
            { (char_type*) void_string.c_str(), (LPSERVICE_MAIN_FUNCTION) service_main_entry },
            { NULL, NULL }
         };

         // register with the scm
         if(StartServiceCtrlDispatcher(serviceTable) != TRUE)
            ec = last_error_code();
      }

      // returns a reference to the server_application_impl
      static server_application_impl_& instance()
      {
         return *instance_;
      }

      //
      // handlers
      //

      bool stop(void)
      {
         csbl::shared_ptr<termination_handler> th =
               context_.find<termination_handler>();

         if(th)
         {
            handler<>::callback* cb = 0;

            if(th->get(cb))
            {
               if(!(*cb)())
               {
                  return false;
               }
            }
         }

         // if don't have stop ,default is can stop
         return true;
      }

      bool pause(void)
      {
         csbl::shared_ptr<pause_handler> ph =
            context_.find<pause_handler>();

         if(ph)
         {
            handler<>::callback* cb = 0;

            if(ph->get(cb))
            {
               if((*cb)())
               {
                  return true;
               }
            }
         }

         // don't accept, returns the service could not accept control messages
         return false;
      }

      bool resume(void)
      {
        csbl::shared_ptr<resume_handler> rh =
           context_.find<resume_handler>();

         if(rh)
         {
            handler<>::callback* cb = 0;

            if(rh->get(cb))
            {
               if((*cb)())
               {
                  return true;
               }
            }
         }

         // don't accept, returns the service could not accept control messages
         return false;
      }

      //
      // service implementation
      //

      // Handle SCM signals
      void service_handler(DWORD dwOpcode)
      {
         csbl::shared_ptr<status> st =
            context_.find<status>();

         switch (dwOpcode)
         {
            case SERVICE_CONTROL_STOP:
            {
               if(!stop())
                  // don't accept, returns the service
                  // could not accept control messages
                  return;

               request_terminate(0);
            }
            break;
            case SERVICE_CONTROL_SHUTDOWN:
            {
               stop();
               request_terminate(0);
            }
            case SERVICE_CONTROL_PAUSE:
            {
               if(!pause())
                  // don't accept, returns the service
                  // could not accept control messages
                  return;

               if (!send_status_to_scm(SERVICE_PAUSE_PENDING, 1, 1000))
               {
                  request_terminate(GetLastError());
                  return;
               }

               if(st) st->state(status::paused);

               if (!send_status_to_scm(SERVICE_PAUSED, 0, 0))
               {
                  request_terminate(GetLastError());
                  return;
               }
            }
            break;

            case SERVICE_CONTROL_CONTINUE:
            {
               if(!resume())
                  // don't accept, returns the service
                  // could not accept control messages
                  return;

               if (!send_status_to_scm(SERVICE_CONTINUE_PENDING, 1, 1000))
               {
                  request_terminate(GetLastError());
                  return;
               }

               if(st) st->state(status::running);

               if (!send_status_to_scm(SERVICE_RUNNING, 0, 0))
               {
               	request_terminate(GetLastError());
                  return;
               }
            }
            break;

            case SERVICE_CONTROL_INTERROGATE:
            {
               // Nothing here!
            }
            break;
         }
      }

      // check accepted ctrs
      DWORD accepted_controls(void) const
      {
         DWORD accepted_controls = 0;

         // invoke client code to check if we will accept and handle stop.
         // this also will show stop link in SCM
         if(context_.find<termination_handler>())
         {
            accepted_controls = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
         }

         // user code accept pause
         if(context_.find<pause_handler>() && context_.find<resume_handler>())
         {
            accepted_controls |= SERVICE_ACCEPT_PAUSE_CONTINUE;
         }

         return accepted_controls;
      }

       // this function consolidates the activities of
      // updating the service status of service on scm
      BOOL send_status_to_scm(DWORD dw_current_state,
                              DWORD dw_check_point,
                              DWORD dw_wait_hint = 1000,
                              DWORD dw_win32_exit_code = NO_ERROR,
                              DWORD dw_service_specific_exit_code = 0)
      {
         BOOL success;

         // Service Status structure
         SERVICE_STATUS service_status;

         // Type of service executable.
         // SERVICE_WIN32_OWN_PROCESS for one service, or
         // SERVICE_WIN32_SHARE_PROCESS for more, and you can use OR
         // SERVICE_INTERACTIVE_PROCESS if you can interact with desktop
         service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;

         // Current State of your service
         service_status.dwCurrentState = dw_current_state;

         // If in the process of something, then accept
         // no control events, else accept anything
         if (dw_current_state == SERVICE_START_PENDING)
         {
            service_status.dwControlsAccepted = 0;
         }
         else
         {
            service_status.dwControlsAccepted = accepted_controls();
         }

         // if a specific exit code is defines, set up
         // the win32 exit code properly
         if (dw_service_specific_exit_code == 0)
         {
            service_status.dwWin32ExitCode =	dw_win32_exit_code;
         }
         else
         {
            service_status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
            service_status.dwServiceSpecificExitCode = dw_service_specific_exit_code;
         }

         service_status.dwCheckPoint = dw_check_point;
         service_status.dwWaitHint = dw_wait_hint;

         // pass the status record to the scm
         success = SetServiceStatus(service_status_handle_, &service_status);

         return success;
      }
	  void request_terminate(DWORD error)
	  {
	  	  terminate_code_ = error;
	  	  if(terminate_event_ != NULL)
	  	  	  SetEvent(terminate_event_);
	  }
      // handle an error from service_main by cleaning up
      // and telling SCM that the service didn't start.
      void terminate(DWORD error)
      {
      	if(error == 0 && terminate_code_ != 0)
         {
         	 error = terminate_code_;
         }

      	stop();

         csbl::shared_ptr<status> st =
            context_.find<status>();
         csbl::shared_ptr<wait_for_termination_request> tr =
            context_.find<wait_for_termination_request>();

         if(st) st->state(status::stopped);
         if(tr) tr->proceed();

         launch_thread_->join();

         // Send a message to the scm to tell about stopage
         send_status_to_scm(SERVICE_STOPPED, 0, 0, error);

         // Do not need to close serviceStatusHandle
      }

      // scm service thread
      void service_main(DWORD dw_argc, LPTSTR* lpsz_argv)
      {
         string_type void_string;

         // like this service is SERVICE_WIN32_OWN_PROCESS,
         // then the RegisterServiceCtrlHandler
         // function does not verify if the name is valid, because there is only
         // one registered service in the process. can be  "" (void_string)
         service_status_handle_ =
            RegisterServiceCtrlHandler( (char_type*)void_string.c_str(),
            (LPHANDLER_FUNCTION) service_handler_entry);

         if (!service_status_handle_)
         {
            return;
         }

         // notify SCM of progress
         if (!send_status_to_scm(SERVICE_START_PENDING, 1, 1000))
         {
            send_status_to_scm(SERVICE_STOPPED, 0, 0, GetLastError());
            return;
         }

         // create the termination event
         terminate_event_ = CreateEvent (0, TRUE, FALSE, 0);

         if (!terminate_event_)
         {
            send_status_to_scm(SERVICE_STOPPED, 0, 0, GetLastError());
            return;
         }

         // notify SCM of progress
         if (!send_status_to_scm(SERVICE_START_PENDING, 2, 5000))
         {
            send_status_to_scm(SERVICE_STOPPED, 0, 0, GetLastError());
            return;
         }

         // Launch work thread (main)
         launch_thread_ = new csbl::thread(
            boost::bind(&server_application_impl_::work_thread, this, dw_argc, lpsz_argv));

         HANDLE hevent[2];

         hevent[0] = launch_thread_->native_handle();
         if(hevent[0] == NULL)
         {
            send_status_to_scm(SERVICE_STOPPED, 0, 0, -1);
            return;
         }

         // The service is now running.
         // Notify SCM of progress
         if (!send_status_to_scm(SERVICE_RUNNING, 0, 0))
         {
            terminate(GetLastError());
            return;
         }

         hevent[1] = terminate_event_;

         // Wait for stop signal, and then terminate
         WaitForMultipleObjects(2,hevent,FALSE,INFINITE);

         if (!send_status_to_scm(SERVICE_STOP_PENDING, 1, 5000))
         {
            terminate(GetLastError());
            return;
         }
         // terminate service, no error!
         terminate(result_code_);
      }

      void work_thread(int argc, char_type** argv)
      {
         context_.exchange<application::args>(
            csbl::make_shared<application::args>(argc, argv));

         result_code_ = main_();
      }

      //
      // exposed to scm service interface
      //

      static void __stdcall service_main_entry(DWORD dw_argc, LPTSTR* lpsz_argv)
      {
         server_application_impl_::instance().service_main(dw_argc, lpsz_argv);
      }

      static void __stdcall service_handler_entry(DWORD dw_opcode)
      {
         server_application_impl_::instance().service_handler(dw_opcode);
      }

   private:

      // Event used to hold ServiceMain from completing
      HANDLE terminate_event_;
      DWORD terminate_code_;

      // Status of service
      SERVICE_STATUS service_status_;

      // Handle used to communicate status info with
      // the SCM. Created by RegisterServiceCtrlHandler
      SERVICE_STATUS_HANDLE service_status_handle_;

      csbl::thread *launch_thread_;

      // app code
      mainop main_;

      int result_code_;

      static server_application_impl_* instance_;
   };

   // The unique instance of server application (Windows)
   template <typename CharType> server_application_impl_<CharType>*
      server_application_impl_<CharType>::instance_ = 0;  // definition

   /////////////////////////////////////////////////////////////////////////////
   // server_application_impl
   //

   // args versions for Multi-Byte string and Unicode string
   typedef server_application_impl_<character_types::char_type> server_application_impl;
   // wchar_t / char

}} // boost::application

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#endif // BOOST_APPLICATION_SERVER_APPLICATION_IMPL_HPP

