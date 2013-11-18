
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
// 18-11-2013 dd-mm-yyyy - Renato Forti - Operations are thread safe now
//
// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_ENTITY_ASPECT_MAP_HPP
#define BOOST_APPLICATION_ENTITY_ASPECT_MAP_HPP

#include <boost/config.hpp>
#include <boost/application/config.hpp>
#include <boost/application/detail/csbl.hpp>
#include <boost/thread.hpp>

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

            mutable boost::mutex mutex_;

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
               boost::lock_guard<boost::mutex> lock(mutex_);
               add_aspect(asp, lock);
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
               boost::lock_guard<boost::mutex> lock(mutex_);

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
               boost::lock_guard<boost::mutex> lock(mutex_);

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
               boost::lock_guard<boost::mutex> lock(mutex_);

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
               boost::lock_guard<boost::mutex> lock(mutex_);

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
               boost::lock_guard<boost::mutex> lock(mutex_);

               if (!(aspects_.cend() != aspects_.find(key_type(typeid(T)))))
               {
                  throw std::logic_error(
                     std::string("Type ") + key_type(typeid(T)).name() +
                     " is not on the pool");
               }

               key_type ti = csbl::get_type_id<T>();

               if(aspects_.erase(ti))
                  aspects_.insert(std::make_pair(ti, asp));
            }

            /*!
             * Remove a stored aspect.
             *
             */
            template <class T>
            void remove_aspect()
            {
               boost::lock_guard<boost::mutex> lock(mutex_);

               if (!(aspects_.cend() != aspects_.find(key_type(typeid(T)))))
               {
                  throw std::logic_error(
                     std::string("Type ") + key_type(typeid(T)).name() +
                     " is not on the pool");
               }

               key_type ti = csbl::get_type_id<T>();

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
               boost::lock_guard<boost::mutex> lock(mutex_);

               if (!(aspects_.cend() != aspects_.find(key_type(typeid(T))))) {
                  add_aspect(asp, lock);
                  return true;
               }

               return false;
            }

         protected:

            template <class T>
            void add_aspect(csbl::shared_ptr<T> asp, boost::lock_guard<boost::mutex> &lock)
            {
               key_type ti = csbl::get_type_id<T>();
               if (!aspects_.insert(std::make_pair(ti, asp)).second)
                  throw std::logic_error(
                     std::string("Type ") + ti.name() + " already added as an aspect");
            }

   }; // aspect_map

} } } // boost::application::entity

#endif // BOOST_APPLICATION_ENTITY_ASPECT_MAP_HPP
