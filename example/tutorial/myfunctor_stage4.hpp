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

   myapp(application::context& context)
      : context_(context)
   {
   }

   int operator()()
   {
      std::string logfile 
         = context_.find<application::path>()->executable_path().string() + "/log.txt";
      
      my_log_file_.open(logfile.c_str());
      my_log_file_ << "Start Log..." << std::endl;

      /*<< Here we use wait_for_termination_request instead application loop >>*/
      context_.find<application::wait_for_termination_request>()->wait();

      return 0;
   }

   /*<< The 'stop' handler, available on windows and posix >>*/
   bool stop()
   {
      my_log_file_ << "Stoping my application..." << std::endl;
      my_log_file_.close();

      /*<< Return true to resume, false to tell to application mode engine to ignore the event >>*/
      return true;
   }

   /*<< The 'pause' handler, available on windows only, ignored on posix >>*/
   bool pause()
   {
      my_log_file_ << "Pause my application..." << std::endl;

      /*<< Return true to resume, false to tell to application mode engine to ignore the event >>*/
      return true;
   }

   /*<< The 'resume' handler, available on windows only, ignored on posix >>*/
   bool resume()
   {
      my_log_file_ << "Resume my application..." << std::endl;

      /*<< Return true to resume, false to tell to application mode engine to ignore the event >>*/
      return true; 
   }

private:
   application::context& context_;

   /*<< Our simple log >>*/
   std::ofstream my_log_file_;
  
};

//]

