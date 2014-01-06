//
// Copyright (c) Antony Polukhin, 2012-2013.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_TYPE_INDEX_TYPE_INDEX_HPP
#define BOOST_TYPE_INDEX_TYPE_INDEX_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/// \file boost/type_index/type_index.hpp
/// \brief Contains implementation of boost::type_index class.
///
/// boost::type_index class is used in situations when RTTI is enabled, it is designed to be a drop-in
/// replacement for C++11 std::type_index class.
///
/// When RTTI is disabled boost::template_index will be usually (some compilers allow calling typeid(T)
/// even if RTTI is disabled) used instead of this class.  

#include <boost/config.hpp>
#include <boost/type_index/type_info.hpp>

// MSVC is capable of calling typeid(T) even when RTTI is off
#if (!defined(BOOST_NO_RTTI) && !defined(BOOST_TYPE_INDEX_FORCE_NO_RTTI_COMPATIBILITY)) || defined(BOOST_MSVC)

#if !defined(BOOST_NO_IOSTREAM)
#if !defined(BOOST_NO_IOSFWD)
#include <iosfwd>               // for std::basic_ostream
#else
#include <ostream>
#endif
#endif

namespace boost {

/// This class is designed to be a drop-in replacement for C++11 std::type_index class.
///
/// Copyable std::type_index class that requires RTTI.
/// When RTTI is disabled boost::template_index will be used instead of
/// this class.
class type_index {
private:
    const type_info* pinfo_;

public:
    typedef detail::stl_type_info stl_type_info;

    /// Default constructor.
    type_index() BOOST_NOEXCEPT
        : pinfo_(static_cast<const type_info*>(&typeid(void)))
    {}

    /// Constructs type_index from an instance of boost::type_info.
    type_index(const type_info& inf) BOOST_NOEXCEPT
        : pinfo_(&inf)
    {}

    /// Constructs type_index from an instance of std::type_info.
    type_index(const stl_type_info& inf) BOOST_NOEXCEPT
        : pinfo_(static_cast<const type_info*>(&inf))
    {}

    /// Returns true if the type precedes the type of rhs in the collation order.
    /// The collation order is just an internal order.
    bool before(type_index const& rhs) const BOOST_NOEXCEPT {
        return pinfo_->before(*rhs.pinfo_);
    }

    /// Returns raw name
    const char* name() const BOOST_NOEXCEPT {
        return pinfo_->name();
    }

    /// Returns user-friendly name
    std::string name_demangled() const {
        return pinfo_->name_demangled();
    }

    bool operator == (type_index const& rhs) const BOOST_NOEXCEPT {
        return *pinfo_ == *rhs.pinfo_;
    }

    bool operator != (type_index const& rhs) const BOOST_NOEXCEPT {
        return !(*pinfo_ == *rhs.pinfo_);
    }

    bool operator < (type_index const& rhs) const BOOST_NOEXCEPT {
        return before(rhs);
    }

    bool operator > (type_index const& rhs) const BOOST_NOEXCEPT {
        return (rhs < *this);
    }

    bool operator <= (type_index const& rhs) const BOOST_NOEXCEPT {
        return !(*this > rhs);
    }

    bool operator >= (type_index const& rhs) const BOOST_NOEXCEPT {
        return !(*this < rhs);
    }


    bool operator == (stl_type_info const& rhs) const BOOST_NOEXCEPT {
        return *this == type_index(rhs);
    }

    bool operator != (stl_type_info const& rhs) const BOOST_NOEXCEPT {
        return *this != type_index(rhs);
    }

    bool operator < (stl_type_info const& rhs) const BOOST_NOEXCEPT {
        return *this < type_index(rhs);
    }

    bool operator > (stl_type_info const& rhs) const BOOST_NOEXCEPT {
        return *this > type_index(rhs);
    }

    bool operator <= (stl_type_info const& rhs) const BOOST_NOEXCEPT {
        return *this <= type_index(rhs);
    }

    bool operator >= (stl_type_info const& rhs) const BOOST_NOEXCEPT {
        return *this >= type_index(rhs);
    }


    /// Function for getting hash value
    std::size_t hash_code() const BOOST_NOEXCEPT {
        return pinfo_->hash_code();
    }
};

/* *************** type_index free functions ******************* */

#ifndef BOOST_TYPE_INDEX_DOXYGEN_INVOKED

inline bool operator == (detail::stl_type_info const& lhs, type_index const& rhs) BOOST_NOEXCEPT {
    return rhs == lhs; // Operation is commutative
}

inline bool operator != (detail::stl_type_info const& lhs, type_index const& rhs) BOOST_NOEXCEPT {
    return rhs != lhs; // Operation is commutative
}

inline bool operator < (detail::stl_type_info const& lhs, type_index const& rhs) BOOST_NOEXCEPT {
    return rhs > lhs;
}

inline bool operator > (detail::stl_type_info const& lhs, type_index const& rhs) BOOST_NOEXCEPT {
    return rhs < lhs;
}

inline bool operator <= (detail::stl_type_info const& lhs, type_index const& rhs) BOOST_NOEXCEPT {
    return rhs >= lhs;
}

inline bool operator >= (detail::stl_type_info const& lhs, type_index const& rhs) BOOST_NOEXCEPT {
    return rhs <= lhs;
}

#else // BOOST_TYPE_INDEX_DOXYGEN_INVOKED

inline bool operator == (std::type_info const& lhs, type_index const& rhs) BOOST_NOEXCEPT;
inline bool operator != (std::type_info const& lhs, type_index const& rhs) BOOST_NOEXCEPT;
inline bool operator < (std::type_info const& lhs, type_index const& rhs) BOOST_NOEXCEPT;
inline bool operator > (std::type_info const& lhs, type_index const& rhs) BOOST_NOEXCEPT;
inline bool operator <= (std::type_info const& lhs, type_index const& rhs) BOOST_NOEXCEPT;
inline bool operator >= (std::type_info const& lhs, type_index const& rhs) BOOST_NOEXCEPT;

#endif // BOOST_TYPE_INDEX_DOXYGEN_INVOKED

/* *************** type_index free functions ******************* */

#ifndef BOOST_NO_IOSTREAM
#ifdef BOOST_NO_TEMPLATED_IOSTREAMS

/// Ostream operator that will output demangled name.
inline std::ostream& operator<<(std::ostream& ostr, type_index const& ind) {
    ostr << ind.name_demangled();
    return ostr;
}

#else

/// Ostream operator that will output demangled name.
template <class CharT, class TriatT>
inline std::basic_ostream<CharT, TriatT>& operator<<(std::basic_ostream<CharT, TriatT>& ostr, type_index const& ind) {
    ostr << ind.name_demangled();
    return ostr;
}

#endif // BOOST_NO_TEMPLATED_IOSTREAMS
#endif // BOOST_NO_IOSTREAM

/// hash_value function overload for type_index.
inline std::size_t hash_value(type_index const& v) BOOST_NOEXCEPT {
    return v.hash_code();
}

} // namespace boost

#else // (!defined(BOOST_NO_RTTI) && !defined(BOOST_TYPE_INDEX_FORCE_NO_RTTI_COMPATIBILITY)) || defined(BOOST_MSVC)

#include <boost/type_index/template_index.hpp>
namespace boost {

typedef template_index type_index;

}

#endif // (!defined(BOOST_NO_RTTI) && !defined(BOOST_TYPE_INDEX_FORCE_NO_RTTI_COMPATIBILITY)) || defined(BOOST_MSVC)

#endif // BOOST_TYPE_INDEX_TYPE_INDEX_HPP

