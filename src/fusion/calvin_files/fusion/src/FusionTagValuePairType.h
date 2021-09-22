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


#ifndef _AffymetrixFusionTagValuePairType_HEADER_
#define _AffymetrixFusionTagValuePairType_HEADER_

/*! \file FusionTagValuePairType.h This file defines a fusion tag-value pair class
 */

#include "calvin_files/parameter/src/ParameterNameValueType.h"
//
#include <cstring>
#include <list>
#include <string>
//

namespace affymetrix_fusion_io
{

/*! This is a tag (name) - value pair class*/
class FusionTagValuePairType 
{
public:
	/*! Name of the item */
	std::wstring Tag;
	/*! Value of the item in string form */
	std::wstring Value;
	/*! Assignment operator */
	FusionTagValuePairType operator=(const FusionTagValuePairType vp) { Tag = vp.Tag; Value = vp.Value; nvt = vp.nvt; return *this; }
	/*! Equality operator */
	bool operator==(const FusionTagValuePairType vp) { if (vp.Tag == Tag) return true; return false; }
	/*! Equality operator
	 *	@param tag Name of the tag
	 *	@return True if the tag argument matches the object tag
	 */
	bool operator==(const wchar_t *tag) { if (Tag == tag) return true; return false; }
	/*! Get the Calvin ParameterNameValueType object
	 *	@return Calvin ParameterNameValueType
	 */
	affymetrix_calvin_parameter::ParameterNameValueType& DetailedType() { return nvt; }
private:
	/*! Embbedded Calvin parameter type object */
	affymetrix_calvin_parameter::ParameterNameValueType nvt;
};

/*! FusionTagValuePairType list */
typedef std::list<FusionTagValuePairType> FusionTagValuePairTypeList;


}

#endif
