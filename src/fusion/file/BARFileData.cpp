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

#include "file/BARFileData.h"
#include "file/FileIO.h"
//
#include <cassert>
#include <fstream>
#include <istream>
#include <sys/stat.h>
#include <sys/types.h>
//

#ifndef _MSC_VER
#include <sys/mman.h>
#endif

#ifdef _INCLUDE_UNISTD_HEADER_
#include <unistd.h>
#endif

//////////////////////////////////////////////////////////////////////

using namespace affxbar;

//////////////////////////////////////////////////////////////////////

CGDACSequenceResultItem::CGDACSequenceResultItem() :
	m_NumberDataPoints(0),
	m_NumberColumns(0),
	m_NumberParameters(0),
	m_pColumnTypes(NULL),
	m_ppData(NULL),
	m_pppData(NULL),
	m_bMapped(false),
	m_lpData(NULL),
	m_DataStartPosition(0)
{
}

//////////////////////////////////////////////////////////////////////

CGDACSequenceResultItem::~CGDACSequenceResultItem()
{
	for (int i=0; i<m_NumberDataPoints && m_ppData; i++)
	{
		delete[] m_ppData[i];
		m_ppData[i] = NULL;
	}
	delete[] m_ppData;
	m_ppData = NULL;
	m_pColumnTypes = NULL;
	m_NumberDataPoints = 0;
	m_Parameters.erase(m_Parameters.begin(), m_Parameters.end());
}

//////////////////////////////////////////////////////////////////////

void CGDACSequenceResultItem::GetData(int iData, int iCol, BarSequenceResultData &data )
{
	if (m_bMapped == false)
	{
		data = (*m_pppData)[iData][iCol];
	}
	else
	{
		int offset = ((iData * GetNumberColumns()) + iCol) * INT_SIZE;
		if (this->GetColumnType(iCol) == BAR_DATA_INTEGER)
		{
			data.iValue = MmGetUInt32_N((uint32_t*)(m_lpData + offset));
		}
		else
		{
			data.fValue = MmGetFloat_N((float*)(m_lpData + offset));
		}
	}
}

//////////////////////////////////////////////////////////////////////

void CGDACSequenceResultItem::MakeShallowCopy(CGDACSequenceResultItem &orig)
{
	m_Name = orig.m_Name;
	m_Version = orig.m_Version;
	m_GroupName = orig.m_GroupName;
	m_NumberDataPoints = orig.m_NumberDataPoints;
	m_NumberColumns = orig.m_NumberColumns;
	m_NumberParameters = orig.m_NumberParameters;
	m_pColumnTypes = orig.m_pColumnTypes;
	m_Parameters = orig.m_Parameters;
	m_pppData = &orig.m_ppData;
	m_ppData = NULL;
	m_bMapped = orig.m_bMapped;
	m_lpData = orig.m_lpData;
	m_DataStartPosition = orig.m_DataStartPosition;
}

//////////////////////////////////////////////////////////////////////

void CGDACSequenceResultItem::SetNumberDataPoints(int n)
{
	m_NumberDataPoints = n;
	m_ppData = new BarSequenceResultData *[m_NumberDataPoints];
	m_pppData = &m_ppData;
	for (int j=0; j<m_NumberDataPoints; j++)
	{
		m_ppData[j] = new BarSequenceResultData[m_NumberColumns];
	}
}

//////////////////////////////////////////////////////////////////////

void CGDACSequenceResultItem::SetDataPoint(int nIndex, int colIndex, BarSequenceResultData &data)
{
	m_ppData[nIndex][colIndex] = data;
}

//////////////////////////////////////////////////////////////////////

void CGDACSequenceResultItem::AddParameter(std::string tag, std::string value)
{
	++m_NumberParameters;
	m_Parameters.resize(m_NumberParameters);
	m_Parameters[m_NumberParameters-1].Tag = tag;
	m_Parameters[m_NumberParameters-1].Value = value;
}

//////////////////////////////////////////////////////////////////////

std::string CGDACSequenceResultItem::GetFullName() const
{
	if (m_Version.length() > 0)
	{
		return (m_GroupName + ":" + m_Version + ";" + m_Name);
	}
	else
		return m_Name;
}

//////////////////////////////////////////////////////////////////////

CBARFileData::CBARFileData() :
	m_Version(0), m_NumberSequences(0), m_NumberColumns(0),
	m_NumberParameters(0),
	m_lpFileMap(NULL),
	m_lpData(NULL),
	m_bFileOpen(false),
	m_bFileMapped(false)
{
#ifdef _MSC_VER
	m_hFileMap = INVALID_HANDLE_VALUE;
	m_hFile = INVALID_HANDLE_VALUE;
#else
	m_MapLen = 0;
	m_fp = NULL;
#endif
}

//////////////////////////////////////////////////////////////////////

CBARFileData::~CBARFileData()
{
	Close();
}

//////////////////////////////////////////////////////////////////////

void CBARFileData::GetResults(int index, CGDACSequenceResultItem &seq)
{
	seq.MakeShallowCopy(m_Results[index]);
}

//////////////////////////////////////////////////////////////////////

bool CBARFileData::Exists()
{
	// Find the file stats.
	struct stat st;
	return (stat(m_FileName.c_str(), &st) == 0);
}

//////////////////////////////////////////////////////////////////////

bool CBARFileData::ReadHeader()
{
	// Read the header, clear memory if failed.
	if (ReadFile(true) == false)
	{
		Close();
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CBARFileData::Read()
{
	// Open the file
	if (ReadFile() == false)
	{
		Close();
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CBARFileData::ReadFile(bool bReadHeaderOnly)
{
	//bool retVal = false;

	// First close the file.
	Close();

	// Read the header
	if (ReadHeaderSection() == false)
	{
		Close();
		return false;
	}

	// Stop if just reading  
	if (bReadHeaderOnly)
	{
		return true;
	}

	// Read the data.
	return ReadDataSection();
}

//////////////////////////////////////////////////////////////////////

bool CBARFileData::ReadHeaderSection()
{
	// Open the file.
	std::ifstream instr;
	instr.open(m_FileName.c_str(), std::ios::in | std::ios::binary);

	// Check if open
	if (!instr)
	{
		m_strError = "Unable to open the file.";
		return false;
	}

	// Magic number
	std::string magic;
	ReadFixedString(instr, magic, 8);

	// Version
	ReadFloat_N(instr, m_Version);

	// Number of sequendes
	int32_t cType;
	ReadInt32_N(instr, cType);
	m_NumberSequences=cType;


	// Columns
	int i = 0;
	ReadInt32_N(instr, cType);
	m_NumberColumns=cType;
	m_ColumnTypes.resize(m_NumberColumns);
	for (i=0; i<m_NumberColumns; i++)
	{
		ReadInt32_N(instr, cType);
		m_ColumnTypes[i] = (GDACFILES_BAR_DATA_TYPE) cType;
	}

	// Parameter
	std::string str;
	ReadInt32_N(instr, cType);
	m_NumberParameters=cType;
	m_Parameters.resize(m_NumberParameters);
	TagValuePairType param;
	for (i=0; i<m_NumberParameters; i++)
	{
		ReadString_N(instr, str);
		m_Parameters[i].Tag = str;
		ReadString_N(instr, str);
		m_Parameters[i].Value = str;
	}

	// Determine the position of the start of the data
	m_DataStartPosition = instr.tellg();

	// Close the file
	instr.close();

	return true;
}

//////////////////////////////////////////////////////////////////////

int CBARFileData::GetDataRowSize()
{
	int size=0;
	m_ColumnTypes.resize(m_NumberColumns);
	for (int i=0; i<m_NumberColumns; i++)
	{
		switch (m_ColumnTypes[i])
		{
		case BAR_DATA_FLOAT:
		case BAR_DATA_INTEGER:
		case BAR_DATA_UINTEGER:
			size += INT_SIZE;
			break;

		case BAR_DATA_SHORT:
		case BAR_DATA_USHORT:
			size += SHORT_SIZE;
			break;

		case BAR_DATA_CHAR:
		case BAR_DATA_UCHAR:
			size += CHAR_SIZE;
			break;
		// BAR_DATA_DOUBLE is not handled.
		default:
			assert(0); // case not handled
		}
	}
	return size;
}

//////////////////////////////////////////////////////////////////////

bool CBARFileData::ReadDataSection()
{
	// Open the file.
	std::ifstream instr;
	instr.open(m_FileName.c_str(), std::ios::in | std::ios::binary);

	// Check if open
	if (!instr)
	{
		m_strError = "Unable to open the file.";
		return false;
	}

	// Skip to the data section
	instr.seekg(m_DataStartPosition);

	// Read the results
	bool bVersion2 = ((int)(m_Version + 0.1) == 2);
	m_Results.resize(m_NumberSequences);
	std::string tag;
	std::string val;
	for (int i=0; i<m_NumberSequences; i++)
	{
		ReadString_N(instr, m_Results[i].m_Name);
		if (bVersion2)
		{
			ReadString_N(instr, m_Results[i].m_GroupName);
		}
		ReadString_N(instr, m_Results[i].m_Version);
		if (bVersion2)
		{
			int32_t nParams=0;
			ReadInt32_N(instr, nParams);
			for (int iParam=0; iParam<nParams; iParam++)
			{
				ReadString_N(instr, tag);
				ReadString_N(instr, val);
				m_Results[i].AddParameter(tag, val);
			}
		}
		int32_t cType;
		ReadInt32_N(instr, cType);
		m_Results[i].m_NumberDataPoints=cType;
		m_Results[i].m_NumberColumns = m_NumberColumns;
		m_Results[i].m_pColumnTypes = &m_ColumnTypes;
		m_Results[i].m_DataStartPosition = instr.tellg();

#ifndef _USE_MEM_MAPPING_

		m_Results[i].m_ppData = new BarSequenceResultData *[m_Results[i].m_NumberDataPoints];
		m_Results[i].m_pppData = &m_Results[i].m_ppData;
		for (int j=0; j<m_Results[i].m_NumberDataPoints; j++)
		{
			m_Results[i].m_ppData[j] = new BarSequenceResultData[m_NumberColumns];
			for (int k=0; k<m_NumberColumns; k++)
			{
				if (m_ColumnTypes[k] == BAR_DATA_INTEGER) {
					ReadInt32_N(instr, cType);
					m_Results[i].m_ppData[j][k].iValue = cType;
				} else {
					
					ReadFloat_N(instr, m_Results[i].m_ppData[j][k].fValue);
				}
			}
		}

#else

    // Determine the size of each row
    int rowSize = GetDataRowSize();

		m_Results[i].m_ppData = NULL;
		m_Results[i].m_pppData = NULL;
		int offset = m_Results[i].m_NumberDataPoints * rowSize;
		instr.seekg(offset, std::ios_base::cur);

#endif

	}

	// Close the file
	instr.close();



#ifdef _USE_MEM_MAPPING_
	
	// Initialize the bool flags.
	m_bFileOpen = false;
	m_bFileMapped = false;

#ifdef _MSC_VER

	// Create the file.
	m_hFile = CreateFile(m_FileName.c_str(), GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		m_strError = "Unable to open the file.";
		return false;
	}
	m_bFileOpen = true;


	// Map the file.
	m_hFileMap = CreateFileMapping(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (m_hFileMap != NULL)
	{
		m_lpFileMap = MapViewOfFile(m_hFileMap, FILE_MAP_READ, 0, 0, 0);
		if (m_lpFileMap == NULL)
		{
			CloseHandle (m_hFileMap);
			m_hFileMap = NULL;
			CloseHandle (m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
			m_strError = "Unable to map view for the memory map file.";
			Close();
			return false;
		}
		else
		{
			m_lpData = (char *)m_lpFileMap;
			m_bFileMapped = true;
		}
	}
	else
	{
		m_strError = "Unable to create memory map file.";
		CloseHandle (m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
		Close();
		return false;
	}

#else

	// Open the file
	m_fp = fopen(m_FileName.c_str(), "r");
	if (m_fp == NULL)
	{
		m_strError = "Failed to open the file for memory mapping.";
		Close();
		return false;
	}
	m_bFileOpen = true;
	
	// Get the file size
	struct stat st;
	m_MapLen = 0;
	if (stat(m_FileName.c_str(), &st) == 0)
	{
		m_MapLen = st.st_size;
	}

	// Map the file.
	m_lpFileMap = mmap(NULL, m_MapLen, PROT_READ, MAP_SHARED, fileno(m_fp), 0);
    if (m_lpFileMap == MAP_FAILED)
	{
		m_strError = "Unable to map view for the memory map file.";
		Close();
		return false;
	}
	else
	{
		m_lpData = (char *)m_lpFileMap;
		m_bFileMapped = true;
	}

#endif

	// Set the data pointer for each sequence item.
	for (int iSeq=0; iSeq<m_NumberSequences; iSeq++)
	{
		m_Results[iSeq].m_bMapped = true;
		m_Results[iSeq].m_lpData = m_lpData + m_Results[iSeq].m_DataStartPosition;
	}

#endif

	return true;
}

//////////////////////////////////////////////////////////////////////

void CBARFileData::Close()
{
	m_Version=0;
	m_NumberSequences=0;
	m_NumberColumns=0;
	m_NumberParameters=0;
	m_DataStartPosition = 0;
	m_Parameters.erase(m_Parameters.begin(), m_Parameters.end());
	m_ColumnTypes.erase(m_ColumnTypes.begin(), m_ColumnTypes.end());
	m_Results.erase(m_Results.begin(), m_Results.end());

#ifdef _MSC_VER
	if (m_bFileOpen)
	{
		if (m_bFileMapped)
		{
			UnmapViewOfFile(m_lpFileMap);
			m_lpFileMap = NULL;
			CloseHandle(m_hFileMap);
			m_hFileMap = NULL;
			m_lpData = NULL;
		}
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
		m_bFileOpen = false;
	}
#else
	if (m_fp == NULL)
	{
		if (m_bFileMapped == true)
		{
			munmap(m_lpFileMap, m_MapLen);
			m_MapLen = 0;
		}
		if (m_fp)
			fclose(m_fp);
		m_fp = NULL;
	}
#endif
}

//////////////////////////////////////////////////////////////////////

void CBARFileData::AddAlgorithmParameter(const char *tag, const char *value)
{
	++m_NumberParameters;
	m_Parameters.resize(m_NumberParameters);
	m_Parameters[m_NumberParameters-1].Tag = tag;
	m_Parameters[m_NumberParameters-1].Value = value;
}

//////////////////////////////////////////////////////////////////////

void CBARFileData::AddColumn(GDACFILES_BAR_DATA_TYPE ctype)
{
	++m_NumberColumns;
	m_ColumnTypes.resize(m_NumberColumns);
	m_ColumnTypes[m_NumberColumns-1] = ctype;
}

//////////////////////////////////////////////////////////////////////

void CBARFileData::SetNumberSequences(int n)
{
	m_NumberSequences = n;
	m_Results.resize(m_NumberSequences);
	for (int i=0; i<m_NumberSequences; i++)
	{
		m_Results[i].m_NumberColumns = m_NumberColumns;
		m_Results[i].m_pColumnTypes = &m_ColumnTypes;
	}
}

//////////////////////////////////////////////////////////////////////

