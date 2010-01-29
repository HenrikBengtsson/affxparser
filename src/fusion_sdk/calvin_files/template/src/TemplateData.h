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


#ifndef _TemplateData_HEADER_
#define _TemplateData_HEADER_

/*! \file TemplateData.h This file provides interfaces to store information in a template file.
 */

#include "calvin_files/parameter/src/ParameterNameValueType.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
#include "calvin_files/utils/src/AffymetrixGuid.h"
//
#include <cstring>
#include <string>
//

namespace affymetrix_calvin_template
{

/*! This class provides interfaces to store array template information. */
class TemplateData
{
public:
	/*! Constructor */
	TemplateData();

	/*! Destructor */
	~TemplateData();

protected:
	/*! A unique idendifier for the array set object */
	affymetrix_calvin_utilities::AffymetrixGuidType fileId;

	/*! An identifier to the type of data stored in the file */
	affymetrix_calvin_utilities::AffymetrixGuidType dataTypeId;

	/*! The date and time of initial creation. */
	std::wstring creationDateTime;

	/*! The user who created the data object. */
	std::wstring createdBy;

	/*! The user attributes */
	affymetrix_calvin_parameter::ParameterNameValueDefaultRequiredTypeList userAttributes;

public:

	/*! The unique idendifier for the array set.
	 * @return The unique idendifier for the array set.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType &TemplateFileIdentifier() { return fileId; }

	/*! The identifier of the type of data stored in the file.
	 * @return The identifier of the type of data.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType &DataTypeIdentifier() { return dataTypeId; }

	/*! The date and time of initial creation.
	 * @return The creation date and time.
	 */
	std::wstring &CreationDateTime() { return creationDateTime; }

	/*! The user who created the data object.
	 * @return The user name.
	 */
	std::wstring &CreatedBy() { return createdBy; }

	/*! The user attributes.
	 * @return The vector of user attributes.
	 */
	affymetrix_calvin_parameter::ParameterNameValueDefaultRequiredTypeList &UserAttributes() { return userAttributes; }

	/*! Clears the member objects. */
	void Clear();
};

};

#endif // _TemplateData_HEADER_
