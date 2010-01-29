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


#ifndef _TilingResultData_HEADER_
#define _TilingResultData_HEADER_

/*! \file TilingResultData.h This file provides types to hold tiling results.
 */


#include "calvin_files/parameter/src/ParameterNameValueType.h"
//
#include <cstring>
#include <string>
//

/*! Stores data for a tiling array sequence. */
typedef struct _TilingSequenceData
{
	/*! The name of the sequence. */
	std::wstring name;

	/*! The version associated to the sequence. */
	std::wstring version;

	/*! The group name for the sequence. */
	std::wstring groupName;

	/*! The parameter name/value array. */
	affymetrix_calvin_parameter::ParameterNameValueTypeList parameters;

} TilingSequenceData;

#endif

