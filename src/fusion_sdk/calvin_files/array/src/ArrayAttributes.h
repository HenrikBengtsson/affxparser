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


#ifndef _ArrayAttributes_HEADER_
#define _ArrayAttributes_HEADER_

/*! \file ArrayAttributes.h This file provides interfaces to store attributes about a physical array.
 */

#include "calvin_files/array/src/ArrayMedia.h"
#include "calvin_files/array/src/CreateStep.h"
#include "calvin_files/array/src/PATAssignment.h"
#include "calvin_files/parameter/src/Parameter.h"
#include "calvin_files/utils/src/AffymetrixGuid.h"
//
#include <cstring>
#include <string>
#include <vector>
//

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
	affymetrix_calvin_utilities::AffymetrixGuidType id;

	/*! The array attributes */
	affymetrix_calvin_parameter::ParameterNameValuePairVector attributes;

	/*! The array name. */
	std::string arrayName;

	/*! The barcode on the array cartridge. */
	std::string arrayBarcode;

	/*! The type of assembly. */
	ArrayMedia media;

	/*! The row number of the plate or strip. */
	int mediaRow;

	/*! The column number of the plate or strip. */
	int mediaCol;

	/*! The name of the media file. */
	std::string mediaFileName;

	/*! The master file guid. */
	affymetrix_calvin_utilities::AffymetrixGuidType mediaFileGUID;

	/*! The name of the library file package. */
	std::string libraryPackageName;

	/*! The associated master file. */
	std::string masterFile;

	/*! A unique idendifier for the master file */
	affymetrix_calvin_utilities::AffymetrixGuidType masterFileId;

	/*! The method the probe array type was assigned. */
	PATAssignmentMethod patAssignment;

	/*! The date the array object was created. */
	std::wstring creationDateTime;

	/*! The user who created the data object. */
	std::wstring createdBy;

	/*! A user comment. */
	std::wstring comment;

	/*! The step in Calvin that created the array set data. */
	CreateStep createdStep;

public:
	/*! The unique idendifier for the object.
	 * @return The unique idendifier for the object.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType &Identifier() { return id; }

	/*! The array name.
	 * @return The array name.
	 */
	std::string &ArrayName() { return arrayName; }

	/*! The barcode on the array cartridge.
	 * @return The barcode.
	 */
	std::string &ArrayBarcode() { return arrayBarcode; }

	/*! The type of assembly.
	 * @return The assembly type.
	 */
	ArrayMedia &Media() { return media; }

	/*! The row number of the media or strip.
	 * @return The row.
	 */
	int &MediaRow() { return mediaRow; }

	/*! The column number of the media or strip.
	 * @return The column.
	 */
	int &MediaCol() { return mediaCol; }

	/*! The name of the media file.
	 * @return The media file name.
	 */
	std::string &MediaFileName() { return mediaFileName; }

	/*! The master file guid.
	 * @return The master file guid.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType &MediaFileGUID() { return mediaFileGUID; }

	/*! The name of the library file package.
	 * @return The package name
	 */
	std::string &LibraryPackageName() { return libraryPackageName; }

	/*! The associated master file.
	 * @return The master file name.
	 */
	std::string &MasterFile() { return masterFile; }

	/*! The unique idendifier for the master file.
	 * @return The master file guid.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType &MasterFileId() { return masterFileId; }

	/*! The method the probe array type was assigned.
	 * @return The assignment method.
	 */
	PATAssignmentMethod &PatAssignment() { return patAssignment; }

	/*! The date and time of initial creation.
	 * @return The creation date and time.
	 */
	std::wstring &CreationDateTime() { return creationDateTime; }

	/*! The user who created the data object. */
	std::wstring &CreatedBy() { return createdBy; }

	/*! A user comment.
	 * @return A user comment.
	 */
	std::wstring &Comment() { return comment; }

	/*! The step in Calvin that created the array set data.
	 * @return The step in calvin that create the array set data.
	 */
	CreateStep &CreatedStep() { return createdStep; }

	/*! The array attributes.
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
