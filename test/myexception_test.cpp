// Copyright 2011-2014 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#define BOOST_TEST_MODULE myexception_test


#include <iostream>
#include <boost/application.hpp>
#include <boost/test/unit_test.hpp>

using namespace boost;

#include <iostream>
#include <sstream>

// you should probably add it to a header
// ->

// generic exception to be used insted Boost.System
class myexception_base : public std::exception
{
public:
   // build a new exception using LastError on Windows and errno on Unix
   explicit myexception_base(const std::string& msg, unsigned int code)
      : message_(msg) , code_(code)
   { }

   virtual const char * what () const throw () {
      return message_.c_str();
   }

   unsigned int code() const  {
      return code_;
   }

private:
   std::string message_;  int code_;  
};

struct myexception : public myexception_base 
{ 
   myexception(const std::string& what, unsigned int error_code) 
     : myexception_base(what, error_code) {}

   std::string mymessage()
   {
       std::stringstream msg;

       msg 
          << what ()
          << "("
          << code()
          << ")"
          << std::endl;

      return msg.str();
   }
};

#define BOOST_APPLICATION_TROWN_MY_OWN_EXCEPTION throw myexception
#include <boost/application.hpp>
// <-

using namespace boost;

class myapp
{
public:

   myapp(application::context& context)
      : context_(context)
   {
   }

   int operator()()
   {
      if(!context_.find<application::args>())
      {
         throw myexception("custon error", 4121);
      }

      return 0;
   }

private:
   application::context& context_;

};




BOOST_AUTO_TEST_CASE(myexception_throw)
{
   {
      application::context app_context;      
      myapp app(app_context);

      BOOST_CHECK_THROW(application::launch<application::common>(app, app_context), myexception);
   }
}

/*
int test_main(int argc, char** argv)
{   

   try 
   {
      application::context app_context;      
      myapp app(app_context);

       BOOST_CHECK_THROW(application::launch<application::common>(app, app_context), myexception);


      return application::launch<application::common>(app, app_context);
   }
   catch(myexception &e)
   {
      std::cerr << e.mymessage();

      BOOST_CHECK(e.code() == 4121);

      return 0;
   }

   BOOST_CHECK(0);

   return 0;
}



*/