#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <iostream>
#include <boost/application.hpp>

using namespace boost;

inline application::global_context_ptr this_application() {
   return application::global_context::get();
}

class myapp
{
public:
   myapp(void);
   ~myapp(void);

   int operator()(void);
};

