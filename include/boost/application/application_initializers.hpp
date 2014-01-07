// application_initializers.hpp ----------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2012 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 05-04-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_APPLICATION_INITIALIZERS_HPP
#define BOOST_APPLICATION_APPLICATION_INITIALIZERS_HPP

#include <boost/application/config.hpp>
#include <boost/application/application_types.hpp>

#include <boost/filesystem/path.hpp>

namespace boost { namespace application {

   // setup session
   inline setup_type<character_types::string_type>
      setup_arg(const character_types::char_type *s)
   {
      return setup_type<character_types::string_type>(s);
   }

   inline setup_type<character_types::string_type>
      setup_arg(const character_types::string_type &s)
   {
      return setup_type<character_types::string_type>(s);
   }

   inline setup_type<character_types::string_type>
      setup_arg(const boost::filesystem::path &p)
   {
#if defined(BOOST_APPLICATION_STD_WSTRING)
      return setup_type<character_types::string_type>(p.wstring());
#else
      return setup_type<character_types::string_type>(p.string());
#endif
   }

}} // boost::application

#endif // BOOST_APPLICATION_SHARED_LIBRARY_INITIALIZERS_HPP

