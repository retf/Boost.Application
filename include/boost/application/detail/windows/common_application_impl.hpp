//  application_impl.hpp  ---------------------------------------------------//
// -----------------------------------------------------------------------------

//  Copyright 2011-2013 Renato Tegon Forti

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 23-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_COMMON_APPLICATION_IMPL_HPP
#define BOOST_APPLICATION_COMMON_APPLICATION_IMPL_HPP

#include <boost/application/config.hpp>
#include <boost/application/base_type.hpp>
#include <boost/application/detail/application_impl.hpp>
#include <boost/application/aspects/termination_handler.hpp>
#include <boost/application/signal_binder.hpp>

#include <boost/thread/thread.hpp>
#include <boost/lambda/lambda.hpp>

// Note that singularity is in approval process,
// refer to the above link to know more:
// http://www.boost.org/community/review_schedule.html
#include <singularity/singularity.hpp>

namespace boost { namespace application {

   template <typename CharType>
   class common_application_impl_ : public application_impl
   {
   public:

      typedef boost::function< int (application::context&) > main_parameter;
      typedef boost::function< int (void) >                  main_singleton;

      // string types to be used internaly to handle unicode on windows
      typedef CharType char_type;
      typedef std::basic_string<char_type> string_type;

      common_application_impl_(const main_parameter &main,
                               signal_binder &sb,
                               application::context &context,
                               boost::system::error_code& ec)
         : application_impl(parameter, context)
         , main_parameter_(main)
      {
         sb.start();
      }

      common_application_impl_(const main_singleton &main,
                               signal_binder &sb,
                               singularity<application::context> &context,
                               boost::system::error_code& ec)
         : application_impl(singleton, context.get_global())
         , main_singleton_(main)
      {
         sb.start();
      }

      virtual ~common_application_impl_()
      {
      }

      int run()
      {
         if(type_ == parameter){
            return main_parameter_(context_);
         }
         else if(type_ == singleton){
            return main_singleton_() ;
         }

         return 1; // error
      }

   private:

      main_parameter main_parameter_;
      main_singleton main_singleton_;

   };

   /////////////////////////////////////////////////////////////////////////////
   // server_application_impl
   //

   // args versions for Multi-Byte string and Unicode string
   typedef common_application_impl_<character_types::char_type> common_application_impl;
   // wchar_t / char

}} // boost::application

#endif // BOOST_APPLICATION_COMMON_APPLICATION_IMPL_HPP

