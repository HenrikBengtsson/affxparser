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


#ifndef _ParameterException_HEADER_
#define _ParameterException_HEADER_

#include "calvin_files/exception/src/ExceptionBase.h"
//

/*! \file ParameterException.h This file defines parameter exceptions.
*/

namespace affymetrix_calvin_exceptions
{

/*! This exception indicates the parameter does not match the requested value
 *	TODO: Add member to say what type is expected
 */
class ParameterMismatchException : public CalvinException
{
public:
  ParameterMismatchException() : CalvinException() {}
  ParameterMismatchException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode):
    CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode) {}
    
  const std::wstring ToString();
};

/*! This exception indicates the parameter type does not match the expected type
 *
 */
class UnexpectedParameterException : public CalvinException
{
public:
    UnexpectedParameterException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode, const std::wstring& expectedType_):
      CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode),
      expectedType(expectedType_){}
  const std::wstring ToString();
protected:
	/*! String with the name of the expected type */
	std::wstring expectedType;
};

/*! This exception indicates a parameter value is out-of-range */
class OutOfRangeParameterException : public CalvinException
{
public:
	/*! Constructor
	 *	@param lower_ lower limit of the range
	 *	@param upper_ upper limit of the range
	 */
	OutOfRangeParameterException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode, const std::wstring& lower_, const std::wstring& upper_):
          CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode), 
          lower(lower_), upper(upper_) {}
  const std::wstring ToString();
protected:
	std::wstring lower;
	std::wstring upper;
};

/*! This exception indicates that the parameter string is too long */
class ParameterStringTooLongException : public CalvinException
{
public:
	/*! Constructor
	 *	@param len Max length of the string
	 */
	ParameterStringTooLongException(std::wstring _Source, std::wstring _Description, std::wstring _TimeStamp, std::string _FileName, u_int16_t _LineNumber, u_int64_t _ErrorCode, int32_t len):
          CalvinException(_Source, _Description, _TimeStamp, _FileName,  _LineNumber, _ErrorCode), maxLen(len) {}
          const std::wstring ToString();
protected:
	int32_t maxLen;
};

}

#endif
