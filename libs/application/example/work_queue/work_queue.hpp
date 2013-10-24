// Copyright 2011-2013 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_APPLICATION_WORK_QUEUE_HPP
#define BOOST_APPLICATION_WORK_QUEUE_HPP

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

// work_queue class 
// NWorkers : nuber of worker threads 
// provide 0 to use available cores of machine
template <int NWorkers = 0>
class work_queue
{
public:

   work_queue()
   {
      work_ctrl_ = new boost::asio::io_service::work(io_service_);

      int workers = boost::thread::hardware_concurrency();
      if(NWorkers > 0)
         workers = NWorkers;
      
      for (std::size_t i = 0; i < workers; ++i)
      {
         threads_.create_thread(boost::bind(&boost::asio::io_service::run, &io_service_));
      }
   }

   virtual ~work_queue()
   {
      delete work_ctrl_;
   }

   template <typename TTask>
   void add_task(TTask task)
   {
      // c++11
      // io_service_.dispatch(std::move(task));
      io_service_.dispatch(task);
   }

private:

   boost::asio::io_service io_service_;
   boost::thread_group threads_;

   boost::asio::io_service::work *work_ctrl_;
};

#endif // BOOST_APPLICATION_WORK_QUEUE_HPP


