// exceptions.hpp  -----------------------------------------------------------//

//  Copyright 2011-2012 Renato Tegon Forti

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

// Revision History
// 06-01-2012 dd-mm-yyyy - Initial Release

// The header contains exception classes declarations.
// 

#ifndef BOOST_APPLICATION_EXCEPTIONS_HPP
#define BOOST_APPLICATION_EXCEPTIONS_HPP

#include <boost/exception/all.hpp>

namespace boost { namespace application {
 
   // generic exception to be used insted Boost.System
   class application_exception : public std::exception
   {
   public:

      // Build a new exception exception using LastError on Windows and errno
      // on Unix
      explicit application_exception(const std::string& message, unsigned int code)
         : message_(message)
         , code_(code)
      {
      }

      ~application_exception() throw()
      {
      }

      virtual const char * what () const throw ()
      {
         return message_.c_str();
      }

      unsigned int code() const
      {
         return code_;
      }

   private:

      // The error message
      std::string message_;

      // The error code, on windows LastError, On UNIX errno
      int code_;
	  
   };
   
}} // boost::application 

#endif // BOOST_APPLICATION_EXCEPTIONS_HPP

