// path.hpp ------------------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2014 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 14-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_ASPECT_PATH_HPP
#define BOOST_APPLICATION_ASPECT_PATH_HPP

#include <boost/application/config.hpp>
#include <boost/application/detail/csbl.hpp>
#if defined( BOOST_WINDOWS_API )
#include <boost/application/detail/windows/path_impl.hpp>
#elif defined( BOOST_POSIX_API )
#include <boost/application/detail/posix/path_impl.hpp>
#else
#error "Sorry, no boost application are available for this platform."
#endif

namespace boost { namespace application {

   /*!
    * \brief A contract class to be used by the user on your own
    *        class implementation of path aspect.
    *
    */
   class path : noncopyable
   {
   public:
      path() {}
      virtual ~path() {}

      /*!
       * Retrieves the application working path.
       *
       * \return an filesystem::path that hold the requested information.
       *
       * e.g.:
       *
       * [Windows]
       * E:\project.boost.app.v4\libs\application\vc11ide
       *
       * [POSIX]
       * /project.boost.app.v4/libs/application/vc11ide
       *
       */
      virtual filesystem::path current_path(void)
      {
         boost::lock_guard<boost::mutex> lock(mutex_);

         return filesystem::current_path();
      }

      /*!
       * Retrieves the application executable file name (excluding extension)
       *
       * \return an filesystem::path that hold the requested information.
       *
       * e.g.:
       *
       * [Windows]
       * path_exe
       *
       * [POSIX]
       * path_exe
       *
       */
      virtual filesystem::path executable_name(void)
      {
         boost::lock_guard<boost::mutex> lock(mutex_);

         filesystem::path module_path(module_path_name_);
         return module_path.stem();
      }

      /*!
       * Retrieves the application executable full file name (including extension)
       *
       * \return an filesystem::path that hold the requested information.
       *
       * e.g.:
       *
       * [Windows]
       * path_exe.exe
       *
       * [POSIX]
       * path_exe.out
       *
       */
      virtual filesystem::path executable_full_name(void)
      {
         boost::lock_guard<boost::mutex> lock(mutex_);

         filesystem::path module_path(module_path_name_);
         return module_path.filename();
      }

      /*!
       * Retrieves the absolute path of application executable
       *
       * \return an filesystem::path that hold the requested information.
       *
       * e.g.:
       *
       * [Windows]
       * E:\project.boost.app.v4\libs\application\vc11ide\Debug
       *
       * [POSIX]
       * /project.boost.app.v4/libs/application/vc11ide/Debug
       *
       */
      virtual filesystem::path executable_path(void)
      {
         boost::lock_guard<boost::mutex> lock(mutex_);

         filesystem::path module_path(module_path_name_);
         return module_path.parent_path();
      }

      /*!
       * Retrieves  the application executable full file name (including extension)
       *
       * \return an filesystem::path that hold the requested information.
       *
       * e.g.:
       *
       * [Windows]
       * E:\project.boost.app.v4\libs\application\vc11ide\Debug\path.exe
       *
       * [POSIX]
       * /project.boost.app.v4/libs/application/vc11ide/Debug/path.out
       *
       */
      virtual filesystem::path& executable_path_name(void)
      {
         boost::lock_guard<boost::mutex> lock(mutex_);

         return module_path_name_;
      }

      protected:
         filesystem::path module_path_name_;

      private:
        boost::mutex mutex_;

   };

   /*!
    * \brief This aspect class handle paths for of application.
    *        User can override this behavior and define your own.
    *
    */
   template <typename CharType>
   class path_default_behaviour_
      : public path
   {
   public:

      // string types to be used internaly to handle unicode on windows
      typedef CharType char_type;
      typedef std::basic_string<char_type> string_type;

      /*!
       * Constructs an path_default_behaviour_.
       *
       * \param argc An argc from main.
       * \param argv An argv from main.
       * \param ec Variable (boost::system::error_code) that will be
       *        set to the result of the operation.
       *
       */
      path_default_behaviour_(int argc, char_type *argv[], 
                              boost::system::error_code &ec)
         : impl_(new path_impl(argc, argv, module_path_name_, ec)) {}

      /*!
       * Constructs an path_default_behaviour_.
       *
       * \param argc An argc from main.
       * \param argv An argv from main.
       *
       */
      path_default_behaviour_(int argc, char_type *argv[])
         : impl_(new path_impl(argc, argv, module_path_name_, ec_))
      {
         if(ec_)
            BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
               "path_default_behaviour() failed", ec_);
      }

   private:

      csbl::shared_ptr<path_impl> impl_;

      boost::system::error_code ec_;

   };

   /////////////////////////////////////////////////////////////////////////////
   // path_default_behaviour
   //

   // args versions for Multi-Byte string and Unicode string
   typedef path_default_behaviour_<character_types::char_type>
      path_default_behaviour;
   // wchar_t / char

}} // boost::application

#endif // BOOST_APPLICATION_ASPECT_PATH_HPP


