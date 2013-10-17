// shared_library_mode.hpp ----------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 17-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_SHARED_LIBRARY_MODE_HPP
#define BOOST_APPLICATION_SHARED_LIBRARY_MODE_HPP

#include <boost/application/config.hpp>

namespace boost { namespace application {

   enum shared_library_load_mode
   {
#if defined( BOOST_WINDOWS_API )
      // windows
      dont_resolve_dll_references             = DONT_RESOLVE_DLL_REFERENCES,         // 0x00000001,   
      load_ignore_code_authz_level            = LOAD_IGNORE_CODE_AUTHZ_LEVEL,        // 0x00000010,
      load_library_as_datafile                = LOAD_LIBRARY_AS_DATAFILE,            // 0x00000002
      load_library_as_datafile_exclusive      = LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE,  // 0x00000040
      load_library_as_image_resource          = LOAD_LIBRARY_AS_IMAGE_RESOURCE,      // 0x00000020
      load_library_search_application_dir     = LOAD_LIBRARY_SEARCH_APPLICATION_DIR, // 0x00000200
      load_library_search_default_dirs        = LOAD_LIBRARY_SEARCH_DEFAULT_DIRS,    // 0x00001000
      load_library_search_dll_load_dir        = LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR,    // 0x00000100
      load_library_search_system32            = LOAD_LIBRARY_SEARCH_SYSTEM32,        // 0x00000800
      load_library_search_user_dirs           = LOAD_LIBRARY_SEARCH_USER_DIRS,       // 0x00000400
      load_with_altered_search_path           = LOAD_WITH_ALTERED_SEARCH_PATH        // 0x00000008
#elif defined( BOOST_POSIX_API )
      // unix
      rtld_lazy   = RTLD_LAZY,   // 1, 
      rtld_now    = RTLD_NOW,    // 2, 
      rtld_global = RTLD_GLOBAL, // 3,
      rtld_local  = RTLD_LOCAL   // 4
#endif
   };

}} // boost::application

#endif // BOOST_APPLICATION_SHARED_LIBRARY_MODE_HPP

