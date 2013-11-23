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

   void set(const std::string& hi_message) 
   {
      hi_message_ = hi_message;
   }  

};

shared_ptr<my_aspect_test> my_reduction_function(
   shared_ptr<my_aspect_test> lhs, shared_ptr<my_aspect_test> rhs)
{
   shared_ptr<my_aspect_test> temp(boost::make_shared<my_aspect_test>(""));

   temp->set(lhs->say_hi() + " : " + rhs->say_hi());

   return temp;
}

class parallel_access
{
   boost::thread_group group_;
   application::aspect_map my_aspect_map_;

   typedef boost::function2<shared_ptr<my_aspect_test> , shared_ptr<my_aspect_test> , shared_ptr<my_aspect_test> > reduction;

public:

   parallel_access(int n)
   {
      for ( int i = 0; i < n; ++i )
         group_.add_thread( new boost::thread(&parallel_access::simple_work_thread, this) );
   }

   void wait()
   {    
      // wait for them
      group_.join_all();
   }

   void simple_work_thread()
   {
      strict_lock<application::aspect_map> guard(my_aspect_map_); 
      {
         my_aspect_map_.insert<my_aspect_test>(boost::make_shared<my_aspect_test>("HI"), guard);

         boost::shared_ptr<my_aspect_test> myaspect = my_aspect_map_.find<my_aspect_test>(guard);

         BOOST_CHECK(myaspect);

         my_aspect_map_.erase<my_aspect_test>(guard);

         BOOST_CHECK((!my_aspect_map_.find<my_aspect_test>(guard)));

         my_aspect_map_.insert<my_aspect_test>(boost::make_shared<my_aspect_test>("HI"), guard);

         my_aspect_map_.exchange<my_aspect_test>(boost::make_shared<my_aspect_test>("HI_REPLACED"), guard);

         BOOST_CHECK((my_aspect_map_.find<my_aspect_test>(guard)->say_hi() == "HI_REPLACED"));

         reduction rf = &my_reduction_function;

         my_aspect_map_.reduce<my_aspect_test, reduction >(boost::make_shared<my_aspect_test>("HI"), rf, guard);
      }
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

   {
      typedef boost::function2<shared_ptr<my_aspect_test> , shared_ptr<my_aspect_test> , shared_ptr<my_aspect_test> > reduction;
      reduction rf = &my_reduction_function;

      application::aspect_map my_aspect_map;
      my_aspect_map.insert<my_aspect_test>(boost::make_shared<my_aspect_test>("HI"));
      my_aspect_map.reduce<my_aspect_test, reduction >(boost::make_shared<my_aspect_test>("HI"), rf);

      BOOST_CHECK(my_aspect_map.find<my_aspect_test>()->say_hi() == "HI : HI");
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

         my_aspect_map.reduce<my_aspect_test, reduction >(boost::make_shared<my_aspect_test>("HI"), rf, guard);

         BOOST_CHECK(my_aspect_map.find<my_aspect_test>(guard)->say_hi() == "HI_REPLACED : HI");
      }
   }

   parallel_access(10).wait();

   return 0;
}



