//
// Copyright (c) Antony Polukhin, 2012-2013.
//
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_TYPE_INDEX_TYPE_INFO_HPP
#define BOOST_TYPE_INDEX_TYPE_INFO_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/// \file type_info.hpp
/// \brief Contains implementation of boost::type_info class.
///
/// boost::type_info class can be used as a drop-in replacement for std::type_info, but unlike std::type_info
/// this class has a name_demangled() function for getting human-readable type names.
///
/// boost::type_info class is used in situations when RTTI is enabled.
/// When RTTI is disabled or BOOST_TYPE_INDEX_FORCE_NO_RTTI_COMPATIBILITY macro is defined boost::template_info
/// is usually used instead of it (some compilers allow calling typeid(T)
/// even if RTTI is disabled, those copilers will continue to use boost::type_info class).

#include <boost/config.hpp>


// MSVC is capable of calling typeid(T) even when RTTI is off
#if (!defined(BOOST_NO_RTTI) && !defined(BOOST_TYPE_INDEX_FORCE_NO_RTTI_COMPATIBILITY)) || defined(BOOST_MSVC)

#include <cstring>
#include <string>
#include <boost/config.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/is_reference.hpp>
#include <boost/type_traits/is_volatile.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/mpl/if.hpp>
#include <boost/detail/no_exceptions_support.hpp>
#include <boost/functional/hash_fwd.hpp>

#include <typeinfo>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

namespace boost {

/// @cond

// for this compiler at least, cross-shared-library type_info
// comparisons don't work, so we are using typeid(x).name() instead.
# if (defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 5))) \
    || defined(_AIX) \
    || (defined(__sgi) && defined(__host_mips)) \
    || (defined(__hpux) && defined(__HP_aCC)) \
    || (defined(linux) && defined(__INTEL_COMPILER) && defined(__ICC))
#  define BOOST_CLASSINFO_COMPARE_BY_NAMES
# endif

/// @endcond

namespace detail {
#ifdef BOOST_NO_STD_TYPEINFO
    typedef type_info stl_type_info;
#else
    typedef std::type_info stl_type_info;
#endif

    template <class T> class cvr_saver{};
}

/// boost::type_info is a class that can be used as a drop-in replacement for std::type_info.
///
/// boost::type_info class is used in situations when RTTI is enabled.
/// When RTTI is disabled or BOOST_TYPE_INDEX_FORCE_NO_RTTI_COMPATIBILITY macro is defined boost::template_info
/// is used instead of it.
///
/// Unlike std::type_info this class:
/// * has a name_demangled() function for getting human-readable type names
/// * name() function always noexcept and returns simple mangled name as character a character array
/// * contains workarounds for some compiler issues
///
/// boost::type_info is not copyable and is not default constructible. Use boost::type_id* functions
/// to get const references to instances of boost::type_info objects. 
class type_info: public detail::stl_type_info {
#ifndef BOOST_NO_DELETED_FUNCTIONS
    type_info() = delete;
    type_info(const type_info&) = delete;
#else
    type_info();
    type_info(const type_info&);
#endif
public:
    typedef detail::stl_type_info stl_type_info;

    /// Factory method for constructing boost::type_info instance for type T.
    /// Strips const, volatile and & modifiers from T.
    ///
    /// Works exactly like boost::type_id().
    template <class T>
    static const boost::type_info& construct() BOOST_NOEXCEPT {
        typedef BOOST_DEDUCED_TYPENAME boost::remove_reference<T>::type no_ref_t;
        typedef BOOST_DEDUCED_TYPENAME boost::remove_cv<no_ref_t>::type no_cvr_t;

        #  if (defined(__EDG_VERSION__) && __EDG_VERSION__ < 245) \
            || (defined(__sgi) && defined(_COMPILER_VERSION) && _COMPILER_VERSION <= 744)
                BOOST_STATIC_ASSERT_MSG( !boost::is_arithmetic<no_cvr_t>::type::value
                    , "Your EDG-based compiler seems to mistakenly distinguish 'int' from 'signed int', in typeid() expressions.");
        #endif

        return static_cast<const boost::type_info&>(typeid(no_cvr_t));
    }

    /// Factory method for constructing boost::type_info instance for type T.
    /// Does not strip const, volatile, & and && modifiers from T.
    /// If T has no const, volatile, & and && modifiers, then returns exactly 
    /// the same result as in case of calling `construct<T>()`.
    ///
    /// Works exactly like boost::type_id_with_cvr().
    template <class T>
    static const boost::type_info& construct_with_cvr() BOOST_NOEXCEPT {
        typedef typename boost::mpl::if_c<
            boost::is_reference<T>::value
                || boost::is_const<T>::value
                || boost::is_volatile<T>::value,
            detail::cvr_saver<T>,
            T
        >::type type;

        return static_cast<const boost::type_info&>(typeid(type));
    }

    /// Factory function, that works exactly like C++ typeid(rtti_val) call, but returns boost::type_info.
    /// This method available only with RTTI enabled.
    ///
    /// Same as boost::type_id_rtti_only().
    template <class T>
    static const type_info& construct_rtti_only(T& rtti_val) BOOST_NOEXCEPT {
#ifdef BOOST_NO_RTTI 
        BOOST_STATIC_ASSERT_MSG(sizeof(T) && false, 
            "boost::type_id_rtti_only(T&) and boost::type_info::construct_rtti_only(T&) require RTTI");
#endif
        return static_cast<const boost::type_info&>(typeid(rtti_val));
    }

    /// Factory function, that works exactly like C++ typeid(rtti_val) call, but returns boost::type_info.
    /// This method available only with RTTI enabled.
    ///
    /// Same as boost::type_id_rtti_only().
    template <class T>
    static const type_info& construct_rtti_only(T* rtti_val) {
#ifdef BOOST_NO_RTTI 
        BOOST_STATIC_ASSERT_MSG(sizeof(T) && false, 
            "boost::type_id_rtti_only(T*) and boost::type_info::construct_rtti_only(T*) require RTTI");
#endif
        return static_cast<const boost::type_info&>(typeid(rtti_val));
    }

    /// Returns mangled type name.
    const char* name() const BOOST_NOEXCEPT {
    #ifdef _MSC_VER
        return stl_type_info::raw_name();
    #else
        return stl_type_info::name();
    #endif
    }

    /// Returns user-friendly name
    std::string name_demangled() const {
        #if defined(_MSC_VER)
            std::string ret = stl_type_info::name();
        #else
            std::string ret;
            int status = 0;
            char* demang = abi::__cxa_demangle(name(), NULL, 0, &status);
            BOOST_ASSERT(!status);

            BOOST_TRY {
                ret = demang; // may throw out of memory exception
            } BOOST_CATCH (...) {
                free(demang);
                BOOST_RETHROW;
            } BOOST_CATCH_END

            free(demang);
        #endif
            std::string::size_type pos = ret.find("boost::detail::cvr_saver<");
            if (pos == std::string::npos) {
                return ret;
            }

            pos += sizeof("boost::detail::cvr_saver<") - 1;
            while (ret[pos] == ' ') {
                ++ pos;
            }
            std::string::size_type end = ret.rfind(">");
            BOOST_ASSERT(end != std::string::npos);
            while (ret[end - 1] == ' ') {
                -- end;
            }

            return ret.substr(pos, end - pos);
    }

    bool operator == (type_info const& rhs) const BOOST_NOEXCEPT {
        #ifdef BOOST_CLASSINFO_COMPARE_BY_NAMES
            return name() == rhs.name() || !std::strcmp(name(), rhs.name());
        #else
            return static_cast<const stl_type_info&>(*this) == static_cast<const stl_type_info&>(rhs);
        #endif
    }

    bool operator != (type_info const& rhs) const BOOST_NOEXCEPT {
        return !(*this == rhs);
    }

    bool operator == (stl_type_info const& rhs) const BOOST_NOEXCEPT {
        return *this == static_cast<const boost::type_info&>(rhs);
    }

    bool operator != (stl_type_info const& rhs) const BOOST_NOEXCEPT {
        return !(*this == rhs);
    }

    /// Returns true if the type precedes the type of rhs in the collation order.
    /// The collation order is just an internal order.
    /// Works exactly like operator <
    bool before(type_info const& rhs) const BOOST_NOEXCEPT {
        #ifdef BOOST_CLASSINFO_COMPARE_BY_NAMES
            return name() != rhs.name() && std::strcmp(name(), rhs.name()) < 0;
        #else
            return !!stl_type_info::before(rhs);
        #endif
    }

    /// Returns true if the type precedes the type of rhs in the collation order.
    /// The collation order is just an internal order.
    /// Works exactly like operator <
    bool before(stl_type_info const& rhs) const BOOST_NOEXCEPT {
        return before(static_cast<const boost::type_info&>(rhs));
    }

    /// Function for getting hash value
    std::size_t hash_code() const BOOST_NOEXCEPT {
#if _MSC_VER > 1600 || (__GNUC__ == 4 && __GNUC_MINOR__ > 5 && defined(__GXX_EXPERIMENTAL_CXX0X__))
        return stl_type_info::hash_code();
#else 
        return boost::hash_range(name(), name() + std::strlen(name()));
#endif 
    }
};

#ifdef BOOST_CLASSINFO_COMPARE_BY_NAMES
#undef BOOST_CLASSINFO_COMPARE_BY_NAMES
#endif

/// Function to get boost::type_info for a type T. 
/// Strips const, volatile and & modifiers from T.
template <class T>
inline const type_info& type_id() BOOST_NOEXCEPT {
    return type_info::construct<T>();
}

/// Function for constructing boost::type_info instance for type T.
/// Does not strip const, volatile, & and && modifiers from T.
/// If T has no const, volatile, & and && modifiers, then returns exactly 
/// the same result as in case of calling `type_id<T>()`.
template <class T>
inline const type_info& type_id_with_cvr() BOOST_NOEXCEPT {
    return type_info::construct_with_cvr<T>();
}

/// Function that works exactly like C++ typeid(rtti_val) call, but returns boost::type_info.
/// This method available only with RTTI enabled. Without RTTI support it won't compile, 
/// producing a compile-time error with message: "boost::type_id_rtti_only(T&) requires RTTI"
template <class T>
inline const type_info& type_id_rtti_only(T& rtti_val) BOOST_NOEXCEPT {
#ifdef BOOST_NO_RTTI
    BOOST_STATIC_ASSERT_MSG(sizeof(T) && false, "boost::type_id_rtti_only(T&) requires RTTI");
#endif
    return static_cast<const type_info&>(typeid(rtti_val));
}

/// Function that works exactly like C++ typeid(rtti_val) call, but returns boost::type_info.
/// This method available only with RTTI enabled. Without RTTI support it won't compile, 
/// producing a compile-time error with message: "boost::type_id_rtti_only(T*) requires RTTI"
template <class T>
inline const type_info& type_id_rtti_only(T* rtti_val) {
#ifdef BOOST_NO_RTTI
    BOOST_STATIC_ASSERT_MSG(sizeof(T) && false, "boost::type_id_rtti_only(T*) requires RTTI");
#endif
    return static_cast<const type_info&>(typeid(rtti_val));
}

/* *************** type_info free functions ******************* */

#ifndef BOOST_TYPE_INDEX_DOXYGEN_INVOKED

inline bool operator == (detail::stl_type_info const& lhs, type_info const& rhs) BOOST_NOEXCEPT {
    return rhs == static_cast<const boost::type_info&>(lhs);
}

inline bool operator != (detail::stl_type_info const& lhs, type_info const& rhs) BOOST_NOEXCEPT {
    return !(lhs == rhs);
}

#else // BOOST_TYPE_INDEX_DOXYGEN_INVOKED

inline bool operator == (std::type_info const& lhs, type_info const& rhs) BOOST_NOEXCEPT;
inline bool operator != (std::type_info const& lhs, type_info const& rhs) BOOST_NOEXCEPT;

#endif // BOOST_TYPE_INDEX_DOXYGEN_INVOKED

/// hash_value function overload for boost::type_info.
inline std::size_t hash_value(type_info const& v) BOOST_NOEXCEPT {
    return v.hash_code();
}

} // namespace boost

#else // !defined(BOOST_NO_RTTI) && !defined(BOOST_TYPE_INDEX_FORCE_NO_RTTI_COMPATIBILITY)
#   include <template_info.hpp>
#   include <boost/static_assert.hpp>

namespace boost {

typedef template_info type_info;

template <class T>
inline const type_info& type_id() BOOST_NOEXCEPT {
    return template_info::construct<T>();
}

template <class T>
inline const type_info& type_id_with_cvr() BOOST_NOEXCEPT {
    return template_info::construct_with_cvr<T>();
}

template <class T>
inline const type_info& type_id_rtti_only(T& rtti_val) BOOST_NOEXCEPT {
    BOOST_STATIC_ASSERT_MSG(sizeof(T) && false, "boost::type_id_rtti_only(T&) requires RTTI");
    return template_info::construct_with_cvr<void>();
}

template <class T>
inline const type_info& type_id_rtti_only(T* rtti_val) {
    BOOST_STATIC_ASSERT_MSG(sizeof(T) && false, "boost::type_id_rtti_only(T*) requires RTTI");
    return template_info::construct_with_cvr<void>();
}

} // namespace boost

#endif //  (!defined(BOOST_NO_RTTI) && !defined(BOOST_TYPE_INDEX_FORCE_NO_RTTI_COMPATIBILITY)) || defined (BOOST_MSVC)

#if defined(BOOST_TYPE_INDEX_DOXYGEN_INVOKED)
/// \def BOOST_TYPE_INDEX_FORCE_NO_RTTI_COMPATIBILITY
/// Define the BOOST_TYPE_INDEX_FORCE_NO_RTTI_COMPATIBILITY macro if you are mixing objects
/// compiled with different RTTI flags. This will force the usage of boost::template_index 
/// class instead of boost::type_index.
#define BOOST_TYPE_INDEX_FORCE_NO_RTTI_COMPATIBILITY
#endif // BOOST_TYPE_INDEX_DOXYGEN_INVOKED

#endif // BOOST_TYPE_INDEX_TYPE_INFO_HPP

