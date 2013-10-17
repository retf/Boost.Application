// shared_library_initializers.hpp -------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 05-04-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_SHARED_LIBRARY_INITIALIZERS_HPP
#define BOOST_APPLICATION_SHARED_LIBRARY_INITIALIZERS_HPP

#include <boost/application/config.hpp>
#include <boost/application/shared_library_types.hpp>

#include <boost/filesystem/path.hpp>

namespace boost { namespace application {

   // library session
   inline library_type<character_types::string_type> 
      library(const character_types::char_type *s)
   {
      return library_type<character_types::string_type>(s);
   }

   inline library_type<character_types::string_type> 
      library(const character_types::string_type &s)
   {
      return library_type<character_types::string_type>(s);
   }

   inline library_type<character_types::string_type> 
      library(const boost::filesystem::path &p)
   {
#if defined(BOOST_APPLICATION_STD_WSTRING)
      return library_type<character_types::string_type>(p.wstring());
#else
      return library_type<character_types::string_type>(p.string());
#endif
   }

   // symbol session
   inline symbol_type<character_types::string_type> 
      symbol(const character_types::char_type *s)
   {
      return symbol_type<character_types::string_type>(s);
   }

   inline symbol_type<character_types::string_type> 
      symbol(const character_types::string_type &s)
   {
      return symbol_type<character_types::string_type>(s);
   }
	
}} // boost::application

#endif // BOOST_APPLICATION_SHARED_LIBRARY_INITIALIZERS_HPP

