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


#include "calvin_files/data/src/DataGroup.h"
//
#include "calvin_files/data/src/GenericData.h"
//

using namespace affymetrix_calvin_io;

#ifndef _MSC_VER
#include <sys/mman.h>
#endif

/*
 * Initialize the object to use memory-mapping to access the file.
 */
DataGroup::DataGroup(const std::string& filename_, const DataGroupHeader& dch, void* handle_, bool loadEntireDataSetHint_)
: filename(filename_), dataGroupHeader(dch), useMemoryMapping(true), handle(handle_), fileStream(0), loadEntireDataSetHint(loadEntireDataSetHint_)
{
}

/*
 * Initialize the object to use std::ifstream to access 
 */
DataGroup::DataGroup(const std::string& filename_, const DataGroupHeader& dch, std::ifstream& ifs, bool loadEntireDataSetHint_)
: filename(filename_), dataGroupHeader(dch), useMemoryMapping(false), handle(0), fileStream(&ifs), loadEntireDataSetHint(loadEntireDataSetHint_)
{
}

/*! Returns a pointer to the DataSet object by DataSet index.
	*  Each call will return a new DataSet object.
	*  The caller should call Delete when finished with the DataSet.
	*
	*	@param dataSetIdx The index of the DataSet to return.
	*  @return DataSet
	*	@exception affymetrix_calvin_exceptions::DataSetNotFoundException DataSet not found.
	*/
affymetrix_calvin_io::DataSet* DataGroup::DataSet(u_int32_t dataSetIdx)
{
	DataSetHeader* dph = GenericData::FindDataSetHeader(&dataGroupHeader, dataSetIdx);
	if (dph)
	{
		if (useMemoryMapping)
			return new affymetrix_calvin_io::DataSet(filename, *dph, handle, loadEntireDataSetHint);
		else
			return new affymetrix_calvin_io::DataSet(filename, *dph, *fileStream, loadEntireDataSetHint);
	}
	else
	{
		affymetrix_calvin_exceptions::DataSetNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

/*! Returns a pointer to the DataSet object by DataSet name.
	*  Each call will return a new DataSet object.
	*  The caller should call Delete when finished with the DataSet.
	*
	*	@param dataSetName The name of the DataSet to return.
	*  @return DataSet
	*	@exception affymetrix_calvin_exceptions::DataSetNotFoundException DataSet not found.
	*/
affymetrix_calvin_io::DataSet* DataGroup::DataSet(const std::wstring& dataSetName)
{
	DataSetHeader* dph = GenericData::FindDataSetHeader(&dataGroupHeader, dataSetName);
	if (dph)
	{
		if (useMemoryMapping)
			return new affymetrix_calvin_io::DataSet(filename, *dph, handle, loadEntireDataSetHint);
		else
			return new affymetrix_calvin_io::DataSet(filename, *dph, *fileStream, loadEntireDataSetHint);
	}
	else
	{
		affymetrix_calvin_exceptions::DataSetNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}
