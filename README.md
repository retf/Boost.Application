Boost.Application
=================

This is a test that add "aspect" concept on Boost.Application interface.

The main purpose of this test is to see how it 
looks and how it behaves in a final use (interface use).
 
To get older version (beta 3) of Boost.Appliation refer to:
https://sourceforge.net/projects/boostapp/

contact: re.tf@acm.org



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

