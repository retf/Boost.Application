// shared_library.hpp --------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 05-04-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_SHARED_LIBRARY_HPP
#define BOOST_APPLICATION_SHARED_LIBRARY_HPP

#include <boost/application/config.hpp>
#include <boost/application/shared_library_load_mode.hpp>
#if defined( BOOST_WINDOWS_API )
#include <boost/application/detail/windows/shared_library_impl.hpp>
#elif defined( BOOST_POSIX_API )
#include <boost/application/detail/posix/shared_library_impl.hpp>
#else
#error "Sorry, no boost application are available for this platform."
#endif

namespace boost { namespace application {

   /*!
    * \brief This class can be used to load a
    *        Dynamic link libraries (DLL's) or Shared Library, also know
    *        as dynamic shared objects (DSO's) and invoke their exported
    *        symbols.
    *
    * Provides a means to extend your application using plugins way.
    *
    */
   class shared_library  : public noncopyable
   {
   public:

      /*!
       * Creates empty shared_library obj, need call load to
       * load a shared object.
       *
       */
      shared_library()
      {
         impl_ = new shared_library_impl();
      }

      /*!
       * Creates a shared_library object loading a library
       * from library() initializer function.
       *
       * The library() can handle std::string, char, std::wstring,
       * wchar_t or filesystem path.
       *
       * \param sl An initializer free function, e.g.:
       *        shared_library sh(library("c:\\mylib.dll"));
       *
       * Throw a boost::system::system_error on a execption.
       *
       */
      shared_library(const library_path &sl)
      {
         boost::system::error_code ec;
         impl_ = new shared_library_impl(sl, ec);

         if(ec)
            BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
			   "shared_library() failed", ec);
      }

      /*!
       * Creates a shared_library object loading a library
       * from library() initializer function.
       *
       * The library() can handle std::string, char, std::wstring,
       * wchar_t or filesystem path.
       *
       * \param sl An initializer free function, e.g.:
       *        shared_library sh(library("c:\\mylib.dll"), ec);
       *
       * \param ec Variable (boost::system::error_code) that will be
       *        set to the result of the operation.
       *
       * Check ec for errors.
       *
       */
      shared_library(const library_path &sl, boost::system::error_code &ec)
      {
         impl_ = new shared_library_impl(sl, ec);
      }

      /*!
       * Creates a shared_library object loading a library
       * from library() initializer function.
       *
       * The library() can handle std::string, char, std::wstring,
       * wchar_t or filesystem path.
       *
       * \param sl An initializer free function, e.g.:
       *        shared_library sh(library("c:\\mylib.dll"), 0x00000001);
       *
       * \param mode An mode that will be used on load.
       *
       * Throw a boost::system::system_error on a execption.
       *
       */
      shared_library(const library_path &sl, shared_library_load_mode mode)
      {
         boost::system::error_code ec;
         impl_ = new shared_library_impl(sl, mode, ec);

         if(ec)
            BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
               "shared_library() failed", ec);
      }

      /*!
       * Creates a shared_library object loading a library
       * from library() initializer function.
       *
       * The library() can handle std::string, char, std::wstring,
       * wchar_t or filesystem path.
       *
       * \param sl An initializer free function, e.g.:
       *        shared_library sh(library("c:\\mylib.dll"), 0x00000001, ec);
       *
       * \param mode An mode that will be used on load.
       *
       * \param ec Variable (boost::system::error_code) that will be
       *        set to the result of the operation.
       *
       * Check ec for errors.
       *
       */
      shared_library(const library_path &sl, shared_library_load_mode mode,
                     boost::system::error_code &ec)
      {
         impl_ = new shared_library_impl(sl, mode, ec);
      }

      /*!
       * Destructor
       * Destroys the SharedLibrary.
       * The actual library is unloaded. The unload() is called!
       *
       */
      virtual ~shared_library()
      {
         delete impl_;
      }

      /*!
       * Check equality of shared_library
       * If the same shared library is loaded, means: same path,
       * and both is loaded!
       *
       */
      bool operator==(const shared_library& sb) const
      {
         return((impl_->path_ == sb.impl_->path_) &&
                (is_loaded() == sb.is_loaded()));
      }

      /*!
       * Loads a shared library from library() initializer function.
       *
       * The library() can handle std::string, char, std::wstring,
       * wchar_t or filesystem path.
       *
       * \param sl An initializer free function, e.g.:
       *        sh.load(library("c:\\mylib.dll"));
       *
       * Throw a boost::system::system_error on a execption.
       *
       * Note that if a library is aread loaded, load will
       * unload it and then load the new provided library.
       *
       */
      void load(const library_path &sl)
      {
         boost::system::error_code ec;
         impl_->load(sl, ec);

         if(ec)
            BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
               "load() failed", ec);
      }

      /*!
       * Loads a shared library from library() initializer function.
       *
       * The library() can handle std::string, char, std::wstring,
       * wchar_t or filesystem path.
       *
       * \param sl An initializer free function, e.g.:
       *        sh.load(library("c:\\mylib.dll"), ec);
       *
       * \param ec Variable (boost::system::error_code) that will be
       *        set to the result of the operation.
       *
       * Check ec for errors.
       *
       * Note that if a library is aread loaded, load will
       * unload it and then load the new provided library.
       *
       */
      void load(const library_path &sl, boost::system::error_code &ec)
      {
         impl_->load(sl, ec);
      }

      /*!
       * Loads a shared library from library() initializer function.
       *
       * The library() can handle std::string, char, std::wstring,
       * wchar_t or filesystem path.
       *
       * \param sl An initializer free function, e.g.:
       *        sl.load(library("c:\\mylib.dll"), 0);
       *
       * \param mode An mode that will be used on load.
       *
       * Throw a boost::system::system_error on a execption.
       *
       * Note that if a library is aread loaded, load will
       * unload it and then load the new provided library.
       *
       */
      void load(const library_path &sl, shared_library_load_mode mode)
      {
         boost::system::error_code ec;
         impl_->load(sl, mode, ec);

         if(ec)
            BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
               "load() failed", ec);
      }

      /*!
       * Loads a shared library from library() initializer function.
       *
       * The library() can handle std::string, char, std::wstring,
       * wchar_t or filesystem path.
       *
       * \param sl An initializer free function, e.g.:
       *        sl.load("c:\\mylib.dll", 0, ec);
       *
       * \param mode An mode that will be used on load.
       *
       * \param ec Variable (boost::system::error_code) that will be
       *        set to the result of the operation.
       *
       * Check ec for errors.
       *
       * Note that if a library is aread loaded, load will
       * unload it and then load the new provided library.
       *
       */
      void load(const library_path &sl, shared_library_load_mode mode,
                boost::system::error_code &ec)
      {
         impl_->load(sl, mode, ec);
      }

      /*!
       * Unloads a shared library.
       *
       */
      void unload()
      {
         impl_->unload();
      }

      /*!
       * Check if an library is loaded.
       *
       * \return true if a library has been loaded
       *
       */
      bool is_loaded() const
      {
         return impl_->is_loaded();
      }

      /*!
       * Seach for d givem symbol on loaded library.
       *
       * The symbol() can handle std::string, char,
       * std::wstring or wchar_t.
       *
       * \param symbol_type An initializer free function, e.g.:
       *        sl.search_symbol(symbol("do_anything"))
       *
       * \return true if the loaded library contains
       *         a symbol from a given name.
       *
       */
      template <typename T>
      bool search_symbol(const symbol_type<T> &sb) const
      {
         return impl_->search_symbol(sb);
      }

      /*!
       * Returns the address of the symbol with the
       * given name to posterior call.
       *
       * Name must be provided using symbol() initializer
       * function that can handle std::string, char,
       * std::wstring or wchar_t.
       *
       * \param symbol_type An initializer free function, e.g.:
       *        sl.get_symbol(symbol("do_anything"))
       *
       * \return the address of symbol.
       *
       * Throw a boost::system::system_error on a execption, or
       * if symbol do not exist.
       *
       */
      template <typename T>
      void* get_symbol(const symbol_type<T> &sb)
      {
         boost::system::error_code ec;
         void* symbol_addr = impl_->get_symbol(sb, ec);

         if(ec)
            BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
               "get_symbol() failed", ec);

         return symbol_addr;
      }

      /*!
       * Returns the address of the symbol with the
       * given name to posterior call.
       *
       * Name must be provided using symbol() initializer
       * function that can handle std::string, char,
       * std::wstring or wchar_t.
       *
       * \param symbol_type An initializer free function, e.g.:
       *        sl.get_symbol(symbol("do_anything"), ec)
       *
       * \param ec Variable (boost::system::error_code) that will be
       *        set to the result of the operation.
       *
       * \return the address of symbol.
       *
       * Check ec for errors.
       *
       */
      template <typename T>
      void* get_symbol(const symbol_type<T> &sb, boost::system::error_code &ec)
      {
         return impl_->get_symbol(sb, ec);
      }

      /*!
       * Returns the address of the symbol with the
       * given name to posterior call.
       *
       * Name must be provided using symbol() initializer
       * function that can handle std::string, char,
       * std::wstring or wchar_t.
       *
       * \param symbol_type An initializer free function, e.g.:
       *        sl(symbol("do_anything"));
       *
       * \return the address of symbol.
       *
       * Throw a boost::system::system_error on a execption, or
       * if symbol do not exist.
       *
       */
      template <typename T>
      void* operator()(const symbol_type<T> &sb)
      {
         boost::system::error_code ec;
         void* symbol_addr = impl_->get_symbol(sb, ec);

         if(ec)
            BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
               "operator() failed", ec);

         return symbol_addr;
      }

      /*!
       * Returns the address of the symbol with the
       * given name to posterior call.
       *
       * Name must be provided using symbol() initializer
       * function that can handle std::string, char,
       * std::wstring or wchar_t.
       *
       * \param symbol_type An initializer free function, e.g.:
       *        sl(symbol("do_anything"), ec);
       *
       * \param ec Variable (boost::system::error_code) that will be
       *        set to the result of the operation.
       *
       * \return the address of symbol.
       *
       * Check ec for errors.
       *
       */
      template <typename T>
      void* operator()(const symbol_type<T> &sb, boost::system::error_code &ec)
      {
         return impl_->get_symbol(sb, ec);
      }

      /*!
       * Returns the path of the library, as specified
       * in a call to load() or the constructor/load.
       *
       * \return the boost::filesystem::path path of module.
       *
       */
      boost::filesystem::path get_path() const
      {
         return impl_->get_path();
      }

      /*!
       * Returns suffix od shared module:
       * in a call to load() or the constructor/load.
       *
       * \return the suffix od shared module, like:
       *
       * .dll (windows)
       * .so (unix)
       * .dylib (mac)
       *
       */
      static character_types::string_type suffix()
      {
         return shared_library_impl::suffix();
      }

   private:

      shared_library_impl *impl_;

   };

}} // boost::application

#endif // BOOST_APPLICATION_SHARED_LIBRARY_HPP

