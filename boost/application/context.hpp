// context.hpp  --------------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 14-10-2013 dd-mm-yyyy - Initial Release
//
// Note about this LIBRARY
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
#include <boost/application/detail/csbl.hpp>
#include <boost/application/aspect_map.hpp>

namespace boost { namespace application {

   /*!
    * \brief This class is the base of Boost.Application.
    *        This class can hold any apect, and user can add one,
    *        or use some application modes that will add specifics
    *        aspects to it.
    *
    *        The heavy work is done by aspect_map.
    *
    *        Thanks to Vicente J. Botet Escriba that provide this
    *        important class and concept to the Boost.Application
    *        library.
    *
    * This class hold all aspects of an application.
    * you can use one of the ready-to-use aspects provided by library,
    * or define your own aspects.
    *
    */
   class context
      : public aspect_map, noncopyable
   {
   public:
      // nothing here! Reserved for future use.
   };

}} // boost::application

#endif // BOOST_APPLICATION_CONTEXT_HPP
