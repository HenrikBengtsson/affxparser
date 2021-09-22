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

#ifndef _TagValuePair_HEADER_
#define _TagValuePair_HEADER_

/*! \file TagValuePair.h This file provides types to hold tag/value parameters.
 */


#include <cstring>
#include <list>
#include <string>
#include <vector>
//

/*! Defines a name/value parameter. */
typedef struct _TagValuePairType
{
	/*! The name (tag) of the parameter. */
	std::string Tag;

	/*! The value of the parameter. */
	std::string Value;

	/*! Assignment operator.
	 * @param vp The parameter to copy.
	 * @return The copied parameter.
	 */
	_TagValuePairType operator=(_TagValuePairType vp) { Tag = vp.Tag; Value = vp.Value; return *this; }
	
	/*! Comparison operator.
	 * @param vp The parameter to compare.
	 * @return True if the tag values are the same.
	 */
	bool operator==(_TagValuePairType vp) { if (vp.Tag == Tag) return true; return false; }

	/*! Comparison operator.
	 * @param tag The tag to compare.
	 * @return True if the tag values are the same.
	 */
	bool operator==(const char *tag) { if (Tag == tag) return true; return false; }
} TagValuePairType;

/*! An STL vector of parameters. */
typedef std::vector<TagValuePairType> TagValuePairTypeVector;

/*! An STL list of tag/value parameters */
typedef std::list<TagValuePairType> TagValuePairTypeList;

#endif

