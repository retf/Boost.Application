#Boost.Application

This is a test that add `aspects` concept on `Boost.Application` interface.

The main purpose of this test is to see how it looks and how it behaves in a final use (interface use).
 
* This release will be identified as : Beta 4
* To get Beta 3 (older version) refer to: [Beta 3] (https://sourceforge.net/projects/boostapp/)

contact: re.tf@acm.org

###Acknowledgements

   A special thanks to Vicente J. Botet Escriba that helped a lot on all areas of this lib.

##Usage

	1.	Introduction (basic roadmap);
	2.	Simple application;
	3.	Simple server application;
	4.	Setup for Windows Service
	5.	Ready-to-use Aspects;
	6.	Methods provided by lib to block and wait for end of application;
	7.	Limit Single Instance;
	8.	Customize a default handler of application;
	9.	Customize SIGNALS/handler of application;
	10.	Using plugin system;

--

###1. Introduction (basic roadmap) 

The library is much more flexible than version 3 (old-version), now the components are independents (loosely coupled).

The way to user build an application using “Boost.Application” is;

	a.	Define a “functor” class with an application operator of desired type (signature);
	
	   The “application operator” provided by “Boost.Application”  can have 2 signatures: 
	
	      * “singleton” version  (void) ;
	      * “param” based version (that receive a “context” of application as parameter).
	
	b.	Create a “context” for application (The context hold the “aspect” pool);
	
	c.	Add desired (ready-to-use aspects provided by lib) aspects to context;
	
	d.	Optionally, define a callback for “handler aspects” type, e.g.: stop, pause, resume
	
	e.	Optionally, define new “Apects”;
	
	f.	Optionally, customize/extend some ready to use “Apects” provided by library;
	
	g.	Optionally, customize SIGNAL of application;
	
	h.	“launch” application using some read-to-use mode, e.g.: “common” or “server” application;
	
	i.	Optionally, use plugin system to extend or change behavior of application at runtime.
	
	j.	Optionally, in case of “Windows Service”, use ready-to-use code to setup service;
	
	k.	Use “aspects” available on context (that user add on step [[c]] or [[e]]).


A basic sample of server application can be:

```c++
class myapp // [[a]]
{   
public:

   // param version
   int operator()(application::context& context) // [[a]]
   {
      // Do some thing

      // [[k]]
      context.use_aspect<application::wait_for_termination_request>().wait();

      return 0;
   }

   // [[d]]
   // param version
   bool stop(application::context& context)
   {
      // Do some thing
      return true; // return true to stop, false to ignore
   }
};

int main(int argc, char *argv[])
{   

   myapp app; // [[a]]
   application::context app_context; // [[b]]

   // my server aspects

   // [[c]]
   app_context.add_aspect<application::path>(
      make_shared<application::path_default_behaviour>(argc, argv));

   // [[c]]
   app_context.add_aspect<application::args>(
      make_shared<application::args>(argc, argv));

   // [[d]]
   application::handler::parameter_callback termination_callback 
      = boost::bind<bool>(&myapp::stop, &app, _1);

   // [[d]]
   app_context.add_aspect<application::termination_handler>(
      make_shared<application::termination_handler_default_behaviour>(termination_callback));

   // [[h]]
   return application::launch<application::server>(app, app_context);
}

```

[Full Code] (libs/application/example/simple_server_application.cpp)

###2. Simple application

This is our more common kind of application. This is a `terminal/console` interactive application, like a unix "grep".

[Full Code] (libs/application/example/simple_application.cpp)

###3. Simple server application;

The application of this type, will be a `Windows Service` on Windows, and a `Daemon` on POSIX. 

Note that all applications types can have your `SIGNAL` schema configured by user (if user desires "no default" actions). This is not true, only to Windows Service. Windows Service have your specific `signal/events` handler.

[Full Code] (libs/application/example/simple_server_application.cpp)

###4. Setup for Windows Service

The user must install the service (a series of complex steps - Windows API calls), Installation makes the SCM aware of the service and cause the SCM to add it to the list of services that appears in Services Console of Control Painel.

The library provides this functionality read-to-use. Check this sample that builds an application than can install/uninstall/check any service for the user:

[Full Code] (libs/application/example/service_setup.cpp)

###5. Ready-to-use Aspects

We have 2 types of `aspects`: a `common`, that can be anything, and a `handler` that always has a `callback` associated to it.

The library provides these aspects, on ready-to-use way:

Common:

*	args (application command line store)
*	path (application common paths store)
*	run_mode (application run mode identification, that can be `server` or `common`)
*	status (application status identification, that can be `stoped`, `running` or `paused`)
*	wait_for_termination_request

Handler:

*	limit_single_instance ( control to single instance mechanism)
*	pause_handler (pause handler callback)
*	resume_handler (resume handler callback)
*	termination_handler (termination handler callback)

###6. Methods provided by lib to block and wait for end of application

The library provides 2 methods to control execution of application:

The first is the use of `wait_for_termination_request` aspect. See `[[k]]` on sample.

The second is the use of “status” aspect. And check the state of this aspect, that reflect if application is running, paused, or stoped.

###7. Limit Single Instance

Some applications should only have a single instance running on a given operating system, this functionality provide this, in this way:

   1.	User need define a unique identity to application. To do this the user need generate a Boost.Uuid
        [Full Code] (libs/application/example/limit_single_instance.cpp)

   2.	Optionally, the user can define a callback that will handle if a new instance of application should continue or exit. If this callback this is not set the default behavior is terminate.
        [Full Code] (libs/application/example/limit_single_instance_callback.cpp)
	
###8. Customize a default handler of application

The user can customize the default behavior of any handler. To do this, the user needs inheriting of contract handler class (abstract) that he wants to modify the behavior.

For sample: The default behavior of `wait_for_termination_request` is unblock and exit of application. To change this behavior user can do:

```c++
// my made by hand behaviour
class wait_for_termination_request_my_behaviour 
   : public application::wait_for_termination_request
{
public:

   void wait(){
      char type;
      do
      {
         std::cout << "Do you want Exit? [y/n]" << std::endl;
         std::cin >> type;
      }
      while( !std::cin.fail() && type!='y' );
   }
};
```

Now, the basic behavior will be ignored and the new behavior will be executed.  The user will receive the message:  `Do you want Exit? [y/n]` 

[Full Code] (libs/application/example/my_own_wait_for_termination_request.cpp)

###9. Customize SIGNALS/handler of application

This feature allows user `configure/add handlers` for others `SIGNALS`.

In this ample we will create a handler to `SIGUSR2` POSIX signal. Note that this signal is not available on Windows.

To do this user need inheriting of `signal_manager` class and add new signal bind, like:

```c++
class my_signal_manager : public signal_manager
{
public:
   my_signal_manager(context &context)
      : signal_manager(context)
   {
      handler::parameter_callback callback 
         = boost::bind<bool>(&my_signal_manager::stop, this, _1);

      bind(SIGUSR2, callback); 
   }

   bool stop(context &context)
   {
      std::cout << "exiting..." << std::endl;

      shared_ptr<wait_for_termination_request> th 
         = context.get_aspect<wait_for_termination_request>();

      th->proceed();

      return false;
   }
};
```

Now, the signal `SIGUSR2` will stop application too.

And on main the user need pass the `new signal manager` to launch, like:

```c++
int main(int argc, char *argv[])
{   
   myapp app;
   context app_context;

   // we will customize our signals behaviour
   my_signal_manager sm(app_context);

   return launch<common>(app, sm, app_context);
}
```

[Full Code] (libs/application/example/my_own_termination_handler.cpp)

###10. Using plugin system

The last feature is plugin system. 

The basic way to use this feature is define a plugin (and plugin API), and load it on process, and then call desired function, like:

Plugin: 

libs/application/example/plugin_api.hpp
libs/application/example/plugin_library.cpp

And to load and call plugin:

libs/application/example/shared_library_load_plugin.cpp

--

Change Log
--------------------------------------------------------------------------------

* 27/10/2013 [dd/mm/yyyy] / Commint : Fully prototype.1.27102013 
--------------------------------------------------------------------------------

The prototype now is fully implemented (all application flavors, and on both sides).

* 24/10/2013 [dd/mm/yyyy] / Commint : Updates 24102013 
--------------------------------------------------------------------------------

In this commit we have some fix, and reorganization of source.

* 22/10/2013 [dd/mm/yyyy] / Commint : Updates 22102013 
--------------------------------------------------------------------------------

In this commit we have first prototype version of an fully implemented
server application (Windows Only), with all possible handlers
(stop/termination_handler, pause, resume, wait for termination request)

This has same functionality of old version (beta 3), but now using
aspects.

The main changes are on:

* app.hpp
* sever_application.hpp

example: 

* simple_server_application.cpp

Now we also have a way to customize the signals/handlers on common app,
refer to example:

* my_own_termination_handler.cpp

Many other modifications and fix were made.


* 18/10/2013 [dd/mm/yyyy] / Commint : Updates 17102013 
--------------------------------------------------------------------------------


In this commit we have first prototype version of an fully implemented of common 
application (Windows Only), with all possible handlers 
(stop/termination_handler, single instance, wait for termination request)

This has same functionality of old version, but now using aspects.

The changes are on:

* common_application.hpp
* handler.hpp
* termination_handler.hpp

The sample program is:

* termination_handler.cpp


* 17/10/2013 [dd/mm/yyyy] / Commint : Updates 17102013 
--------------------------------------------------------------------------------

New Aspects:
-----------

* path

* limit_single_instance_named_mutex_behaviour
  -> Based on Boost.Interprocess

  -> All aspects now live on "application/aspects" folder
  
Updated Files:
-------------

* All public interfaces now use doxigen comments.
* Some aspects now has an abstract class, that allow user extend these aspects to anyone with different behaviour.
* Many minor updates and fix in several files.

New Examples:
------------

 * limit_single_instance.cpp
 * limit_single_instance_boost_interprocess_named_mutex.cpp
 * my_own_wait_for_termination_request.cpp
 * path.cpp
 * simple_application.cpp
 * simple_application_using_singularity.cpp
 * wait_for_termination_request.cpp
 

* 15/10/2013 [dd/mm/yyyy] / Commint : New commit (Single instance and more)
--------------------------------------------------------------------------------

New Aspects:
-----------

* wait_for_termination_request_aspect

* limit_single_instance_aspect
  -> This aspect implement some tests with callback

Updated Aspects:
---------------

* args_aspec
  -> minor fix

Updated Files:
-------------

* common_application
  -> handle new aspects (wait_for_termination_request_aspect, limit_single_instance_aspect)

* server_application
  -> handle new aspects (wait_for_termination_request_aspect, limit_single_instance_aspect)

* launch
  -> Now provide 2 version of free function, one that throws an exception of 
     type boost::system::system_error, and another that set ec variable.

* context
  -> now implement 'add_aspect_if_not_exists' method

New modules:
-----------

* exceptions
* shared_library

New Examples:
------------

* limit_single_instance_callback_using_singularity
* limit_single_instance_callback

