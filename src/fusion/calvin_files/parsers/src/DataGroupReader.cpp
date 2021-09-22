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


#include "calvin_files/parsers/src/DataGroupReader.h"
//
#include "calvin_files/data/src/DataException.h"
#include "calvin_files/parsers/src/DataSetReader.h"
#include "calvin_files/parsers/src/FileException.h"
//

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_exceptions;

DataGroupReader::DataGroupReader(std::ifstream& is, DataGroupHeader& hdr)
: fileStream(is), dataGroupHdr(hdr)
{
}

DataGroupReader::~DataGroupReader()
{
}

/*
 * Get the number of DataSets in the DataGroup
 */
int32_t DataGroupReader::GetDataSetCnt() const
{
	return dataGroupHdr.GetDataSetCnt();
}

/*
 * Get the name of the DataGroup
 */
std::wstring DataGroupReader::GetDataGroupName() const
{
	return dataGroupHdr.GetName();
}

/*
 * Get the DataSetReader by index
 */
DataSetReader DataGroupReader::GetDataSetReader(int32_t index)
{
	if (index < 0 || index > dataGroupHdr.GetDataSetCnt())
	{
          DataSetNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	DataSetHeader& dph = dataGroupHdr.GetDataSet(index);
	DataSetReader dpReader(fileStream, dph);
	return dpReader;
}

/*
 * Get the DataSetReader by name
 */
DataSetReader DataGroupReader::GetDataSetReader(const std::wstring& name)
{
	DataSetHeader* dph = dataGroupHdr.FindDataSetHeader(name);
	if (dph == 0)
	{
		DataSetNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	DataSetReader dpReader(fileStream, *dph);
	return dpReader;
}

