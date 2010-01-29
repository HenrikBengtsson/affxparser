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


#include "calvin_files/writers/src/FileHeaderUpdater.h"
//
#include "calvin_files/writers/src/GenericDataHeaderUpdater.h"
//

using namespace affymetrix_calvin_io;

/*
 * Constructor
 */
FileHeaderUpdater::FileHeaderUpdater()
{
	os = 0;
	fileHdr = 0;
}

FileHeaderUpdater::FileHeaderUpdater(std::ofstream& ostrm, FileHeader& hdr)
{
	os = &ostrm;
	fileHdr = &hdr;
}

/*
 * Destructor
 */
FileHeaderUpdater::~FileHeaderUpdater()
{
}

/*
 * Update the file with new FileHeader information
 */
bool FileHeaderUpdater::Update(std::ofstream& ostrm, FileHeader& updateHdr, FileHeader& currentHdr)
{
	// There is nothing at this time to update in the FileHeader
	// Move on to the GenericDataHeader

	GenericDataHeaderUpdater genUpdater;
	return genUpdater.Update(ostrm, *updateHdr.GetGenericDataHdr(), *currentHdr.GetGenericDataHdr());
}

//void FileHeaderUpdater::UpdateVersion(int8_t version)
//{
//	u_int32_t pos = os->tellp();
//	SeekVersionPosition();
//	FileOutput::WriteInt8(*os, version);
//	os->seekp(pos, std::ios::beg);
//}
//
//void FileHeaderUpdater::UpdateMagicNumber(int8_t magic)
//{
//	u_int32_t pos = os->tellp();
//	os->seekp(0, std::ios::beg);
//	FileOutput::WriteInt8(*os, magic);
//	os->seekp(pos, std::ios::beg);
//}

void FileHeaderUpdater::UpdateDataGroupCount(int32_t count)
{
	u_int32_t pos = os->tellp();
	SeekDataGrpCntPosition();
	FileOutput::WriteUInt32(*os, count);
	os->seekp(pos, std::ios::beg);
}

void FileHeaderUpdater::SeekVersionPosition()
{
	os->seekp(0, std::ios::beg);
	int32_t pos = os->tellp();
	pos += sizeof(int8_t); // magic number position
	os->seekp(pos, std::ios::beg);
}

void FileHeaderUpdater::SeekDataGrpCntPosition()
{
	os->seekp(0, std::ios::beg);
	int32_t pos = os->tellp();
	pos += sizeof(int8_t); // magic number position
	pos += sizeof(int8_t); // version position
	os->seekp(pos, std::ios::beg);
}

void FileHeaderUpdater::SeekLeadDataGrpPosition()
{
	os->seekp(0, std::ios::beg);
	int32_t pos = os->tellp();
	pos += sizeof(int8_t); // magic number position
	pos += sizeof(int8_t); // version position
	pos += sizeof(u_int32_t); // data group count position
	os->seekp(pos, std::ios::beg);
}

void FileHeaderUpdater::UpdateLeadDataGroupOffset(u_int32_t offset)
{
	u_int32_t pos = os->tellp();
	SeekLeadDataGrpPosition();
	FileOutput::WriteUInt32(*os, offset);
	os->seekp(pos, std::ios::beg);
}

void FileHeaderUpdater::AppendDataGroupHeader(DataGroupHeader& grpHdr)
{
	os->seekp(0, std::ios::end);
	int sz = fileHdr->GetDataGroupCnt();
	u_int32_t grpPos = os->tellp();
	grpHdr.SetHeaderStartFilePos(grpPos);
	if(sz > 0)
	{
		//update previous data group with new position
		DataGroupHeader lastHdr = fileHdr->GetDataGroup(sz - 1);
		DataGroupHeaderUpdater lastGrpUpdater(*os, lastHdr);
		lastGrpUpdater.UpdateNextDataGroupOffset(grpPos);
	}
	else
	{
		//this will be the first data group
		UpdateLeadDataGroupOffset(grpPos);
	}
	//increment the number of data sets in the group
	UpdateDataGroupCount(sz + 1);
	DataGroupWriter grpWriter(os, &grpHdr);
	grpWriter.WriteHeader();
	grpHdr.SetDataSetPos(os->tellp());
	DataSetWriterIt begin, end;
	grpWriter.GetDataSetWriters(begin, end);
	while(begin != end)
	{
		begin->WriteHeader();
		os->seekp(begin->GetDataSetSize(), std::ios::cur);
		begin->UpdateNextDataSetOffset();
		begin++;
	}

	//keep the next offset at zero since this will be the last data group and it complies with the file spec
	//grpWriter.UpdateNextDataGroupPos();
	//grpHdr.SetNextGroupPos(os->tellp());

	fileHdr->AddDataGroupHdr(grpHdr);
}

