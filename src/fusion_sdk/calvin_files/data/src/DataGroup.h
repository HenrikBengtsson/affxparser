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

#ifndef _DataGroup_HEADER_
#define _DataGroup_HEADER_

/*! \file DataGroup.h This file provides access to the DataSets in a DataGroup.
 */

#include "AffymetrixBaseTypes.h"
#include "DataGroupHeader.h"
#include "DataSet.h"
#include "DataException.h"
#include <string>
#include <fstream>

#ifdef WIN32
#include <windows.h>
#endif

namespace affymetrix_calvin_io
{

// forward declare
class GenericData;

/*! This class provides methods to get a DataSet in a DataGroup. */
class DataGroup
{
public:
	/*! Constructor
	 *	@param name The name of the generic file to access.
	 *	@param dch The DataSetHeader of the DataSet to access.
	 *	@param handle A handle to the file mapping object
	 */
	DataGroup(const std::string& filename, const DataGroupHeader& dch, void* handle);

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
	/*! File mapping object handle */
	void* handle;
};

}

#endif	//_DataGroup_HEADER_
