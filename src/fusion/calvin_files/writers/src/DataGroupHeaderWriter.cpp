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
#include "calvin_files/writers/src/DataGroupHeaderWriter.h"
//
#include "calvin_files/writers/src/FileOutput.h"
//

using namespace affymetrix_calvin_io;

DataGroupHeaderWriter::DataGroupHeaderWriter()
{
	dataSetPos = 0; 
	nextDataGroupPos = 0;
}

void DataGroupHeaderWriter::Write(std::ofstream &os, const DataGroupHeader &dc)
{
	WriteNextDataGroupPos(os, 0);
	WriteDataSetPos(os, 0);
	WriteDataSetCnt(os, dc);
	WriteName(os, dc);
}

void DataGroupHeaderWriter::WriteDataSetPos(std::ofstream &os, u_int32_t pos)
{
	dataSetPos = os.tellp();
	FileOutput::WriteInt32(os, pos);
}

void DataGroupHeaderWriter::UpdateDataSetPos(std::ofstream &os, u_int32_t pos) const
{
	if(dataSetPos > 0) 
	{
		os.seekp(dataSetPos, std::ios::beg);
		FileOutput::WriteUInt32(os, pos);
		os.seekp(pos, std::ios::beg);
	}
}

void DataGroupHeaderWriter::WriteNextDataGroupPos(std::ofstream &os, u_int32_t pos)
{
	nextDataGroupPos = os.tellp();
	FileOutput::WriteInt32(os, pos);
}

void DataGroupHeaderWriter::UpdateNextDataGroupPos(std::ofstream &os, u_int32_t pos) const
{
	if(nextDataGroupPos > 0) 
	{
		os.seekp(nextDataGroupPos, std::ios::beg);
		FileOutput::WriteUInt32(os, pos);
		os.seekp(pos, std::ios::beg);
	}
}

void DataGroupHeaderWriter::WriteName(std::ofstream &os, const DataGroupHeader &dc) const
{
	FileOutput::WriteString16(os, dc.GetName());
}

void DataGroupHeaderWriter::WriteDataSetCnt(std::ofstream &os, const DataGroupHeader &dc) const
{
	FileOutput::WriteInt32(os, dc.GetDataSetCnt());
}
