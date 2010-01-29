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
#include "calvin_files/writers/src/DataGroupWriter.h"
//
#include "calvin_files/writers/src/DataGroupHeaderWriter.h"
#include "calvin_files/writers/src/FileWriteException.h"
//

using namespace affymetrix_calvin_io;

DataGroupWriter::DataGroupWriter(std::ofstream* o, DataGroupHeader* p) 
{
	os = o;
	dataGroupHdr = p;
	CreateWriters();
}
DataGroupWriter::~DataGroupWriter() 
{ 
	writers.clear();
}

std::wstring DataGroupWriter::GetName() 
{ 
	return dataGroupHdr->GetName();
}

void DataGroupWriter::WriteHeader()
{
	dataGroupHdrWriter.Write(*os, *dataGroupHdr);
	u_int32_t currentPos = os->tellp();
	dataGroupHdrWriter.UpdateDataSetPos(*os, currentPos);
}

void DataGroupWriter::Close() const
{
	u_int32_t currentPos = os->tellp();
	dataGroupHdrWriter.UpdateNextDataGroupPos(*os, currentPos);
}

void DataGroupWriter::GetDataSetWriters(DataSetWriterIt &begin, DataSetWriterIt &end)
{
	begin = writers.begin();
	end = writers.end();
}

DataSetWriter& DataGroupWriter::GetDataSetWriter(int32_t index)
{
	return writers[index];
}

int32_t DataGroupWriter::GetDataSetWriterCnt() const
{
	return (int32_t)writers.size();
}

void DataGroupWriter::CreateWriters()
{
	int sz = dataGroupHdr->GetDataSetCnt();
	for(int i = 0; i < sz; i++)
	{
		DataSetWriter* p = CreateDataSetWriter(dataGroupHdr->GetDataSet(i));
		writers.push_back(*p);
		delete p;
	}
}

DataSetWriter* DataGroupWriter::CreateDataSetWriter(DataSetHeader& hdr)
{
	return new DataSetWriter(os, &hdr);
}

std::wstring DataGroupWriter::GetDataGroupName() const
{
	return dataGroupHdr->GetName();
}

void DataGroupWriter::UpdateNextDataGroupPos() const
{
	u_int32_t currentPos = os->tellp();
	dataGroupHdrWriter.UpdateNextDataGroupPos(*os, currentPos);
}
