// Copyright 2011-2012 Renato Tegon Forti
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#include <iostream>

#if defined(_WIN32)
#   define LIBRARY_API __declspec(dllexport)
#else
#   define LIBRARY_API
#endif

extern "C" void LIBRARY_API say_hello(void);
extern "C" float LIBRARY_API lib_version(void);
extern "C" int LIBRARY_API increment(int);

void say_hello(void)
{
   std::cout << "Hello, Boost.Application!" << std::endl;
}

float lib_version(void)
{
   return 1.0;
}

int increment(int n)
{
   return ++n;
}


