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

#ifndef _Parameter_HEADER_
#define _Parameter_HEADER_

/*! \file Parameter.h This file provides definitions of parameter values.
 */

#include "AffymetrixBaseTypes.h"
#include "ParameterException.h"
#include <string>
#include <vector>
#include <list>

#ifdef WIN32
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_parameter
{

/*! A structure to hold a parameter name/value pair. */
typedef struct _tagParameterNameValuePair
{
	/*! The name of the parameter */
	std::wstring Name;

	/*! The value of the parameter */
	std::wstring Value;

	/*! A copy operator.
	 *
	 * @param param The parameter to copy.
	 */
	_tagParameterNameValuePair operator=(_tagParameterNameValuePair param) { Name = param.Name; Value = param.Value; return *this; }

	/*! An equality operator.
	 *
	 * @param param The parameter to compare.
	 * @return True if the parameter names are the same.
	 */
	bool operator==(_tagParameterNameValuePair param) { if (param.Name == Name) return true; return false; }

	/*! An equality operator.
	 *
	 * @param name A parameter name to compare.
	 * @return True if the parameter names are the same.
	 */
	bool operator==(const std::wstring &name) { if (Name == name) return true; return false; }
} ParameterNameValuePair;

/*! An STL vector or parameter name value pairs. */
typedef std::vector<ParameterNameValuePair> ParameterNameValuePairVector;

/*! An STL list or parameter name value pairs. */
typedef std::list<ParameterNameValuePair> ParameterNameValuePairList;


/*! A structure to hold a parameter name/value pair with a controlled vocabulary list. */
typedef struct _tagParameterNameValueControlVocabulary
{
	/*! The name of the parameter */
	std::wstring Name;

	/*! The value of the parameter */
	std::wstring Value;

	/*! The controlled vocabulary */
	std::vector<std::wstring> ControlledVocabulary;

	/*! A copy operator.
	 *
	 * @param param The parameter to copy.
	 */
	_tagParameterNameValueControlVocabulary operator=(_tagParameterNameValueControlVocabulary param)
	{
		Name = param.Name;
		Value = param.Value;
		ControlledVocabulary = param.ControlledVocabulary;
		return *this;
	}

	/*! An equality operator.
	 *
	 * @param param The parameter to compare.
	 * @return True if the parameter names are the same.
	 */
	bool operator==(_tagParameterNameValueControlVocabulary param) { if (param.Name == Name) return true; return false; }

	/*! An equality operator.
	 *
	 * @param name A parameter name to compare.
	 * @return True if the parameter names are the same.
	 */
	bool operator==(const std::wstring &name) { if (Name == name) return true; return false; }
} ParameterNameValueControlVocabulary;

/*! An STL vector or parameter name value pairs. */
typedef std::vector<ParameterNameValueControlVocabulary> ParameterNameValueControlVocabularyVector;

/*! An STL list or parameter name value pairs. */
typedef std::list<ParameterNameValueControlVocabulary> ParameterNameValueControlVocabularyList;


/*! Defines a signed integral number type */
#define PARAMETER_VALUE_SIGNED_INTEGRAL 0

/*! Defines an unsigned integral number type */
#define PARAMETER_VALUE_UNSIGNED_INTEGRAL 1

/*! Defines a floating point number type */
#define PARAMETER_VALUE_FLOATING_POINT 2

/*! Defines a 8 bit string type */
#define PARAMETER_VALUE_STRING8_TYPE 3

/*! Defines a 16 bit string type */
#define PARAMETER_VALUE_STRING16_TYPE 3

/*! A type for defining the type of parameter value. */
typedef u_int8_t ParameterValueType;

/*! The possible data types for a parameter value. */
typedef union _ParameterDataValue
{
	int8_t			i8;		/*! 8 bit integer. */
	int16_t			i16;	/*! 16 bit integer. */
	int32_t			i32;	/*! 32 bit integer. */
	u_int8_t		iu8;	/*! 8 bit unsigned integer. */
	u_int16_t		iu16;	/*! 16 bit unsigned integer. */
	u_int32_t		iu32;	/*! 32 bit unsigned integer. */
	float			f32;	/*! 32 bit floating point. */
} ParameterDataValue;

/*! A structure to store a parameter value - of varying types */
typedef struct _tagParameterValue
{
	/*! The type of parameter value */
	ParameterValueType paramValueType;

	/*! The size of the value object. For strings this is the length of the string. */
	int32_t valueSize;

	/*! A union of all possible data types. */
	ParameterDataValue paramValue;

} ParameterValue;

}

#endif // _Parameter_HEADER_
