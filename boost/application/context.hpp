// context.hpp  --------------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 14-10-2013 dd-mm-yyyy - Initial Release
// 
// Note about this code
//
// This is a test that add "aspect" concept on Boost.Application interface.
//
// The main purpose of this test is to see how it 
// looks and how it behaves in a final use (interface use).
// 
// To get older version of Boost.Appliation refer to:
// https://sourceforge.net/projects/boostapp/
// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_CONTEXT_HPP
#define BOOST_APPLICATION_CONTEXT_HPP

// application
#include <boost/application/config.hpp>
#include <boost/application/aspect_map.hpp>

namespace boost { namespace application {

   // this class hold all aspects of an application.
   // you can use one of the ready-to-use aspects provided by library, 
   // or define your own aspects.
   class context 
      : public entity::aspect_map, noncopyable
   {
   public: 
   };

}} // boost::application 

#endif // BOOST_APPLICATION_CONTEXT_HPP
 