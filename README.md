#Boost.Application (Aspect Based)

* This release will be identified as : 0.4
* To get 0.3 (older version) refer to: [0.3] (https://sourceforge.net/projects/boostapp/)

In line with new Boost Git Structure

contact: re.tf@acm.org

### Caution

This is not yet an official Boost C++ library. It wasn't reviewed and can't be downloaded from www.boost.org. This beta is available to boost community to know real interest and get comments for refinement.The intention is to submit library to formal review, if community think that it is interesting!

## Introduction

Boost.Application provides a application environment, or start point to any people that want a basic infrastructure to build an system application on Windows or Unix Variants (e.g. Linux, MacOS).

### Hello World (server)

```cpp

#define BOOST_ALL_DYN_LINK
#define BOOST_LIB_DIAGNOSTIC

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <fstream>

#include <boost/application.hpp>

using namespace boost;

// my application code

class myapp
{   
public:

   myapp(application::context& context)
      : context_(context)
   {
   }

   void worker()
   {
      // my application behaviour

      // dump args

      std::vector<std::string> arg_vector = 
         context_.find<application::args>()->arg_vector();

      my_log_file_ << "-----------------------------" << std::endl;
      my_log_file_ << "---------- Arg List ---------" << std::endl;
      my_log_file_ << "-----------------------------" << std::endl;

      // only print args on screen
      for(std::vector<std::string>::iterator it = arg_vector.begin(); 
         it != arg_vector.end(); ++it) {
         my_log_file_ << *it << std::endl;
      }

      my_log_file_ << "-----------------------------" << std::endl;

      // run logic

      boost::shared_ptr<application::status> st =          
         context_.find<application::status>();

      int count = 0;
      while(st->state() != application::status::stoped)
      {
         boost::this_thread::sleep(boost::posix_time::seconds(1));

         if(st->state() == application::status::paused)
            my_log_file_ << count++ << ", paused..." << std::endl;
         else
            my_log_file_ << count++ << ", running..." << std::endl;
      }
   }

   // param
   int operator()()
   {
      std::string logfile 
         = context_.find<application::path>()->executable_path().string() + "/log.txt";
      
      my_log_file_.open(logfile.c_str());
      my_log_file_ << "Start Log..." << std::endl;

      // launch a work thread
      boost::thread thread(&myapp::worker, this);
      
      context_.find<application::wait_for_termination_request>()->wait();

      // to run direct
      // worker(&context);

      return 0;
   }

   // windows/posix

   bool stop()
   {
      my_log_file_ << "Stoping my application..." << std::endl;
      my_log_file_.close();

      return true; // return true to stop, false to ignore
   }

   // windows specific (ignored on posix)

   bool pause()
   {
      my_log_file_ << "Pause my application..." << std::endl;
      return true; // return true to pause, false to ignore
   }

   bool resume()
   {
      my_log_file_ << "Resume my application..." << std::endl;
      return true; // return true to resume, false to ignore
   }

private:

   std::ofstream my_log_file_;
   application::context& context_;

};

int main(int argc, char *argv[])
{   
   application::context app_context;
   
   // auto_handler will automatically add termination, pause and resume (windows) handlers
   application::auto_handler<myapp> app(app_context);

   // my server aspects

   // to handle path
   app_context.insert<application::path>(
      boost::make_shared<application::path_default_behaviour>(argc, argv));

   // to handle args
   app_context.insert<application::args>(
      boost::make_shared<application::args>(argc, argv));  

   // my server instantiation

   boost::system::error_code ec;
   int result = application::launch<application::server>(app, app_context, ec);

   if(ec)
   {
      std::cout << "[E] " << ec.message() 
         << " <" << ec.value() << "> " << std::cout;
   }
   
   return result;
}

```
     
### On-line Documentation (Work in Progress)

[Online Docs (updated)] (http://www.dokfile.com/appbeta4/docs/libs/application/doc/html/index.html) 
<br>[Wiki] (https://github.com/retf/Boost.Application/wiki)

### Questions, suggestions or bugs, please use Boost ML, stackoverflow or this group: 

[Discussion Group] (https://groups.google.com/forum/embed/?place=forum/boost_application)

### Articles

Note that here we are using version 4.8 (0.4.8). If you are using current version 4.9 (0.4.9) you need adapt all article samples.

[Creating a New Application Mode] (http://www.codeproject.com/Articles/695937/Creating-a-New-Application-Mode)

[Build a Server Application using Application Library] (http://www.codeproject.com/Articles/756866/Build-a-Server-Application-using-Application-Libra)

###Installation Notes

If you are using boost lower than 1.56.0 you need install TypeIndex library manually:

* [TypeIndex] (https://github.com/boostorg/type_index)

###Review Schedule

Boost Review Schedule (http://www.boost.org/community/review_schedule.html)

### Use

If you intend to use 'Application' on your application, please send-me your name and project. I am looking for create a list of users of ‘Application’.
<br>re.tf@acm.org (Renato Tegon Forti)

### Acknowledgements

A special thanks to Vicente J. Botet Escriba that helped a lot on all areas of this lib.
   
Thanks to Benjamin Dieckmann for global_context implementation.
   
