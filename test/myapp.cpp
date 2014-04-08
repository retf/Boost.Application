#include "myapp.h"

myapp::myapp(void)
{
}

myapp::~myapp(void)
{
}

int myapp::operator()(void)
{
   shared_ptr<application::args> myargs 
      = this_application()->find<application::args>();

   if (myargs)
   {
      std::vector<std::string> arg_vector = myargs->arg_vector();
   }

   return 0;
}