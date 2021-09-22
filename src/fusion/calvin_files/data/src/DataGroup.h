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


#ifndef _DataGroup_HEADER_
#define _DataGroup_HEADER_

/*! \file DataGroup.h This file provides access to the DataSets in a DataGroup.
 */

#ifdef _MSC_VER
#include <windows.h>
#endif

#include "calvin_files/data/src/DataException.h"
#include "calvin_files/data/src/DataGroupHeader.h"
#include "calvin_files/data/src/DataSet.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <fstream>
#include <string>
//


namespace affymetrix_calvin_io
{

// forward declare
class GenericData;

/*! This class provides methods to get a DataSet in a DataGroup. */
class DataGroup
{
public:
	/*! Constructor
	 *	@param filename The name of the generic file to access.
	 *	@param dch The DataGroupHeader of the DataGroup to access.
	 *	@param handle A handle to the file mapping object
	 *	@param loadEntireDataSetHint Indicates if DataSets created by DataGroup will attempt to read the entire DataSet data into a memory buffer.
	 */
	DataGroup(const std::string& filename, const DataGroupHeader& dch, void* handle, bool loadEntireDataSetHint=false);

	/*! Constructor
	 *	@param filename The name of the generic file to access.
	 *	@param dch The DataGroupHeader of the DataGroup to access.
	 *	@param ifs An open ifstream object.
	 *	@param loadEntireDataSetHint Indicates if DataSets created by DataGroup will attempt to read the entire DataSet data into a memory buffer.
	 */
	DataGroup(const std::string& filename, const DataGroupHeader& dch, std::ifstream& ifs, bool loadEntireDataSetHint=false);

	/*! Method to get a reference to the DataGroupHeader
	 *	@return A reference to the DataGroupHeader.
	 */
	const affymetrix_calvin_io::DataGroupHeader& Header() { return dataGroupHeader; }

	/*! Returns a pointer to the DataSet object by DataSet index.
	 *  Each call will return a new DataSet object.
	 *  The caller should call Delete when finished with the DataSet.
	 *
	 *	@param dataSetIdx The index of the DataSet to return.
	 *  @return DataSet
	 *	@exception affymetrix_calvin_exceptions::DataGroupNotFoundException DataGroup not found.
	 *	@exception affymetrix_calvin_exceptions::DataSetNotFoundException DataSet not found.
	 */
	affymetrix_calvin_io::DataSet* DataSet(u_int32_t dataSetIdx);

	/*! Returns a pointer to the DataSet object by DataSet name.
	 *  Each call will return a new DataSet object.
	 *  The caller should call Delete when finished with the DataSet.
	 *
	 *	@param dataSetName The name of the DataSet to return.
	 *  @return DataSet
	 *	@exception affymetrix_calvin_exceptions::DataGroupNotFoundException DataGroup not found.
	 *	@exception affymetrix_calvin_exceptions::DataSetNotFoundException DataSet not found.
	 */
	affymetrix_calvin_io::DataSet* DataSet(const std::wstring& dataSetName);

protected:
	/*! Name of the generic file to access */
	std::string filename;
	/*! DataGroupHeader of the DataGroup from which to get DataSets*/
	DataGroupHeader dataGroupHeader;
	/*! A flag the indicates the data access mode.  True = access the data using memory-mapping.  False = access the data using std::ifstream */
	bool useMemoryMapping;
	/*! File mapping object handle */
	void* handle;
	/*! An open ifstream object */
	std::ifstream* fileStream;
	/*! Indicates whether DataSets created by DataGroup should attempt to read all data into a memory buffer. */
	bool loadEntireDataSetHint;

};

}

#endif	//_DataGroup_HEADER_
