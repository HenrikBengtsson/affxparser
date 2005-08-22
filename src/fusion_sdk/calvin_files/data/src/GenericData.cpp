/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License,
// or (at your option) any later version.
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
/////////////////////////////////////////////////////////////////

#include "GenericData.h"
#include "DataGroupHeaderReader.h"
#include "DataSetHeaderReader.h"
#include <string>
#include <iterator>
#include <algorithm>

using namespace affymetrix_calvin_io;

/*
 * Initialize the class.
 */
GenericData::GenericData()
{
#ifdef WIN32
	fileMapHandle = NULL;
	fileHandle = INVALID_HANDLE_VALUE;
#endif
}

/*
 * Clean up.
 */
GenericData::~GenericData()
{
	Clear();
}

/*
 * Get the file identifier
 */
affymetrix_calvin_utilities::AffymetrixGuidType GenericData::FileIdentifier()
{
	return header.GetGenericDataHdr()->GetFileId();
}

/*
 * Get the parent array file identifier
 */
affymetrix_calvin_utilities::AffymetrixGuidType GenericData::ArrayFileIdentifier()
{
	// Find the parent array file generic header
	affymetrix_calvin_utilities::AffymetrixGuidType arrayID;
	GenDataHdrVectorIt begin;
	GenDataHdrVectorIt end;
	GenericDataHeader* hdr = header.GetGenericDataHdr();
	hdr->GetParentIterators(begin,end);
	for (GenDataHdrVectorIt ii=begin; ii != end; ++ii)
	{
		if( ii->GetFileTypeId() == ARRAY_FILE_TYPE_IDENTIFIER)
		{
			arrayID = ii->GetFileId();
			break;
		}
	}
	return arrayID;
}

/*
 * Get the number of DataGroups
 */
u_int32_t GenericData::DataGroupCnt() const
{
	return header.GetDataGroupCnt();
}

/*
 * Get the names of the DataGroup
 */
void GenericData::DataGroupNames(std::vector<std::wstring>& names)
{
	names.clear();
	DataGroupHdrIt begin;
	DataGroupHdrIt end;
	header.GetDataGroupIts(begin,end);
	for (DataGroupHdrIt ii=begin; ii!=end; ++ii)
	{
		names.push_back(ii->GetName());
	}
}

/*
 * Get the number of DataSets given a DataGroup index
 */
u_int32_t GenericData::DataSetCnt(u_int32_t dataGroupIdx)
{
	DataGroupHeader* dch = FindDataGroupHeader(dataGroupIdx);
	if (dch)
		return dch->GetDataSetCnt();
	else
	{
		affymetrix_calvin_exceptions::DataGroupNotFoundException e;
		throw e;
	}
}

/*
 * Get the number of DataSets given a DataGroup name
 */
u_int32_t GenericData::DataSetCnt(const std::wstring& dataGroupName)
{
	DataGroupHeader* dch = FindDataGroupHeader(dataGroupName);
	if (dch)
		return dch->GetDataSetCnt();
	else
	{
		affymetrix_calvin_exceptions::DataGroupNotFoundException e;
		throw e;
	}
}

/*
 * Get the DataSet names given a DataGroup index
 */
void GenericData::DataSetNames(u_int32_t dataGroupIdx, std::vector<std::wstring>& names)
{
	DataGroupHeader* dch = FindDataGroupHeader(dataGroupIdx);
	if (dch == 0)
	{
		affymetrix_calvin_exceptions::DataGroupNotFoundException e;
		throw e;
	}

	names.clear();
	DataSetHdrIt begin;
	DataSetHdrIt end;
	dch->GetDataSetIterators(begin,end);
	for (DataSetHdrIt ii=begin; ii!=end; ++ii)
	{
		names.push_back(ii->GetName());
	}
}

/*
 * Get the DataSet names given a DataGroup name
 */
void GenericData::DataSetNames(const std::wstring& dataGroupName, std::vector<std::wstring>& names)
{
	DataGroupHeader* dch = FindDataGroupHeader(dataGroupName);
	if (dch == 0)
	{
		affymetrix_calvin_exceptions::DataGroupNotFoundException e;
		throw e;
	}

	names.clear();
	DataSetHdrIt begin;
	DataSetHdrIt end;
	dch->GetDataSetIterators(begin,end);
	for (DataSetHdrIt ii=begin; ii!=end; ++ii)
	{
		names.push_back(ii->GetName());
	}}

/*
 * Get the DataSet given a DataGroup and DataSet index
 */
DataSet* GenericData::DataSet(u_int32_t dataGroupIdx, u_int32_t dataSetIdx)
{
	MapFile();
	DataGroupHeader* dch = FindDataGroupHeader(dataGroupIdx);
	if (dch)
	{
		DataSetHeader* dph = FindDataSetHeader(dch, dataSetIdx);
		if (dph)
		{
			void* handle = 0;
#ifdef WIN32
			handle = fileMapHandle;
#endif
			ReadFullDataSetHeader(dph);
			affymetrix_calvin_io::DataSet* dp = new affymetrix_calvin_io::DataSet(Header().GetFilename(), *dph, handle);
			return dp;
		}
		else
		{
			affymetrix_calvin_exceptions::DataSetNotFoundException e;
			throw e;
		}
	}
	else
	{
		affymetrix_calvin_exceptions::DataGroupNotFoundException e;
		throw e;
	}
}

/*
 * Get the DataSet given a DataGroup and DataSet names
 */
DataSet* GenericData::DataSet(const std::wstring& dataGroupName, const std::wstring& dataSetName)
{
	MapFile();
	DataGroupHeader* dch = FindDataGroupHeader(dataGroupName);
	if (dch)
	{
		DataSetHeader* dph = FindDataSetHeader(dch, dataSetName);
		if (dph)
		{
			void* handle = 0;
#ifdef WIN32
			handle = fileMapHandle;
#endif
			ReadFullDataSetHeader(dph);
			affymetrix_calvin_io::DataSet* dp = new affymetrix_calvin_io::DataSet(Header().GetFilename(), *dph, handle);
			return dp;
		}
		else
		{
			affymetrix_calvin_exceptions::DataSetNotFoundException e;
			throw e;
		}
	}
	else
	{
		affymetrix_calvin_exceptions::DataGroupNotFoundException e;
		throw e;
	}
}

/*
 * Returns a DataGroup object based on a DataGroup file position
 */
affymetrix_calvin_io::DataGroup GenericData::DataGroup(u_int32_t dataGroupFilePos)
{
	MapFile();

	// Open a file stream
	std::ifstream fs;
	OpenFStream(fs);

	// and position it
	fs.seekg(dataGroupFilePos, std::ios_base::beg);

	// Read the DataGroupHeader and all DataSetHeaders
	DataGroupHeader dch;
	DataGroupHeaderReader reader;
	reader.Read(fs, dch);

	fs.close();

	void* handle = 0;
#ifdef WIN32
	handle = fileMapHandle;
#endif

	return affymetrix_calvin_io::DataGroup(Header().GetFilename(), dch, handle);
}


/*
 * Erase the members data.
 */
void GenericData::Clear()
{
	// Clear the header
	header.Clear();
	UnmapFile();
}

/*
 * Finds a DataGroupHeader by name
 */
DataGroupHeader* GenericData::FindDataGroupHeader(const std::wstring& name)
{
	return header.FindDataGroupHeader(name);
}

/*
 * Finds a DataGroupHeader by index
 */

DataGroupHeader* GenericData::FindDataGroupHeader(int32_t index)
{
	DataGroupHeader* dch = 0;
	if (index >= 0 && index < header.GetDataGroupCnt())
	{
		dch = &header.GetDataGroup(index);
	}
	return dch;
}

/*
 * Find a DataSetHeader given the DataGroupHeader and DataSet name
 */
DataSetHeader* GenericData::FindDataSetHeader(DataGroupHeader* dch, const std::wstring& dataSetName)
{
	DataSetHeader* dph = 0;
	if (dch != 0)
	{
		dph = dch->FindDataSetHeader(dataSetName);
	}
	return dph;
}

/*
 * Find a DataSetHeader given the DataGroupHeader and DataSet index
 */
DataSetHeader* GenericData::FindDataSetHeader(DataGroupHeader* dch, u_int32_t dataSetIdx)
{
	DataSetHeader* dph = 0;
	if (dch != 0)
	{
		if (dataSetIdx >= 0 && (int32_t)dataSetIdx < dch->GetDataSetCnt())
		{
			dph = &dch->GetDataSet(dataSetIdx);
		}
	}
	return dph;
}

/*
 * Read the full DataSetHeader if it has only been parially read.
 */
void GenericData::ReadFullDataSetHeader(DataSetHeader* dph)
{
	// Check if the DataSet has been read fully.
	if (IsDPHPartiallyRead(dph))
	{
		// Open a file stream
		std::ifstream fs;
		OpenFStream(fs);

		// and position it
		fs.seekg(dph->GetHeaderStartFilePos(), std::ios_base::beg);

		// Read the header
		DataSetHeaderReader reader;
		reader.Read(fs, *dph);

		fs.close();
	}
}

/*
 * Determine if the DataSetHeader has been partially read.
 */
bool GenericData::IsDPHPartiallyRead(const DataSetHeader* dph)
{
	if (dph == 0)
		return false;
	if (dph->GetRowCnt() == 0 && dph->GetColumnCnt() == 0)
		return true;
	return false;
}

/*
 * Open the ifstream
 */
void GenericData::OpenFStream(std::ifstream& fileStream)
{
	fileStream.open(Header().GetFilename().c_str(), std::ios::in | std::ios::binary);
	if (!fileStream)
	{
		affymetrix_calvin_exceptions::FileNotFoundException e;
		throw e;
	}
}

bool GenericData::MapFile()
{
#ifdef WIN32
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		// Create the file.
		fileHandle = CreateFile(Header().GetFilename().c_str(), GENERIC_READ, FILE_SHARE_READ,
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (fileHandle == INVALID_HANDLE_VALUE)
		{
			return false;
		}
	}
	if (fileMapHandle == NULL)
	{
		// Use the current size of the file.
		DWORD dwSizeHigh = 0;
		DWORD dwSizeLow  = 0;
		fileMapHandle = CreateFileMapping(fileHandle, NULL, PAGE_READONLY, dwSizeHigh, dwSizeLow, NULL);
		if (fileMapHandle == NULL)
			return false;
	}
#endif
	return true;
}

void GenericData::UnmapFile()
{
#ifdef WIN32

	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		if (fileMapHandle != NULL)
		{
			CloseHandle(fileMapHandle);
			fileMapHandle = NULL;
		}
		CloseHandle (fileHandle);
		fileHandle = INVALID_HANDLE_VALUE;
	}
#endif
}