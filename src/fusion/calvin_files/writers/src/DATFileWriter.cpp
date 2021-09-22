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
#include "calvin_files/writers/src/DATFileWriter.h"
//
#include "calvin_files/data/src/DATData.h"
//

using namespace affymetrix_calvin_io;

DATFileWriter::DATFileWriter(DATData &p) 
{ 
	writer = new GenericFileWriter(p.GetFileHeader());
	pixelPos = 0;
	statsPos = 0;
	WriteHeaders();
}

DATFileWriter::~DATFileWriter() 
{
	delete writer;
}

void DATFileWriter::WriteHeaders()
{
	writer->WriteHeader();
	DataGroupWriter* dataGroupWriter = &writer->GetDataGroupWriter(0);
	dataGroupWriter->WriteHeader();

	DataSetWriterIt begin;
	DataSetWriterIt end;
	dataGroupWriter->GetDataSetWriters(begin, end);
	while(begin != end)
	{
		dataSetWriter = &(*begin);
		begin->WriteHeader();
		SetFilePositions();
		begin++;
	}
}

void DATFileWriter::WriteStats(const Uint16Vector &v)
{
	writer->SeekFromBeginPos(statsPos);
	Uint16VectorConstIt begin = v.begin();
	Uint16VectorConstIt end = v.end();
	while(begin != end)
	{
		dataSetWriter->Write(*begin);
		begin++;
	}
	statsPos = writer->GetFilePos();
}

void DATFileWriter::WritePixels(const Uint16Vector &v)
{
	writer->SeekFromBeginPos(pixelPos);
	Uint16VectorConstIt begin = v.begin();
	Uint16VectorConstIt end = v.end();
	while(begin != end)
	{
		dataSetWriter->Write(*begin);
		begin++;
	}
	pixelPos = writer->GetFilePos();
}

void DATFileWriter::SetFilePositions()
{
	const std::wstring &name = dataSetWriter->GetDataSetName();
	int32_t dataSetSz = dataSetWriter->GetDataSetSize();
	
	if(name == DAT_PIXEL)
	{
		pixelPos = writer->GetFilePos();
		pixelLimitPos = pixelPos + dataSetSz;
	}
	else
	{
		statsPos = writer->GetFilePos();
		statsLimitPos = statsPos + dataSetSz;
	}

	writer->SeekFromCurrentPos(dataSetSz + 1);
	dataSetWriter->UpdateNextDataSetOffset();
}
