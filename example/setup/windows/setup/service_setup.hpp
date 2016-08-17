// service_setup.hpp --------------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 22-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_IMPL_WINDOWS_SERVICE_SETUP_HPP
#define BOOST_APPLICATION_IMPL_WINDOWS_SERVICE_SETUP_HPP

// application
#include <boost/application/config.hpp>

// boost
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <vector>

//
#include "../../initializers.hpp"

namespace boost { namespace application { namespace example {

   // util class to open/close the SCM on current machine
   class windows_scm : noncopyable
   {
   public:

      windows_scm(DWORD dw_desired_access)
      {
         boost::system::error_code ec;
         open(dw_desired_access, ec);

         if(ec)
         {
            BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
			   "OpenSCManager() failed", ec);
         }
      }

      windows_scm(DWORD dw_desired_access, boost::system::error_code &ec)
      {
         open(dw_desired_access, ec);
      }

      ~windows_scm()
      {
         CloseServiceHandle(handle_);
      }

      SC_HANDLE get() const
      {
         return handle_;
      }

   protected:

      void open(DWORD dw_desired_access, boost::system::error_code &ec)
      {
         // open the SCM on this machine.
         handle_ = OpenSCManager(NULL, NULL, dw_desired_access);

         if (handle_ == NULL)
         {
            ec = last_error_code();
         }
      }

   private:

      SC_HANDLE handle_;
   };

   //
   // windows service setup behaviours
   //
   // check_windows_service     : check if service (by name) is isntaled on local scm
   // uninstall_windows_service : uninstall service (by name, amd path ) on local scm
   // install_windows_service   : install service on local scm

   // check if a given Windows Service (by name) is installed
   template <typename value_type>
   class check_windows_service_ : noncopyable
   {
   public:

      typedef value_type char_type;

      template <typename T>
      check_windows_service_(const setup_type<T> &service_name)
      {
         service_name_ = service_name.get();
      }

      virtual ~check_windows_service_()
      {
      }

      virtual bool exist(boost::system::error_code &ec)
      {
         return is_installed(ec);
      }

      virtual bool exist()
      {
         boost::system::error_code ec;
         bool installed = is_installed(ec);

         if(ec)
         {
            BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
			   "is_installed() failed", ec);
         }

         return installed;
      }

   protected:

      // This method check if a given service (by name) is installed
      virtual bool is_installed(boost::system::error_code &ec)
      {
         windows_scm scm(SC_MANAGER_ENUMERATE_SERVICE, ec);

         if(ec)
         {
            return false;
         }

         ENUM_SERVICE_STATUS service;

         DWORD dwBytesNeeded = 0;
         DWORD dwServicesReturned = 0;
         DWORD dwResumedHandle = 0;

         // Query services
         BOOL ret_value = EnumServicesStatus(scm.get(), SERVICE_WIN32,
         SERVICE_STATE_ALL, &service, sizeof(ENUM_SERVICE_STATUS),
         &dwBytesNeeded, &dwServicesReturned, &dwResumedHandle);

         if (!ret_value)
         {
            // Need big buffer
            if (ERROR_MORE_DATA == GetLastError())
            {
               // Set the buffer
               DWORD dwBytes = sizeof(ENUM_SERVICE_STATUS) + dwBytesNeeded;
               ENUM_SERVICE_STATUS* pServices = NULL;
               pServices = new ENUM_SERVICE_STATUS [dwBytes];

               // Now query again for services
               EnumServicesStatus(scm.get(), SERVICE_WIN32, SERVICE_STATE_ALL,
               pServices, dwBytes, &dwBytesNeeded,
               &dwServicesReturned, &dwResumedHandle);

               // now traverse each service to get information
               for (unsigned iIndex = 0; iIndex < dwServicesReturned; iIndex++)
               {
                  std::basic_string<char_type> service;
                  service = (pServices + iIndex)->lpServiceName;

                  if(service == service_name_)
                  {
                     delete [] pServices;
                     pServices = NULL;
                     return true;
                  }
               }

               delete [] pServices;
               pServices = NULL;
            }
            else
            {
               ec = last_error_code();
            }
         }
         else
         {
            ec = last_error_code();
         }

         return false;
      }

      std::basic_string<char_type> service_name_;

   }; // check_windows_service

   // check_windows_service versions for common string and wide string
   typedef check_windows_service_<character_types::char_type> check_windows_service;
   // wchar_t / char

   // uninstall a given Windows Service (by name)
   template <typename value_type>
   class uninstall_windows_service_ : noncopyable
   {
   public:

      typedef value_type char_type;

      template <typename T>
      uninstall_windows_service_(const setup_type<T> &service_name, const setup_type<T> &service_path_name)
      {
         service_name_ = service_name.get();
         service_path_name_ = service_path_name.get();
      }

      virtual ~uninstall_windows_service_()
      {
      }

      virtual void uninstall(boost::system::error_code &ec)
      {
         uninstall_service(ec);
      }

      virtual void uninstall()
      {
         boost::system::error_code ec;
         uninstall_service(ec);

         if(ec)
         {
            BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
			   "uninstall() failed", ec);
         }
      }

   protected:

      // This method uninstall a given service (by name) on current machine
      void uninstall_service(boost::system::error_code &ec)
      {
         boost::filesystem::path path(service_path_name_);

         if(!boost::filesystem::exists(path))
         {
            ec = boost::system::error_code(2, boost::system::system_category());
            return;
         }

         // Open the SCM on this machine.
         windows_scm scm(SC_MANAGER_CONNECT , ec);

         if(ec)
         {
             return;
         }

         // Open this service for DELETE access
         SC_HANDLE hservice = OpenService(scm.get(), service_name_.c_str(), DELETE);

         if (hservice == NULL)
         {
            ec = last_error_code();
            return;
         }

         // Remove this service from the SCM's database.
         DeleteService(hservice);
         CloseServiceHandle(hservice);

         unregister_application(ec);
      }

      // Unregister the App Paths of application.
      void unregister_application(boost::system::error_code &ec)
      {
         LONG error;

#if defined(BOOST_APPLICATION_STD_WSTRING)
         std::basic_string<char_type> subKey =
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\" + service_path_name_;

#else
         std::basic_string<char_type> subKey =
            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\" + service_path_name_;
#endif

         error = RegDeleteKey(HKEY_LOCAL_MACHINE, subKey.c_str());

         if (error != NO_ERROR)
         {
            ec = last_error_code();
         }
      }

      std::basic_string<char_type> service_name_;
      std::basic_string<char_type> service_path_name_;

   }; // uninstall_windows_service

   // uninstall_windows_service versions for common string and wide string
   typedef uninstall_windows_service_<character_types::char_type> uninstall_windows_service;
   // wchar_t / char

   // install a given Windows Service (by name)
   template <typename value_type>
   class install_windows_service_ : noncopyable
   {
   public:

      typedef value_type char_type;
			
      template <typename T>
      install_windows_service_(
         const setup_type<T> &service_name,
         const setup_type<T> &service_display_name,
         const setup_type<T> &service_description,
         const setup_type<T> &service_path_name,		 
         const setup_type<T> &service_user = setup_type<T>(""),
         const setup_type<T> &service_password = setup_type<T>(""),
         const setup_type<T> &service_start_mode = setup_type<T>("auto"),
         const setup_type<T> &service_depends = setup_type<T>(""),
         const setup_type<T> &service_option_string = setup_type<T>(""))
      {
         service_name_ = service_name.get();
         service_display_name_ = service_display_name.get();
         service_description_ = service_description.get();
         service_path_name_ = service_path_name.get();
         service_user_ = service_user.get();
         service_password_ = service_password.get();
         service_option_string_ = service_option_string.get();
         service_start_mode_ = service_start_mode.get();
         service_depends_ = service_depends.get();
      }

      virtual ~install_windows_service_()
      {
      }

      virtual void install(boost::system::error_code &ec)
      {
         install_service(ec);
      }

      virtual void install()
      {
         boost::system::error_code ec;
         install_service(ec);

         if(ec)
         {
            BOOST_APPLICATION_THROW_LAST_SYSTEM_ERROR_USING_MY_EC(
			   "install() failed", ec);
         }
      }

   protected:

      // this method install a given service (by name) on current machine
      void install_service(boost::system::error_code &ec)
      {
         boost::filesystem::path path(service_path_name_);

         if(!boost::filesystem::exists(path))
         {
            ec = boost::system::error_code(2, boost::system::system_category());
            return;
         }

         windows_scm scm(SC_MANAGER_CREATE_SERVICE, ec);

         if(ec)
         {
             return;
         }

         std::basic_string<char_type> pathname = service_path_name_;
         // Append the switch that causes the process to run as a service.
         if(service_option_string_.size()) {
            pathname += char_type(' ') + service_option_string_;
            // e.g. c:\myservice\service.exe --s
         }

         DWORD create_service_start_mode = SERVICE_AUTO_START;
         if (service_start_mode_ == std::basic_string<char_type>("manaul"))
           create_service_start_mode = SERVICE_DEMAND_START;

         // e.g. the format of service depends str is "service1\0services2\0services3\0\0"
         std::vector<char_type> create_service_depends(service_depends_.length() + 2, 0);
         std::copy(service_depends_.begin(), service_depends_.end(), create_service_depends.begin());
         std::replace(create_service_depends.begin(), create_service_depends.end(), '\\', '\0');

         // Add this service to the SCM's database.
         SC_HANDLE hservice = CreateService(
            scm.get(),
            service_name_.c_str(),
            service_display_name_.c_str(),
            SERVICE_CHANGE_CONFIG,
            SERVICE_WIN32_OWN_PROCESS,
            create_service_start_mode,
            SERVICE_ERROR_NORMAL,
            pathname.c_str(),
            NULL,
            NULL,
            create_service_depends.data(), // service dependencies
            NULL,
            NULL);

         if (hservice == NULL)
         {
            ec = last_error_code();
            return;
         }
		 
		   bool succeed = false;

         // grant user "Login as a Service" permission.
         if ( !service_user_.empty() ) 
         {
            std::basic_string<char_type> actual_service_user;
            if ( service_user_.find(std::basic_string<char_type>("\\")) ==  std::basic_string<char_type>::npos ) 
            {
               actual_service_user = std::basic_string<char_type>(".\\") + service_user_;
            }
            else 
            {
               actual_service_user = service_user_;
            }
	
            // If the function succeeds, the return value is nonzero.
            succeed = ::ChangeServiceConfig(
               hservice,                   // service handle
               SERVICE_NO_CHANGE,          // service type
               SERVICE_NO_CHANGE,          // start type
               SERVICE_NO_CHANGE,          // error control
               NULL,                       // path
               NULL,                       // load order group
               NULL,                       // tag id
               NULL,                       // dependencies
               actual_service_user.c_str(),// user account
               service_password_.c_str(),  // user account password
               NULL ) ? true : false;                     // service display name

            if ( !succeed )
            {
               ec = last_error_code();
               return;
            }
         }

         char_type serviceDescription[2048];

#if defined(BOOST_APPLICATION_STD_WSTRING)
         wcscpy_s(serviceDescription, sizeof(serviceDescription) / sizeof(serviceDescription[0]),
             service_description_.c_str());
#else
         strcpy_s(serviceDescription, sizeof(serviceDescription),
                  service_description_.c_str());
#endif

         SERVICE_DESCRIPTION sd = {
            serviceDescription
         };

         ChangeServiceConfig2(hservice, SERVICE_CONFIG_DESCRIPTION, &sd);
         CloseServiceHandle(hservice);

         register_application(ec);
      }

      // The App Paths registry subkeys are used to register and control the behavior
      // of the system on behalf of applications.
      // Note: if returns 5 (Access is denied) - you need run app as admin.
      void register_application(boost::system::error_code &ec)
      {
         boost::filesystem::path path(service_path_name_);

         std::basic_string<char_type> sub_key, path_entry, default_entry;

         HKEY hkey = NULL;

         LONG error;

#if defined(BOOST_APPLICATION_STD_WSTRING)
         sub_key = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\"
            + path.filename().wstring();
#else
         sub_key = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\"
            + path.filename().string();
#endif

         error = RegCreateKeyEx(HKEY_LOCAL_MACHINE, sub_key.c_str(), 0, NULL,
                                REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL,
                                &hkey, NULL);

         if (error != NO_ERROR)
         {
            if (hkey != NULL) RegCloseKey(hkey);

            ec = last_error_code();
            return;
         }

#if defined(BOOST_APPLICATION_STD_WSTRING)
        default_entry = path.wstring();
#else
        default_entry = path.string();
#endif

         error = RegSetValueEx(hkey, NULL, 0, REG_EXPAND_SZ, (PBYTE)
                               default_entry.c_str(), default_entry.length());

         if (error != NO_ERROR)
         {
            if (hkey != NULL) RegCloseKey(hkey);
            ec = last_error_code();
            return;
         }

#if defined(BOOST_APPLICATION_STD_WSTRING)
        path_entry = path.branch_path().wstring();
#else
        path_entry = path.branch_path().string();
#endif

         error = RegSetValueEx(hkey, TEXT("path"), 0, REG_EXPAND_SZ,
                               (PBYTE) path_entry.c_str(), path_entry.length());

         if (error != NO_ERROR)
         {
            if (hkey != NULL) RegCloseKey(hkey);
            ec = last_error_code();
            return;
         }

         if (hkey != NULL) RegCloseKey(hkey);
      }

      std::basic_string<char_type> service_name_;
      std::basic_string<char_type> service_display_name_;
      std::basic_string<char_type> service_description_;
      std::basic_string<char_type> service_path_name_;
      std::basic_string<char_type> service_option_string_;
      std::basic_string<char_type> service_start_mode_;
      std::basic_string<char_type> service_depends_;
	  
      std::basic_string<char_type> service_user_;
      std::basic_string<char_type> service_password_;

   }; // install_windows_service

   // install_windows_service versions for common string and wide string
   typedef install_windows_service_<character_types::char_type> install_windows_service;
   // wchar_t / char

}}} // boost::application::example

#endif // BOOST_APPLICATION_IMPL_WINDOWS_SERVICE_SETUP_HPP


