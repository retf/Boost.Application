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

#include <iostream>
#include <boost/application.hpp>
#include <boost/application/aspects/selfpipe.hpp>
#include <boost/logic/tribool.hpp>

using namespace boost;

// singleton access

inline application::global_context_ptr this_application() {
   return application::global_context::get();
}

// my functor application

//[selfpipe

class selfpipe_state
{
   boost::mutex mutex_;
   boost::logic::tribool state_; 

public:

   selfpipe_state() 
      : state_(false) {}

   void signal()
   {
      boost::lock_guard<boost::mutex> lock(mutex_);
      state_ = true;
   }

   void error()
   {
      boost::lock_guard<boost::mutex> lock(mutex_);
      state_ = boost::logic::indeterminate;
   }

   boost::logic::tribool state()
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
         = this_application()->find<application::selfpipe>();

      fd_set readfds;
      FD_ZERO(&readfds);
      FD_SET(selfpipe->read_fd(), &readfds);

      /*<<Launch a work thread>>*/
      boost::thread thread(&myapp::worker, this);

      int retval = 0;

      /*<<Use select posix system call to wait for SIGUSR2 signal, using our self-pipe>>*/
      while((retval = select(selfpipe->read_fd() + 1, &readfds, 0, 0, 0)) == -1 && errno == EINTR) // block and wait
      {
         // nothing here, restart when signal is catch
         std::cout << "signal is catch" << std::endl;
      }
      // we are poked

      if(retval == -1) 
      {
         selfpipe_state_.error();
      }
      else
      {
         /*<<Was poked, notify work thread using selfpipe_state_>>*/
         selfpipe_state_.signal();
      } 

      /*<<Wait for the end of the work>>*/
      thread.join();

      return 0;
   }

protected:

   void worker()
   {
      while(selfpipe_state_.state() == false)
      {
         boost::this_thread::sleep(boost::posix_time::seconds(1));
         std::cout << "working..." << std::endl;
      }

      // select fail
      if(selfpipe_state_.state() == boost::logic::indeterminate) {
         std::cout << "other end work when we have an error..." << std::endl;
      }

      boost::this_thread::sleep(boost::posix_time::seconds(1));
      std::cout << "other end work..." << std::endl;
   }

private:

   selfpipe_state selfpipe_state_;

};

/*<<Define a new signal_manager to act on SIGUSR2>>*/
class signal_usr2 : public application::signal_manager
{
public:

   /*<< Customize SIGNALS bind >>*/
   signal_usr2(application::global_context_ptr cxt)
      : application::signal_manager(cxt)
   {
      application::handler<>::parameter_callback callback1
         = boost::bind(&signal_usr2::signal_usr1_handler, this);

      application::handler<>::parameter_callback callback2
         = boost::bind(&signal_usr2::signal_usr2_handler, this);

      /*<< Define signal bind >>*/ 
      bind(SIGUSR1, callback1); 
      bind(SIGUSR2, callback2); 
   }

   bool signal_usr1_handler()
   {
      std::cout << "signal_usr1_handler" << std::endl;

      return false;
   }

   /*<< Define signal callback >>*/
   bool signal_usr2_handler()
   {
      std::cout << "signal_usr2_handler" << std::endl;

      boost::shared_ptr<application::selfpipe> selfpipe 
         = this_application()->find<application::selfpipe>();

      /*<<Notify application in case of reception of SIGUSR2 signal, unsing self-pipe>>*/
      selfpipe->poke();

      return false;
   }
};

// main

int main(int argc, char *argv[])
{   
   myapp app;
 
   application::global_context_ptr ctx = application::global_context::create();
   
   /*<<Add selfpipe to application context>>*/
   this_application()->insert<application::posix::selfpipe>(
      boost::make_shared<application::posix::selfpipe>());
	  
   signal_usr2 sm(ctx);
   int ret = application::launch<application::common>(app, sm, ctx);

   return ret;
}
//]

