// server_application.hpp ----------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2012 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 10-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_SERVER_APPLICATION_HPP
#define BOOST_APPLICATION_SERVER_APPLICATION_HPP

#include <boost/application/config.hpp>
#include <boost/application/context.hpp>
#include <boost/application/status_aspect.hpp>
#include <boost/application/run_mode_aspect.hpp>

#ifndef BOOST_NO_CXX11_SMART_PTR
#include <memory>
#else
#include <boost/make_shared.hpp> 
#endif

namespace boost { namespace application {
	
#ifndef BOOST_NO_CXX11_SMART_PTR
   using std::make_shared;
#else
   using boost::make_shared;
#endif

   class server
   {
   public:
      server(application::context &context)
      {
        context.add_aspect<run_mode>(
            make_shared<run_mode>(run_mode::server));

         context.add_aspect<status>(
            make_shared<status>(status::running));
      }
   };

}} // boost::application 

#endif // BOOST_APPLICATION_SERVER_APPLICATION_HPP
       