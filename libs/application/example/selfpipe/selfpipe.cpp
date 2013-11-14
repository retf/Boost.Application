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
class myapp
{
public:

   myapp()
      : signal_usr2_received_(false){ }

   int operator()()
   {
      boost::shared_ptr<application::posix::selfpipe> selfpipe 
         = this_application().get_aspect<application::posix::selfpipe>();

      fd_set readfds;
      FD_ZERO(&readfds);
      FD_SET(selfpipe->read_fd(), &readfds);

      /*<<Launch a work thread>>*/
      boost::thread thread(&myapp::worker, this);
	 
      int ready = 0; 

      /*<<Use select posix system call to waith for SIGUSR2 signal, unsing our self-pipe>>*/
      while ((ready = select(selfpipe->read_fd() + 1, &readfds, NULL, NULL, NULL)) == -1 && errno == EINTR)
      {
         /*<<SIGUSR2 signal are received, notify work thread using signal_usr2_received_>>*/
         signal_usr2_received_ = true;

         /*<<Wait for the end of the work>>*/
         thread.join(); 
         break;
      }   
              
      return 0;
   }

protected:

   void worker()
   {
      while(!signal_usr2_received_)
      {
         boost::this_thread::sleep(boost::posix_time::seconds(1));
         std::cout << "working..." << std::endl;
      }

      boost::this_thread::sleep(boost::posix_time::seconds(1));
      std::cout << "other end work..." << std::endl;
   }

private:

   volatile bool signal_usr2_received_;

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
      boost::shared_ptr<application::posix::selfpipe> selfpipe 
         = this_application().get_aspect<application::posix::selfpipe>();

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

