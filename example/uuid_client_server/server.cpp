// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// server.cpp : Defines the entry point for the console application.
//

#define BOOST_ALL_DYN_LINK
#define BOOST_LIB_DIAGNOSTIC

#include <boost/asio.hpp>

#include <fstream>

#include <boost/application.hpp>
#include <boost/program_options.hpp>

// Plugin API
#include "plugin_api.hpp"

using namespace boost;
namespace po = program_options;

class my_server
{
   // plugin entry point
   typedef my_plugin_api* (*pluginapi_create) (void);
   typedef void (*pluginapi_delete) (my_plugin_api* myplugin);

public:

   my_server(application::context& context)
      : context_(context)
   {
   }

   int operator()()
   {
      BOOST_APPLICATION_FEATURE_SELECT

      // launch a work thread
      boost::thread thread(boost::bind(&my_server::work_thread, this, &context_));

      plugin_path_ = context_.find<
         application::path>()->executable_path().string() 
          + "/uuid_plugin" + application::shared_library::suffix();

      plugin_.load(application::library(plugin_path_));

      context_.find<application::wait_for_termination_request>()->wait();
      thread.join(); // the last connection need be served to app exit, comment this to exit quick

      return 0;
   }

   bool stop()
   {
      plugin_.unload();
      return true;
   }

   bool pause()
   {
      plugin_.unload();
      return true;
   }
   
   bool resume()
   {
      plugin_.load(application::library(plugin_path_));
      return true;
   }

protected:

   void work_thread(application::context* context)
   {
      BOOST_APPLICATION_FEATURE_SELECT

      // application logic
      using boost::asio::ip::tcp;

      shared_ptr<application::status> st =          
         context->find<application::status>();

      try
      {
         boost::asio::io_service io_service;

         tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 9512));

         for (;;)
         {
            if(st->state() != application::status::stoped)
            {
               return;
            }

            boost::system::error_code error;

            tcp::socket socket(io_service);
            acceptor.accept(socket);

            // our data is limited to 1024 bytes
            char data[1024];
               
            size_t length = socket.read_some(boost::asio::buffer(data), error);

            if(error == boost::asio::error::eof)
               break; // Connection closed cleanly by peer.
            else if (error)
               throw boost::system::system_error(error); // Some other error.
         
            // response (echo)
            std::string message(data, length);
       
            // detect pause state
            if(st->state() != application::status::paused)
            {
               // if app is on pause state we will aswer a pause message.
               message = "application uuid engine is paused, try again later!";
            }
            else
            {
               if(plugin_.is_loaded())
               {
                  my_plugin_api* plugin = NULL;

                  if(plugin_.search_symbol(application::symbol("create_my_plugin")))
                  {
                     plugin = ((pluginapi_create)plugin_(application::symbol("create_my_plugin")))();
                  }

                  if(plugin != NULL)
                  {
                     message = plugin->transform_string(message);

                     ((pluginapi_delete)plugin_(application::symbol("delete_my_plugin")))(plugin);
                  }
               }
               else
               {
                  message = "some problem with plugin load, try again later!";
               }
            }
               
            boost::asio::write(socket, boost::asio::buffer(message), 
                  boost::asio::transfer_all(), error);
         }
      }
      catch (std::exception& e)
      {
         std::cerr << e.what() << std::endl;
      }
   }

private:

   application::shared_library plugin_;
   filesystem::path plugin_path_;

   application::context& context_;

}; // my_server class


int main(int argc, char *argv[])
{
   BOOST_APPLICATION_FEATURE_SELECT

   try 
   {
      application::context app_context;      
      my_server app(app_context);

      // my server aspects

      app_context.insert<application::path>(
         make_shared<application::path_default_behaviour>(argc, argv));

      app_context.insert<application::args>(
         make_shared<application::args>(argc, argv));

      // add termination handler

      application::handler<>::callback termination_callback 
         = boost::bind<bool>(&my_server::stop, &app);

      app_context.insert<application::termination_handler>(
         make_shared<application::termination_handler_default_behaviour>(termination_callback));

      // To  "pause/resume" works, is required to add the 2 handlers.

   #if defined(BOOST_WINDOWS_API) 

      // windows only : add pause handler
     
      application::handler<>::callback pause_callback 
         = boost::bind<bool>(&my_server::pause, &app);

      app_context.insert<application::pause_handler>(
         make_shared<application::pause_handler_default_behaviour>(pause_callback));

      // windows only : add resume handler

      application::handler<>::callback resume_callback 
         = boost::bind<bool>(&my_server::resume, &app);

      app_context.insert<application::resume_handler>(
         make_shared<application::resume_handler_default_behaviour>(resume_callback));

   #endif     

      // my common/server instantiation

      po::variables_map vm;
      po::options_description desc;

      desc.add_options()
         (",h", "Shows help.")
         (",f", "Run as common applicatio")
         ("help", "produce a help message")
         ;

      po::store(po::parse_command_line(argc, argv, desc), vm);

      if (vm.count("-h")) 
      {
         std::cout << desc << std::endl;
         return 0;
      }

      if (vm.count("-f")) {
         return application::launch<application::common>(app, app_context);
      }
      else {
         return application::launch<application::server>(app, app_context);
      }
   }
   catch(boost::system::system_error& se)
   {
      std::cerr << se.what() << std::endl;
      return 1;
   }
   catch(std::exception &e)
   {
      std::cerr << e.what() << std::endl;
      return 1;
   }
   catch(...)
   {
      std::cerr << "Unknown error." << std::endl;
      return 1;
   }

   return 0;
}


