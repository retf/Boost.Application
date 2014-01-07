//  path_impl.hpp -----------------------------------------------------------//
// -----------------------------------------------------------------------------

//  Copyright 2011-2013 Renato Tegon Forti

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 17-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_DETAIL_WINDOWS_PATH_IMPL_HPP
#define BOOST_APPLICATION_DETAIL_WINDOWS_PATH_IMPL_HPP

#include <boost/application/config.hpp>
#include <boost/application/base_type.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/thread.hpp>

namespace boost { namespace application {

   // This class hold specific OS implementation of path.
   // [ Windows Version ]
   template <typename CharType>
   class path_impl_ : noncopyable
   {
   public:

      // string types to be used internaly to handle unicode on windows
      typedef CharType char_type;
      typedef std::basic_string<char_type> string_type;

      // Get module path name
      path_impl_(int argc, char_type *argv[],
                filesystem::path& path, boost::system::error_code &ec)
      {
         char_type module_name[MAX_PATH];

         if (GetModuleFileName(0, module_name, sizeof(module_name)) > 0)
         {
            path = string_type(module_name);
         }
         else
         {
            ec = boost::application::last_error_code();
         }
      }
   };

   /////////////////////////////////////////////////////////////////////////////
   // path_impl
   //

   // args versions for Multi-Byte string and Unicode string
   typedef path_impl_<character_types::char_type> path_impl;
   // wchar_t / char

}} // boost::application


#endif // BOOST_APPLICATION_DETAIL_WINDOWS_PATH_IMPL_HPP
