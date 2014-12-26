//  limit_single_instance_impl.hpp -------------------------------------------//
// -----------------------------------------------------------------------------

//  Copyright 2011-2012 Renato Tegon Forti

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 06-01-2012 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_IMPL_WINDOWS_LIMIT_SINGLE_INSTANCE_IMPL_HPP
#define BOOST_APPLICATION_IMPL_WINDOWS_LIMIT_SINGLE_INSTANCE_IMPL_HPP

#include <boost/application/config.hpp>
#include <boost/application/base_type.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace boost { namespace application {

   // This code is from Q243953 in case you lose the article and wonder
   // where this code came from.
   template <typename CharType>
   class limit_single_instance_impl_ : noncopyable
   {

   public:

      typedef CharType char_type;
      typedef std::basic_string<char_type> string_type;

      limit_single_instance_impl_()
         : mutex_(0) {}

      ~limit_single_instance_impl_() {
         release();
      }

      bool lock(const uuids::uuid& instance_id, boost::system::error_code &ec)
      {
         // Make sure that you use a name that is unique for this application otherwise
         // two apps may think they are the same if they are using same name for
         // 3rd parm to CreateMutex

         // allways we will use upper case
         string_type normalized_uuid_text =
            boost::to_upper_copy(boost::lexical_cast<string_type>(instance_id));

         mutex_ = CreateMutex(0, FALSE, normalized_uuid_text.c_str());

         if(mutex_ == 0)
         {
           ec = boost::application::last_error_code(); return false;
         }
         else
            last_error_ = GetLastError(); //save for use later...

         return is_another_instance_running();
      }

      void release(bool force = false)
      {
         // force: do nothing here force
         
         if (mutex_ == 0)
         {
            CloseHandle(mutex_);
            mutex_ = 0;
         }
      }

      bool is_another_instance_running()
      {
         return (ERROR_ALREADY_EXISTS == last_error_);
      }

   private:

      DWORD  last_error_;
      HANDLE mutex_;

   };

   typedef limit_single_instance_impl_<character_types::char_type> limit_single_instance_impl;

} }  // boost::application::win

#endif // BOOST_APPLICATION_IMPL_WINDOWS_LIMIT_SINGLE_INSTANCE_IMPL_HPP
