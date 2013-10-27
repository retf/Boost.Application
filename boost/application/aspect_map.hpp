
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


// Changes
// 
// 22-10-2013 dd-mm-yyyy - Renato Forti - Add replace_aspect method
// 23-10-2013 dd-mm-yyyy - Renato Forti - Add remove_aspect method
// 25-10-2013 dd-mm-yyyy - Renato Forti - Enable use of csbl::
// 
// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_ENTITY_ASPECT_MAP_HPP
#define BOOST_APPLICATION_ENTITY_ASPECT_MAP_HPP

#include <boost/config.hpp>
#include <boost/application/config.hpp>
#include <boost/application/detail/csbl.hpp>

#include <utility>

namespace boost{
   namespace application {  
      namespace entity {

         class aspect_map
         {
            typedef csbl::type_index key_type;
            typedef csbl::shared_ptr<void> value_type;
            typedef csbl::unordered_map<key_type, value_type> map_type;

            map_type aspects_;

         public:

            template <class T>
            void add_aspect(csbl::shared_ptr<T> asp)
            {
#ifndef BOOST_NO_CXX11_HDR_TYPEINDEX
               key_type ti = typeid(T);
#else
               key_type ti = type_id<T>();
#endif

               if (!aspects_.insert(std::make_pair(ti, asp)).second) 
                  throw std::logic_error(
                     std::string("Type ") + ti.name() + " already added as an aspect");
            }

            template <class T>
            bool has_aspect() const
            {
               return aspects_.cend() != aspects_.find(key_type(typeid(T)));
            }

            template <class T>
            csbl::shared_ptr<T> get_aspect()
            {
               map_type::const_iterator it = aspects_.find(key_type(typeid(T)));

               if (aspects_.cend() == it) 
                  return csbl::shared_ptr<T>();

               return csbl::static_pointer_cast<T>(it->second);
            }

            template <class T>
            csbl::shared_ptr<const T> get_aspect() const
            {
               map_type::const_iterator it = aspects_.find(key_type(typeid(T)));

               if (aspects_.cend() == it) 
                  return csbl::shared_ptr<T>();

               return csbl::static_pointer_cast<const T>(it->second);
            }

            template <class T>
            T& use_aspect()
            {
               map_type::const_iterator it = aspects_.find(key_type(typeid(T)));

               if (aspects_.cend() == it)
                  throw std::logic_error(
                     std::string("Type ") + key_type(typeid(T)).name() + " is not an aspect");

               return *csbl::static_pointer_cast<T>(it->second);
            }

            template <class T>
            T const& use_aspect() const
            {
               map_type::const_iterator it = aspects_.find(key_type(typeid(T)));

               if (aspects_.cend() == it)
                  throw std::logic_error(
                     std::string("Type ") + key_type(typeid(T)).name() + " is not an aspect");

               return *csbl::static_pointer_cast<T>(it->second);
            }

            template <class T>
            void replace_aspect(csbl::shared_ptr<T> asp)
            {
#ifndef BOOST_NO_CXX11_HDR_TYPEINDEX
               key_type ti = typeid(T);
#else
               key_type ti = type_id<T>();
#endif

               if(aspects_.erase(ti))
                  aspects_.insert(std::make_pair(ti, asp));
            }

            template <class T>
            void remove_aspect(csbl::shared_ptr<T> asp)
            {
#ifndef BOOST_NO_CXX11_HDR_TYPEINDEX
               key_type ti = typeid(T);
#else
               key_type ti = type_id<T>();
#endif

               aspects_.erase(ti);
            }

   }; // aspect_map

} } } // boost::application::entity

#endif // BOOST_APPLICATION_ENTITY_ASPECT_MAP_HPP
