// Copyright 2011-2014 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

// #define BOOST_ALL_DYN_LINK
// #define BOOST_LIB_DIAGNOSTIC

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <boost/application.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/test/minimal.hpp>

using namespace boost;

class myapp1 {
   application::context& context_;

public:
   myapp1(application::context& context)
      : context_(context) { }

   int operator()() {
      std::cout << "myapp1" << std::endl;

      application::csbl::shared_ptr<application::args> myargs 
         = context_.find<application::args>();

      if (myargs)
      {
         const std::vector<std::string> &arg_vector = myargs->arg_vector();

         // only print args on screen
         for(std::vector<std::string>::const_iterator it = arg_vector.begin(); 
            it != arg_vector.end(); ++it) {
            std::cout << *it << std::endl;
         }

         BOOST_CHECK(arg_vector.size());
      }

      return 0;
   }
};

class myapp2 {
public:
   int operator()() {
      std::cout << "myapp2" << std::endl;

      application::csbl::shared_ptr<application::args> myargs 
         =  application::global_context::get()->find<application::args>();

      if (myargs)
      {
         const std::vector<std::string> &arg_vector = myargs->arg_vector();

         // only print args on screen
         for(std::vector<std::string>::const_iterator it = arg_vector.begin(); 
            it != arg_vector.end(); ++it) {
            std::cout << *it << std::endl;
         }

         BOOST_CHECK(arg_vector.size());
      }

      return 0;
   }
};

int test_main(int argc, char** argv)
{   

   boost::system::error_code ec;

   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp2, application::global_context>(), ec) == 0);
   /*
   BOOST_CHECK(!ec);
   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp1, application::context>(), ec) == 0);
   BOOST_CHECK(!ec);
   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp1>(), ec) == 0);
   BOOST_CHECK(!ec);

   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp2, application::global_context>()) == 0);
   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp1, application::context>()) == 0);
   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp1>()) == 0);

   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp2, application::global_context>(), boost::uuids::string_generator()("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}"), ec) == 0);
   BOOST_CHECK(!ec);
   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp1, application::context>(),boost::uuids::string_generator()("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}"), ec) == 0);
   BOOST_CHECK(!ec);
   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp1>(), boost::uuids::string_generator()("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}"), ec) == 0);
   BOOST_CHECK(!ec);

   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp2, application::global_context>(), boost::uuids::string_generator()("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}")) == 0);
   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp1, application::context>(),boost::uuids::string_generator()("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}")) == 0);
   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp1>(), boost::uuids::string_generator()("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}")) == 0);

   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp2, application::global_context>(), argc, argv, ec) == 0);
   BOOST_CHECK(!ec);
   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp1, application::context>(), argc, argv, ec) == 0);
   BOOST_CHECK(!ec);
   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp1>(), argc, argv, ec) == 0);
   BOOST_CHECK(!ec);

   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp2, application::global_context>(), argc, argv) == 0);
   BOOST_CHECK(!ec);
   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp1, application::context>(), argc, argv) == 0);
   BOOST_CHECK(!ec);
   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp1>(), argc, argv) == 0);
   BOOST_CHECK(!ec);

   
   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp2, application::global_context>(), argc, argv, boost::uuids::string_generator()("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}"), ec) == 0);
   BOOST_CHECK(!ec);
   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp1, application::context>(), argc, argv, boost::uuids::string_generator()("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}"), ec) == 0);
   BOOST_CHECK(!ec);
   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp1>(), argc, argv, boost::uuids::string_generator()("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}"), ec) == 0);
   BOOST_CHECK(!ec);

   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp2, application::global_context>(), argc, argv, boost::uuids::string_generator()("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}")) == 0);
   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp1, application::context>(), argc, argv, boost::uuids::string_generator()("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}")) == 0);
   BOOST_CHECK(application::launch<application::common>(application::auto_app<myapp1>(), argc, argv, boost::uuids::string_generator()("{2F66E4AD-ECA5-475D-8784-4BAA329EF9F1}")) == 0);
   */
   return 0;
}



