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

#include "DataGroup.h"
#include "GenericData.h"

using namespace affymetrix_calvin_io;

#ifndef WIN32
#include <sys/mman.h>
#endif

/*
 * Initialize the class.
 */
DataGroup::DataGroup(const std::string& filename_, const DataGroupHeader& dch, void* handle_)
: filename(filename_), dataGroupHeader(dch), handle(handle_)
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
		affymetrix_calvin_io::DataSet* dp = new affymetrix_calvin_io::DataSet(filename, *dph, handle);
		return dp;
	}
	else
	{
		affymetrix_calvin_exceptions::DataSetNotFoundException e;
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
		affymetrix_calvin_io::DataSet* dp = new affymetrix_calvin_io::DataSet(filename, *dph, handle);
		return dp;
	}
	else
	{
		affymetrix_calvin_exceptions::DataSetNotFoundException e;
		throw e;
	}
}