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


/*! \file ExceptionBase.h This file provides class definition for the base exception types.
 */ 

#include "calvin_files/exception/src/ExceptionBase.h"
//
#include <sstream>
//

using namespace affymetrix_calvin_exceptions;
using namespace affymetrix_calvin_utilities;

/*! Constructor
 * Default constructor. Initializes all variables to 0 if numeric, "" if string and the date to the current date and time.
 */
CalvinException::CalvinException()
{
	errorCode = 0;
	lineNumber = 0;
	fileName = "";
	timeStamp = DateTime::GetCurrentDateTime().ToString();
	errorDescription = L"";
	sourceName = L"";			
}

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
CalvinException::CalvinException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode)
{
	Source(_Source);
	Description(_Description);
	LineNumber(_LineNumber);
	SourceFile(_FileName);
	TimeStamp(_TimeStamp);
	ErrorCode(_ErrorCode);
}

/*! Constructor
 * Constructs a CalvinException object
 * Note: The time date stamp will be automatically set to the current time.
 *
 * @param _Source Typically will be the application name. Can also be the file sdk object type E.g. "Array File Reader".
 * @param _Description A brief message which describes what happened.
 * @param _ErrorCode An numeric value the is unique to this error/exception type
 */
CalvinException::CalvinException(std::wstring _Source, std::wstring _Description, u_int64_t _ErrorCode)
{
	Source(_Source);
	Description(_Description);
	LineNumber(0);
	SourceFile("");
	TimeStamp(DateTime::GetCurrentDateTime().ToString());
	ErrorCode(_ErrorCode);
}

/*! Constructor
 * Constructs a CalvinException object
 * Note: The time date stamp will be automatically set to the current time.
 *
 * @param _Description A brief message which describes what happened.
 * @param _ErrorCode An numeric value the is unique to this error/exception type
 */
CalvinException::CalvinException(std::wstring _Description, u_int64_t _ErrorCode)
{
	Source(L"");
	Description(_Description);
	LineNumber(0);
	SourceFile("");
	TimeStamp(DateTime::GetCurrentDateTime().ToString());
	ErrorCode(_ErrorCode);
}
/*! Constructor
 * Constructs a CalvinException object
 * Note: The time date stamp will be automatically set to the current time.
 *
 * @param _ErrorCode An numeric value the is unique to this error/exception type
 */
CalvinException::CalvinException(u_int64_t _ErrorCode)
{
	Source(L"");
	Description(L"");
	LineNumber(0);
	SourceFile("");
	TimeStamp(DateTime::GetCurrentDateTime().ToString());
	ErrorCode(_ErrorCode);
}

/*! The source name associated with the exception.
 *
 * @return Source name associated with the exception.
 */
std::wstring CalvinException::Source()
{
	return sourceName;
}
/*! The source name associated with the exception.
 *
 * @param value Source name associated with the exception.
 */
void CalvinException::Source(std::wstring value)
{
	sourceName = value;
}

/*! The error description associated with the exception.
 *
 * @return The error description associated with the exception.
 */
std::wstring CalvinException::Description()
{
	return errorDescription;
}
/*! The error description associated with the exception.
 *
 * @param value Error description associated with the exception.
 */
void CalvinException::Description(std::wstring value)
{
	errorDescription = value;
}

/*! The error time stamp associated with the exception.
 *
 * @return Error time stamp associated with the exception.
 */
std::wstring CalvinException::TimeStamp()
{
	return timeStamp;
}
/*! The error time stamp associated with the exception.
 *
 * @param value Error time stamp associated with the exception.
 */
void CalvinException::TimeStamp(std::wstring value)
{
	timeStamp = value;
}

/*! The error source file name associated with the exception.
 *
 * @return The source file name associated with the exception.
 */
std::string CalvinException::SourceFile()
{
	return fileName;
}
/*! The error source file name associated with the exception.
 *
 * @param value Source file name associated with the exception.
 */
void CalvinException::SourceFile(std::string value)
{
	fileName = value;
}

/*! The error source line number associated with the exception.
 *
 * @return The source line number associated with the exception.
 */
u_int16_t CalvinException::LineNumber()
{
	return lineNumber;
}
/*! The error source line number associated with the exception.
 *
 * @param value Source line number associated with the exception.
 */
void CalvinException::LineNumber(u_int16_t value)
{
	lineNumber = value;
}

/*! The error code associated with the exception.
 *
 * @return The error code associated with the exception.
 */
u_int64_t CalvinException::ErrorCode()
{
	return errorCode;
}
/*! The error code associated with the exception.
 *
 * @param value Error code associated with the exception.
 */
void CalvinException::ErrorCode(u_int64_t value)
{
	errorCode = value;
}

/*! The error code associated with the exception.
 *
 * @return Error code associated with the exception.
 */
const std::wstring CalvinException::ToString()
{ 
	std::wstring fullMsg;

	fullMsg = L"Not implemented yet.";
	
	return fullMsg; 
}


/*! Format source file, line and time stamp.
 *
 * @return Returns a string combining source file, line number and time stamp.
 */
std::wstring CalvinException::SystemInfo()
{ 
  std::wstring systemInfo;

  // TIME STAMP
  if ( !timeStamp.empty() ) {
    systemInfo = timeStamp;
  }
  
  // FILE NAME
  if ( !fileName.empty() ) {
    std::wstring wfileName( fileName.length(), L' ' );
    std::copy(fileName.begin(), fileName.end(), wfileName.begin());
    if( systemInfo.empty() ) {
      systemInfo = wfileName;
    }
    else {
      systemInfo = systemInfo + L":" + wfileName;
    }
  }
  // LINE NUMBER
  if ( lineNumber ) {
    std::wostringstream line;
    line << lineNumber;
    if( systemInfo.empty() ) {
      systemInfo = line.str();
    }
    else {
      systemInfo = systemInfo + L":" + line.str();
    }
  }
  if ( !systemInfo.empty() ) {
    systemInfo = systemInfo + L":";
  }

  return systemInfo;
}
