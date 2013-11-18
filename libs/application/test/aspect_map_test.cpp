// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#define BOOST_ALL_DYN_LINK
#define BOOST_LIB_DIAGNOSTIC

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <boost/test/minimal.hpp>
#include <boost/application.hpp>

using namespace boost;

class my_aspect_test
{
   std::string hi_message_;

public:

   my_aspect_test(const std::string& hi_message)
      : hi_message_ (hi_message)
   {
   }
   
   const std::string& say_hi() const
   {
      return hi_message_;
   }  
   
};

int test_main(int argc, char** argv)
{   
	 
   // test add_aspect / has_aspect
   {
      application::entity::aspect_map my_aspect_map;
      my_aspect_map.add_aspect<my_aspect_test>(boost::make_shared<my_aspect_test>("HI"));

      BOOST_CHECK(my_aspect_map.has_aspect<my_aspect_test>());
   }

   // test get_aspect
   {
      application::entity::aspect_map my_aspect_map;
      my_aspect_map.add_aspect<my_aspect_test>(boost::make_shared<my_aspect_test>("HI"));
      
      boost::shared_ptr<my_aspect_test> aspect 
         = my_aspect_map.get_aspect<my_aspect_test>();

      BOOST_CHECK(aspect->say_hi() == "HI");
   }

   // test use_aspect
   {
      application::entity::aspect_map my_aspect_map;
      my_aspect_map.add_aspect<my_aspect_test>(boost::make_shared<my_aspect_test>("HI"));
      
      BOOST_CHECK(my_aspect_map.use_aspect<my_aspect_test>().say_hi()== "HI");
   }

   // test replace_aspect
   {
      application::entity::aspect_map my_aspect_map;
      my_aspect_map.add_aspect<my_aspect_test>(boost::make_shared<my_aspect_test>("HI"));

      my_aspect_map.replace_aspect<my_aspect_test>(boost::make_shared<my_aspect_test>("HI_REPLACED"));
      
      BOOST_CHECK(my_aspect_map.use_aspect<my_aspect_test>().say_hi()== "HI_REPLACED");
   }

   // test remove_aspect
   {
      application::entity::aspect_map my_aspect_map;
      my_aspect_map.add_aspect<my_aspect_test>(boost::make_shared<my_aspect_test>("HI"));

      my_aspect_map.remove_aspect<my_aspect_test>();
      
      BOOST_CHECK(my_aspect_map.has_aspect<my_aspect_test>() == false);
   }

   // test add_aspect_if_not_exists
   {
      application::entity::aspect_map my_aspect_map;
      BOOST_CHECK(my_aspect_map.add_aspect_if_not_exists<my_aspect_test>(boost::make_shared<my_aspect_test>("HI")));
      BOOST_CHECK(my_aspect_map.add_aspect_if_not_exists<my_aspect_test>(boost::make_shared<my_aspect_test>("HI")) == false);
   }

   return 0;
}



