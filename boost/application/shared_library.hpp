// shared_library.hpp -------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2012 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 05-04-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_SHARED_LIBRARY_HPP
#define BOOST_APPLICATION_SHARED_LIBRARY_HPP

#include <boost/application/config.hpp>
#if defined( BOOST_WINDOWS_API )
#include <boost/application/impl/windows/shared_library_impl.hpp>
#elif defined( BOOST_POSIX_API )
#include <boost/application/impl/posix/shared_library_impl.hpp>
#else
#error "Sorry, no boost application are available for this platform."
#endif

namespace boost { namespace application {

   // This class can be used to load a 
   // Dynamic link libraries (DLL's) or Shared Library, also know 
   // as dynamic shared objects (DSO's) and invoke their exported symbols.
   // 
   // Provides a means to extend your application using plugins way.
   //
   // The simplest use is as:
   //
   // // library.cpp
   // // ...
   // extern "C" std::string LIBRARY_API lib_do_anything(int x);
   // // ...
   // std::string lib_do_anything(int x)
   // {
   //    return boost::lexical_cast<std::string>(x*x);
   // };
   // 
   // // app.cpp
   // // ...
   // typedef std::string (*my_plugin_function) (int);
   // // ...
   // int main()
   // {
   //    shared_library sl(library("/my/plugin.so"));
   //    std::cout << 
   //       ((my_plugin_function) sl(symbol("lib_do_anything")))(10) 
   //           << std::endl;
   // }
   //
   class shared_library  : public noncopyable 
   {
   public:

      // Creates enpty shared_library obj, need call load
      shared_library()
      {
         impl_ = new shared_library_impl();
      }
    
      // Creates a shared_library object loading a library
      // from library() initializer function.
      // The library() can handle std::string, char, std::wstring, 
      // wchar_t or filesystem path.
      // Eg.: shared_library sh(library("c:\\mylib.dll"));
      // Throw a boost::system::system_error on a execption.
      template <typename T>
      shared_library(const library_type<T> &sl)
      {
         impl_ = new shared_library_impl(sl);
      }

      // Creates a shared_library object loading a library
      // from library() initializer function.
      // The library() can handle std::string, char, std::wstring, 
      // wchar_t or filesystem path.
      // Eg.: shared_library sh(library("c:\\mylib.dll"), ec);
      // check ec for errors.
      template <typename T>
      shared_library(const library_type<T> &sl, boost::system::error_code &ec)
      {
         impl_ = new shared_library_impl(sl, ec);
      }

      // Creates a shared_library object loading a library
      // from library() initializer function.
      // The library() can handle std::string, char, std::wstring, 
      // wchar_t or filesystem path.
      // Accept load mode(nix)/flag(win), to know more about load mode, 
      // check on windows LoadLibraryEx, and unix dlopen docs.
      // Eg.: shared_library sh(library("c:\\mylib.dll"), 0x00000001);
      // Throw a boost::system::system_error on a execption.
      template <typename T>
      shared_library(const library_type<T> &sl, unsigned long mode)
      {
         impl_ = new shared_library_impl(sl, mode);
      }

      // Creates a shared_library object loading a library
      // from library() initializer function.
      // The library() can handle std::string, char, std::wstring, 
      // wchar_t or filesystem path.
      // Accept load mode(nix)/flag(win), to know more about load mode, 
      // check on windows LoadLibraryEx, and unix dlopen docs.
      // Eg.: shared_library sh(library("c:\\mylib.dll"), 0x00000001, ec);
      // check ec for errors.
      template <typename T>
      shared_library(const library_type<T> &sl, unsigned long mode, 
                     boost::system::error_code &ec)
      {
         impl_ = new shared_library_impl(sl, mode, ec);
      }

      // Destroys the SharedLibrary. The actual library
      // is unloaded. The unload() is called!
      virtual ~shared_library()
      {
         delete impl_;
      }

      // Check equality of shared_library
      // If the same shared library is loaded, means: same path, and both is loaded!
      bool operator==(const shared_library& sb) const
      {
         return((impl_->path_ == sb.impl_->path_) && (is_loaded() == sb.is_loaded()));
      }

      // Loads a shared library 
      // from library() initializer function.
      // The library() can handle std::string, char, std::wstring, 
      // wchar_t or filesystem path.
      // Eg.: sh.load(library("c:\\mylib.dll"));
      // Throw a boost::system::system_error on a execption.
      // Note that if a library is aread loaded, load will
      // unload it and then load the new provided library.
      template <typename T>
      void load(const library_type<T> &sl)
      {
         impl_->load(sl);
      }

      // Loads a shared library 
      // from library() initializer function.
      // The library() can handle std::string, char, std::wstring, 
      // wchar_t or filesystem path.
      // Eg.: sh.load(library("c:\\mylib.dll"), ec);
      // check ec for errors.
      // Note that if a library is aread loaded, load will
      // unload it and then load the new provided library.
      template <typename T>
      void load(const library_type<T> &sl, boost::system::error_code &ec)
      {
         impl_->load(sl, ec);
      }

      // Loads a shared library 
      // from library() initializer function.
      // The library() can handle std::string, char, std::wstring, 
      // wchar_t or filesystem path.
      // Accept load mode(nix)/flag(win), to know more about load mode, 
      // check on windows LoadLibraryEx, and unix dlopen docs.
      // Eg.: sl.load(library("c:\\mylib.dll"), mode);
      // Throw a boost::system::system_error on a execption.
      // Note that if a library is aread loaded, load will
      // unload it and then load the new provided library.
      template <typename T>
      void load(const library_type<T> &sl, unsigned long mode)
      {
         impl_->load(sl, mode);
      }

      // Loads a shared library 
      // from library() initializer function.
      // The library() can handle std::string, char, std::wstring, 
      // wchar_t or filesystem path.
      // Accept load mode(nix)/flag(win), to know more about load mode, 
      // check on windows LoadLibraryEx, and unix dlopen docs.
      // Eg.: sl.load(library("c:\\mylib.dll"), mode);
      // check ec for errors.
      // Note that if a library is aread loaded, load will
      // unload it and then load the new provided library.
      template <typename T>
      void load(const library_type<T> &sl, unsigned long mode, 
                boost::system::error_code &ec)
      {
         impl_->load(sl, mode, ec);
      }

      // Unloads a shared library.
      void unload()
      {
         impl_->unload();
      }
   
      // Returns true if a library has been loaded.
      bool is_loaded() const
      {
         return impl_->is_loaded();
      }

      // Returns true if the loaded library contains
      // a symbol from a given name.
      // Name must be provided using symbol() initializer function.
      // The symbol() can handle std::string, char, std::wstring or
      // wchar_t.
      // Eg.: sl.search_symbol(symbol("do_anything"))
      template <typename T>
      bool search_symbol(const symbol_type<T> &sb) const 
      {
         return impl_->search_symbol(sb);
      }

      // Returns the address of the symbol with
      // the given name. 	
      // Name must be provided using symbol() initializer function.
      // The symbol() can handle std::string, char, std::wstring or
      // wchar_t.
      // Eg.: sl.get_symbol(symbol("do_anything"))
      // Throw a boost::system::system_error on a execption.
      template <typename T>
      void* get_symbol(const symbol_type<T> &sb)
      {
         return impl_->get_symbol(sb);
      }

      // Returns the address of the symbol with
      // the given name. 	
      // Name must be provided using symbol() initializer function.
      // The symbol() can handle std::string, char, std::wstring or
      // wchar_t.
      // Eg.: sl.get_symbol(symbol("do_anything"), ec)
      // check ec for errors.
      template <typename T>
      void* get_symbol(const symbol_type<T> &sb, boost::system::error_code &ec)
      {
         return impl_->get_symbol(sb, ec);
      }

      // Returns the address of the symbol with
      // the given name. 	
      // Name must be provided using symbol() initializer function.
      // The symbol() can handle std::string, char, std::wstring or
      // wchar_t.
      // Eg.: sl(symbol("do_anything"))
      // Throw a boost::system::system_error on a execption.
      template <typename T>
      void* operator()(const symbol_type<T> &sb)
      {
         return (*impl_)(sb);
      }

      // Returns the address of the symbol with
      // the given name. 	
      // Name must be provided using symbol() initializer function.
      // The symbol() can handle std::string, char, std::wstring or
      // wchar_t.
      // Eg.: sl(symbol("do_anything"))
      // check ec for errors.
      template <typename T>
      void* operator()(const symbol_type<T> &sb, boost::system::error_code &ec)
      {
         return (*impl_)(sb, ec);
      }

      // Returns the path of the library, as
      // specified in a call to load() or the 
      // constructor.	
      const boost::filesystem::path& get_path() const
      {
         return impl_->get_path();
      }

      // Returns suffix od shared module: 
      // .dll (windows)
      // .so (unix)
      static character_types::string_type suffix()
      {
         return shared_library_impl::suffix();
      }

   private:

      shared_library_impl *impl_;

   };

}} // boost::application

#endif // BOOST_APPLICATION_SHARED_LIBRARY_HPP

