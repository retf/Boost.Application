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
// windows_service_setup.exe -i --name "My Service" --path "c:\myservice\service.exe"
// windows_service_setup.exe -i --name "My Service" --path "c:\myservice\service.exe" --display "My Service"
// windows_service_setup.exe -i --name "My Service" --path "c:\myservice\service.exe" --display "My Service" --description "Service Description"
// [check]
// windows_service_setup.exe -c --name "My Service" 
// [uninstallation]
// windows_service_setup.exe -u --name "My Service" --path "c:\myservice\service.exe" 
// 
// Note that when arg name are not priovided, the name will be the name of
// executable, in thiscase, service name will be: 'windows_service_setup'
// -----------------------------------------------------------------------------

#define BOOST_ALL_DYN_LINK
#define BOOST_LIB_DIAGNOSTIC

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <boost\application.hpp>
#include <boost\program_options.hpp>

using namespace boost;
namespace po = program_options;

//[wss
class windows_service_setup
{
public:

   int operator()(application::context& context)
   {
      std::cout << "Setup Windows Service " 
         << "(Note that you need run this AS ADMIN!)" << std::endl;

      boost::shared_ptr<application::args> myargs 
         = context.find<application::args>();

      // define our simple installation schema options
      po::options_description install("service options");
      install.add_options()
         ("help", "produce a help message")
         (",i", "install service")
         (",c", "check service")
         (",u", "unistall service")
         ("path", "service path")
         ("name", "service name")
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
         application::install_windows_service(
         application::setup_arg(vm["name"].as<std::string>()), 
         application::setup_arg(vm["display"].as<std::string>()), 
         application::setup_arg(vm["description"].as<std::string>()), 
         application::setup_arg(vm["path"].as<std::string>())).install(ec);

         std::cout << ec.message() << std::endl;

         return true;
      }

      if (vm.count("-c")) 
      {
         bool exist =
         application::check_windows_service(
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
         application::uninstall_windows_service(
            application::setup_arg(vm["name"].as<std::string>()), 
            application::setup_arg(vm["path"].as<std::string>())).uninstall(ec);
			   
         std::cout << ec.message() << std::endl;

         return true;
      }

      return 0;
   }
};

int main(int argc, char *argv[])
{
   try 
   {
      windows_service_setup app;
      application::context app_context;

      app_context.insert<application::args>(
         boost::make_shared<application::args>(argc, argv));

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

