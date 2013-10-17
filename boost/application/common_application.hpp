// common_application.hpp ----------------------------------------------------//
// -----------------------------------------------------------------------------

// Copyright 2011-2013 Renato Tegon Forti

// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt

// -----------------------------------------------------------------------------

// Revision History
// 10-10-2013 dd-mm-yyyy - Initial Release

// -----------------------------------------------------------------------------

#ifndef BOOST_APPLICATION_COMMON_APPLICATION_HPP
#define BOOST_APPLICATION_COMMON_APPLICATION_HPP

#include <boost/application/config.hpp>
#include <boost/application/context.hpp>
// internal aspects
#include <boost/application/aspects/status.hpp>
#include <boost/application/aspects/run_mode.hpp>
// public aspects
#include <boost/application/ready_to_use_aspects.hpp>

namespace boost { namespace application {

   /*!
    * \brief This class hold a application mode system. 
    *  
    * common : Can be seen as Terminal/Console/Interactive 
    *          aplication type. 
    *  
    * An application mode is a collection of aspects that 
    * define application instantiation and behaviour. 
    *  
    * The usual use of this class is to pass it as 
    * template param on launch free function. 
    * 
    */
   class common
   {
   public:

      /*!
       * Creates a common application.
       *
       * \param context An context of application, that hold all
       *        aspects.
       * 
       * \param ec Variable (boost::system::error_code) that will be 
       *        set to the result of the operation.
       * 
       * Check ec for errors.
       * 
       */
      common(application::context &context, boost::system::error_code& ec)
      {
         BOOST_APPLICATION_FEATURE_SELECT

         // default aspects patterns added to this kind of application

         context.add_aspect_if_not_exists<run_mode>(
            make_shared<run_mode>(run_mode::common));

         context.add_aspect_if_not_exists<status>(
            make_shared<status>(status::running));

         context.add_aspect_if_not_exists<wait_for_termination_request>(
            shared_ptr<wait_for_termination_request>(
               new wait_for_termination_request_default_behaviour));
      }

   };
	
}} // boost::application 

#endif // BOOST_APPLICATION_COMMON_APPLICATION_HPP
       