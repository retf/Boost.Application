// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <boost/application.hpp>
#include <boost/test/minimal.hpp>

using namespace boost;

class myapp
{
public:
   int operator()()
   {
      return 0;
   }
};

class myapp2
{
public:
   myapp2(application::context& context)
      : context_(context)
   {
   }
   
   int operator()()
   {
      return 0;
   }
     
private:
   application::context& context_;
};

class my_signal_manager : public application::signal_manager
{
public:

   my_signal_manager(application::context &context)
      : signal_manager(context)
   {
      application::handler<>::callback cb
         = boost::bind(&my_signal_manager::stop, this);

      bind(SIGINT,  cb);
   }

   bool stop()
   {
      application::csbl::shared_ptr<application::wait_for_termination_request> th
         = context_.find<application::wait_for_termination_request>();

      th->proceed();

      return false;
   }
};

int test_main(int argc, char** argv)
{
   // common

   {
      application::context cxt;
      myapp2 app(cxt);

      BOOST_CHECK(!application::launch<application::common>(app, cxt));
   }

   {
      myapp app;
      application::global_context::create();

      BOOST_CHECK(!application::launch<application::common>(app, application::global_context::get()));

      application::global_context::destroy();
   }

   {
      application::context cxt;
      myapp2 app(cxt);

      boost::system::error_code ec;
      BOOST_CHECK(!application::launch<application::common>(app, cxt, ec));
      BOOST_CHECK(!ec);
   }

   {
      myapp app;
      application::global_context::create();

      boost::system::error_code ec;
      BOOST_CHECK(!application::launch<application::common>(app, application::global_context::get(), ec));
      BOOST_CHECK(!ec);

      application::global_context::destroy();
   }

   // sm

   {
      application::context cxt;
      myapp2 app(cxt);
      
      my_signal_manager sm(cxt);

      BOOST_CHECK(!application::launch<application::common>(app, sm, cxt));
   }

   {
      myapp app;
      application::global_context::create();

      my_signal_manager sm(*application::global_context::get().get());

      BOOST_CHECK(!application::launch<application::common>(app, sm, application::global_context::get()));

      application::global_context::destroy();
   }

   {
      application::context cxt; 
      myapp2 app(cxt);
      
      my_signal_manager sm(cxt);

      boost::system::error_code ec;
      BOOST_CHECK(!application::launch<application::common>(app, sm, cxt, ec));
      BOOST_CHECK(!ec);
   }

   {
      myapp app;
      application::global_context::create();
      my_signal_manager sm(*application::global_context::get().get());

      boost::system::error_code ec;
      BOOST_CHECK(!application::launch<application::common>(app, sm, application::global_context::get(), ec));
      BOOST_CHECK(!ec);
      
      application::global_context::destroy();
   }

   // server

#if defined( BOOST_WINDOWS_API )

   // on windows we always will receive error:
   // 1063 (0x427)
   // The service process could not connect to the service controller.

   {
      application::context cxt;
      myapp2 app(cxt);

      try
      {
         int ret = application::launch<application::server>(app, cxt);
      }
      catch(boost::system::system_error& se)
      {
         BOOST_CHECK(se.code().value() == 1063);
      }
   }

   {
      myapp app;
      application::global_context::create();

      try
      {
         int ret = application::launch<application::server>(app, application::global_context::get());
      }
      catch(boost::system::system_error& se)
      {
         BOOST_CHECK(se.code().value() == 1056);
      }

      application::global_context::destroy();
   }

   {
      application::context cxt;
      myapp2 app(cxt);

      boost::system::error_code ec;
      int ret = application::launch<application::server>(app, cxt, ec);
      BOOST_CHECK(ec.value() == 1056);
   }

   {
      myapp app;
      application::global_context::create();

      boost::system::error_code ec;
      int ret = application::launch<application::server>(app, application::global_context::get(), ec);
      BOOST_CHECK(ec.value() == 1056);

      application::global_context::destroy();
   }

   // sm

   {
      application::context cxt;
      myapp2 app(cxt);
      
      my_signal_manager sm(cxt);

      try
      {
         int ret = application::launch<application::server>(app, sm, cxt);
      }
      catch(boost::system::system_error& se)
      {
         BOOST_CHECK(se.code().value() == 1056);
      }
   }

   {
      myapp app;
      application::global_context::create();
      my_signal_manager sm(*application::global_context::get().get());

      try
      {
         int ret = application::launch<application::server>(app, sm, application::global_context::get());
      }
      catch(boost::system::system_error& se)
      {
         BOOST_CHECK(se.code().value() == 1056);
      }

      application::global_context::destroy();
   }

   {
     
      application::context cxt; 
      myapp2 app(cxt);
      
      my_signal_manager sm(cxt);

      boost::system::error_code ec;
      int ret = application::launch<application::server>(app, sm, cxt, ec);
      BOOST_CHECK(ec.value() == 1056);
   }

   {
      myapp app;
      application::global_context::create();
      my_signal_manager sm(*application::global_context::get().get());

      boost::system::error_code ec;
      int ret = application::launch<application::server>(app, sm, application::global_context::get(), ec);
      BOOST_CHECK(ec.value() == 1056);

      application::global_context::destroy();
   }

#elif defined( BOOST_POSIX_API )

   {
      
      application::context cxt;
      myapp2 app(cxt);

      BOOST_CHECK(!application::launch<application::server>(app, cxt));
   }

    {
      myapp app;
      application::global_context::create();

      BOOST_CHECK(!application::launch<application::server>(app, application::global_context::get()));

      application::global_context::destroy();
   }

   {
      application::context cxt;
      myapp2 app(cxt);

      boost::system::error_code ec;
      BOOST_CHECK(!application::launch<application::server>(app, cxt, ec));
      BOOST_CHECK(!ec);
   }

   {
      myapp app;
      application::global_context::create();

      boost::system::error_code ec;
      BOOST_CHECK(!application::launch<application::server>(app, application::global_context::get(), ec));
      BOOST_CHECK(!ec);

      application::global_context::destroy();
   }

   // sm

   {
      
      application::context cxt;
      myapp2 app(cxt);
      
      my_signal_manager sm(cxt);

      BOOST_CHECK(!application::launch<application::server>(app, sm, cxt));
   }

   {
      myapp app;
      application::global_context::create();

      my_signal_manager sm(*application::global_context::get().get());

      BOOST_CHECK(!application::launch<application::server>(app, sm, application::global_context::get()));

      application::global_context::destroy();
   }

   {
      application::context cxt;
      myapp2 app(cxt);
      
      my_signal_manager sm(cxt);

      boost::system::error_code ec;
      BOOST_CHECK(!application::launch<application::server>(app, sm, cxt, ec));
      BOOST_CHECK(!ec);
   }

   {
      myapp app;
      application::global_context::create();
      my_signal_manager sm(*application::global_context::get().get());

      boost::system::error_code ec;
      BOOST_CHECK(!application::launch<application::server>(app, sm, application::global_context::get(), ec));
      BOOST_CHECK(!ec);

      application::global_context::destroy();
   }

#endif

   return 0;
}





