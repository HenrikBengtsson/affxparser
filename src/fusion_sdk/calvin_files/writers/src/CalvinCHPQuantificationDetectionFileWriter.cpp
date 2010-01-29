////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006 Affymetrix, Inc.
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


#include "calvin_files/writers/src/CalvinCHPQuantificationDetectionFileWriter.h"
//
#include "calvin_files/data/src/CHPQuantificationDetectionData.h"
//

using namespace affymetrix_calvin_io;

CHPQuantificationDetectionFileWriter::CHPQuantificationDetectionFileWriter(CHPQuantificationDetectionData &p)
{
    maxProbeSetName = p.GetMaxProbeSetName();
	writer = new GenericFileWriter(p.GetFileHeader());
	WriteHeaders();
}

CHPQuantificationDetectionFileWriter::~CHPQuantificationDetectionFileWriter()
{
	delete writer;
}

void CHPQuantificationDetectionFileWriter::WriteHeaders()
{
	writer->WriteHeader();
	DataGroupWriter &dataGroupWriter = writer->GetDataGroupWriter(0);
	dataGroupWriter.WriteHeader();

	//int iSet=0;
	DataSetWriterIt beginSet;
	DataSetWriterIt endSet;
	dataGroupWriter.GetDataSetWriters(beginSet, endSet);
	if (beginSet != endSet)
	{
		dataSetWriter = &(*beginSet);
		beginSet->WriteHeader();
		entryPos = SetFilePositions();
		dataGroupWriter.UpdateNextDataGroupPos();
	}
}

void CHPQuantificationDetectionFileWriter::SeekToDataSet()
{
	DataGroupWriter &dataGroupWriter = writer->GetDataGroupWriter(0);
	dataSetWriter = &dataGroupWriter.GetDataSetWriter(0);
	writer->SeekFromBeginPos(entryPos);
}

void CHPQuantificationDetectionFileWriter::WriteEntry(const affymetrix_calvin_data::ProbeSetQuantificationDetectionData& p)
{
    if (maxProbeSetName == -1)
        dataSetWriter->Write(p.id);
    else
        dataSetWriter->Write(p.name, maxProbeSetName);
	dataSetWriter->Write(p.quantification);
    dataSetWriter->Write(p.pvalue);
}

int32_t CHPQuantificationDetectionFileWriter::SetFilePositions()
{
	int32_t dataSetSz = dataSetWriter->GetDataSetSize();
	int32_t offset = writer->GetFilePos();
	writer->SeekFromCurrentPos(dataSetSz + 1);
	dataSetWriter->UpdateNextDataSetOffset();
	return offset;
}
