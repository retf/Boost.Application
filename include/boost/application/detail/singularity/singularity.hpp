
//               Copyright Ben Robinson 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//----------------------------------------------------------------------------
//! \file
//! \brief Singularity design pattern enforces a single instance of a class.
//!
//! Unlike Singleton, singularity does not force global access, nor does it
//! require that the class have a default constructor.  The lifetime of the
//! object is simply defined between ::create() and ::destroy().
//! An object created with singularity must be passed into objects which depend
//! on them, just like any other object.  Unless created with
//! ::create_global(), in which case the object is accessible with ::get().
//----------------------------------------------------------------------------
//  Event event;
//
//  Usage as a Factory:
//
//  Horizon & horizonF = singularity<Horizon, single_threaded>::create(1, &event, event);
//                       singularity<Horizon, single_threaded>::destroy();
//
//  Usage as a Base Class:
//
//  class Horizon : public singularity<Horizon, multi_threaded>
//  Horizon & horizonB = Horizon::create_global(1, &event, event);
//  Horizon & horizonC = Horizon::get_global();
//                       Horizon::destroy();
//----------------------------------------------------------------------------

#ifndef SINGULARITY_HPP
#define SINGULARITY_HPP

#include <exception>
#include <boost/config.hpp>

#if defined( BOOST_NO_CXX11_SMART_PTR )
#include <boost/scoped_ptr.hpp>
#endif

#if defined( BOOST_NO_CXX11_VARIADIC_TEMPLATES )
#include <boost/mpl/assert.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/arithmetic/div.hpp>
#include <boost/preprocessor/arithmetic/mod.hpp>
#include "detail/pow2.hpp"
#endif

#include "singularity_policies.hpp"

// The user can choose a different arbitrary upper limit to the
// maximum number of constructor arguments.
#ifndef BOOST_SINGULARITY_PERFECT_FORWARD_ARG_SIZE
#define BOOST_SINGULARITY_PERFECT_FORWARD_ARG_SIZE 3
#endif

#ifndef BOOST_SINGULARITY_NONCONST_REFERENCE_ARG_SIZE
#define BOOST_SINGULARITY_NONCONST_REFERENCE_ARG_SIZE 10
#endif

namespace boost {

struct singularity_already_created : virtual std::exception
{
    virtual char const *what() const throw()
    {
        return "boost::singularity_already_created";
    }
};

struct singularity_already_destroyed : virtual std::exception
{
    virtual char const *what() const throw()
    {
        return "boost::singularity_already_destroyed";
    }
};

struct singularity_not_created : virtual std::exception
{
    virtual char const *what() const throw()
    {
        return "boost::singularity_not_created";
    }
};

struct singularity_no_global_access : virtual std::exception
{
    virtual char const *what() const throw()
    {
        return "boost::singularity_no_global_access";
    }
};

namespace detail {

// This pointer only depends on type T, so regardless of the threading
// model, only one singularity of type T can be created.
template <class T> struct singularity_instance
{
    static bool get_enabled;
#if defined( BOOST_NO_CXX11_SMART_PTR )
    typedef ::boost::scoped_ptr<T> ptrtype;
    static ::boost::scoped_ptr<T> ptr;
#else
    static std::unique_ptr<T> ptr;
#endif
};

template <class T> bool singularity_instance<T>::get_enabled = false;
#if defined( BOOST_NO_CXX11_SMART_PTR )
template <class T> typename singularity_instance<T>::ptrtype singularity_instance<T>::ptr(0);
#else
template <class T> std::unique_ptr<T> singularity_instance<T>::ptr(nullptr);
#endif

} // detail namespace

// And now, presenting the singularity class itself.
template <class T, template <class X> class M = single_threaded>
class singularity
{
public:

#if defined( BOOST_NO_CXX11_VARIADIC_TEMPLATES )
// Generate the 2^(n+1)-1 which is O(2^n) create(...) function overloads
// where n is the maximum number of constructor arguments.
//
// na = Number of arguments
// n  = Argument Index
// fi = Function Index
#define SINGULARITY_CREATE_ARGUMENTS(z, n, fi) BOOST_PP_COMMA_IF(n) A##n BOOST_PP_IF(BOOST_PP_MOD(BOOST_PP_IF(n,BOOST_PP_DIV(fi,BOOST_PP_POW2(n)),fi),2),const &,&) arg##n

#define SINGULARITY_CREATE_BODY(z, fi, na) \
    BOOST_PP_IF(na,template <,) BOOST_PP_ENUM_PARAMS(na, class A) BOOST_PP_IF(na,>,) \
    static inline T& create( BOOST_PP_REPEAT(na, SINGULARITY_CREATE_ARGUMENTS, fi) ) \
    { \
        M<T> guard; \
        (void)guard; \
        \
        verify_not_created(); \
        \
        detail::singularity_instance<T>::get_enabled = false; \
        detail::singularity_instance<T>::ptr.reset(new T(BOOST_PP_ENUM_PARAMS(na, arg))); \
        return *detail::singularity_instance<T>::ptr; \
    }

#define SINGULARITY_CREATE_ENABLE_GET_BODY(z, fi, na) \
    BOOST_PP_IF(na,template <,) BOOST_PP_ENUM_PARAMS(na, class A) BOOST_PP_IF(na,>,) \
    static inline T& create_global( BOOST_PP_REPEAT(na, SINGULARITY_CREATE_ARGUMENTS, fi) ) \
    { \
        M<T> guard; \
        (void)guard; \
        \
        verify_not_created(); \
        \
        detail::singularity_instance<T>::get_enabled = true; \
        detail::singularity_instance<T>::ptr.reset(new T(BOOST_PP_ENUM_PARAMS(na, arg))); \
        return *detail::singularity_instance<T>::ptr; \
    }

#define SINGULARITY_CREATE_OVERLOADS(z, na, text) BOOST_PP_REPEAT(BOOST_PP_POW2(na), SINGULARITY_CREATE_BODY, na)
#define SINGULARITY_CREATE_ENABLE_GET_OVERLOADS(z, na, text) BOOST_PP_REPEAT(BOOST_PP_POW2(na), SINGULARITY_CREATE_ENABLE_GET_BODY, na)

BOOST_PP_REPEAT(BOOST_PP_INC(BOOST_SINGULARITY_PERFECT_FORWARD_ARG_SIZE), \
                SINGULARITY_CREATE_OVERLOADS, _)

BOOST_PP_REPEAT(BOOST_PP_INC(BOOST_SINGULARITY_PERFECT_FORWARD_ARG_SIZE), \
                SINGULARITY_CREATE_ENABLE_GET_OVERLOADS, _)

#undef SINGULARITY_CREATE_OVERLOADS
#undef SINGULARITY_CREATE_ENABLE_GET_OVERLOADS
#undef SINGULARITY_CREATE_BODY
#undef SINGULARITY_CREATE_ENABLE_GET_BODY
#undef SINGULARITY_CREATE_ARGUMENTS

#else

    template <class ...A>
    static inline T& create(A && ...args)
    {
        M<T> guard;
        (void)guard;

        verify_not_created();

        detail::singularity_instance<T>::get_enabled = false;
        detail::singularity_instance<T>::ptr.reset(new T(std::forward<A>(args)...));
        return *detail::singularity_instance<T>::ptr;
    }

    template <class ...A>
    static inline T& create_global(A && ...args)
    {
        M<T> guard;
        (void)guard;

        verify_not_created();

        detail::singularity_instance<T>::get_enabled = true;
        detail::singularity_instance<T>::ptr.reset(new T(std::forward<A>(args)...));
        return *detail::singularity_instance<T>::ptr;
    }


#endif

// Generates: Family of create(...) functions
#define SINGULARITY_CREATE_ARGUMENTS(z, n, text) BOOST_PP_COMMA_IF(n) A##n & arg##n

#define SINGULARITY_CREATE_BODY(z, n, text) \
    BOOST_PP_IF(n,template <,) BOOST_PP_ENUM_PARAMS(n, class A) BOOST_PP_IF(n,>,) \
    static inline T& create( BOOST_PP_REPEAT(n, SINGULARITY_CREATE_ARGUMENTS, _) ) \
    { \
        M<T> guard; \
        (void)guard; \
        \
        verify_not_created(); \
        \
        detail::singularity_instance<T>::get_enabled = false; \
        detail::singularity_instance<T>::ptr.reset(new T(BOOST_PP_ENUM_PARAMS(n, arg))); \
        return *detail::singularity_instance<T>::ptr; \
    }

#define SINGULARITY_CREATE_ENABLE_GET_BODY(z, n, text) \
    BOOST_PP_IF(n,template <,) BOOST_PP_ENUM_PARAMS(n, class A) BOOST_PP_IF(n,>,) \
    static inline T& create_global( BOOST_PP_REPEAT(n, SINGULARITY_CREATE_ARGUMENTS, _) ) \
    { \
        M<T> guard; \
        (void)guard; \
        \
        verify_not_created(); \
        \
        detail::singularity_instance<T>::get_enabled = true; \
        detail::singularity_instance<T>::ptr.reset(new T(BOOST_PP_ENUM_PARAMS(n, arg))); \
        return *detail::singularity_instance<T>::ptr; \
    }

    BOOST_PP_REPEAT_FROM_TO(BOOST_PP_INC(BOOST_SINGULARITY_PERFECT_FORWARD_ARG_SIZE), \
                            BOOST_SINGULARITY_NONCONST_REFERENCE_ARG_SIZE, \
                            SINGULARITY_CREATE_BODY, _)

    BOOST_PP_REPEAT_FROM_TO(BOOST_PP_INC(BOOST_SINGULARITY_PERFECT_FORWARD_ARG_SIZE), \
                            BOOST_SINGULARITY_NONCONST_REFERENCE_ARG_SIZE, \
                            SINGULARITY_CREATE_ENABLE_GET_BODY, _)

#undef SINGULARITY_CREATE_ARGUMENTS
#undef SINGULARITY_CREATE_BODY
#undef SINGULARITY_CREATE_ENABLE_GET_BODY

    static inline void destroy()
    {
        M<T> guard;
        (void)guard;

        if (detail::singularity_instance<T>::ptr.get() == 0)
        {
            BOOST_THROW_EXCEPTION(singularity_already_destroyed());
        }

        detail::singularity_instance<T>::ptr.reset();
    }

    static inline T& get_global()
    {
        M<T> guard;
        (void)guard;

        if (detail::singularity_instance<T>::get_enabled == false) {
            BOOST_THROW_EXCEPTION(singularity_no_global_access());
        }

        if (detail::singularity_instance<T>::ptr.get() == 0)
        {
            BOOST_THROW_EXCEPTION(singularity_not_created());
        }

        return *detail::singularity_instance<T>::ptr;
    }
private:
    static inline void verify_not_created()
    {
        if (detail::singularity_instance<T>::ptr.get() != 0)
        {
            BOOST_THROW_EXCEPTION(singularity_already_created());
        }
    }
};

// Convenience macro which generates the required friend statement
// for use inside classes which use singularity as a factory.
#define FRIEND_CLASS_SINGULARITY \
    template <class T, template <class T> class M> friend class singularity

} // boost namespace

#endif // SINGULARITY_HPP
