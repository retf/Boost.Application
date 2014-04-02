// application_impl.hpp ------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 26-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_DETAIL_APPLICATION_IMPL_HPP
#define BOOST_APPLICATION_DETAIL_APPLICATION_IMPL_HPP

// application
#include <boost/application/config.hpp>
#include <boost/application/context.hpp>

namespace boost { namespace application {

   class application_impl : noncopyable
   {
   private:
      global_context_ptr context_ptr_;

   public:
      enum instantiation_type
      {
         parameter,
         singleton
      } ;

      application_impl(instantiation_type type, context &cxt)
         : type_(type)
         , context_(cxt)
      {
      }

      application_impl(instantiation_type type, global_context_ptr cxt)
         : context_ptr_(cxt)
         , type_(type)
         , context_(*cxt.get())
      {
      }

      context &get_context()
      {
         return context_;
      }

   protected:

      instantiation_type type_;
      context &context_;
   };

}} // boost::application

#endif // BOOST_APPLICATION_DETAIL_APPLICATION_IMPL_HPP
