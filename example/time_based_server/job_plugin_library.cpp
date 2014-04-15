// Copyright 2011-2013 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

// we will use boost as shared libraries 
#define BOOST_ALL_DYN_LINK
#define BOOST_LIB_DIAGNOSTIC

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <iterator>

#include <boost\application.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

//[plugcpp

#if defined(_WIN32)
#   define LIBRARY_API __declspec(dllexport)
#else
#   define LIBRARY_API
#endif
   
#include "job_plugin_api.hpp"

extern "C" LIBRARY_API my_plugin_api* create_my_plugin(void);
extern "C" LIBRARY_API void delete_my_plugin(my_plugin_api* myplugin);

class my_job_plugin : public my_plugin_api
{
public:

   float version() 
   { 
      return 1.0;  
   };

   bool do_the_job(const std::string& query, const std::string& resource, std::string& result, std::string& err) 
   { 
      try
      {
         boost::asio::io_service io_service;

         // Get a list of endpoints corresponding to the server name.
         tcp::resolver resolver(io_service);
         tcp::resolver::query q(query, "http");
         tcp::resolver::iterator endpoint_iterator = resolver.resolve(q);

         // Try each endpoint until we successfully establish a connection.
         tcp::socket socket(io_service);
         boost::asio::connect(socket, endpoint_iterator);

         // Form the request. We specify the "Connection: close" header so that the
         // server will close the socket after transmitting the response. This will
         // allow us to treat all data up until the EOF as the content.
         boost::asio::streambuf request;
         std::ostream request_stream(&request);
         request_stream << "GET " << resource << " HTTP/1.0\r\n";
         request_stream << "Host: " << query << "\r\n";
         request_stream << "Accept: */*\r\n";
         request_stream << "Connection: close\r\n\r\n";

         // Send the request.
         boost::asio::write(socket, request);

         // Read the response status line. The response streambuf will automatically
         // grow to accommodate the entire line. The growth may be limited by passing
         // a maximum size to the streambuf constructor.
         boost::asio::streambuf response;
         boost::asio::read_until(socket, response, "\r\n");

         // Check that response is OK.
         std::istream response_stream(&response);
         std::string http_version;
         response_stream >> http_version;
         unsigned int status_code;
         response_stream >> status_code;
         std::string status_message;
         std::getline(response_stream, status_message);

         if (!response_stream || http_version.substr(0, 5) != "HTTP/")
         {
            err = "Invalid response";
            return true;
         }

         if (status_code != 200)
         {
            err = "Response returned with status code " + boost::lexical_cast<std::string>(status_code);
            return true;
         }

         // Read the response headers, which are terminated by a blank line.
         boost::asio::read_until(socket, response, "\r\n\r\n");

         // Process the response headers.
         std::string header;
         while (std::getline(response_stream, header) && header != "\r");

         // Read until EOF, writing data to output as we go.
         boost::system::error_code error;
         while (boost::asio::read(socket, response,   boost::asio::transfer_at_least(1), error))
         {    
            boost::asio::streambuf::const_buffers_type bufs = response.data();
            result += std::string (boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + response.size());
         }
      
         if (error != boost::asio::error::eof)
            throw boost::system::system_error(error);

      }
      catch (std::exception& e)
      {
         err = e.what();
         return true;
      }

      return false;
 
   };
   
   ~my_job_plugin()
   {
      std::cout << ";o)" << std::endl;
   }
};

my_plugin_api* create_my_plugin(void)
{
   my_plugin_api *myplugin = new my_job_plugin();
   return myplugin;
}

void delete_my_plugin(my_plugin_api* myplugin)
{
   delete myplugin;
}
//]


// platform dependent initialization sample
//

#if defined( BOOST_WINDOWS_API )

// return TRUE on success and FALSE if an error occurs. returning
// FALSE will cause the library to be unloaded.
BOOL WINAPI DllMain
(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpReserved
)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // add initialization code...
        break;
    case DLL_PROCESS_DETACH:
        // add clean-up code...
        break;
    }

    return (TRUE);
}

#elif defined( BOOST_POSIX_API )

// GCC
void __attribute__ ((constructor)) my_load(void);
void __attribute__ ((destructor)) my_unload(void);

// called when the library is loaded and before dlopen() returns
void my_load(void)
{
   // Add initialization code…
}

// called when the library is unloaded and before dlclose() returns
void my_unload(void)
{
   // Add clean-up code…
}

#endif



