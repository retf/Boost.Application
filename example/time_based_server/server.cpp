
// we will use boost as shared libraries 
#define BOOST_ALL_DYN_LINK
#define BOOST_LIB_DIAGNOSTIC

// we will use boost for type-index and smart pointers and so on
#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST
// you can use STD, to do this define BOOST_APPLICATION_FEATURE_NS_SELECT_STD
// then use std::make_shared and so on

#include <iostream>
#include <fstream>

#include <boost/application.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
// -
#include "job_plugin_api.hpp"

// provide setup example for windows service   
#if defined(BOOST_WINDOWS_API)      
#   include "setup/windows/setup/service_setup.hpp"
#endif

using namespace boost::application;

namespace pt = boost::property_tree;
namespace po = boost::program_options;

// we will use global_context for our sample, we can access global_context in 
// any place of our application
inline global_context_ptr this_application_cxt() {
   return global_context::get();
}

// our timer aspect
template< typename Handler >
class timer_job : boost::noncopyable
{
   typedef handler<bool>::callback job_callback;

public:

   timer_job(Handler h, unsigned int timeout)
      : jtimer_(io_service_), timeout_(timeout), job_(h)
   {
      trigger();
      boost::thread(
         boost::bind(&boost::asio::io_service::run, 
            boost::ref(io_service_))).detach(); 
   }

   void stop()
   {
      io_service_.stop(); 
   }

protected:

   bool job()
   {
      if(job_()) // retur true from job to stop 
         return true;

      trigger(); // trigger next timeout event
      return false; 
   }

   void trigger()
   {
      jtimer_.expires_from_now(boost::posix_time::seconds(timeout_));
      jtimer_.async_wait(
         [this](const boost::system::error_code& ec) -> void {
            if(!ec) {
               if(this->job()) {
                  io_service_.stop(); // error 
               }
            } 
            else 
               io_service_.stop(); // 'ec' is true 
         } 
      );
   }

private:
   unsigned int timeout_;

   job_callback job_;

   boost::asio::io_service io_service_; 
   boost::asio::deadline_timer jtimer_;  
};

typedef timer_job< handler<bool>::callback > timer_callback;

// our server class
class time_based_job : boost::noncopyable
{
   typedef my_plugin_api* (*pluginapi_create) (void);
   typedef void (*pluginapi_delete) (my_plugin_api* myplugin);

   std::ofstream weather_log_;

public:

   int operator()()
   {
      boost::shared_ptr<path> pt 
         = this_application_cxt()->find<path>();

      boost::filesystem::path scan_dir(pt->executable_path().string() + "/weather_log.txt");

      weather_log_.open(scan_dir.string(), std::ios::out | std::ios::app); 

      this_application_cxt()->find<wait_for_termination_request>()->wait();
      return 0;
   }

   bool stop()
   {
      if (weather_log_.is_open())
      {
         weather_log_.close();
      }

      this_application_cxt()->find<timer_callback>()->stop();

      return true;
   }

   bool doit()
   {  
     boost::shared_ptr<path> pt 
         = this_application_cxt()->find<path>();

      boost::filesystem::path scan_dir(pt->executable_path());

      std::vector<boost::filesystem::path> v; // store paths
      std::copy(boost::filesystem::directory_iterator(scan_dir), 
			      boost::filesystem::directory_iterator(), back_inserter(v));
  
      int count_synced_files = 0;
      for (std::vector<boost::filesystem::path>::const_iterator it
            (v.begin()); it != v.end(); ++it)
      {
         boost::filesystem::path p(*it);

         std::string base = boost::to_lower_copy(p.extension().string());
  
         if( base == shared_library::suffix() ) // dll or so
         {
            std::cout << "our plugin: " << p.string() << std::endl;

            my_plugin_api* plugin = NULL;

            shared_library sl(library(p.string()));

            if(sl.search_symbol(symbol("create_my_plugin")))
            {
               plugin = ((pluginapi_create)sl(symbol("create_my_plugin")))();
            }

            if(plugin != NULL)
            {   
               boost::shared_ptr<void> delete_plugin(nullptr, [&](void*) {
                     // this code will be executed at the scope exit
                     ((pluginapi_delete) sl(symbol("delete_my_plugin")))(plugin);
               });

               std::cout << "Plugin Version: " << plugin->version() << std::endl;

               std::string query = "api.openweathermap.org", resource = "/data/2.5/find?q=Americana,br&mode=json", aswer, err;

               if(plugin->do_the_job(query, resource, aswer, err))
                  std::cout << "Error from plugin: " << err << std::endl;
               else
               {
                  /* 
                  {
                      "message": "accurate",
                      "cod": "200",
                      "count": 1,
                      "list": [
                          {
                              "id": 3472343,
                              "name": "Americana",
                              "coord": {
                                  "lon": -47.33139,
                                  "lat": -22.73917
                              },
                              "main": {
                                  "temp": 302.15,
                                  "pressure": 1018,
                                  "humidity": 48,
                                  "temp_min": 302.15,
                                  "temp_max": 302.15
                              },
                              "dt": 1396634400,
                              "wind": {
                                  "speed": 6.7,
                                  "deg": 130
                              },
                              "sys": {
                                  "country": "BR"
                              },
                              "clouds": {
                                  "all": 40
                              },
                              "weather": [
                                  {
                                      "id": 802,
                                      "main": "Clouds",
                                      "description": "scattered clouds",
                                      "icon": "03d"
                                  }
                              ]
                          }
                      ]
                  }
                  */

                  pt::ptree json_reponse; std::istringstream is(aswer);
                  read_json(is, json_reponse);

                  BOOST_FOREACH(boost::property_tree::ptree::value_type& v, json_reponse.get_child("list"))
                  {
                     if(v.second.get<int>("main.humidity") < 80)
                     {
                        // record
                        weather_log_ << v.second.get<std::string>("dt") << ":" << v.second.get<std::string>("main.humidity") << std::endl;
                     }
                  }
               }
            }

         }
      }

      return false;
   }
};

// my setup code for windows service
bool setup()
{
   boost::shared_ptr<args> myargs 
      = this_application_cxt()->find<args>();

   boost::shared_ptr<path> mypath 
      = this_application_cxt()->find<path>();
   
// provide setup for windows service   
#if defined(BOOST_WINDOWS_API)      

   // get our executable path name
   boost::filesystem::path executable_path_name = mypath->executable_path_name();

   std::string exename = mypath->executable_name().stem().string();

   // define our simple installation schema options
   po::options_description install("service options");
   install.add_options()
      ("help", "produce a help message")
      (",i", "install service")
      (",u", "unistall service")
      ("user", po::value<std::string>()->default_value(""), 
         "user logon (optional, installation only)")
      ("pass", po::value<std::string>()->default_value(""), 
         "user password (optional, installation only)")
      ("name", po::value<std::string>()->default_value(exename), 
         "service name")
      ("display", po::value<std::string>()->default_value(""), 
         "service display name (optional, installation only)")
      ("description", po::value<std::string>()->default_value(""), 
         "service description (optional, installation only)")
      ;

      po::parsed_options parsed = 
      po::command_line_parser(myargs->argc(), myargs->argv()
         ).options(install).allow_unregistered().run();  

      po::variables_map vm;
      po::store(parsed, vm);
      boost::system::error_code ec;

      if (vm.count("help")) 
      {
         std::cout << install << std::cout;
         return true;
      }

      if (vm.count("-i")) 
      {
         example::install_windows_service(
         setup_arg(vm["name"].as<std::string>()), 
         setup_arg(vm["display"].as<std::string>()), 
         setup_arg(vm["description"].as<std::string>()), 
         setup_arg(executable_path_name),
         setup_arg(vm["user"].as<std::string>()), 
         setup_arg(vm["pass"].as<std::string>())).install(ec);

         std::cout << ec.message() << std::endl;

         return true;
      }

      if (vm.count("-u")) 
      {
         example::uninstall_windows_service(
            setup_arg(vm["name"].as<std::string>()), 
            setup_arg(executable_path_name)).uninstall(ec);
			   
         std::cout << ec.message() << std::endl;

         return true;
      }
#endif

   return false;
}

// main
int main(int argc, char *argv[])
{  
   time_based_job app; 

   // we will get erros checking 'boost::system::error_code', 
   // if you want you can use 'try' here and catch 
   // for 'boost::system::system_error'!
   boost::system::error_code ec;
 
   // create a global context for application (our aspect_map), 
   // from this point it is available to be used on any part of our application.
   global_context::create();
   boost::shared_ptr<void> global_context_destroy(nullptr, [&](void*) {
      // this code will be executed at the scope exit
      // destroy our global_context and ends our application
      global_context::destroy();
   });

   // add some aspects
   this_application_cxt()->insert<args>(boost::make_shared<args>(argc, argv));

   // start our timer  

   this_application_cxt()->insert<
      timer_callback >(boost::make_shared<
         timer_callback >(
            handler<bool>::make_callback(app, &time_based_job::doit), 5) );
 
   this_application_cxt()->insert<termination_handler>(
      boost::make_shared<termination_handler_default_behaviour>(
         handler<bool>::make_callback(app, &time_based_job::stop)));

   this_application_cxt()->insert<path>(
     boost::make_shared<path_default_behaviour>(argc, argv));

   // check if we need setup

   if(setup())
   {
      std::cout << "[I] Setup changed the current configuration." << std::endl;
      return 0;
   }

   // we will provide a way to start an application as a 'common' application
   // mainly for debug purpose. 
   // A 'common' application is a usual Interactive Terminal application.

   std::vector<std::string> arg_list 
      = this_application_cxt()->find<args>()->arg_vector();

   int ret = 0;
   if(std::find(arg_list.begin(), arg_list.end(), "--common") != arg_list.end())
      // launch our application as a common app
      ret = launch<common>(app, this_application_cxt(), ec);
   else
   {
      // launch our application as a server app (service / daemon)
      ret = launch<server>(app, this_application_cxt(), ec);
   }

   if(ec)
   {
      std::cerr << "[E] " << ec.message() 
         << " <" << ec.value() << "> " << std::cout;
   }

   return ret;
}





