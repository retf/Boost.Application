// termination_handler.hpp  --------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 18-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_ASPECT_TERMINATION_HANDLER_HPP
#define BOOST_APPLICATION_ASPECT_TERMINATION_HANDLER_HPP

#include <boost/application/config.hpp>
#if defined( BOOST_WINDOWS_API )
//#include <boost/application/detail/windows/termination_request_impl.hpp>
#elif defined( BOOST_POSIX_API )
//#include <boost/application/detail/posix/termination_request_impl.hpp>
#else
#error "Sorry, no boost application are available for this platform."
#endif
#include <boost/application/application_types.hpp>

namespace boost { namespace application {

   // We need know the type of application to know what type of 
   // signal/event handler we will use internally. 
   // 
   // E.g.: 
   // 
   // -> on server mode
   // 
   // * WINDOWS we need handle Service sing Windows API;
   // * POSIX we need handle sigals using ASIO.
   //
   // -> on common mode
   // 
   // * WINDOWS we need handle sigals using ASIO;
   // * POSIX we need handle sigals using ASIO.
   //

   class termination_handler : public handler        
   {
   public:
      termination_handler(const parameter_callback& callback)
         : handler(callback) {}

      termination_handler(const singleton_callback& callback)
         : handler(callback)  {}
   };

   class termination_handler_default_behaviour : public termination_handler        
   {
   public:
      termination_handler_default_behaviour(const parameter_callback& callback)
         : termination_handler(callback) {}

      termination_handler_default_behaviour(const singleton_callback& callback)
         : termination_handler(callback)  {}
   };
  
}} // boost::application

#endif // BOOST_APPLICATION_ASPECT_TERMINATION_HANDLER_HPP

