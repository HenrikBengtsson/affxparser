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
#include "calvin_files/writers/src/CalvinCHPTilingFileWriter.h"
//
#include "calvin_files/data/src/CHPTilingData.h"
//

using namespace affymetrix_calvin_io;

CHPTilingFileWriter::CHPTilingFileWriter(CHPTilingData &p)
{
	dataSetIndex= -1;
	entryPos = new int32_t[p.GetNumberSequences()];
	writer = new GenericFileWriter(p.GetFileHeader());
	WriteHeaders();
}

CHPTilingFileWriter::~CHPTilingFileWriter()
{
	delete [] entryPos;
	delete writer;
}

void CHPTilingFileWriter::WriteHeaders()
{
	writer->WriteHeader();
	DataGroupWriter &dataGroupWriter = writer->GetDataGroupWriter(0);
	dataGroupWriter.WriteHeader();

	int iSet=0;
	DataSetWriterIt beginSet;
	DataSetWriterIt endSet;
	dataGroupWriter.GetDataSetWriters(beginSet, endSet);
	while(beginSet != endSet)
	{
		dataSetWriter = &(*beginSet);
		beginSet->WriteHeader();
		entryPos[iSet++] = SetFilePositions();
		beginSet++;
	}
	dataGroupWriter.UpdateNextDataGroupPos();
}

void CHPTilingFileWriter::SeekToDataSet(int index)
{
	DataGroupWriter &dataGroupWriter = writer->GetDataGroupWriter(0);
	dataSetWriter = &dataGroupWriter.GetDataSetWriter(index);
	writer->SeekFromBeginPos(entryPos[index]);
	dataSetIndex = index;
}

void CHPTilingFileWriter::WriteTilingEntry(const CHPTilingEntry& p)
{
	dataSetWriter->Write(p.position);
	dataSetWriter->Write(p.value);
}

int32_t CHPTilingFileWriter::SetFilePositions()
{
	int32_t dataSetSz = dataSetWriter->GetDataSetSize();
	int32_t offset = writer->GetFilePos();
	writer->SeekFromCurrentPos(dataSetSz + 1);
	dataSetWriter->UpdateNextDataSetOffset();
	return offset;
}
