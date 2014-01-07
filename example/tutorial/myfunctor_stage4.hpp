// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// This file is part of tutorial, refer to 'last stage' to see a code that 
// can be compiled.
//

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <boost/application.hpp>

using namespace boost;

//[tutorials4

class myapp
{
public:

  // param
   int operator()(application::context& context)
   {
      std::string logfile 
         = context.find<application::path>()->executable_path().string() + "/log.txt";
      
      my_log_file_.open(logfile.c_str());
      my_log_file_ << "Start Log..." << std::endl;

      /*<< Here we use wait_for_termination_request instead application loop >>*/
      context.find<application::wait_for_termination_request>()->wait();

      return 0;
   }

   /*<< The 'stop' handler, available on windows and posix >>*/
   bool stop(application::context& context)
   {
      my_log_file_ << "Stoping my application..." << std::endl;
      my_log_file_.close();

      /*<< Return true to resume, false to tell to application mode engine to ignore the event >>*/
      return true;
   }

   /*<< The 'pause' handler, available on windows only, ignored on posix >>*/
   bool pause(application::context& context)
   {
      my_log_file_ << "Pause my application..." << std::endl;

      /*<< Return true to resume, false to tell to application mode engine to ignore the event >>*/
      return true;
   }

   /*<< The 'resume' handler, available on windows only, ignored on posix >>*/
   bool resume(application::context& context)
   {
      my_log_file_ << "Resume my application..." << std::endl;

      /*<< Return true to resume, false to tell to application mode engine to ignore the event >>*/
      return true; 
   }

private:

   /*<< Our simple log >>*/
   std::ofstream my_log_file_;
  
};

//]

