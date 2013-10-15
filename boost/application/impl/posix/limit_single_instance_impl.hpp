// limit_single_instance.hpp ------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2012 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 03-02-2012 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_IMPL_POSIX_LIMIT_SINGLE_INSTANCE_IMPL_HPP
#define BOOST_APPLICATION_IMPL_POSIX_LIMIT_SINGLE_INSTANCE_IMPL_HPP

#include <boost/application/config.hpp>
#include <boost/application/base_type.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/application/limit_single_instance.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/scoped_ptr.hpp>

#include <cstdlib>

namespace boost { namespace application {

   template <typename value_type>
   class limit_single_instance_impl_ : noncopyable
   {

   public:

      typedef value_type char_type;
      typedef std::basic_string<char_type> string_type;

      limit_single_instance_impl_()
         : has_anyone_(false)
      {
      }

      ~limit_single_instance_impl_() 
      {
         release();
      }

      // If the thread acquires ownership of the mutex, 
      // returns true, if the another thread has ownership 
      // of the mutex, returns false. 
      bool lock(const uuids::uuid& instance_id, boost::system::error_code &ec)
      {
         name_ = 
            boost::to_upper_copy(boost::lexical_cast<string_type>(instance_id));

         try
         {
            create_shared_memory_or_die_.reset(
               new interprocess::shared_memory_object(
               interprocess::create_only, name_.c_str(), 
               interprocess::read_write));

            has_anyone_ = false; // we lock now
         } 
         catch(...)
         {
            // executable is already running
            has_anyone_ = true;
         }

         return has_anyone_;
      }
      
      void release(void)
      {
         // if I create it, I can remove it
         if(has_anyone_ == false) {
            interprocess::shared_memory_object::remove(name_.c_str());
         }

         has_anyone_ = false;
      }

      bool is_another_instance_running() 
      {
         return has_anyone_;
      }

   private:

      string_type name_;

      scoped_ptr<interprocess::shared_memory_object> create_shared_memory_or_die_;
      bool has_anyone_;
   };

   typedef limit_single_instance_impl_<character_types::char_type> limit_single_instance_impl;

}} // boost::application 

#endif // BOOST_APPLICATION_IMPL_POSIX_LIMIT_SINGLE_INSTANCE_IMPL_HPP
		
