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


#ifndef _PATAssignment_HEADER_
#define _PATAssignment_HEADER_

/*! \file PATAssignment.h This file provides a type to define the method the probe array type was assigned.
 */

#include <cstring>
#include <string>
//

namespace affymetrix_calvin_array
{

/*! The method the probe array type was assigned. */
typedef enum _PATAssignmentMethod
{
	NoAssignment,				/*! Unknown */
	AffyBarcodeAssignment,		/*! User entered an Affy barcode. */
	UserSelectedAssignment,		/*! User selected. */
	OtherAssignment				/*! Other method. */
} PATAssignmentMethod;

/*! Converts the probe array assignment type to a string.
 * @param pat The probe array assignment
 * @return The string representation.
 */
std::wstring PATAssignmentMethodToString(PATAssignmentMethod pat);

/*! Converts a string to the probe array assignment type.
 * @param pat The string representation.
 * @return The probe array assignment
 */
PATAssignmentMethod PATAssignmentMethodFromString(const std::wstring &pat);

};

#endif // _PATAssignment_HEADER_
