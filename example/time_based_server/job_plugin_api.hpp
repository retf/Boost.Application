// Copyright 2011-2013 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_APPLICATION_MY_PLUGIN_API_HPP
#define BOOST_APPLICATION_MY_PLUGIN_API_HPP

class my_plugin_api
{
public:
   virtual ~my_plugin_api(){};
   virtual float version() = 0;
   virtual bool do_the_job(
      const std::string& query, const std::string& resource, 
      std::string& result, std::string& err) = 0;
};

#endif // BOOST_APPLICATION_MY_PLUGIN_API_HPP

