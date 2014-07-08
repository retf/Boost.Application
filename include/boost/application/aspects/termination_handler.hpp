// termination_handler.hpp  --------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2014 Renato Tegon Forti

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
      termination_handler(const callback& cb)
         : handler<>(cb) {}
   };

   class termination_handler_default_behaviour : public termination_handler
   {
   public:
      termination_handler_default_behaviour(const callback& cb)
         : termination_handler(cb) {}
   };

}} // boost::application

#endif // BOOST_APPLICATION_ASPECT_TERMINATION_HANDLER_HPP

