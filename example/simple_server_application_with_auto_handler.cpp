// -----------------------------------------------------------------------------
// simple_server_application_with_auto_handler.cpp : examples that show how use
// Boost.Application to make a simplest interactive (terminal) application
//
// Note 1: The Boost.Application (Aspects v4) and this sample are in
//         development process.
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// -----------------------------------------------------------------------------

//
// *****************************************************************************
// note that this code uses experimental module auto_handler
// *****************************************************************************
//


// To call "stop" on POSIX, use:
//
// kill -2 <PID>
// kill -INT <PID>

// output sample on windows service (log.txt)
//
// Start Log...
// -----------------------------
// ---------- Arg List ---------
// -----------------------------
// simple_server_application
// -----------------------------
// running...
// running...
// running...
// [...]
// Pause my application...
// paused...
// paused...
// paused...
// [...]
// Resume my application...
// running...
// running...
// running...
// [...]
// Stoping my application...
//

#define BOOST_ALL_DYN_LINK
#define BOOST_LIB_DIAGNOSTIC

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/application.hpp>
#include <boost/application/auto_handler.hpp>

// provide setup example for windows service
#if defined(BOOST_WINDOWS_API)
#   include "setup/windows/setup/service_setup.hpp"
#endif

namespace po = boost::program_options;
using namespace boost;

// my application code

class myapp
{
public:

   myapp(application::context& context)
      : context_(context)
   {
   }

   void worker()
   {
      // my application behaviour

      // dump args

      std::vector<std::string> arg_vector =
         context_.find<application::args>()->arg_vector();

      my_log_file_ << "-----------------------------" << std::endl;
      my_log_file_ << "---------- Arg List ---------" << std::endl;
      my_log_file_ << "-----------------------------" << std::endl;

      // only print args on screen
      for(std::vector<std::string>::iterator it = arg_vector.begin();
         it != arg_vector.end(); ++it) {
         my_log_file_ << *it << std::endl;
      }

      my_log_file_ << "-----------------------------" << std::endl;

      boost::shared_ptr<application::process_id> pid 
         = context_.find<application::process_id>();

      my_log_file_ << "PID: " << pid->pid() << std::endl;

      my_log_file_ << "-----------------------------" << std::endl;

      // run logic

      boost::shared_ptr<application::status> st =
         context_.find<application::status>();

      int count = 0;
      while(st->state() != application::status::stopped)
      {
         boost::this_thread::sleep(boost::posix_time::seconds(1));

         if(st->state() == application::status::paused)
            my_log_file_ << count++ << ", paused..." << std::endl;
         else
            my_log_file_ << count++ << ", running..." << std::endl;
      }
   }

   // param
   int operator()()
   {
      std::string logfile
         = context_.find<application::path>()->executable_path().string() + "/log.txt";

      my_log_file_.open(logfile.c_str());
      my_log_file_ << "Start Log..." << std::endl;

      // launch a work thread
      boost::thread thread(&myapp::worker, this);

      context_.find<application::wait_for_termination_request>()->wait();

      // to run direct
      // worker(&context);

      return 0;
   }

   // windows/posix

   bool stop()
   {
      my_log_file_ << "Stoping my application..." << std::endl;
      my_log_file_.close();

      return true; // return true to stop, false to ignore
   }

   // windows specific (ignored on posix)

   bool pause()
   {
      my_log_file_ << "Pause my application..." << std::endl;
      return true; // return true to pause, false to ignore
   }

   bool resume()
   {
      my_log_file_ << "Resume my application..." << std::endl;
      return true; // return true to resume, false to ignore
   }

private:

   std::ofstream my_log_file_;
   application::context& context_;

};

// my setup code for windows service

bool setup(application::context& context)
{
   strict_lock<application::aspect_map> guard(context);

   boost::shared_ptr<application::args> myargs
      = context.find<application::args>(guard);

   boost::shared_ptr<application::path> mypath
      = context.find<application::path>(guard);

// provide setup for windows service
#if defined(BOOST_WINDOWS_API)
#if !defined(__MINGW32__)

   // get our executable path name
   boost::filesystem::path executable_path_name = mypath->executable_path_name();

   // define our simple installation schema options
   po::options_description install("service options");
   install.add_options()
      ("help", "produce a help message")
      (",i", "install service")
      (",u", "unistall service")
      ("name", po::value<std::string>()->default_value(mypath->executable_name().stem().string()), "service name")
      ("display", po::value<std::string>()->default_value(""), "service display name (optional, installation only)")
      ("description", po::value<std::string>()->default_value(""), "service description (optional, installation only)")
      ;

      po::variables_map vm;
      po::store(po::parse_command_line(myargs->argc(), myargs->argv(), install), vm);
      boost::system::error_code ec;

      if (vm.count("help"))
      {
         std::cout << install << std::cout;
         return true;
      }

      if (vm.count("-i"))
      {
         application::example::install_windows_service(
         application::setup_arg(vm["name"].as<std::string>()),
         application::setup_arg(vm["display"].as<std::string>()),
         application::setup_arg(vm["description"].as<std::string>()),
         application::setup_arg(executable_path_name)).install(ec);

         std::cout << ec.message() << std::endl;

         return true;
      }

      if (vm.count("-u"))
      {
         application::example::uninstall_windows_service(
            application::setup_arg(vm["name"].as<std::string>()),
            application::setup_arg(executable_path_name)).uninstall(ec);

         std::cout << ec.message() << std::endl;

         return true;
      }

#endif
#endif

   return false;
}
// main

int main(int argc, char *argv[])
{
   application::context app_context;

   // auto_handler will automatically add termination, pause and resume (windows) handlers
   application::auto_handler<myapp> app(app_context);
   // application::detail::handler_auto_set<myapp> app(app_context);

   // my server aspects

   app_context.insert<application::path>(
      boost::make_shared<application::path>());

   app_context.insert<application::args>(
      boost::make_shared<application::args>(argc, argv));

   // check if we need setup

   if(setup(app_context))
   {
      std::cout << "[I] Setup changed the current configuration." << std::endl;
      return 0;
   }

   // my server instantiation

   boost::system::error_code ec;
   int result = application::launch<application::server>(app, app_context, ec);

   if(ec)
   {
      std::cout << "[E] " << ec.message()
         << " <" << ec.value() << "> " << std::cout;
   }

   return result;
}
