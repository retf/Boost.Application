// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// -----------------------------------------------------------------------------
// simple compiler test
// -----------------------------------------------------------------------------

#include <iostream>

#include <boost/thread/thread.hpp>
#include <boost/application.hpp>

using namespace boost::application;

int main(int argc, char* argv[])
{
   boost::system::error_code ec;

   while(1) 
   {
      boost::this_thread::sleep(boost::posix_time::seconds(1));
      std::cout << "runing...";
   }

   return 1;
}
