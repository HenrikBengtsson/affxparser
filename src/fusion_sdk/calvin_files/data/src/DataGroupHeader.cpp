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


#include "calvin_files/data/src/DataGroupHeader.h"
//
#include "calvin_files/data/src/FileHeader.h"
//

using namespace affymetrix_calvin_io;

DataGroupHeader::DataGroupHeader() 
{
	dataSetPos = 0;
	nextGrpPos = 0;
}
DataGroupHeader::DataGroupHeader(const std::wstring &n) 
{
	name = n;
	dataSetPos = 0;
	nextGrpPos = 0;
	headerStartFilePos = 0;
}

DataGroupHeader::~DataGroupHeader() 
{
	Clear(); 
}

void DataGroupHeader::Clear()
{
	name.clear();
	dataSetPos = 0;
	nextGrpPos = 0;
	dataSetHdrs.clear();
}

void DataGroupHeader::SetName(const std::wstring &p)
{
	name = p;
}

std::wstring DataGroupHeader::GetName() const
{
	return name;
}

int32_t DataGroupHeader::GetDataSetCnt() const
{
	return (int32_t)dataSetHdrs.size();
}

void DataGroupHeader::AddDataSetHdr(const DataSetHeader &p)
{
	dataSetHdrs.push_back(p);
}

void DataGroupHeader::ReplaceDataSetHdr(const DataSetHeader &p)
{
	for(int i = 0 ; i < dataSetHdrs.size(); i++)
	{
		if(dataSetHdrs[i].GetName() == p.GetName())
		{
			dataSetHdrs[i] = p;
			return;
		}
	}
}

DataSetHeader& DataGroupHeader::GetDataSet(int32_t index)
{
	return dataSetHdrs[index];
}

const DataSetHeader& DataGroupHeader::GetDataSetConst(int32_t index) const
{
	return dataSetHdrs[index];
}

void DataGroupHeader::GetDataSetIterators(DataSetHdrIt &begin, DataSetHdrIt &end)
{
	begin = dataSetHdrs.begin();
	end = dataSetHdrs.end();
}

/*
 * Find a DataSetHeader given DataSet name
 */
DataSetHeader* DataGroupHeader::FindDataSetHeader(const std::wstring& dataSetName)
{
	DataSetHeader* dph = 0;
	DataSetHdrIt begin;
	DataSetHdrIt end;
	GetDataSetIterators(begin,end);
	for (DataSetHdrIt ii=begin; ii!=end; ++ii)
	{
		if (ii->GetName() == dataSetName)
		{
			dph = &(*ii);
			break;
		}
	}
	return dph;
}
