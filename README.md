Boost.Application
=================

This is a test that add "aspect" concept on Boost.Application interface.

The main purpose of this test is to see how it 
looks and how it behaves in a final use (interface use).
 
To get older version of Boost.Appliation refer to:
https://sourceforge.net/projects/boostapp/

contact: re.tf@acm.org

--------------------------------------------------------------------------------
Change Log
--------------------------------------------------------------------------------

* 17/10/2013 [dd/mm/yyyy]

TODO : Many updates, I have to update later.

* 15/10/2013 [dd/mm/yyyy]

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

