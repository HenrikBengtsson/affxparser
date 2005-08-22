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

#ifndef _GenericData_HEADER_
#define _GenericData_HEADER_

/*! \file GenericData.h This file provides access to generic data.
 */

#include "AffymetrixBaseTypes.h"
#include "AffymetrixGuid.h"
#include "FileHeader.h"
#include "DataSet.h"
#include "DataGroup.h"
#include "DataException.h"
#include "FileException.h"
#include "ArrayId.h"
#include <string>
#include <vector>

#ifdef WIN32
#include <windows.h>
#endif

namespace affymetrix_calvin_io
{
/*! This class provides interfaces to store analysis results and data. */
class GenericData
{
public:
	/*! Constructor */
	GenericData();

	/*! Destructor */
	~GenericData();

public:
	/*! Returns a const reference to the file identifier.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType FileIdentifier();

	/*! Returns a const reference to the parent array file identifier.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType ArrayFileIdentifier();

	/*! Returns a reference to the file header object
	 *
	 * @return File header object 
	 */
	affymetrix_calvin_io::FileHeader& Header() { return header; }

	/*! Return the number of DataGroups in the GenericData object.
	 *  @return Number of DataGroups.
	 */
	u_int32_t DataGroupCnt() const;

	/*! Return the names of the DataGroups in the generic data object.
	 *  @param names An [in,out] vector that will receive the names of all DataGroups.
	 */
	void DataGroupNames(std::vector<std::wstring>& names);

	/*! Return the number of DataSets in the DataGroup referenced by index.
	 *	@param dataGroupIndex Index of the DataGroup.
	 *	@return Number of DataSets associated with the DataGroup.
	 *	@exception affymetrix_calvin_exceptions::DataGroupNotFoundException DataGroup not found.
	 */
	u_int32_t DataSetCnt(u_int32_t dataGroupIdx);

	/*! Return the number of DataSets in the DataGroup referenced by name.
	 *	@param name Name of the DataGroup.
	 *	@return Number of DataSets associated with the DataGroup.
	 *	@exception affymetrix_calvin_exceptions::DataGroupNotFoundException DataGroup not found.
	 */
	u_int32_t DataSetCnt(const std::wstring& dataGroupName);

	/*! Return the DataSet names associated with a DataGroup.
	 *	@param dataGroupIdx Index of the DataGroup from which to retrieve the DataSet names.
	 *  @param names An [in,out] vector that will receive the names of all DataSets.
	 *	@exception affymetrix_calvin_exceptions::DataGroupNotFoundException DataGroup not found.
	 */
	void DataSetNames(u_int32_t dataGroupIdx, std::vector<std::wstring>& names);

	/*! Return the DataSet names associated with a DataGroup.
	 *	@param dataGroupName Name of the DataGroup from which to retrieve the DataSet names.
	 *  @param names An [in,out] vector that will receive the names of all DataSets.
	 *	@exception affymetrix_calvin_exceptions::DataGroupNotFoundException DataGroup not found.
	 */
	void DataSetNames(const std::wstring& dataGroupName, std::vector<std::wstring>& names);

	/*! Returns a pointer to the DataSet object by DataGroup and DataSet index.
	 *  Each call will return a new DataSet object.
	 *  The caller should call Delete when finished with the DataSet.
	 *
	 *  @param dataGroupIdx The index of the DataGroup from which to find the DataSet.
	 *	@param dataSetIdx The index of the DataSet to return.
	 *  @return DataSet
	 *	@exception affymetrix_calvin_exceptions::DataGroupNotFoundException DataGroup not found.
	 *	@exception affymetrix_calvin_exceptions::DataSetNotFoundException DataSet not found.
	 */
	affymetrix_calvin_io::DataSet* DataSet(u_int32_t dataGroupIdx, u_int32_t dataSetIdx);

	/*! Returns a pointer to the DataSet object by DataGroup and DataSet name.
	 *  Each call will return a new DataSet object.
	 *  The caller should call Delete when finished with the DataSet.
	 *
	 *  @param dataGroupName The name of the DataGroup from which to find the DataSet.
	 *	@param dataSetName The name of the DataSet to return.
	 *  @return DataSet
	 *	@exception affymetrix_calvin_exceptions::DataGroupNotFoundException DataGroup not found.
	 *	@exception affymetrix_calvin_exceptions::DataSetNotFoundException DataSet not found.
	 */
	affymetrix_calvin_io::DataSet* DataSet(const std::wstring& dataGroupName, const std::wstring& dataSetName);

	/*! Returns a DataGroup object based on a DataGroup file position
	 *	@param dataGroupFilePos File position of the DataGroup in the current file
	 *	@return DataGroup object.
	*/
	affymetrix_calvin_io::DataGroup DataGroup(u_int32_t dataGroupFilePos);


	/*! Clears the contents of the class.*/
	void Clear();

	// Protected members
public:

	/*! Read the full DataSetHeader if it has only been parially read.
	 *	@param dph Pointer to the DataSetHeader to read
	 */
	void ReadFullDataSetHeader(affymetrix_calvin_io::DataSetHeader* dph);

	/*! Determine if the DataSetHeader has been partially read.
	 *	@param dph Pointer to the DataSetHeader to check
	 *	@return true if the dph has only been partially read or is 0, otherwise false.
	 */
	bool IsDPHPartiallyRead(const affymetrix_calvin_io::DataSetHeader* dph);

	void OpenFStream(std::ifstream& fileStream);

	/*! Finds a DataSetHeader by name.
	 *	@param name The name of the DataGroup
	 *  @return A pointer to the DataGroupHeader.  If not found, the return is 0.
	 */
	affymetrix_calvin_io::DataGroupHeader* FindDataGroupHeader(const std::wstring& name);

	/*! Finds a DataGroupHeader by index.
	 *	@param index The index of the DataGroup.
	 *  @return A pointer to the DataGroupHeader.  If not found, the return is 0.
	 */
	affymetrix_calvin_io::DataGroupHeader* FindDataGroupHeader(int32_t index);

	/*!
	 */
	static affymetrix_calvin_io::DataSetHeader* FindDataSetHeader(affymetrix_calvin_io::DataGroupHeader* dch, u_int32_t dataSetIdx);

	/*!
	 */
	static affymetrix_calvin_io::DataSetHeader* FindDataSetHeader(affymetrix_calvin_io::DataGroupHeader* dch, const std::wstring& dataSetName);

	bool MapFile();
	void UnmapFile();

protected:
	/*! The header and generic header objects */
	affymetrix_calvin_io::FileHeader header;

#ifdef WIN32
	/*! Handle returned by CreateFileMapping */
	HANDLE fileMapHandle;

	/*! Handle returned by CreateFile */
	HANDLE fileHandle;
#endif

	friend class DataGroup;
};

}

#endif // _GenericData_HEADER_
