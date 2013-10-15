// shared_library.hpp --------------------------------------------------------//

// Copyright 2011-2012 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// Revision History
// 05-04-2013 dd-mm-yyyy - Initial Release

#ifndef BOOST_APPLICATION_SHARED_LIBRARY_TYPES_HPP
#define BOOST_APPLICATION_SHARED_LIBRARY_TYPES_HPP

#include <boost/application/config.hpp>
#include <boost/application/base_type.hpp>

namespace boost { namespace application {

   template <typename String>
   class library_type 
      : public base_type<String>
   {
   public:
      explicit library_type(const String &s)
         : base_type<String>(s) {}
   };

   template <typename String>
   class symbol_type 
      : public base_type<String>
   {
   public:
      explicit symbol_type(const String &s)
         : base_type<String>(s) {}
   };

}} // boost::application

#endif // BOOST_APPLICATION_SHARED_LIBRARY_TYPES_HPP

