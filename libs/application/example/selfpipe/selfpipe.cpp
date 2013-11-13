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
      : signal_usr2_received_(false)
   {
   }

   int operator()()
   {
      boost::shared_ptr<application::posix::selfpipe> selfpipe 
         = this_application().get_aspect<application::posix::selfpipe>();

      struct sigaction sa;
      memset(&sa, 0, sizeof(sa));

      sa.sa_handler = siganal_handler;
      sa.sa_flags = SA_RESTART;

      sigaction(SIGUSR2, &sa, NULL);

      fd_set readfds;
      FD_ZERO(&readfds);
      FD_SET(selfpipe->read_fd(), &readfds);

      // launch a work thread
      boost::thread thread(&myapp::worker, this);
	 
      int ready = 0; 
      /*<<Use select posix sustem call to waith for SIGUSR2 signal, unsing our self-pipe>>*/
      while ((ready = select(3, &readfds, NULL, NULL, NULL)) == -1 && errno == EINTR)
      {
         /*<<SIGUSR2 signal are received, stop application>>*/
         signal_usr2_received_ = true;
         thread.join(); 
         break;
      }   
              
      return 0;
   }

protected:

   /*<<Define your custon siganl handler>>*/
   static void siganal_handler(int signum)
   {
      boost::shared_ptr<application::posix::selfpipe> selfpipe 
         = this_application().get_aspect<application::posix::selfpipe>();

      if(signum == SIGUSR2) 
      {
         /*<<Notify application in case of reception of SIGUSR2 signal, unsing self-pipe>>*/
         selfpipe->poke(); 
      }
   }

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

   bool signal_usr2_received_;

};

// main

int main(int argc, char *argv[])
{   
   myapp app;
 
   boost::singularity<application::context>::create_global();
   
   /*<<Add selfpipe to application context>>*/
   this_application().add_aspect<application::posix::selfpipe>(
      boost::make_shared<application::posix::selfpipe>());
	  
   int ret = application::launch<application::common>(app, global_context);

   boost::singularity<application::context>::destroy();

   return ret;
}
//]

