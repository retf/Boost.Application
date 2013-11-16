// -----------------------------------------------------------------------------
// selfpipe.cpp: examples that show how use od self-pipe on POISX
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// -----------------------------------------------------------------------------

#define BOOST_ALL_DYN_LINK
#define BOOST_LIB_DIAGNOSTIC

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

//[self
#include <iostream>
#include <boost/application.hpp>
#include <boost/application/aspects/selfpipe.hpp>

using namespace boost;

// singularity access 

boost::singularity<application::context> global_context;
inline application::context& this_application() {
   return global_context.get_global();
}

// my functor application

//[selfpipe

class sigal_state
{
   boost::mutex mutex_;
   int state_; 

public:

   sigal_state() 
      : state_(0) {}

   void alarm()
   {
      boost::lock_guard<boost::mutex> lock(mutex_);
      state_ = !state_;
   }

   void error(int errno)
   {
      boost::lock_guard<boost::mutex> lock(mutex_);
      state_ = errno;
   }

   int state()
   {
      boost::lock_guard<boost::mutex> lock(mutex_);
      return state_;
   }
};

class myapp
{
public:

   int operator()()
   {
      boost::shared_ptr<application::selfpipe> selfpipe 
         = this_application().get_aspect<application::selfpipe>();

      fd_set readfds;
      FD_ZERO(&readfds);
      FD_SET(selfpipe->read_fd(), &readfds);

      /*<<Launch a work thread>>*/
      boost::thread thread(&myapp::worker, this);

      /*<<Use select posix system call to wait for SIGUSR2 signal, using our self-pipe>>*/
      int retval = select(selfpipe->read_fd() + 1, &readfds, 0, 0, 0);

      if(retval == -1 && errno == EINTR) 
      {  
         /*<<SIGUSR2 signal are received, notify work thread using signal_usr2_received_>>*/
         sigal_state_.alarm();
      }
      else
      {
         sigal_state_.error(errno);
      }

      /*<<Wait for the end of the work>>*/
      thread.join();

      return 0;
   }

protected:

   void worker()
   {
      while(sigal_state_.state() == 0)
      {
         boost::this_thread::sleep(boost::posix_time::seconds(1));
         std::cout << "working..." << std::endl;
      }

      boost::this_thread::sleep(boost::posix_time::seconds(1));
      std::cout << "other end work..." << std::endl;
   }

private:

   sigal_state sigal_state_;

};

/*<<Define a new signal_manager to act on SIGUSR2>>*/
class signal_usr2 : public application::signal_manager
{
public:

   /*<< Customize SIGNALS bind >>*/
   signal_usr2(singularity<application::context> &cxt)
      : application::signal_manager(cxt)
   {
      application::handler::parameter_callback callback 
         = boost::bind<bool>(&signal_usr2::signal_usr2_handler, this);

      /*<< Define signal bind >>*/
      bind(SIGUSR2,  callback); 
   }

   /*<< Define signal callback >>*/
   bool signal_usr2_handler()
   {
      boost::shared_ptr<application::selfpipe> selfpipe 
         = this_application().get_aspect<application::selfpipe>();

      /*<<Notify application in case of reception of SIGUSR2 signal, unsing self-pipe>>*/
      selfpipe->poke();

      return false;
   }

};

// main

int main(int argc, char *argv[])
{   
   myapp app;
 
   boost::singularity<application::context>::create_global();
   
   /*<<Add selfpipe to application context>>*/
   this_application().add_aspect<application::posix::selfpipe>(
      boost::make_shared<application::posix::selfpipe>());
	  
   signal_usr2 sm(global_context);
   int ret = application::launch<application::common>(app, sm, global_context);

   boost::singularity<application::context>::destroy();

   return ret;
}
//]

