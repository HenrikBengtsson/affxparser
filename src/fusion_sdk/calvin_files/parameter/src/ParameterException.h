/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License,
// or (at your option) any later version.
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
/////////////////////////////////////////////////////////////////

#ifndef _ParameterException_HEADER_
#define _ParameterException_HEADER_

#include "ExceptionBase.h"

/*! \file ParameterException.h This file defines parameter exceptions.
*/

namespace affymetrix_calvin_exceptions
{

/*! This exception is thrown when the parameter does not match the requested value
 *	TODO: Add member to say what type is expected
 */
class ParameterMismatchException : public CalvinException
{
};

/*! This exception is thrown when the parameter type does not match the expected type
 *
 */
class UnexpectedParameterException : public CalvinException
{
public:
	UnexpectedParameterException(const std::wstring& expectedType_) : expectedType(expectedType_) {}
protected:
	/*! String with the name of the expected type */
	std::wstring expectedType;
};

/*! This exception is thrown when a parameter value is out-of-range */
class OutOfRangeParameterException : public CalvinException
{
public:
	OutOfRangeParameterException(const std::wstring& lower_, const std::wstring& upper_):
		lower(lower_), upper(upper_) {}
protected:
	std::wstring lower;
	std::wstring upper;
};

}

#endif