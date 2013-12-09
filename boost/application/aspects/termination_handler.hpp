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
#include <boost/application/handler.hpp>

namespace boost { namespace application {

   class termination_handler : public handler<>
   {
   public:
      termination_handler(const parameter_callback& callback)
         : handler<>(callback) {}

      termination_handler(const singleton_callback& callback)
         : handler<>(callback)  {}
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

