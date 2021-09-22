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
#include "calvin_files/writers/src/CDFFileWriter.h"
//
#include "calvin_files/data/src/CDFData.h"
//

using namespace affymetrix_calvin_io;

CDFFileWriter::CDFFileWriter(CDFData &p) 
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

CDFFileWriter::~CDFFileWriter() 
{
	delete writer;
	if(dataGroupWriter != 0) delete dataGroupWriter;
}

void CDFFileWriter::WriteHeaders()
{
	writer->WriteHeader();
	DataGroupWriter* dataGroupWriter = &writer->GetDataGroupWriter(0);
	dataGroupWriter->WriteHeader();
	contentsWriter = &dataGroupWriter->GetDataSetWriter(0);
	contentsWriter->WriteHeader();
	contentsPos = writer->GetFilePos();
}

CDFProbeSetWriter* CDFFileWriter::CreateProbeSetWriter(const std::wstring& xdaBlockName,
																											u_int8_t unitType,
																											u_int8_t direction,
																											u_int32_t atoms,
																											u_int32_t cells,
																											u_int32_t probeSetNumber,
																											u_int8_t cellsPerAtom)
{
	//create data dataSet writer
	probeSetPlaneHdr.Clear();
	probeSetPlaneHdr.SetName(xdaBlockName);

	ParameterNameValueType t1;
	t1.SetName(CDF_UNIT_TYPE);
	t1.SetValueUInt8(unitType);
	probeSetPlaneHdr.AddNameValParam(t1);

	ParameterNameValueType t2;
	t2.SetName(CDF_DIRECTION);
	t2.SetValueUInt8(direction);
	probeSetPlaneHdr.AddNameValParam(t2);

	ParameterNameValueType t3;
	t3.SetName(CDF_ATOMS);
	t3.SetValueUInt32(atoms);
	probeSetPlaneHdr.AddNameValParam(t3);

	ParameterNameValueType t4;
	t4.SetName(CDF_CELLS);
	t4.SetValueUInt32(cells);
	probeSetPlaneHdr.AddNameValParam(t4);

	ParameterNameValueType t5;
	t5.SetName(CDF_PROBE_SET_NUMBER);
	t5.SetValueUInt32(probeSetNumber);
	probeSetPlaneHdr.AddNameValParam(t5);

	ParameterNameValueType t6;
	t6.SetName(CDF_CELLS_PER_ATOM);
	t6.SetValueUInt8(cellsPerAtom);
	probeSetPlaneHdr.AddNameValParam(t6);

	probeSetPlaneHdr.AddUShortColumn(L"");
	probeSetPlaneHdr.AddUShortColumn(L"");
	probeSetPlaneHdr.AddUIntColumn(L"");
	probeSetPlaneHdr.AddUIntColumn(L"");
	probeSetPlaneHdr.AddByteColumn(L"");
	probeSetPlaneHdr.AddByteColumn(L"");

	// Set the number of rows
	// cells should be the same as atoms*cellPerAtom
	probeSetPlaneHdr.SetRowCnt(cells);	

	return new CDFProbeSetWriter(dataGroupWriter->CreateDataSetWriter(probeSetPlaneHdr));
}

void CDFFileWriter::OpenDataGroup(const std::wstring& probeSetName, int dataSetCnt)
{
	DataGroupHeader dataGroup(probeSetName);
	for(int i = 0; i < dataSetCnt; i++)
	{
		DataSetHeader dsh;
		dataGroup.AddDataSetHdr(dsh);
	}
	dataGroupWriter = writer->CreateDataGroupWriter(dataGroup);
	WriteContentsEntry(probeSetName);
	dataGroupWriter->WriteHeader();
}

void CDFFileWriter::CloseDataGroup()
{
	dataGroupWriter->Close();
	delete dataGroupWriter;
	dataGroupWriter = 0;
}

void CDFFileWriter::WriteContentsEntry(const std::wstring& probeSetName)
{
	u_int32_t currentPos = writer->GetFilePos();
	writer->SeekFromBeginPos(contentsPos);
	contentsWriter->Write(probeSetName, MAX_CDF_PROBE_SET_NAME_LENGTH);
	contentsWriter->Write(currentPos);
	contentsPos = writer->GetFilePos();
	writer->SeekFromBeginPos(currentPos);
}
