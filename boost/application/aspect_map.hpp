
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

            /*!
             * Add a new aspect to the aspect map, throws an exception of type
             * std::logic_error on error.
             *      
             * \param asp Shared Ptr of aspect to be added.
             *
             */
            template <class T>
            void add_aspect(csbl::shared_ptr<T> asp)
            {
// todo: move to func
#if defined( BOOST_APPLICATION_FEATURE_NS_SELECT_STD )
               key_type ti = typeid(T);
#elif defined( BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST )
               key_type ti = type_id<T>();
#else // auto detect
#   ifndef BOOST_NO_CXX11_HDR_TYPEINDEX 
               key_type ti = typeid(T);
#   else
               key_type ti = type_id<T>();
#   endif
#endif
               if (!aspects_.insert(std::make_pair(ti, asp)).second) 
                  throw std::logic_error(
                     std::string("Type ") + ti.name() + " already added as an aspect");
            }

            /*!
             * Check if aspect exist on aspect map.
             *      
             * \param Template type of aspect.
             *  
             * \return True if aspect exist.
             *
             */
            template <class T>
            bool has_aspect() const
            {
               return aspects_.cend() != aspects_.find(key_type(typeid(T)));
            }

            /*!
             * Get a stored aspect of the aspect map.
             *      
             * \param asp Shared Ptr of aspect to be added.
             *  
             * \return Shared Ptr of aspect or null Shared Ptr if aspect don't
             *         exist.
             *
             */
            template <class T>
            csbl::shared_ptr<T> get_aspect()
            {
               map_type::const_iterator it = aspects_.find(key_type(typeid(T)));

               if (aspects_.cend() == it) 
                  return csbl::shared_ptr<T>();

               return csbl::static_pointer_cast<T>(it->second);
            }

            /*!
             * Get a stored aspect of the aspect map (const version).
             *      
             * \param asp Shared Ptr of aspect to be added.
             *  
             * \return Shared Ptr of aspect or null Shared Ptr if aspect don't
             *         exist.
             *
             */
            template <class T>
            csbl::shared_ptr<const T> get_aspect() const
            {
               map_type::const_iterator it = aspects_.find(key_type(typeid(T)));

               if (aspects_.cend() == it) 
                  return csbl::shared_ptr<T>();

               return csbl::static_pointer_cast<const T>(it->second);
            }

            /*!
             * Use a stored aspect of the aspect map, throws an exception of type
             * std::logic_error on error.
             *       
             * \return The aspect reference.
             *
             */
            template <class T>
            T& use_aspect()
            {
               map_type::const_iterator it = aspects_.find(key_type(typeid(T)));

               if (aspects_.cend() == it)
                  throw std::logic_error(
                     std::string("Type ") + key_type(typeid(T)).name() + 
                     " is not an aspect");

               return *csbl::static_pointer_cast<T>(it->second);
            }

            /*!
             * Use a stored aspect of the aspect map, throws an exception of type
             * std::logic_error on error.
             *       
             * \return The aspect reference.
             *
             */
            template <class T>
            T const& use_aspect() const
            {
               map_type::const_iterator it = aspects_.find(key_type(typeid(T)));

               if (aspects_.cend() == it)
                  throw std::logic_error(
                     std::string("Type ") + key_type(typeid(T)).name() + 
                     " is not an aspect");

               return *csbl::static_pointer_cast<T>(it->second);
            }

            /*!
             * Replace a stored aspect to another one.
             *      
             * \param asp Shared Ptr of aspect to be added.
             *  
             */
            template <class T>
            void replace_aspect(csbl::shared_ptr<T> asp)
            {
// todo: move to func
#if defined( BOOST_APPLICATION_FEATURE_NS_SELECT_STD )
               key_type ti = typeid(T);
#elif defined( BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST )
               key_type ti = type_id<T>();
#else // auto detect
#   ifndef BOOST_NO_CXX11_HDR_TYPEINDEX 
               key_type ti = typeid(T);
#   else
               key_type ti = type_id<T>();
#   endif
#endif
               if(aspects_.erase(ti))
                  aspects_.insert(std::make_pair(ti, asp));
            }

            /*!
             * Remove a stored aspect.
             *  
             */
            template <class T>
            void remove_aspect(csbl::shared_ptr<T> asp)
            {
// todo: move to func
#if defined( BOOST_APPLICATION_FEATURE_NS_SELECT_STD )
               key_type ti = typeid(T);
#elif defined( BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST )
               key_type ti = type_id<T>();
#else // auto detect
#   ifndef BOOST_NO_CXX11_HDR_TYPEINDEX 
               key_type ti = typeid(T);
#   else
               key_type ti = type_id<T>();
#   endif
#endif

               aspects_.erase(ti);
            }

            /*!
             * Add a new aspect to the aspect pool if it don't already exists 
             * on aspect pool of application context. 
             *  
             * \return true if the aspect did not exist and was successfully added, 
             *         if the aspect already exists, returns false and
             *         nothing is done.
             *      
             */
            template <class T>
            bool add_aspect_if_not_exists(csbl::shared_ptr<T> asp) 
            {
               if (!has_aspect<T>()) {
                  add_aspect<T>(asp);
                  return true;
               }

               return false;
            }

   }; // aspect_map

} } } // boost::application::entity

#endif // BOOST_APPLICATION_ENTITY_ASPECT_MAP_HPP
