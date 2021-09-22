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


#include "calvin_files/writers/src/GenericFileUpdater.h"
//

using namespace affymetrix_calvin_io;

GenericFileUpdater::GenericFileUpdater(const std::string& path) 
{ 
	fileHdr = ReadFileHeader(path);
	writer = new GenericFileWriter(&fileHdr, false);
	lastDataGrpOffset = FindLastDataGroupOffset();
	lastDataGrpDataSetCnt = GetLastDataGroupDataSetCnt(); 
}

GenericFileUpdater::~GenericFileUpdater() 
{
	delete writer;
}

DataSetWriter* GenericFileUpdater::GetUpdateDataSetWriter(DataSetHeader& dataSetHdr)
{
	DataSetHeader lastDataSetHdr;
	if(FindLastDataSetHeader(fileHdr, lastDataSetHdr))
	{
		u_int32_t offset = 0;
		if(dataSetHdr.GetName().compare(lastDataSetHdr.GetName()) == 0)
		{
			//overwrite last dataset
			offset = lastDataSetHdr.GetHeaderStartFilePos();
		}
		else
		{
			//append dataset
			offset = lastDataSetHdr.GetNextSetFilePos();

			//increment data set count
			IncrementDataSetCnt();

		}
		u_int32_t lastDataGrpIndex = fileHdr.GetDataGroupCnt() - 1;
		writer->SeekFromBeginPos(offset);
		return writer->GetDataGroupWriter(lastDataGrpIndex).CreateDataSetWriter(dataSetHdr);
	}
	return 0;
}

//DataSetWriter* GenericFileUpdater::WriteDataSet(GenericFileWriter& writer, u_int32_t grpIndex,  u_int32_t offset, DataSetHeader& dataSetHdr, Uint8Vector& data) const
//{
//	writer.SeekFromBeginPos(offset);
//	DataSetWriter* dsWriter = writer.GetDataGroupWriter(grpIndex).CreateDataSetWriter(dataSetHdr);
//	WriteDataSet(dsWriter, dataSetHdr, data);
//	delete dsWriter;
//}

//void GenericFileUpdater::WriteDataSet(DataSetWriter* writer, DataSetHeader& dataSetHdr, std::string& data) const
//{
//	writer->WriteHeader();
//	for(size_t i = 0; i = data.size(); i++)
//	{
//		writer->Write(data[i]);
//	}
//	writer->UpdateNextDataSetOffset();
//}

FileHeader GenericFileUpdater::ReadFileHeader(const std::string& path) const
{
	GenericData gData;
	GenericFileReader reader;
	reader.SetFilename(path);
	reader.ReadHeader(gData);
	gData.Close();
	return gData.Header();
}

bool GenericFileUpdater::FindLastDataSetHeader(FileHeader& fileHdr, DataSetHeader& dataSetHdr) const
{
	int32_t index = fileHdr.GetDataGroupCnt();
	if(index > 0)
	{
		DataGroupHeader dataGrpHdr = fileHdr.GetDataGroup(index - 1);
		index = dataGrpHdr.GetDataSetCnt();
		if(index > 0)
		{
			dataSetHdr = dataGrpHdr.GetDataSet(index - 1);
			return true;
		}

	}
	return false;
}

void GenericFileUpdater::IncrementDataSetCnt()
{
	lastDataGrpDataSetCnt++;
	u_int32_t currentPos = writer->GetFilePos();
	writer->SeekFromBeginPos(lastDataGrpOffset + 8);
	writer->Write(lastDataGrpDataSetCnt);
	writer->SeekFromBeginPos(currentPos);
}

u_int32_t GenericFileUpdater::GetLastDataGroupDataSetCnt()
{
	DataGroupHdrIt begin;
	DataGroupHdrIt end;
	fileHdr.GetDataGroupIts(begin, end);
	while(begin != end)
	{
		if(begin->GetNextGroupPos() == 0)
		{
			return begin->GetDataSetCnt();
		}
		begin++;
	}
	return 0;
}

u_int32_t GenericFileUpdater::FindLastDataGroupOffset()
{
	DataGroupHdrIt begin;
	DataGroupHdrIt end;
	fileHdr.GetDataGroupIts(begin, end);
	u_int32_t currentOffset = fileHdr.GetFirstDataGroupFilePos();
	while(begin != end)
	{
		if(begin->GetNextGroupPos() == 0)
		{
			break;
		}
		else
		{
			currentOffset = begin->GetNextGroupPos();
		}
		begin++;
	}
	return currentOffset;
}
