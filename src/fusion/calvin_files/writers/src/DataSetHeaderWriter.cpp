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
#include "calvin_files/writers/src/DataSetHeaderWriter.h"
//
#include "calvin_files/writers/src/FileOutput.h"
//

using namespace affymetrix_calvin_io;

void DataSetHeaderWriter::Write(std::ofstream &os, DataSetHeader &dc)
{
	WriteDataOffset(os, 0);
	WriteNextDataSetOffset(os, 0);
	WriteName(os, dc);
	WriteNameValCnt(os, dc);
	WriteNameValParams(os, dc);
	WriteColumnCnt(os, dc);
	WriteColumnTypes(os, dc);
	WriteRowCnt(os, dc);
}

void DataSetHeaderWriter::WriteDataOffset(std::ofstream &os, u_int32_t pos)
{
	dataPos = os.tellp();
	FileOutput::WriteInt32(os, pos);
}

void DataSetHeaderWriter::UpdateDataOffset(std::ofstream &os, u_int32_t pos) const
{
	if(dataPos > 0) 
	{
		os.seekp(dataPos, std::ios::beg);
		FileOutput::WriteUInt32(os, pos);
		os.seekp(pos, std::ios::beg);
	}
}

void DataSetHeaderWriter::WriteNextDataSetOffset(std::ofstream &os, u_int32_t pos)
{
	nextDataSetPos = os.tellp();
	FileOutput::WriteInt32(os, pos);
}

void DataSetHeaderWriter::UpdateNextDataSetOffset(std::ofstream &os, u_int32_t pos) const
{
	if(nextDataSetPos > 0) 
	{
		os.seekp(nextDataSetPos, std::ios::beg);
		FileOutput::WriteUInt32(os, pos);
		os.seekp(pos, std::ios::beg);
	}
}

void DataSetHeaderWriter::WriteName(std::ofstream &os, const DataSetHeader &dc) const
{
	FileOutput::WriteString16(os, dc.GetName());
}

void DataSetHeaderWriter::WriteNameValCnt(std::ofstream &os, const DataSetHeader &dc) const
{
	FileOutput::WriteInt32(os, dc.GetNameValParamCnt());
}

void DataSetHeaderWriter::WriteNameValParams(std::ofstream &os, DataSetHeader &dc)
{
	ParameterNameValueTypeConstIt begin;
	ParameterNameValueTypeConstIt end;
	dc.GetNameValIterators(begin, end);
	while(begin != end)
	{
		FileOutput::WriteString16(os, begin->GetName());
		MIMEValue mv = begin->GetMIMEValue();
		u_int32_t sz;
		const void* ptr = mv.GetValue(sz);
		FileOutput::WriteBlob(os, ptr, sz);
		FileOutput::WriteString16(os, begin->GetMIMEType());
		begin++;
	}
}

void DataSetHeaderWriter::WriteColumnCnt(std::ofstream &os, const DataSetHeader &dc) const
{
	FileOutput::WriteInt32(os, dc.GetColumnCnt());
}

void DataSetHeaderWriter::WriteColumnTypes(std::ofstream &os, const DataSetHeader &dc) const
{
	int32_t sz = dc.GetColumnCnt();
	
	// Write the types
	for(int i = 0; i < sz; i++)
	{
		ColumnInfo col = dc.GetColumnInfo(i);
		FileOutput::WriteString16(os, col.GetName());
		FileOutput::WriteInt8(os, col.GetColumnType());
		FileOutput::WriteInt32(os, col.GetSize());
	}
}

void DataSetHeaderWriter::WriteRowCnt(std::ofstream &os, const DataSetHeader &dc) const
{
	FileOutput::WriteInt32(os, dc.GetRowCnt());
}

