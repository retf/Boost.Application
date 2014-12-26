// Copyright 2014 Renato Tegon Forti.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_APPLICATION_MODES_REGISTER_HPP
#define BOOST_APPLICATION_MODES_REGISTER_HPP

#include <boost/atomic.hpp>
#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

/// \file boost/application/application_mode_register.hpp
/// \brief Contains a template function that generate a unique identifier
/// for each application mode

namespace boost { namespace application {

   /*!
    * Creates a unique identifier generator for each application mode, e.g.: 
    * common, server and so on.
    *
    * \b Examples:
    * \code
    * struct my_new_mode {
    *    static int mode() {
    *       static int id = new_run_mode<int>();
    *       return id;
    *    }
    * };
    * \endcode 
    *
    * \return T that holds an new generated unique identifier to be used on 
    * application mode.
    */
   template <typename T>
   T new_run_mode() {
      static boost::atomic<T> id(-1);
      return ++id;
   }

}} // boost::application

#endif // BOOST_APPLICATION_MODES_REGISTER_HPP
