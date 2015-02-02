// Copyright 2011-2013 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// -----------------------------------------------------------------------------
// This example shows how to add both  application types support to application.
// [use]
// application_mode_select[.exe] -f (run as common/foreground)
// application_mode_select[.exe]    (run as server daemon) (default)
// -----------------------------------------------------------------------------

#define BOOST_ALL_DYN_LINK
#define BOOST_LIB_DIAGNOSTIC

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <boost/program_options.hpp>
#include <boost/application.hpp>

using namespace boost;
namespace po = boost::program_options;

class my_application_functor_class
{
public:

   my_application_functor_class(application::context& context)
      : context_(context)
   {
   }

   int operator()()
   {
      // your application logic here!
      // use ctrl to get state of your application...

      boost::shared_ptr<application::run_mode> modes 
         = context_.find<application::run_mode>();

      if(modes->mode() == application::common::mode())
      {
         std::cout << "Yes am I a common application!" << std::endl;
      }

      if(modes->mode() == application::server::mode())
      {
         std::cout << "Yes am I a server application!" << std::endl;
      }

      std::cout << "your application logic!" << std::endl;
      context_.find<application::wait_for_termination_request>()->wait();

      return 0;
   }

   bool stop()
   {
      std::cout << "stop!" << std::endl;
      return true; // return true to stop, false to ignore
   }

private:
   application::context& context_;

};

int main(int argc, char** argv)
{
   // select application mode

   po::variables_map vm;
   po::options_description desc;

   desc.add_options()
      (",f", "run on foreground")
      ;

   po::store(po::parse_command_line(argc, argv, desc), vm);

   application::context app_context;
   my_application_functor_class app(app_context);
   
   // add termination handler

   application::handler<>::callback termination_callback 
      = boost::bind(&my_application_functor_class::stop, &app);

   app_context.insert<application::termination_handler>(
      boost::make_shared<
         application::termination_handler_default_behaviour>(termination_callback));

   int result = 0;
   boost::system::error_code ec;

   // we will run like a daemon or like a common application (foreground)
   if (vm.count("-f")) 
   {
      result = 
         application::launch<
            application::common>(app, app_context, ec);
   }
   else
   {  
      result = 
         application::launch<
            application::server>(app, app_context, ec);
   }

   // check for error

   if(ec)
   {
      std::cout << "[E] " << ec.message() 
         << " <" << ec.value() << "> " << std::cout;
   }
   
   return result;
}

