#Boost.Application (Aspect Based)

* This release will be identified as : 0.4
* To get 0.3 (older version) refer to: [0.3] (https://sourceforge.net/projects/boostapp/)

In line with new Boost Git Structure

contact: re.tf@acm.org

###Acknowledgements

   A special thanks to Vicente J. Botet Escriba that helped a lot on all areas of this lib.
   
   Thanks to Benjamin Dieckmann for global_context implementation.
     
###On-line Documentation (Work in Progress)

[Online Docs (updated)] (http://www.dokfile.com/appbeta4/docs/libs/application/doc/html/index.html) 
<br>[Wiki] (https://github.com/retf/Boost.Application/wiki)

###Articles

Note that here we are using version 4.8 (0.4.8). If you are using current version 4.9 (0.4.9) you need adapt all article samples.

[Creating a New Application Mode] (http://www.codeproject.com/Articles/695937/Creating-a-New-Application-Mode)

[Build a Server Application using Application Library] (http://www.codeproject.com/Articles/756866/Build-a-Server-Application-using-Application-Libra)

###Notes 

As proposed by Rodrigo Madera (Application RM), this new version (0.4.9) changes the creation of application, this:

    struct myapp {
        bool stop(boost::application::context& context);
        bool pause(boost::application::context& context);
        bool resume(boost::application::context& context); 
    };

Become this:

    struct myapp {
        myapp(boost::application::context& context);
   
        bool stop();
        bool pause();
        bool resume();
    };

###Installation Notes

If you are using boost lower than 1.56.0 you need install TypeIndex library manually:

* [TypeIndex] (https://github.com/boostorg/type_index)
  --> In line with the last Boost TypeIndex (typeindex::type_index)

Full Reference (http://www.boost.org/community/review_schedule.html)

### Use

If you intend to use 'Application' on your application, please send-me your name and project. I am looking for create a list of users of ‘Application’.

### Questions, suggestions or bugs, please send an email to: 

re.tf@acm.org (Renato Tegon Forti)

