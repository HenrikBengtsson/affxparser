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


#include "calvin_files/data/src/FileHeader.h"
//

using namespace affymetrix_calvin_io;

FileHeader::FileHeader() 
{
	magic = MAGIC_NUM; 
	version = CALVINIOVERSION; 
}

void FileHeader::Clear()
{
	dataGroupHdrs.clear();
	genericHdr.Clear();
	numDataGroups = 0;
	firstDataGroupFilePos = 0;
}

void FileHeader::SetFilename(const std::string &p)
{
	filename = p;
}

std::string FileHeader::GetFilename() const
{
	return filename;
}

u_int8_t FileHeader::GetMagicNumber() const
{
	return magic;
}

u_int8_t FileHeader::GetVersion() const
{
	return version;
}

void FileHeader::AddDataGroupHdr(const DataGroupHeader &p)
{
	dataGroupHdrs.push_back(p);
}

DataGroupHeader& FileHeader::GetDataGroup(int32_t index)
{
	return dataGroupHdrs[index];
}

const DataGroupHeader& FileHeader::GetDataGroupConst(int32_t index) const
{
	return dataGroupHdrs[index];
}

void FileHeader::GetDataGroupIts(DataGroupHdrIt &begin, DataGroupHdrIt &end)
{
	begin = dataGroupHdrs.begin();
	end = dataGroupHdrs.end();
}

void FileHeader::SetGenericDataHdr(const GenericDataHeader &p)
{
	genericHdr = p;
}

GenericDataHeader* FileHeader::GetGenericDataHdr()
{
	return &genericHdr;
}

int32_t FileHeader::GetDataGroupCnt() const
{
	return (int32_t)dataGroupHdrs.size();
}

/*
 * Finds a DataGroupHeader by name
 */
DataGroupHeader* FileHeader::FindDataGroupHeader(const std::wstring& name)
{
	DataGroupHeader* dch = 0;
	DataGroupHdrIt begin;
	DataGroupHdrIt end;
	GetDataGroupIts(begin,end);
	for (DataGroupHdrIt ii=begin; ii!=end; ++ii)
	{
		if (ii->GetName() == name)
		{
			dch = &(*ii);
			break;
		}
	}

	return dch;
}
