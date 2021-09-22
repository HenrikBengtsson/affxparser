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


#ifndef _Parameter_HEADER_
#define _Parameter_HEADER_

/*! \file Parameter.h This file provides definitions of parameter values.
 */

#include "calvin_files/parameter/src/ParameterException.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <list>
#include <string>
#include <vector>
//

#ifdef _MSC_VER
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

}

#endif // _Parameter_HEADER_
