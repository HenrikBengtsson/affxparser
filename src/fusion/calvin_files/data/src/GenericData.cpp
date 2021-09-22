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


#include "calvin_files/data/src/GenericData.h"
//
#include "calvin_files/array/src/ArrayId.h"
#include "calvin_files/parsers/src/DataGroupHeaderReader.h"
#include "calvin_files/parsers/src/DataSetHeaderReader.h"
//
#include "util/Fs.h"
//
#include <algorithm>
#include <cstring>
#include <iterator>
#include <string>
//

using namespace affymetrix_calvin_io;

/*
 * Initialize the class.
 */
GenericData::GenericData()
{
#ifdef _MSC_VER
	fileMapHandle = NULL;
	fileHandle = INVALID_HANDLE_VALUE;
#endif
	useMemoryMapping = true;
	loadEntireDataSetHint = false;
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
	affymetrix_calvin_utilities::AffymetrixGuidType arrayFileID;
	//GenDataHdrVectorIt begin;
	//GenDataHdrVectorIt end;
	GenericDataHeader* hdr = header.GetGenericDataHdr()->FindParent(ARRAY_TYPE_IDENTIFIER);
	if (hdr)
	{
			arrayFileID = hdr->GetFileId();
	}
	return arrayFileID;
}

/*
 * Returns the parent array identifier.
 */
affymetrix_calvin_utilities::AffymetrixGuidType GenericData::ArrayIdentifier()
{
	// Find the parent array file generic header
	affymetrix_calvin_utilities::AffymetrixGuidType arrayID;
	//GenDataHdrVectorIt begin;
	//GenDataHdrVectorIt end;
	GenericDataHeader* hdr = header.GetGenericDataHdr()->FindParent(ARRAY_TYPE_IDENTIFIER);
	if (hdr)
	{
		ParameterNameValueType nvt;
		if (hdr->FindNameValParam(ARRAY_ID_PARAM_NAME, nvt))
		{
			arrayID = nvt.GetValueAscii();
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
		affymetrix_calvin_exceptions::DataGroupNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
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
		affymetrix_calvin_exceptions::DataGroupNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
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
		affymetrix_calvin_exceptions::DataGroupNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
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
		affymetrix_calvin_exceptions::DataGroupNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
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
 * Get the DataSet given a DataGroup and DataSet index
 */
DataSet* GenericData::DataSet(u_int32_t dataGroupIdx, u_int32_t dataSetIdx)
{
	if (Open() == false)
	{
		affymetrix_calvin_exceptions::FileNotOpenException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	DataGroupHeader* dch = FindDataGroupHeader(dataGroupIdx);
	if (dch)
	{
		DataSetHeader* dph = FindDataSetHeader(dch, dataSetIdx);
		if (dph)
		{
			return CreateDataSet(dph);
		}
		else
		{
			affymetrix_calvin_exceptions::DataSetNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
			throw e;
		}
	}
	else
	{
		affymetrix_calvin_exceptions::DataGroupNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

/*
 * Get the DataSet given a DataGroup and DataSet name
 */
DataSet* GenericData::DataSet(const std::wstring& dataGroupName, const std::wstring& dataSetName)
{
	if (Open() == false)
	{
		affymetrix_calvin_exceptions::FileNotOpenException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	DataGroupHeader* dch = FindDataGroupHeader(dataGroupName);
	if (dch)
	{
		DataSetHeader* dph = FindDataSetHeader(dch, dataSetName);
		if (dph)
		{
			return CreateDataSet(dph);
		}
		else
		{
			affymetrix_calvin_exceptions::DataSetNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
			throw e;
		}
	}
	else
	{
		affymetrix_calvin_exceptions::DataGroupNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

/*
 * Creates a new DataSet based on the dsh argument
 */
DataSet* GenericData::CreateDataSet(DataSetHeader* dsh)
{
	void* handle = 0;
#ifdef _MSC_VER
	handle = fileMapHandle;
#endif
	ReadFullDataSetHeader(dsh);
	if (useMemoryMapping)
	{
		return new affymetrix_calvin_io::DataSet(Header().GetFilename(), *dsh, handle, loadEntireDataSetHint);
	}
	else
	{
		return new affymetrix_calvin_io::DataSet(Header().GetFilename(), *dsh, fileStream, loadEntireDataSetHint);
	}
}

/*
 * Returns a DataGroup object based on a DataGroup file position
 */
affymetrix_calvin_io::DataGroup GenericData::DataGroup(u_int32_t dataGroupFilePos)
{
	if (Open() == false)
	{
		affymetrix_calvin_exceptions::FileNotOpenException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	// A little indirection
	std::ifstream fs;
	std::ifstream* pfs = &fileStream;	// initialize to use GenericData::fileStream

	if (useMemoryMapping)
	{
		// Open a file stream
		OpenFStream(fs);
		pfs = &fs;
	}

	// and position it
	pfs->seekg(dataGroupFilePos, std::ios_base::beg);

	// Read the DataGroupHeader and all DataSetHeaders
	DataGroupHeader dch;
	DataGroupHeaderReader reader;
	reader.Read(*pfs, dch);

	if (useMemoryMapping)
		fs.close();

	void* handle = 0;
#ifdef _MSC_VER
	handle = fileMapHandle;
#endif

	if (useMemoryMapping)
	{
		return affymetrix_calvin_io::DataGroup(Header().GetFilename(), dch, handle, loadEntireDataSetHint);
	}
	else
	{
		return affymetrix_calvin_io::DataGroup(Header().GetFilename(), dch, fileStream, loadEntireDataSetHint);
	}
}


/*
 * Erase the members data.
 */
void GenericData::Clear()
{
	// Clear the header
	header.Clear();
	Close();
	useMemoryMapping = true;
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
                // WAS if (dataSetIdx >= 0 && ...) BUT 'dataSetIdx >= 0'
                // is always true because 'dataSetIdx' is unsigned.
		if ((int32_t)dataSetIdx < dch->GetDataSetCnt())
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
	if (IsDSHPartiallyRead(dph))
	{
		// Open a file stream
		std::ifstream fs;
		std::ifstream* pfs = &fileStream;	// initialize to use GenericData::ifs
		if (useMemoryMapping)
		{
			OpenFStream(fs);
			pfs = &fs;
		}

		// and position it
		pfs->seekg(dph->GetHeaderStartFilePos(), std::ios_base::beg);

		// Read the header
		DataSetHeaderReader reader;
		reader.Read(*pfs, *dph);

		if (useMemoryMapping)
			fs.close();
	}
}

/*
 * Determine if the DataSetHeader has been partially read.
 */
bool GenericData::IsDSHPartiallyRead(const DataSetHeader* dph)
{
	if (dph == 0)
		return false;
	if (dph->GetRowCnt() == 0 && dph->GetColumnCnt() == 0 && dph->GetNameValParamCnt() == 0)
		return true;
	return false;
}

/*
 * Open the ifstream
 */
void GenericData::OpenFStream(std::ifstream& ifs)
{
  Fs::aptOpen(ifs, Header().GetFilename(), std::ios::in | std::ios::binary);
  if (!ifs.is_open() && !ifs.good())
	{
		affymetrix_calvin_exceptions::FileNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

bool GenericData::Open()
{
	if (useMemoryMapping)
		return MapFile();
	else
	{
		if (fileStream.is_open() == false)
			OpenFStream(fileStream);
		return true;
	}
}

void GenericData::Close()
{
	UnmapFile();
	if (fileStream.is_open())
		fileStream.close();
}

/*
 * Open a memory map on the file.
 */
bool GenericData::MapFile()
{
#ifdef _MSC_VER	// On Windows the map is open in the GenericData object, otherwise it is opened in the DataSet
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
          // Create the file.
          std::wstring filename = Fs::convertToUncPathW(Header().GetFilename(), 10);

          fileHandle = CreateFileW(filename.c_str(), GENERIC_READ, FILE_SHARE_READ,
                                  NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
          if (fileHandle == INVALID_HANDLE_VALUE)
            {
				Verbose::out(1, Fs::MSC_VER_GetLastErrorString(std::string("GenericData::MapFile ") + Util::toString(filename) + ": "));
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

/*
 * Close the memory-mapping on the file
 */
void GenericData::UnmapFile()
{
#ifdef _MSC_VER

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
