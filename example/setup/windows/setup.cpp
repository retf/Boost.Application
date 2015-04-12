// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// server.cpp : Defines the entry point for the console application.
//


// -----------------------------------------------------------------------------
// This example shows how to use windows service setup module to provide a
// installation system to application using program_options
//
// [installation]
// service_setup_ex.exe -i --name="My Service" --path="c:\myservice\service.exe"
// service_setup_ex.exe -i --name="My Service" --path="c:\myservice\service.exe" --display="My Service"
// service_setup_ex.exe -i --name="My Service" --path="c:\myservice\service.exe" --display="My Service" --description "Service Description"
// service_setup_ex.exe -i --name="My Service" --path="c:\myservice\service.exe" --user=".\Renato Tegon Forti" --pass="x1x1x1"
// service_setup_ex.exe -i --name="My Service" --path="c:\myservice\service.exe" --start="manaul" --depends="service1\service2\service3"
// [check]
// service_setup_ex.exe -c --name="My Service" 
// [uninstallation]
// service_setup_ex.exe -u --name="My Service" --path="c:\myservice\service.exe" 
// 
// Note that when arg name are not priovided, the name will be the name of
// executable, in thiscase, service name will be: 'windows_service_setup'
// -----------------------------------------------------------------------------

#define BOOST_ALL_DYN_LINK
#define BOOST_LIB_DIAGNOSTIC

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <boost/application.hpp>
#include <boost/program_options.hpp>

#include "setup/service_setup.hpp"

#ifdef BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST
namespace ns = boost;
#else
namespace ns = std;
#endif

using namespace boost;

namespace po = program_options;

//[wss
class windows_service_setup
{
public:

   windows_service_setup(application::context& context)
      : context_(context)
   {
   }

   int operator()()
   {
   
#if !defined(__MINGW32__)  
 
      std::cout << "Setup Windows Service " 
         << "(Note that you need run this AS ADMIN!)" << std::endl;

      ns::shared_ptr<application::args> myargs 
         = context_.find<application::args>();

      // define our simple installation schema options
      po::options_description install("service options");
      install.add_options()
         ("help", "produce a help message")
         (",i", "install service")
         (",c", "check service")
         (",u", "unistall service")
         ("path", po::value<std::string>(), "service path")
         ("name", po::value<std::string>(), "service name")
         ("user", po::value<std::string>()->default_value(""), "user logon (optional, installation only)")
         ("pass", po::value<std::string>()->default_value(""), "user password (optional, installation only)")
         ("display", po::value<std::string>()->default_value(""), "service display name (optional, installation only)")
         ("description", po::value<std::string>()->default_value(""), "service description (optional, installation only)")
         ("start", po::value<std::string>()->default_value("auto"), "service start mode (optional, installation only, auto or manaul, default is auto)")
         ("depends", po::value<std::string>()->default_value(""), "other services of this service depended, multiple services using '\\' separate  (optional, installation only)")
         ;

      po::variables_map vm;
      po::store(po::parse_command_line(myargs->argc(), myargs->argv(), install), vm);
      boost::system::error_code ec;

      if (vm.count("help")) 
      {
         std::cout << install << std::endl;
         return true;
      }

      if (vm.count("-i")) 
      {
         std::string s = vm["name"].as<std::string>();
         application::example::install_windows_service(
         application::setup_arg(vm["name"].as<std::string>()), 
         application::setup_arg(vm["display"].as<std::string>()), 
         application::setup_arg(vm["description"].as<std::string>()),
         application::setup_arg(vm["path"].as<std::string>()),
         application::setup_arg(vm["user"].as<std::string>()), 
         application::setup_arg(vm["pass"].as<std::string>()),
         application::setup_arg(vm["start"].as<std::string>()),
         application::setup_arg(vm["depends"].as<std::string>())).install(ec);

         std::cout << ec.message() << std::endl;

         return true;
      }

      if (vm.count("-c")) 
      {
         bool exist =
         application::example::check_windows_service(
            application::setup_arg(vm["name"].as<std::string>())).exist(ec);

         if(ec) 
           std::cout << ec.message() << std::endl;
         else
         {
            if(exist)
               std::cout 
                  << "The service " 
                  << vm["name"].as<std::string>()
                  <<  " is installed!" 
                  << std::endl;
            else
               std::cout 
                  << "The service " 
                  << vm["name"].as<std::string>()
                  <<  " is NOT installed!" 
                  << std::endl;
         }

         return true;
      }

      if (vm.count("-u")) 
      {
         application::example::uninstall_windows_service(
            application::setup_arg(vm["name"].as<std::string>()), 
            application::setup_arg(vm["path"].as<std::string>())).uninstall(ec);
			   
         std::cout << ec.message() << std::endl;

         return true;
      }
      
      std::cout << install << std::endl;
      return true;

#endif

      return 0;
   }
 
private:
   application::context& context_;

};

int main(int argc, char *argv[])
{
   try 
   {
      application::context app_context;
      windows_service_setup app(app_context);

      app_context.insert<application::args>(
         ns::make_shared<application::args>(argc, argv));

      return application::launch<application::common>(app, app_context);
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
//]

