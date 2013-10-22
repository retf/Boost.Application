
//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/role for documentation.
//
//////////////////////////////////////////////////////////////////////////////

/**
 * @file
 * @brief Defines the aspect map.
 *
 * Uses typeindex as internal key.
 *
 */


// Revision History
// 22-10-2013 dd-mm-yyyy - Renato Forti - Add replace aspect method
// 
// -----------------------------------------------------------------------------


#ifndef BOOST_ASPECT_MAP_HPP
#define BOOST_ASPECT_MAP_HPP

#include <boost/config.hpp>

//#include <boost/smart_ptr/shared_ptr.hpp>
//#include <boost/unordered/unordered_map.hpp>
//#include <boost/typeindex/typeindex.hpp>

#include <memory>
#include <unordered_map>
#include <typeindex>

namespace boost
{
  namespace entity
  {
    class aspect_map
    {
      typedef std::type_index key_type;
      typedef std::shared_ptr<void> value_type;
      typedef std::unordered_map<key_type, value_type> map_type;
      map_type aspects_;

    public:
      template <class T>
      void add_aspect(std::shared_ptr<T> asp)
      {
        key_type ti = typeid(T);
        if (!aspects_.insert(make_pair(ti, asp)).second) throw std::logic_error(
            std::string("Type ") + ti.name() + " already added as an aspect");
      }

      template <class T>
      bool has_aspect() const
      {
        return aspects_.cend() != aspects_.find(key_type(typeid(T)));
      }

      template <class T>
      std::shared_ptr<T> get_aspect()
      {
        map_type::const_iterator it = aspects_.find(key_type(typeid(T)));
        if (aspects_.cend() == it) return std::shared_ptr<T>();
        return std::static_pointer_cast<T>(it->second);
      }

      template <class T>
      std::shared_ptr<const T> get_aspect() const
      {
        map_type::const_iterator it = aspects_.find(key_type(typeid(T)));
        if (aspects_.cend() == it) return std::shared_ptr<T>();
        return std::static_pointer_cast<const T>(it->second);
      }

      template <class T>
      T& use_aspect()
      {
        map_type::const_iterator it = aspects_.find(key_type(typeid(T)));
        if (aspects_.cend() == it)
          throw std::logic_error(std::string("Type ") + key_type(typeid(T)).name() + " is not an aspect");

        return *std::static_pointer_cast<T>(it->second);

      }
      template <class T>
      T const& use_aspect() const
      {
        map_type::const_iterator it = aspects_.find(key_type(typeid(T)));
        if (aspects_.cend() == it)
          throw std::logic_error(std::string("Type ") + key_type(typeid(T)).name() + " is not an aspect");

        return *std::static_pointer_cast<T>(it->second);

      }

      template <class T>
      void replace_aspect(std::shared_ptr<T> asp)
      {
        key_type ti = typeid(T);
        if(aspects_.erase(ti))
          aspects_.insert(make_pair(ti, asp));
      }

    };
  }
}
#endif // header

