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

#ifndef _ArrayAttributes_HEADER_
#define _ArrayAttributes_HEADER_

/*! \file ArrayAttributes.h This file provides interfaces to store attributes about a physical array.
 */

#include "AffymetrixGuid.h"
#include "Parameter.h"
#include <string>
#include <vector>

namespace affymetrix_calvin_array
{

/*! This class provides interfaces to store physical array attributes. */
class ArrayAttributes
{
public:
	/*! Constructor */
	ArrayAttributes();

	/*! Destructor */
	~ArrayAttributes();

protected:
	/*! A unique idendifier for the array object */
	affymetrix_calvin_utilities::AffymetrixGuidType identifier;

	/*! The array attributes */
	affymetrix_calvin_parameter::ParameterNameValuePairVector attributes;

public:
	/*! The unique idendifier for the object.
	 *
	 * @return The unique idendifier for the object.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType &Identifier() { return identifier; }

	/*! The array attributes.
	 *
	 * @return The vector of array attributes.
	 */
	affymetrix_calvin_parameter::ParameterNameValuePairVector &Attributes() { return attributes; }

	/*! Clears the member objects. */
	void Clear();
};

/*! An STL vector of array attributes. */
typedef std::vector<ArrayAttributes> ArrayAttributesVector;

};

#endif // _ArrayAttributes_HEADER_
