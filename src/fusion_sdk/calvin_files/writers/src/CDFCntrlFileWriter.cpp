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
#include "calvin_files/writers/src/CDFCntrlFileWriter.h"
//

using namespace affymetrix_calvin_io;

CDFCntrlFileWriter::CDFCntrlFileWriter(CDFData &p) 
{
	writer = new GenericFileWriter(p.GetFileHeader());
	contentsPos = 0;
	WriteHeaders();
	dataGroupWriter = 0;

	//skip ahead to the position of the first probe set dataSet
	int32_t probes = p.GetProbeSetCnt();
	int32_t size = probes * ((MAX_CDF_PROBE_SET_NAME_LENGTH * 2) + 8); 
	writer->SeekFromCurrentPos(size);
	
	// Set the file position of the next DataGroup
	writer->GetDataGroupWriter(0).UpdateNextDataGroupPos();
}

CDFCntrlFileWriter::~CDFCntrlFileWriter() 
{
	delete writer;
	if(dataGroupWriter != 0) delete dataGroupWriter;
}

void CDFCntrlFileWriter::WriteHeaders()
{
	writer->WriteHeader();
	DataGroupWriter* dataGroupWriter = &writer->GetDataGroupWriter(0);
	dataGroupWriter->WriteHeader();
	contentsWriter = &dataGroupWriter->GetDataSetWriter(0);
	contentsWriter->WriteHeader();
	contentsPos = writer->GetFilePos();
}

CDFCntrlProbeSetWriter* CDFCntrlFileWriter::GetCntrlProbeSetWriter(const std::wstring& probeSetType)
{
	dataSetHdr.Clear();
	dataSetHdr.SetName(probeSetType);

	ParameterNameValueType t1;
	t1.SetName(QC_PROBESET_TYPE);
	t1.SetValueText(probeSetType);
	dataSetHdr.AddNameValParam(t1);

	dataSetHdr.AddUShortColumn(L"");
	dataSetHdr.AddUShortColumn(L"");
	dataSetHdr.AddUByteColumn(L"");
	dataSetHdr.AddUByteColumn(L"");
	dataSetHdr.AddUByteColumn(L"");

	return new CDFCntrlProbeSetWriter(dataGroupWriter->CreateDataSetWriter(dataSetHdr));
}

void CDFCntrlFileWriter::OpenDataGroup(const std::wstring& probeSetType, int dataSetCnt)
{
	DataGroupHeader dataGroup(probeSetType);
	for(int i = 0; i < dataSetCnt; i++)
	{
		DataSetHeader dsh;
		dataGroup.AddDataSetHdr(dsh);
	}
	dataGroupWriter = writer->CreateDataGroupWriter(dataGroup);

	WriteContentsEntry(probeSetType);

	dataGroupWriter->WriteHeader();
}

void CDFCntrlFileWriter::CloseDataGroup()
{
	dataGroupWriter->Close();
	delete dataGroupWriter;
	dataGroupWriter = 0;
}

void CDFCntrlFileWriter::WriteContentsEntry(const std::wstring& probeSetType)
{
	u_int32_t currentPos = writer->GetFilePos();
	writer->SeekFromBeginPos(contentsPos);
	contentsWriter->Write(probeSetType, 32);
	contentsWriter->Write(currentPos);
	contentsPos = writer->GetFilePos();
	writer->SeekFromBeginPos(currentPos);
}
