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


#include "calvin_files/array/src/PATAssignment.h"
//

using namespace affymetrix_calvin_array;

/*! No probe array type assignment. */
#define PAT_ASSIGNMENT_NONE std::wstring(L"None")

/*! Affy barcode selected probe array type. */
#define PAT_ASSIGNMENT_BARCODE std::wstring(L"AffyBarcode")

/*! User selected probe array type. */
#define PAT_ASSIGNMENT_USER_SELECTED std::wstring(L"UserSelected")

/*! Other assignment. */
#define PAT_ASSIGNMENT_OTHER std::wstring(L"Other")

/*
 * Convert the probe array assignment type to a string.
 */
std::wstring affymetrix_calvin_array::PATAssignmentMethodToString(PATAssignmentMethod pat)
{
	switch (pat)
	{
	case NoAssignment:
		return PAT_ASSIGNMENT_NONE;
		break;

	case AffyBarcodeAssignment:
		return PAT_ASSIGNMENT_BARCODE;
		break;

	case UserSelectedAssignment:
		return PAT_ASSIGNMENT_USER_SELECTED;
		break;

	case OtherAssignment:
		return PAT_ASSIGNMENT_OTHER;
		break;

	default:
		return L"";
		break;
	}
}

/*
 * Convert the string to the probe array assignment type.
 */
PATAssignmentMethod affymetrix_calvin_array::PATAssignmentMethodFromString(const std::wstring &pat)
{
	if (pat == PAT_ASSIGNMENT_NONE)
	{
		return NoAssignment;
	}
	else if (pat == PAT_ASSIGNMENT_BARCODE)
	{
		return AffyBarcodeAssignment;
	}
	else if (pat == PAT_ASSIGNMENT_USER_SELECTED)
	{
		return UserSelectedAssignment;
	}
	else if (pat == PAT_ASSIGNMENT_OTHER)
	{
		return OtherAssignment;
	}
	else
	{
		return NoAssignment;
	}
}
