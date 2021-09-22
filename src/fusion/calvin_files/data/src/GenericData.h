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


#ifndef _GenericData_HEADER_
#define _GenericData_HEADER_

/*! \file GenericData.h This file provides access to generic data.
 */

#ifdef _MSC_VER
#include <windows.h>
#endif

#include "calvin_files/data/src/DataException.h"
#include "calvin_files/data/src/DataGroup.h"
#include "calvin_files/data/src/DataSet.h"
#include "calvin_files/data/src/FileHeader.h"
#include "calvin_files/parsers/src/FileException.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
#include "calvin_files/utils/src/AffyStlCollectionTypes.h"
#include "calvin_files/utils/src/AffymetrixGuid.h"
//
#include <cstring>
#include <string>
#include <vector>
//


using namespace affymetrix_calvin_utilities;

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

	/*! Returns the file identifier.
	 *	@return The file identifier.
	 */
	AffymetrixGuidType FileIdentifier();

	/*! Returns the parent array file identifier.
	 *	@return The parent array file identifier.
	 */
	AffymetrixGuidType ArrayFileIdentifier();

	/*! Returns the parent array identifier.
	 *	@return The parent array identifier.
	 */
	AffymetrixGuidType ArrayIdentifier();

	/*! Returns a reference to the file header object
	 *
	 * @return File header object 
	 */
	FileHeader& Header() { return header; }

	/*! Return the number of DataGroups in the GenericData object.
	 *  @return Number of DataGroups.
	 */
	u_int32_t DataGroupCnt() const;

	/*! Return the names of the DataGroups in the generic data object.
	 *  @param names An [in,out] vector that will receive the names of all DataGroups.
	 */
	void DataGroupNames(WStringVector& names);

	/*! Return the number of DataSets in the DataGroup referenced by index.
	 *	@param dataGroupIdx Index of the DataGroup.
	 *	@return Number of DataSets associated with the DataGroup.
	 *	@exception affymetrix_calvin_exceptions::DataGroupNotFoundException DataGroup not found.
	 */
	u_int32_t DataSetCnt(u_int32_t dataGroupIdx);

	/*! Return the number of DataSets in the DataGroup referenced by name.
	 *	@param dataGroupName Name of the DataGroup.
	 *	@return Number of DataSets associated with the DataGroup.
	 *	@exception affymetrix_calvin_exceptions::DataGroupNotFoundException DataGroup not found.
	 */
	u_int32_t DataSetCnt(const std::wstring& dataGroupName);

	/*! Return the DataSet names associated with a DataGroup.
	 *	@param dataGroupIdx Index of the DataGroup from which to retrieve the DataSet names.
	 *  @param names An [in,out] vector that will receive the names of all DataSets.
	 *	@exception affymetrix_calvin_exceptions::DataGroupNotFoundException DataGroup not found.
	 */
	void DataSetNames(u_int32_t dataGroupIdx, WStringVector& names);

	/*! Return the DataSet names associated with a DataGroup.
	 *	@param dataGroupName Name of the DataGroup from which to retrieve the DataSet names.
	 *  @param names An [in,out] vector that will receive the names of all DataSets.
	 *	@exception affymetrix_calvin_exceptions::DataGroupNotFoundException DataGroup not found.
	 */
	void DataSetNames(const std::wstring& dataGroupName, WStringVector& names);

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

	/*! Returns a DataGroup object based on a DataGroup file position.
	 *	This is useful when there are many DataGroups and the file position of each DataGroup is known (Calvin CDF).
	 *	In this case the GenericFileReader::ReadHeader() method should be called with the ReadNoDataGroupHeader flag.
	 *	@param dataGroupFilePos File position of the DataGroup in the current file
	 *	@return DataGroup object.
	*/
	affymetrix_calvin_io::DataGroup DataGroup(u_int32_t dataGroupFilePos);

	/*! Clears the contents of the class.*/
	void Clear();

	/*! Sets the file access method when reading data.  Set this value before getting the first DataSet or DataGroup.
	 *	The default is to use memory-mapping.
	 *	@param value If value is true the file will be accessed with memory-mapping, if false, STL fstream will be used
	 */
	void UseMemoryMapping(bool value) { useMemoryMapping = value; }

	/*! Set the data loading policy for DataSets and DataGroups created by GenericData.
	 *	This is considered a hint to DataSet.  Default value is false.
	 *	@param value If value is true, DataSets created subsequently will attempt to read the entire DataSet data into a memory buffer.
	 */
	void LoadEntireDataSetHint(bool value) { loadEntireDataSetHint = value; }

	// Protected members
public:

	/*! Read the full DataSetHeader if it has only been parially read.
	 *	@param dph Pointer to the DataSetHeader to read
	 */
	void ReadFullDataSetHeader(DataSetHeader* dph);

	/*! Determine if the DataSetHeader has been partially read.
	 *	@param dph Pointer to the DataSetHeader to check
	 *	@return true if the dph has only been partially read or is 0, otherwise false.
	 */
	bool IsDSHPartiallyRead(const affymetrix_calvin_io::DataSetHeader* dph);

	/*! Opens a ifstream object on the file set on the Header() method
	 *	@param ifs The stream to open on the file.
	 */
	void OpenFStream(std::ifstream& ifs);

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

	/*! Finds a DataSetHeader by index.
	 *	@param dch The DataGroupHeader of the DataGroup to which the DataSet belongs.
	 *	@param dataSetIdx The DataSet index of the DataSetHeader to find.
	 *	@return A pointer to the DataSetHeader if it is found, otherwise 0.
	 */
	static affymetrix_calvin_io::DataSetHeader* FindDataSetHeader(affymetrix_calvin_io::DataGroupHeader* dch, u_int32_t dataSetIdx);

	/*!	Finds a DataSetHeader by name.
	 *	@param dch The DataGroupHeader of the DataGroup to which the DataSet belongs.
	 *	@param dataSetName The DataSet name of the DataSetHeader to find.
	 *	@return A pointer to the DataSetHeader if it is found, otherwise 0.
	 */
	static affymetrix_calvin_io::DataSetHeader* FindDataSetHeader(affymetrix_calvin_io::DataGroupHeader* dch, const std::wstring& dataSetName);

	/*! Opens the file for access.  Has no effect on non-Windows systems.
	 *	@return True if the memory-mapping was opened successfully.
	*/
	bool Open();

	/*! Closes the file.
	 */
	void Close();

	/*! Opens a memory map on the file.  Has no effect on non-Windows systems.
	 *	@return True if the memory-mapping was opened successfully.
	 */
	bool MapFile();

	/*! Closes the memory map.  Windows only */
	void UnmapFile();

/*! Creates a new DataSet
 *	@param dsh The DataSetHeader of the DataSet to create.
 *	@return The new DataSet
 */
	affymetrix_calvin_io::DataSet* CreateDataSet(DataSetHeader* dsh);

protected:
	/*! The header and generic header objects */
	affymetrix_calvin_io::FileHeader header;

#ifdef _MSC_VER
	/*! Handle returned by CreateFileMapping */
	HANDLE fileMapHandle;

	/*! Handle returned by CreateFile */
	HANDLE fileHandle;
#endif

	/*! Flag that indicates the file access technique; true = use memory-mapping, false = use ifstream */
	bool useMemoryMapping;

	/*! fstream file access member */
	std::ifstream fileStream;

	/*! Indicates whether DataSets and DataGroups created by GenericData should attempt to read all data into a memory buffer. */
	bool loadEntireDataSetHint;

	friend class DataGroup;
};

}

#endif // _GenericData_HEADER_
