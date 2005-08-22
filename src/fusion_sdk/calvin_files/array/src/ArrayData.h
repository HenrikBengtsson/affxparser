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

#ifndef _ArrayData_HEADER_
#define _ArrayData_HEADER_

/*! \file ArrayData.h This file provides interfaces to store information in an array file.
 */

#include "ArrayId.h"
#include "AffymetrixBaseTypes.h"
#include "AffymetrixGuid.h"
#include "Parameter.h"
#include "ArrayAttributes.h"
#include <string>

namespace affymetrix_calvin_array
{

/*! This class provides interfaces to store array information. */
class ArrayData
{
public:
	/*! Constructor */
	ArrayData();

	/*! Destructor */
	~ArrayData();

protected:
	/*! A unique idendifier for the array set object */
	affymetrix_calvin_utilities::AffymetrixGuidType setIdentifier;

	/*! The arrays attributes for the arrays in the set */
	ArrayAttributesVector physicalArraysAttributes;

	/*! The user attributes */
	affymetrix_calvin_parameter::ParameterNameValueControlVocabularyVector userAttributes;

public:
	/*! The unique idendifier for the array set.
	 *
	 * @return The unique idendifier for the array set.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType &ArraySetIdentifier() { return setIdentifier; }

	/*! The arrays attributes. Each array in a set will have its own attributes.
	 *
	 * @return The vector of arrays attributes.
	 */
	ArrayAttributesVector &PhysicalArraysAttributes() { return physicalArraysAttributes; }

	/*! The user attributes.
	 *
	 * @return The vector of user attributes.
	 */
	affymetrix_calvin_parameter::ParameterNameValueControlVocabularyVector &UserAttributes() { return userAttributes; }

	/*! Clears the member objects. */
	void Clear();
};

};

#endif // _ArrayData_HEADER_
