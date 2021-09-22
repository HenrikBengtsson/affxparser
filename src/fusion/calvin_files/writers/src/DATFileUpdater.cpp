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


#include "calvin_files/writers/src/DATFileUpdater.h"
//

using namespace affymetrix_calvin_io;

// Constant column names.
static const std::wstring GridStatusColName = L"GridStatus";
static const int32_t NumGridCols = 8;
static const std::wstring GridColNames[NumGridCols] = {L"Upper left x", L"Upper left y", L"Upper right x", L"Upper right y", L"Lower right x", L"Lower right y", L"Lower left x", L"Lower left y" };


DATFileUpdater::DATFileUpdater(DATData &p) 
{ 
	updateData = &p;
	currentData = new DATData();
	ReadCurrentData();
	writer = new GenericFileWriter(currentData->GetFileHeader(), false);
}

DATFileUpdater::~DATFileUpdater() 
{
	delete currentData;
	delete writer;
}

void DATFileUpdater::Update()
{
	// determine whether to add grid or subgrid info (or both)
	UpdateFileHeader();
	if(updateData->HasGlobalGridData())
	{
		GenericData* gData = &currentData->GetGenericData();
		u_int32_t gridDataSetOffset = GetGridDataSetOffset(gData->Header());
		DataSetHeader gridHdr = CreateGridDataSetHeader();
		WriteGridData(gridHdr, gridDataSetOffset);
		if(updateData->GetSubgridCnt() > 0)
		{
			u_int32_t subGridDataSetOffset = gridHdr.GetNextSetFilePos();
			DataSetHeader subGridHdr = CreateSubGridDataSetHeader();
			WriteSubGridData(subGridHdr, subGridDataSetOffset);
			UpdateDataSetCnt(gData->Header(), 4);
		}
		else
		{
			UpdateDataSetCnt(gData->Header(), 3);
		}
	}
	else if(updateData->GetSubgridCnt() > 0)
	{
		GenericData* gData = &currentData->GetGenericData();
		u_int32_t subGridDataSetOffset = GetSubGridDataSetOffset(gData->Header());
		if(subGridDataSetOffset > 0)
		{
			DataSetHeader subGridHdr = CreateSubGridDataSetHeader();
			WriteSubGridData(subGridHdr, subGridDataSetOffset);
			UpdateDataSetCnt(gData->Header(), 4);
		}
		else
		{
			// no global grid information was found in the file
			// global grid information has to be in the DATData object or in the file
		}
	}
}

void DATFileUpdater::UpdateDataSetCnt(FileHeader fh, u_int32_t cnt) const
{
	u_int32_t dataGrpOffset = fh.GetFirstDataGroupFilePos();
	writer->SeekFromBeginPos(dataGrpOffset + 8);
	writer->Write(cnt);
}

u_int32_t DATFileUpdater::GetGridDataSetOffset(FileHeader fh) const
{
	u_int32_t result = 0;
	if(fh.GetDataGroupCnt() > 0)
	{
		DataGroupHeader grpHdr = fh.GetDataGroup(0);
		if(grpHdr.GetDataSetCnt() > 1)
		{
			DataSetHeader dsHdr = grpHdr.GetDataSet(1);
			result = dsHdr.GetNextSetFilePos();
		}
	}
	return result;
}

u_int32_t DATFileUpdater::GetSubGridDataSetOffset(FileHeader fh) const
{
	u_int32_t result = 0;
	if(fh.GetDataGroupCnt() > 0)
	{
		DataGroupHeader grpHdr = fh.GetDataGroup(0);
		if(grpHdr.GetDataSetCnt() > 2)
		{
			DataSetHeader dsHdr = grpHdr.GetDataSet(2);
			result = dsHdr.GetNextSetFilePos();
		}
	}
	return result;
}

void DATFileUpdater::WriteGridData(DataSetHeader& gridHdr, u_int32_t offset) const 
{
	writer->SeekFromBeginPos(offset);
	DataSetWriter* dsWriter = writer->GetDataGroupWriter(0).CreateDataSetWriter(gridHdr);
	dsWriter->WriteHeader();
	dsWriter->Write(updateData->GetGlobalGridStatus());
	FPointVector points = updateData->GetGlobalGrid().pts;
	for(u_int32_t i = 0; i < points.size(); i++)
	{
		dsWriter->Write(points[i].x);
		dsWriter->Write(points[i].y);
	}
	dsWriter->UpdateNextDataSetOffset();
	delete dsWriter;
}

void DATFileUpdater::WriteSubGridData(DataSetHeader& subGridHdr, u_int32_t offset) const
{
	writer->SeekFromBeginPos(offset);
	DataSetWriter* dsWriter = writer->GetDataGroupWriter(0).CreateDataSetWriter(subGridHdr);
	dsWriter->WriteHeader();
	u_int32_t cnt = updateData->GetSubgridCnt();
	for(u_int32_t n = 0; n < cnt; n++)
	{
		dsWriter->Write(updateData->GetSubgridStatus(n));
		FPointVector points = updateData->GetSubgrid(n).pts;
		for(u_int32_t i = 0; i < points.size(); i++)
		{
			dsWriter->Write(points[i].x);
			dsWriter->Write(points[i].y);
		}
	}
	dsWriter->UpdateNextDataSetOffset();
	delete dsWriter;
}

DataSetHeader DATFileUpdater::CreateGridDataSetHeader() const 
{
	DataSetHeader result;
	AddGridDataSetHeaderParameters(result);
	result.SetRowCnt(1);
	result.SetName(DAT_GLOBAL_GRID);
	result.AddUIntColumn(GridStatusColName);
	for(int i = 0; i < NumGridCols; i++)
	{
		result.AddFloatColumn(GridColNames[i]);
	}
	return result;
}

DataSetHeader DATFileUpdater::CreateSubGridDataSetHeader() const 
{
	DataSetHeader result;
	AddGridDataSetHeaderParameters(result);
	result.SetRowCnt(updateData->GetSubgridCnt());
	result.SetName(DAT_SUBGRID);
	result.AddUIntColumn(GridStatusColName);
	for(int i = 0; i < NumGridCols; i++)
	{
		result.AddFloatColumn(GridColNames[i]);
	}
	return result;
}

void DATFileUpdater::ReadCurrentData() const
{
	DATFileReader reader;
	currentData->SetFilename(updateData->GetFilename());
	reader.Read(*currentData);
}

void DATFileUpdater::UpdateFileHeader()
{
	// Set the updateData fileId to a new value.
	updateData->GetFileHeader()->GetGenericDataHdr()->SetFileId(AffymetrixGuid::GenerateNewGuid());

	// Update the FileHeader
	FileHeaderUpdater updater;
	updater.Update( writer->GetFileOStream(), *updateData->GetFileHeader(), *currentData->GetFileHeader());
}

void DATFileUpdater::AddGridDataSetHeaderParameters(DataSetHeader& hdr) const
{
	ParameterNameValueTypeVector params;
	updateData->GetGridAlignmentAlgorithmParameters(params);

	for (ParameterNameValueTypeConstIt ii = params.begin(); ii != params.end(); ++ii)
	{
		hdr.AddNameValParam(*ii);
	}
}
