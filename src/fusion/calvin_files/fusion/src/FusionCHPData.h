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


#if !defined _AffymetrixFusionCHPData_HEADER_
#define _AffymetrixFusionCHPData_HEADER_

/*! \file FusionCHPData.h This file provides CHP file reading capabilities.*/

#include "calvin_files/data/src/GenericData.h"
#include "calvin_files/utils/src/AffymetrixGuid.h"
//
#include <cstring>
#include <fstream>
#include <list>
#include <string>
#include <vector>
//

namespace affymetrix_fusion_io
{

/*! A base class for all CHP data objects. */
class FusionCHPData
{
protected:

	/*! Sets the file name.
	 * @param str The file name.
	 */
	void SetFileName(const char *str) { filename = str; }

	/*! Gets the file name.
	 * @return The file name.
	 */
	std::string GetFileName() const { return filename; }

	/*! The CHP file name. */
	std::string filename;

	/*! Reads the contents of the file.
	 * @return True if successfully read.
	 */
	virtual bool Read() = 0;

	/*! Reads the header.
	 * @return True if successfully read.
	 */
	virtual bool ReadHeader() = 0;

	/*! Friend to the registration class as it will call the Read function. */
	friend class FusionCHPDataReg;

	/*! The file type identifiers associated with the CHP files the reader can parse. */
	affymetrix_calvin_utilities::AffymetrixGuidTypeList fileTypeIdentifiers;

	/*! The actual file type identifier in the file. */
	affymetrix_calvin_utilities::AffymetrixGuidType fileTypeIdentifier;

public:
	/*! Destructor */
	virtual ~FusionCHPData() {}

	/*! Get the id of the file (only valid for Command Console "calvin" files)
	 * @return The unique file id.
	 */
	virtual affymetrix_calvin_utilities::AffymetrixGuidType FileId() = 0;

	/*! Returns the GenericData object associated with a Calvin file, NULL for GCOS files. */
	virtual affymetrix_calvin_io::GenericData *GetGenericData() = 0;

	/*! Gets the class name. */
	virtual affymetrix_calvin_utilities::AffymetrixGuidType GetObjectName() = 0;

	/*! Gets the file type identifiers associated with the CHP files the reader can parse.
	 * @return The ids
	 */
	affymetrix_calvin_utilities::AffymetrixGuidTypeList FileTypeIdentifiers() const { return fileTypeIdentifiers; }

	/*! Gets the file type identifier in the file (blank for GCOS files).
	 * @return The id of the file.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType FileTypeIdentifier() { return fileTypeIdentifier; }
};

/*! A class used to self register CHP data classes. */
class FusionCHPDataReg
{
public:
	/*! Constructor */
	FusionCHPDataReg();

	/*! Destructor */
	virtual ~FusionCHPDataReg();

	/* Sets the file type ids.
	 * @param fileTypeIds The identifiers that the CHP object is compatible with.
	 */
	void SetFileTypeIds(const affymetrix_calvin_utilities::AffymetrixGuidTypeList &fileTypeIds);

	/*! Reads the contents of a CHP file.
	 * @param fileName The full path to the input CHP file.
	 * @return A pointer to the CHP data object. NULL if the read failed.
	 */
	static FusionCHPData *Read(const std::string &fileName);

	/*! Reads the header of a CHP file.
	 * @param fileName The full path to the input CHP file.
	 * @return A pointer to the CHP data object. NULL if the read failed.
	 */
	static FusionCHPData *ReadHeader(const std::string &fileName);

private:

	/*! Creates a CHP reading object.
	 * @param fileTypeId The file type in the CHP file.
	 * @return The CHP object, NULL if not able to read the file.
	 */
	static FusionCHPData *CreateObject(const affymetrix_calvin_utilities::AffymetrixGuidType &fileTypeId);

	/*! Makes an CHP data object.
	 * @return The CHP data object.
	 */
	virtual FusionCHPData *MakeObject() = 0;

	/*! A pointer to the first registered CHP reader. */
	static FusionCHPDataReg *m_Head;

	/*! A pointer to the next registered CHP reader. */
	FusionCHPDataReg *m_Next;

	/*! The file type identifiers associated with the CHP files the reader can parse. */
	affymetrix_calvin_utilities::AffymetrixGuidTypeList fileTypeIdentifiers;
};

}

///////////////////////////////////////////////////////////////////////////////////////

#endif
