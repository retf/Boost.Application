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
	 
   //
   // Internal locking Version.
   //

   {
      application::aspect_map my_aspect_map;
      my_aspect_map.insert<my_aspect_test>(boost::make_shared<my_aspect_test>("HI"));

      BOOST_CHECK(my_aspect_map.find<my_aspect_test>());
   }

   {
      application::aspect_map my_aspect_map;
      my_aspect_map.insert<my_aspect_test>(boost::make_shared<my_aspect_test>("HI"));

      BOOST_CHECK(my_aspect_map.find<my_aspect_test>()->say_hi()  == "HI");
   }

   {
      application::aspect_map my_aspect_map;
      my_aspect_map.insert<my_aspect_test>(boost::make_shared<my_aspect_test>("HI"));

      my_aspect_map.exchange<my_aspect_test>(boost::make_shared<my_aspect_test>("HI_REPLACED"));
      
      BOOST_CHECK(my_aspect_map.find<my_aspect_test>()->say_hi()== "HI_REPLACED");
   }

   {
      application::aspect_map my_aspect_map;
      my_aspect_map.insert<my_aspect_test>(boost::make_shared<my_aspect_test>("HI"));

      my_aspect_map.erase<my_aspect_test>();
      
      BOOST_CHECK(my_aspect_map.find<my_aspect_test>() == false);
   }

   // others
   
   {
      application::aspect_map my_aspect_map;
      my_aspect_map.insert<my_aspect_test>(boost::make_shared<my_aspect_test>("HI"));
      
      BOOST_CHECK(my_aspect_map.size());
   }

   {
      application::aspect_map my_aspect_map;
      my_aspect_map.insert<my_aspect_test>(boost::make_shared<my_aspect_test>("HI"));
      
      BOOST_CHECK(!my_aspect_map.empty());
   }

   {
      application::aspect_map my_aspect_map;
      my_aspect_map.insert<my_aspect_test>(boost::make_shared<my_aspect_test>("HI"));

      my_aspect_map.clear();
      
      BOOST_CHECK(my_aspect_map.empty());
   }

   //
   // External locking Version, operations can be combined.
   //

   {
      application::aspect_map my_aspect_map;

      strict_lock<application::aspect_map> guard(my_aspect_map); 

      // atomic
      {
         my_aspect_map.insert<my_aspect_test>(boost::make_shared<my_aspect_test>("HI"), guard);

         boost::shared_ptr<my_aspect_test> myaspect = my_aspect_map.find<my_aspect_test>(guard);

         BOOST_CHECK(myaspect);

         my_aspect_map.erase<my_aspect_test>(guard);

         BOOST_CHECK(!my_aspect_map.find<my_aspect_test>(guard));

         my_aspect_map.insert<my_aspect_test>(boost::make_shared<my_aspect_test>("HI"), guard);

         my_aspect_map.exchange<my_aspect_test>(boost::make_shared<my_aspect_test>("HI_REPLACED"), guard);

         BOOST_CHECK(my_aspect_map.find<my_aspect_test>(guard)->say_hi() == "HI_REPLACED");
      }
   }

   return 0;
}



