// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// -----------------------------------------------------------------------------
// This example shows how implement a work quee (thread pool) to work with
// Boost.Application using Boost.Asio.
//
// The result will be printed on CTRL-C (Stop) signal
// -----------------------------------------------------------------------------

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/application.hpp>
#include <boost/timer/timer.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <math.h>

#include "work_queue.hpp"

using namespace std;
using namespace boost;

// worker class that calculate gaussian blur
// http://en.wikipedia.org/wiki/Gaussian_blur
template< int kernelRadius = 3> 
struct gaussian_blur
{
   typedef boost::function< void (vector< vector<double> >) > callback;

   gaussian_blur(const callback& cb)
      : callback_(cb)
   {
   }

   void operator()()
   {
      boost::timer::cpu_timer timer;

      kernel2d_ = produce_gaussian_kernel(kernelRadius);
      
      boost::timer::cpu_times const elapsed_times(timer.elapsed());

      std::cout 
         << "gaussian_blur takes:" 
         <<  format(elapsed_times, 9) 
         << ", for size: " 
         << kernelRadius 
         << std::endl;

      callback_(kernel2d_);
   }

protected:

   double gaussian (double x, double mu, double sigma)
   {
      return exp( -(((x-mu)/(sigma))*((x-mu)/(sigma)))/2.0 );
   }

   vector< vector<double> > produce_gaussian_kernel (int internalKernelRadius) 
   {
      // get kernel matrix
      vector< vector<double> > kernel2d ( 2*internalKernelRadius+1, vector<double>(2*internalKernelRadius+1) );

      // determine sigma
      double sigma = internalKernelRadius/2.;

      // fill values
      double sum = 0;
      for (int row = 0; row < kernel2d.size(); row++)
      {
         for (int col = 0; col < kernel2d[row].size(); col++) 
         {
            kernel2d[row][col] = gaussian(row, internalKernelRadius, sigma) * gaussian(col, internalKernelRadius, sigma);
            sum += kernel2d[row][col];
         }
      }

      // normalize kernel, or the image becomes dark 
      for (int row = 0; row < kernel2d.size(); row++)
         for (int col = 0; col < kernel2d[row].size(); col++)
            kernel2d[row][col] /= sum;

      return kernel2d;
   }

private:

   callback callback_;
   vector< vector<double> > kernel2d_;
};

// application class
class myapp : work_queue<0> 
{
public: 

   myapp(application::context& context)
      : context_(context)
   {
   }
   
   void add_result(vector< vector<double> > kernel2d)
   {
      boost::lock_guard<boost::mutex> lock(mutex_);

      task_count_++;

      result_.push_back(kernel2d);

      if(task_count_== 3)
      {
         cout << "all tasks are completed, waiting ctrl-c to display the results..." << endl;
      }
   }

   int operator()()
   {
      // your application logic here!
      task_count_ = 0;

      //our tasks
      add_task(gaussian_blur<3>( boost::bind( &myapp::add_result, this, _1 ))); 
      add_task(gaussian_blur<6>( boost::bind( &myapp::add_result, this, _1 ))); 
      add_task(gaussian_blur<9>( boost::bind( &myapp::add_result, this, _1 ))); 
     
      context_.find<application::wait_for_termination_request>()->wait();

      return 0;
   }
   
   bool stop()
   {
      std::cout << "Result..." << std::endl;

      for(int i = 0; i < result_.size(); ++i)
      {
         cout << i << " : -----------------------" << std::endl;

         vector< vector<double> > & kernel2d = result_[i];

         for (int row = 0; row < kernel2d.size(); row++) 
         {
            for (int col = 0; col < kernel2d[row].size(); col++)
            {
               cout << setprecision(5) << fixed << kernel2d[row][col] << " ";
            }
            cout << endl;
         }
      }

      return 1;
   }

private:

   boost::mutex mutex_;  
   vector< vector< vector<double> > > result_;

   int task_count_;

   application::context& context_;
   
}; // myapp 

int main(int argc, char *argv[])
{
   BOOST_APPLICATION_FEATURE_SELECT

   application::context app_context;
   myapp app(app_context);
   
   application::handler<>::callback cb 
      = boost::bind(&myapp::stop, &app);

   app_context.insert<application::termination_handler>(
      make_shared<application::termination_handler_default_behaviour>(cb));
      
   return application::launch<application::common>(app, app_context);
}

