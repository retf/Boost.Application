// common_application.hpp ----------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 21-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_DETAIL_SIGNAL_MANAGER_HPP
#define BOOST_APPLICATION_DETAIL_SIGNAL_MANAGER_HPP

#include <boost/application/config.hpp>
#include <boost/application/context.hpp>
#include <boost/application/handler.hpp>

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

namespace boost { namespace application {

   // This is an attempt to make things more flexible, 
   // this allow user to define your own sinal -> handler map
   template <typename ApplicationMode>
   class signal_manager
   {
   public:
      signal_manager()
         : io_service_thread_(0)
         , signals_(io_service_)
      {
         signals_.async_wait(
            boost::bind(&signal_manager::signal_handler, this, 
            boost::asio::placeholders::error, 
            boost::asio::placeholders::signal_number));

         // launch a signal engine on new thread
         io_service_thread_ = new boost::thread(
            boost::bind(&signal_manager::run_io_service, this));
      }

      virtual ~signal_manager()
      {
         if(io_service_thread_)
         {
            delete io_service_thread_;
         }
      }

      void tie_signal(int signal_number, const handler& h)
      {
         boost::system::error_code ec;
         tie_signal(signal_number, h, handler(), ec);

         if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR("tie_signal() failed");
      }

      void tie_signal(int signal_number, const handler& h1, const handler& h2)
      {
         boost::system::error_code ec;
         tie_signal(signal_number, h1, h2, ec);

         if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR("tie_signal() failed");
      }

      void tie_signal(int signal_number, const handler& h, boost::system::error_code& ec)
      {
         tie_signal(signal_number, h, handler(), ec);
      }

      void tie_signal(int signal_number, const handler& h1, const handler& h2, boost::system::error_code& ec)
      {
         signals_.add(signal_number, ec);
         handler_map_[signal_number] = std::make_pair(h1, h2);
      }

      void untie_signal(int signal_number)
      {
         boost::system::error_code ec;
         untie_signal(signal_number, ec);

         if(ec) BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR("tie_signal() failed");
      }

      void untie_signal(int signal_number, boost::system::error_code& ec)
      {
         if(handler_map_.cend() != handler_map_.find(signal_number))
         {
            // replace
            signals_.remove(signal_number, ec);
            handler_map_.erase(signal_number);
         }
      }

      bool is_tied()
      {
         return (handler_map_.cend() != handler_map_.find(signal_number));
      }
      
   protected:

      void run_io_service()
      {
         io_service_.run();
      }

      void signal_handler(const boost::system::error_code& ec, int signal_number)
      {
         if (!ec)
         {
            context &cxt = static_cast<ApplicationMode*>(this)->context_;

            if(handler_map_[signal_number].first.parameter_callback_is_valid())
            {
               handler::parameter_callback* parameter;

               if(handler_map_[signal_number].first.callback(parameter))
               {
                  if((*parameter)(cxt))
                  {
                     // user tell us to call second callback
                     if(handler_map_[signal_number].second.callback(parameter))
                        (*parameter)(cxt);
                  }

                  return;
               }
            }

            if(handler_map_[signal_number].first.singleton_callback_is_valid())
            {
               handler::singleton_callback* singleton = 0;

               if(handler_map_[signal_number].first.callback(singleton))
               {
                  if((*singleton)())
                  {
                     // user tell us to call second callback
                     if(handler_map_[signal_number].second.callback(singleton))
                        (*singleton)();
                  }

                  return ;
               }
            }
         }
      }

   private:

      // signal < handler / handler> 
      // if first handler returns true, the second handler are called
      BOOST_APPLICATION_FEATURE_NS_SELECT::
         unordered_map<int, std::pair<handler, handler> > handler_map_;

      asio::io_service io_service_;
      asio::signal_set signals_;
      
      boost::thread *io_service_thread_; 
   };

}} // boost::application 

#endif // BOOST_APPLICATION_DETAIL_SIGNAL_MANAGER_HPP
       