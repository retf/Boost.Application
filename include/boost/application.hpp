// Copyright 2014 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_APPLICATION_HPP
#define BOOST_APPLICATION_HPP

/// \file boost/application.hpp
/// \brief Includes all the headers of the Boost.Application library.

// config
#include <boost/config.hpp>
#include <boost/application/config.hpp>
// application
#include <boost/application/version.hpp>
#include <boost/application/context.hpp>
#include <boost/application/launch.hpp>
#include <boost/application/auto_handler.hpp>
// #include <boost/application/auto_app.hpp>
// application/aspects
#include <boost/application/aspects/args.hpp>
#include <boost/application/aspects/limit_single_instance.hpp>
#include <boost/application/aspects/wait_for_termination_request.hpp>
#include <boost/application/aspects/path.hpp>
#include <boost/application/aspects/termination_handler.hpp>
#include <boost/application/aspects/process_id.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

#endif // BOOST_APPLICATION_HPP
