#Boost.Application (0.4.12) [![Build Status](https://travis-ci.org/retf/Boost.Application.svg?branch=master)](https://travis-ci.org/retf/Boost.Application) [![Coverage Status](https://coveralls.io/repos/retf/Boost.Application/badge.svg?branch=master)](https://coveralls.io/r/retf/Boost.Application?branch=master)

### Caution

This is not yet an official Boost C++ library. It wasn't reviewed and can't be downloaded from www.boost.org. This beta is available to boost community to know real interest and get comments for refinement.The intention is to submit library to formal review, if community think that it is interesting!

### Warning

Note that this branch is a on active development, and can receive breaking changes without notifications. The stable version is: 4.8 (0.4.8)

### Help Boost.Application

Due to my lack of time, I need help to finish Boost.Application.

If you are interested in helping, please e-mail: re.tf@acm.org

We have the following open issues:

* Documentation (a lot of things);
* Fix openned BUGS;
* Make new testcases;
* Find and fix new BUGS;
* Build and run in other systems/compilers and send result to be added in docs;
* Provide new features;
* Write tutorial and how-to;
* Write examples;
* Give feedback of use;
* And any other thing that you want to propose.

Thanks a lot

### Introduction

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
      while(st->state() != application::status::stopped)
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

### Note about Plug-In system

Starting with version 0.4.10 the plug-in module has been moved to a new library, called Boost.DLL.

Refer to [Boost.DLL] (https://github.com/apolukhin/Boost.DLL)

Note that versions 0.4.8 and 0.4.9 still have the plugin system, but this version is not longer maintained! 
The new library [Boost.DLL] (https://github.com/apolukhin/Boost.DLL) was refactored, and have a lot of fixes and improvements. 
If you intend to use 'plugin system' and Boost.Application, we recommend that you use the last version with Boost.DLL. 
     
### On-line Documentation (Work in Progress), and other information

* [Online Docs (updated)] (http://retf.github.io/Boost.Application/app.docs/libs/application/doc/html/index.html) 
* [Wiki] (https://github.com/retf/Boost.Application/wiki)
* [Boost Library Incubator](http://rrsd.com/blincubator.com/bi_library/application/)

### Questions, suggestions or bugs, please use Boost ML, stackoverflow or this group: 

[Discussion Group] (https://groups.google.com/forum/embed/?place=forum/boost_application)

### Articles

Note that here we are using version 4.8 (0.4.8). If you are using current version 4.11 (0.4.11) you need adapt all article samples and download [Boost.DLL] (https://github.com/apolukhin/Boost.DLL)

Library use:<br>
[Build a Server Application using Application Library] (http://www.codeproject.com/Articles/756866/Build-a-Server-Application-using-Application-Libra)

Library customization:<br>
[Creating a New Application Mode] (http://www.codeproject.com/Articles/695937/Creating-a-New-Application-Mode)

### Installation Notes

If you are using boost lower than 1.56.0 you need install TypeIndex library manually:

* [TypeIndex] (https://github.com/boostorg/type_index)

### Tested compilers

* Linux (x86/64)
   * GCC 4.7/4.8/4.9

* Windows (x86/x86-64)
   * MinGW 4.7/4.8
   * Visual Studio 2013

### Review Schedule

Boost Review Schedule (http://www.boost.org/community/review_schedule.html)

### Who's Using 'Application'

* [Dokfile Software (Colaboration and Backup System)](http://www.dokfile.com/)

If you intend to use 'Application' on your application, please send-me your name and project. I am looking for create a list of users of ‘Application’.
<br>re.tf@acm.org (Renato Tegon Forti)

### Other 'Application' Frameworks:

 * [Pocoproject (WinService)](http://pocoproject.org/docs-1.5.3/Poco.Util.WinService.html)
 * [Libdaemon (Daemon)] (http://www.tyrfingr.is/projects/libdaemon/)
 * [Qt (qtservice)](http://doc.qt.digia.com/solutions/4/qtservice/qtservice.html)
 
### Changes on 0.4.12

 * The header config.hpp was refactored;
 * global_context can handle errors using boost::system::error_code;
 * global_context now throws boost::system::system_error, not more std::logic_error;
 
 #### Aspects

 * The 'path' aspect do not use path_default interface more.
 
### Changes on 0.4.11

#### Aspects

 * The 'path' aspect do not receive more any params (args) on constructor.
 * The 'path' aspect is added by default on 'common' and 'server' application modes.
 * New 'process_id' aspect, now is provided in context by default to obtaining a PID of proccess.

#### POSIX

 * server_application_impl now follows the BSD-style daemon(3) creation.
 
### Note about other version

* This release will be identified as : 0.4
* To get 0.3 (older version) refer to: [0.3] (https://sourceforge.net/projects/boostapp/)

Note that version 0.3 is no longer maintained! Version 0.4, is now maintained and receives regular updates. 

### Contributing

Want to contribute with Boost.Application project? Open a pull request!
   
### Acknowledgements

A special thanks to Vicente J. Botet Escriba that helped a lot on all areas of this lib.
   
Thanks to Benjamin Dieckmann for global_context implementation.<br>
Thanks to Rodrigo Madera (RM of Boost.Application).<br>
Thanks to Antony Polukhin for improvements on plug-in system module (shared_library), that now is a new library, see notes above. 

### Contact

[Renato Tegon Forti] (mailto:re.tf@acm.org)

### License

Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/LICENSE_1_0.txt).


