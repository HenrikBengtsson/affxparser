////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License 
// (version 2.1) as published by the Free Software Foundation.
// 
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
// for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
//
////////////////////////////////////////////////////////////////


#ifndef _ExceptionBase_HEADER_
#define _ExceptionBase_HEADER_

#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
#include "calvin_files/utils/src/DateTime.h"
//
#include <cstring>
#include <string>
//


/*! \file ExceptionBase.h This file provides base class definition for the exception class.
*/ 
namespace affymetrix_calvin_exceptions
{
	/*! Base exception class for the Calvin File SDK */
	class CalvinException
	{
		protected:
			/*! Source in the _MSC_VER use case is used for registry lookup to determine the resource file. 
				The message code is used incojuction with source to lookup the specific message string based on code.
			*/
			std::wstring sourceName;
			/*! Message for the exception */
			std::wstring errorDescription;
			/*! Date/Time stamp of exception */
			std::wstring timeStamp;
			/*! File the exception occured in */
			std::string fileName;
			/*! Line number that the exception occured */
			u_int16_t lineNumber;
			/*! Message code to be used by client for logic or to use for lookup in localized resource file */
			u_int64_t errorCode;

		public:

		/*! Constructor
		* Default constructor. Initializes all variables to 0 if numeric, "" if string and the date to the current date and time.
		*/
		CalvinException();

		/*! Constructor
		* Constructs a CalvinException object.
		*
		* @param _Source Typically will be the application name. Can also be the file sdk object type E.g. "Array File Reader".
		* @param _Description A brief message which describes what happened.
		* @param _TimeStamp Time and date the exception occured.
		* @param _FileName File name of the source file the exception occured. The __FILE__ can be used to determine this information.
		* @param _LineNumber Line number in the source file that generated the exception. The __LINE__ can be used to determine this information.
		* @param _ErrorCode An numeric value the is unique to this error/exception type
		*/
		CalvinException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode);

		/*! Constructor
		* Constructs a CalvinException object
		* Note: The time date stamp will be automatically set to the current time.
		*
		* @param _Source Typically will be the application name. Can also be the file sdk object type E.g. "Array File Reader".
		* @param _Description A brief message which describes what happened.
		* @param _ErrorCode An numeric value the is unique to this error/exception type
		*/
		CalvinException(std::wstring _Source, std::wstring _Description, u_int64_t _ErrorCode);

		/*! Constructor
		* Constructs a CalvinException object
		* Note: The time date stamp will be automatically set to the current time.
		*
		* @param _Description A brief message which describes what happened.
		* @param _ErrorCode An numeric value the is unique to this error/exception type
		*/
		CalvinException(std::wstring _Description, u_int64_t _ErrorCode);
		/*! Constructor
		* Constructs a CalvinException object
		* Note: The time date stamp will be automatically set to the current time.
		*
		* @param _ErrorCode An numeric value the is unique to this error/exception type
		*/
		CalvinException(u_int64_t _ErrorCode);

		/*! The source name associated with the exception.
		*
		* @return The source name associated with the exception.
		*/
		std::wstring Source();
		/*! The source name associated with the exception.
		*
		* @param value Source name associated with the exception.
		*/
		void Source(std::wstring value);
		/*! The error description associated with the exception.
		*
		* @return The error description associated with the exception.
		*/
		std::wstring Description();
		/*! The error description associated with the exception.
		*
		* @param value Error description associated with the exception.
		*/
		void Description(std::wstring value);
		/*! The error time stamp associated with the exception.
		*
		* @return The error time stamp associated with the exception.
		*/
		std::wstring TimeStamp();
		/*! The error time stamp associated with the exception.
		*
		* @param value Error time stamp associated with the exception.
		*/
		void TimeStamp(std::wstring value);
		/*! The error source file name associated with the exception.
		*
		* @return The source file name associated with the exception.
		*/
		std::string SourceFile();
		/*! The error source file name associated with the exception.
		*
		* @param value Source file name associated with the exception.
		*/
		void SourceFile(std::string value);
		/*! The error source line number associated with the exception.
		*
		* @return The source line number associated with the exception.
		*/
		u_int16_t LineNumber();
		/*! The error source line number associated with the exception.
		*
		* @param value Source line number associated with the exception.
		*/
		void LineNumber(u_int16_t value);

		/*! The error code associated with the exception.
		*
		* @return The error code associated with the exception.
		*/
		u_int64_t ErrorCode();
		/*! The error code associated with the exception.
		*
		* @param value Error code associated with the exception.
		*/
		void ErrorCode(u_int64_t value);

		/*! Returns a string describing the exception
		*
		* @return Returns a string describing the exception.
		*/
		const std::wstring ToString();

                /*! Format source file, line and time stamp.
                 *
                 * @return Returns a string combining source file, line number and time stamp.
                 */
                std::wstring SystemInfo();
                
	};

}

#endif // _ExceptionBase_HEADER_

