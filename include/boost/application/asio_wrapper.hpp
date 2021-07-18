#ifndef BOOST_APPLICATION_ASIO_WRAPPER_HPP
#define BOOST_APPLICATION_ASIO_WRAPPER_HPP

#ifdef ASIO_STANDALONE
#include <asio.hpp>
#include <asio/error_code.hpp>
#include <asio/system_error.hpp>
#include <filesystem>
#else
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#endif

namespace boost { namespace application {

#ifdef ASIO_STANDALONE

using aio = asio::io_context;
using error_code_t = asio::error_code;
using system_error_t = asio::system_error;
namespace fs = std::filesystem;

#else

using aio = boost::asio::io_context;
using error_code_t = boost::system::error_code;
using system_error_t = boost::system::system_error;
namespace fs = boost::filesystem;
#endif

}} // boost::application

#endif
