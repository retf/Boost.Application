// args_aspect.hpp  ----------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 14-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_ARGS_ASPECT_HPP
#define BOOST_APPLICATION_ARGS_ASPECT_HPP

#include <boost/application/config.hpp>
#include <boost/application/base_type.hpp>

namespace boost { namespace application {

   // args aspect
   //
   // an aspect that will hold args, and provide a easy way to access they
   // or user can extrat args from it to use with Program_options, getopts or argp ...
   template <typename value_type>
   class args_          
   {
   public:

      // string types to be used internaly to handle unicode on windows
      typedef value_type char_type;
      typedef std::basic_string<char_type> string_type;

      args_(int argc, char_type *argv[]) 
      {  
         for(int i=0; i <argc; i++)
            arguments_.push_back(argv[i]);
      }

      int argc() const
      {
         return arguments_.size();
      }

      char** argv() 
      {
         return &arguments_[0];
      }

      std::vector< string_type > arg_vector()
      {
         std::vector< string_type > args;
         for (int i = 0; i < argc(); ++i)
            args.push_back(argv()[i]);

         return args;
      }

   private:
      
      std::vector<char_type*> arguments_;

   };

   /////////////////////////////////////////////////////////////////////////////
   // args
   //

   // args versions for Multi-Byte string and Unicode string
   typedef args_<character_types::char_type> args;
   // wchar_t / char
      
}} // boost::application

#endif // BOOST_APPLICATION_ARGS_ASPECT_HPP

