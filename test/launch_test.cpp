// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

// #define BOOST_ALL_DYN_LINK
// #define BOOST_LIB_DIAGNOSTIC

#include <iostream>
#include <boost/application.hpp>
#include <boost/test/minimal.hpp>

using namespace boost;

boost::singularity<application::context> global_context;
inline application::context& this_application() {
   return global_context.get_global();
}

class myapp_parameter
{
public:
   int operator()(application::context& context)
   {
      return 0;
   }
};

class myapp_singleton
{
public:
   int operator()()
   {
      return 0;
   }
};

class my_signal_manager : public application::signal_manager
{
public:

   my_signal_manager(application::context &context)
      : signal_manager(context)
   {
      application::handler<>::parameter_callback callback
         = boost::bind<bool>(&my_signal_manager::stop, this, _1);

      bind(SIGINT,  callback);
   }

   my_signal_manager(boost::singularity<application::context> &context)
      : signal_manager(context)
   {
      application::handler<>::singleton_callback callback
         = boost::bind<bool>(&my_signal_manager::stop, this);

      bind(SIGINT,  callback);
   }

   bool stop(application::context &context)
   {
      application::csbl::shared_ptr<application::wait_for_termination_request> th
         = context.find<application::wait_for_termination_request>();

      th->proceed();

      return false;
   }

   bool stop()
   {
      application::csbl::shared_ptr<application::wait_for_termination_request> th
         = this_application().find<application::wait_for_termination_request>();

      th->proceed();

      return false;
   }
};

int test_main(int argc, char** argv)
{
   application::context app_context;

   // common

   {
      myapp_parameter app;
      application::context cxt;

      BOOST_CHECK(!application::launch<application::common>(app, cxt));
   }

   {
      myapp_singleton app;
      boost::singularity<application::context>::create_global();

      BOOST_CHECK(!application::launch<application::common>(app, global_context));

      boost::singularity<application::context>::destroy();
   }

   {
      myapp_parameter app;
      application::context cxt;

      boost::system::error_code ec;
      BOOST_CHECK(!application::launch<application::common>(app, cxt, ec));
      BOOST_CHECK(!ec);
   }

   {
      myapp_singleton app;
      boost::singularity<application::context>::create_global();

      boost::system::error_code ec;
      BOOST_CHECK(!application::launch<application::common>(app, global_context, ec));
      BOOST_CHECK(!ec);

      boost::singularity<application::context>::destroy();
   }

   // sm

   {
      myapp_parameter app;
      application::context cxt;
      my_signal_manager sm(cxt);

      BOOST_CHECK(!application::launch<application::common>(app, sm, cxt));
   }

   {
      myapp_singleton app;
      boost::singularity<application::context>::create_global();

      my_signal_manager sm(global_context);

      BOOST_CHECK(!application::launch<application::common>(app, sm, global_context));

      boost::singularity<application::context>::destroy();
   }

   {
      myapp_parameter app;
      application::context cxt;
      my_signal_manager sm(cxt);

      boost::system::error_code ec;
      BOOST_CHECK(!application::launch<application::common>(app, sm, cxt, ec));
      BOOST_CHECK(!ec);
   }

   {
      myapp_singleton app;
      boost::singularity<application::context>::create_global();
      my_signal_manager sm(global_context);

      boost::system::error_code ec;
      BOOST_CHECK(!application::launch<application::common>(app, sm, global_context, ec));
      BOOST_CHECK(!ec);

      boost::singularity<application::context>::destroy();
   }

   // server

#if defined( BOOST_WINDOWS_API )

   // on windows we always will receive error:
   // 1063 (0x427)
   // The service process could not connect to the service controller.

   {
      myapp_parameter app;
      application::context cxt;

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
      myapp_singleton app;
      boost::singularity<application::context>::create_global();

      try
      {
         int ret = application::launch<application::server>(app, global_context);
      }
      catch(boost::system::system_error& se)
      {
         BOOST_CHECK(se.code().value() == 1056);
      }

      boost::singularity<application::context>::destroy();
   }

   {
      myapp_parameter app;
      application::context cxt;

      boost::system::error_code ec;
      int ret = application::launch<application::server>(app, cxt, ec);
      BOOST_CHECK(ec.value() == 1056);
   }

   {
      myapp_singleton app;
      boost::singularity<application::context>::create_global();

      boost::system::error_code ec;
      int ret = application::launch<application::server>(app, global_context, ec);
      BOOST_CHECK(ec.value() == 1056);

      boost::singularity<application::context>::destroy();
   }

   // sm

   {
      myapp_parameter app;
      application::context cxt;
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
      myapp_singleton app;
      boost::singularity<application::context>::create_global();
      my_signal_manager sm(global_context);

      try
      {
         int ret = application::launch<application::server>(app, sm, global_context);
      }
      catch(boost::system::system_error& se)
      {
         BOOST_CHECK(se.code().value() == 1056);
      }

      boost::singularity<application::context>::destroy();
   }

   {
      myapp_parameter app;
      application::context cxt;
      my_signal_manager sm(cxt);

      boost::system::error_code ec;
      int ret = application::launch<application::server>(app, sm, cxt, ec);
      BOOST_CHECK(ec.value() == 1056);
   }

   {
      myapp_singleton app;
      boost::singularity<application::context>::create_global();
      my_signal_manager sm(global_context);

      boost::system::error_code ec;
      int ret = application::launch<application::server>(app, sm, global_context, ec);
      BOOST_CHECK(ec.value() == 1056);

      boost::singularity<application::context>::destroy();
   }

#elif defined( BOOST_POSIX_API )

   {
      myapp_parameter app;
      application::context cxt;

      BOOST_CHECK(!application::launch<application::server>(app, cxt));
   }

    {
      myapp_singleton app;
      boost::singularity<application::context>::create_global();

      BOOST_CHECK(!application::launch<application::server>(app, global_context));

      boost::singularity<application::context>::destroy();
   }

   {
      myapp_parameter app;
      application::context cxt;

      boost::system::error_code ec;
      BOOST_CHECK(!application::launch<application::server>(app, cxt, ec));
      BOOST_CHECK(!ec);
   }

   {
      myapp_singleton app;
      boost::singularity<application::context>::create_global();

      boost::system::error_code ec;
      BOOST_CHECK(!application::launch<application::server>(app, global_context, ec));
      BOOST_CHECK(!ec);

      boost::singularity<application::context>::destroy();
   }

   // sm

   {
      myapp_parameter app;
      application::context cxt;
      my_signal_manager sm(cxt);

      BOOST_CHECK(!application::launch<application::server>(app, sm, cxt));
   }

   {
      myapp_singleton app;
      boost::singularity<application::context>::create_global();

      my_signal_manager sm(global_context);

      BOOST_CHECK(!application::launch<application::server>(app, sm, global_context));

      boost::singularity<application::context>::destroy();
   }

   {
      myapp_parameter app;
      application::context cxt;
      my_signal_manager sm(cxt);

      boost::system::error_code ec;
      BOOST_CHECK(!application::launch<application::server>(app, sm, cxt, ec));
      BOOST_CHECK(!ec);
   }

   {
      myapp_singleton app;
      boost::singularity<application::context>::create_global();
      my_signal_manager sm(global_context);

      boost::system::error_code ec;
      BOOST_CHECK(!application::launch<application::server>(app, sm, global_context, ec));
      BOOST_CHECK(!ec);

      boost::singularity<application::context>::destroy();
   }

#endif

   return 0;
}





