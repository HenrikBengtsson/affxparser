/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2004 Affymetrix, Inc.
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

#include "CDFFileData.h"
#include "FileIO.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <istream>
#include <fstream>

#include <iostream>
#include <stdio.h>

#ifndef WIN32
#include <sys/mman.h>
#endif

#ifdef _INCLUDE_UNISTD_HEADER_
#include <unistd.h>
#endif

// not quite clear about what this does.
//
//#define _USE_MEM_MAPPING

using namespace affxcdf;

//////////////////////////////////////////////////////////////////////

#define CDF_FILE_MAGIC_NUMBER 67
#define CDF_FILE_VERSION_NUMBER 1

#ifdef WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

//////////////////////////////////////////////////////////////////////

CCDFFileHeader::CCDFFileHeader() :
	m_Magic(0),
	m_Version(0),
	m_NumProbeSets(0),
	m_NumQCProbeSets(0),
	m_Cols(0),
	m_Rows(0)
{
}

//////////////////////////////////////////////////////////////////////

CCDFFileData::CCDFFileData() :
  m_bFileOpen(false),
  m_lpFileMap(NULL),
  m_lpData(NULL),
  m_bFileMapped(false),
  
  /** JHB **/
  m_complementary_logic(false) 
  
{
#ifdef WIN32
	m_hFileMap = INVALID_HANDLE_VALUE;
	m_hFile = INVALID_HANDLE_VALUE;
#else
	m_MapLen = 0;
	m_fp = NULL;
#endif
}

//////////////////////////////////////////////////////////////////////

CCDFFileData::~CCDFFileData()
{
	Close();
}

//////////////////////////////////////////////////////////////////////

CCDFProbeInformation::CCDFProbeInformation() :
	m_ListIndex(0),
	m_Expos(0),
	m_X(0),
	m_Y(0),
	m_PBase(' '),
	m_TBase(' ')
{
}

//////////////////////////////////////////////////////////////////////

CCDFProbeGroupInformation::CCDFProbeGroupInformation() :
	m_NumLists(0),
	m_NumCells(0),
	m_Start(0),
	m_Stop(0),
	m_ProbeSetIndex(0),
	m_GroupIndex(0),
	m_NumCellsPerList(0),
	m_Direction(0),
	m_bMapped(false),
	m_lpData(NULL),
	m_pCells(NULL)
{
}

//////////////////////////////////////////////////////////////////////

CCDFProbeGroupInformation::~CCDFProbeGroupInformation()
{
	m_Cells.clear();
}

//////////////////////////////////////////////////////////////////////

void CCDFProbeGroupInformation::MakeShallowCopy(CCDFProbeGroupInformation &orig)
{
	m_NumLists = orig.m_NumLists;
	m_NumCells = orig.m_NumCells;
	m_Start = orig.m_Start;
	m_Stop = orig.m_Stop;
	m_ProbeSetIndex = orig.m_ProbeSetIndex;
	m_GroupIndex = orig.m_GroupIndex;
	m_Name = orig.m_Name;
	m_NumCellsPerList = orig.m_NumCellsPerList;
	m_Direction = orig.m_Direction;
	m_pCells = &orig.m_Cells;
	m_bMapped = orig.m_bMapped;
	m_lpData = orig.m_lpData;
}

//////////////////////////////////////////////////////////////////////

void CCDFProbeGroupInformation::GetCell(int cell_index, CCDFProbeInformation & info)
{
#if defined(_USE_MEM_MAPPING_)
	if (m_bMapped == false)
		info = (*m_pCells)[cell_index];
	else
	{
		char *lpData = m_lpData + PROBE_GROUP_SIZE + (cell_index * PROBE_SIZE);

		info.m_ListIndex = *(int *)(lpData);
		lpData += INT_SIZE;

		info.m_X = *(unsigned short *)(lpData);
		lpData += SHORT_SIZE;

		info.m_Y = *(unsigned short *)(lpData);
		lpData += SHORT_SIZE;

		info.m_Expos = *(int *)(lpData);
		lpData += INT_SIZE;

		info.m_PBase = *(char *)(lpData);
		lpData += CHAR_SIZE;

		info.m_TBase = *(char *)(lpData);

#ifdef IS_BIG_ENDIAN
		int ival;
		unsigned short usval;

		ival = info.m_ListIndex;
		SwapBytes((char *)&ival, (char *)&info.m_ListIndex, INT_SIZE);

		ival = info.m_Expos;
		SwapBytes((char *)&ival, (char *)&info.m_Expos, INT_SIZE);

		usval = info.m_X;
		SwapBytes((char *)&usval, (char *)&info.m_X, USHORT_SIZE);

		usval = info.m_Y;
		SwapBytes((char *)&usval, (char *)&info.m_Y, USHORT_SIZE);
#endif

	}
#else
	info = (*m_pCells)[cell_index];;
#endif
}

//////////////////////////////////////////////////////////////////////

CCDFQCProbeSetInformation::CCDFQCProbeSetInformation() :
	m_NumCells(0),
	m_QCProbeSetType(UnknownQCProbeSetType),
	m_bMapped(false),
	m_lpData(NULL),
	m_pCells(NULL)
{
}

//////////////////////////////////////////////////////////////////////

CCDFQCProbeSetInformation::~CCDFQCProbeSetInformation()
{
	m_Cells.erase(m_Cells.begin(), m_Cells.end());
}

//////////////////////////////////////////////////////////////////////

void CCDFQCProbeSetInformation::MakeShallowCopy(CCDFQCProbeSetInformation &orig)
{
	m_NumCells = orig.m_NumCells;
	m_QCProbeSetType = orig.m_QCProbeSetType;
	m_pCells = &orig.m_Cells;
	m_bMapped = orig.m_bMapped;
	m_lpData = orig.m_lpData;
}

//////////////////////////////////////////////////////////////////////

void CCDFQCProbeSetInformation::GetProbeInformation(int index, CCDFQCProbeInformation & info)
{
#if defined(_USE_MEM_MAPPING_)
	if (m_bMapped == false)
		info = (*m_pCells)[index];
	else
	{
		char *lpData = m_lpData + QC_PROBE_SET_SIZE + (index * QC_PROBE_SIZE);

		info.m_X = *(unsigned short *)(lpData);
		lpData += SHORT_SIZE;

		info.m_Y = *(unsigned short *)(lpData);
		lpData += SHORT_SIZE;

		info.m_PLen = *(unsigned char *)(lpData);
		lpData += CHAR_SIZE;

		info.m_PMProbe = *(unsigned char *)(lpData);
		lpData += CHAR_SIZE;

		info.m_Background = *(unsigned char *)(lpData);

#ifdef IS_BIG_ENDIAN
		unsigned short usval;

		usval = info.m_X;
		SwapBytes((char *)&usval, (char *)&info.m_X, USHORT_SIZE);

		usval = info.m_Y;
		SwapBytes((char *)&usval, (char *)&info.m_Y, USHORT_SIZE);
#endif

	}
#else
	info = (*m_pCells)[index];;
#endif
}

//////////////////////////////////////////////////////////////////////

CCDFProbeSetInformation::CCDFProbeSetInformation() :
	m_NumLists(0),
	m_NumGroups(0),
	m_NumCells(0),
	m_Index(0),
	m_ProbeSetNumber(0),
	m_ProbeSetType(UnknownProbeSetType),
	m_Direction(NoDirection),
	m_NumCellsPerList(0),
	m_bMapped(false),
	m_lpData(NULL),
	m_pGroups(NULL)
{
}

//////////////////////////////////////////////////////////////////////

CCDFProbeSetInformation::~CCDFProbeSetInformation()
{
	m_Groups.erase(m_Groups.begin(), m_Groups.end());
}

//////////////////////////////////////////////////////////////////////

void CCDFProbeSetInformation::MakeShallowCopy(CCDFProbeSetInformation &orig)
{
	m_NumLists = orig.m_NumLists;
	m_NumGroups = orig.m_NumGroups;
	m_NumCells = orig.m_NumCells;
	m_Index = orig.m_Index;
	m_ProbeSetNumber = orig.m_ProbeSetNumber;
	m_ProbeSetType = orig.m_ProbeSetType;
	m_Direction = orig.m_Direction;
	m_NumCellsPerList = orig.m_NumCellsPerList;
	m_pGroups = &orig.m_Groups;
	m_bMapped = orig.m_bMapped;
	m_lpData = orig.m_lpData;
}

//////////////////////////////////////////////////////////////////////

void CCDFProbeSetInformation::GetGroupInformation(int index, CCDFProbeGroupInformation & info)
{
#if defined(_USE_MEM_MAPPING_)
	if (m_bMapped == false)
		info.MakeShallowCopy((*m_pGroups)[index]);
	else
	{
		char *lpData = m_lpData + PROBE_SET_SIZE;
		for (int i=0; i<index; i++)
		{
			int cells = *(int *)(lpData + INT_SIZE);
			
#ifdef IS_BIG_ENDIAN
			int cellsOrig = cells;
			SwapBytes((char *)&cellsOrig, (char *)&cells, INT_SIZE);
#endif

			lpData += PROBE_GROUP_SIZE;
			lpData += (cells * PROBE_SIZE);
		}

		info.m_ProbeSetIndex = m_Index;
		info.m_bMapped = true;
		info.m_lpData = lpData;
		info.m_GroupIndex = index;
		info.m_pCells = NULL;

		info.m_NumLists = *(int *)(lpData);
		lpData += INT_SIZE;

		info.m_NumCells = *(int *)(lpData);
		lpData += INT_SIZE;

		info.m_NumCellsPerList = *(unsigned char *)(lpData);
		lpData += CHAR_SIZE;

		info.m_Direction = *(unsigned char *)(lpData);
		lpData += CHAR_SIZE;

		info.m_Start = *(int *)(lpData);
		lpData += INT_SIZE;

		info.m_Stop = *(int *)(lpData);
		lpData += INT_SIZE;


#ifdef IS_BIG_ENDIAN
		int ival;

		ival = info.m_NumLists;
		SwapBytes((char *)&ival, (char *)&info.m_NumLists, INT_SIZE);

		ival = info.m_NumCells;
		SwapBytes((char *)&ival, (char *)&info.m_NumCells, INT_SIZE);

		ival = info.m_Start;
		SwapBytes((char *)&ival, (char *)&info.m_Start, INT_SIZE);

		ival = info.m_Stop;
		SwapBytes((char *)&ival, (char *)&info.m_Stop, INT_SIZE);
#endif

		char str[MAX_PROBE_SET_NAME_LENGTH];
		memcpy(str, lpData, MAX_PROBE_SET_NAME_LENGTH);
		info.m_Name = str;


		// Reset the start/stop
		CCDFProbeInformation probeInfo;
		info.GetCell(0, probeInfo);
		info.m_Start = probeInfo.GetListIndex();
		info.GetCell(info.m_NumCells-1, probeInfo);
		info.m_Stop = probeInfo.GetListIndex();
	}
#else
	info.MakeShallowCopy((*m_pGroups)[index]);
#endif
}

//////////////////////////////////////////////////////////////////////

CCDFProbeSetNames::CCDFProbeSetNames() : m_bMapped(false), m_lpData(NULL)
{
}

//////////////////////////////////////////////////////////////////////

CCDFProbeSetNames::~CCDFProbeSetNames()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////

void CCDFProbeSetNames::Clear()
{
	m_ProbeSetNames.erase(m_ProbeSetNames.begin(), m_ProbeSetNames.end());
}

//////////////////////////////////////////////////////////////////////

std::string CCDFProbeSetNames::GetName(int index) const
{
	if (m_bMapped == true)
	{
		char name[MAX_PROBE_SET_NAME_LENGTH + 1];
		memcpy(name, m_lpData + index*MAX_PROBE_SET_NAME_LENGTH, MAX_PROBE_SET_NAME_LENGTH);
		name[MAX_PROBE_SET_NAME_LENGTH] = NULL;
		return std::string(name);
	}
	else
	{
		return m_ProbeSetNames[index];
	}
}

//////////////////////////////////////////////////////////////////////

void CCDFProbeSetNames::Resize(int size)
{
	m_ProbeSetNames.resize(size);
}

//////////////////////////////////////////////////////////////////////

void CCDFProbeSetNames::SetName(int index, std::string name)
{
	m_ProbeSetNames[index] = name;
}

//////////////////////////////////////////////////////////////////////

std::string CCDFFileData::GetProbeSetName(int index) const
{
	return m_ProbeSetNames.GetName(index);
}

//////////////////////////////////////////////////////////////////////

void CCDFFileData::Close()
{
	int n = (int) m_ProbeSets.size();
	m_ProbeSets.clear();
	m_QCProbeSets.clear();
	m_ProbeSetNames.Clear();

#ifdef WIN32
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
	m_ProbeSetPositions.clear();
	m_QCProbeSetPositions.clear();
}

//////////////////////////////////////////////////////////////////////

bool CCDFFileData::IsXDACompatibleFile()
{
	// Format the full path name.
	m_FullPathName = m_LibPath;
	m_FullPathName += PATH_SEPARATOR;
	m_FullPathName += m_FileName;

	// Open the file.
	std::ifstream instr(m_FullPathName.c_str(), std::ios::in | std::ios::binary);
	if (!instr)
	  return false;

	// Read the magic number from the file.
	int magic=0;
	READ_INT(instr, magic);
	bool bXDAFile = (magic == CDF_FILE_MAGIC_NUMBER);
	instr.close();

	return bXDAFile;
}

//////////////////////////////////////////////////////////////////////

std::string CCDFFileData::GetChipType() const
{
	std::string chiptype;
	if (m_FileName.empty() == false)
	{
		int index = (int) m_FileName.rfind(PATH_SEPARATOR);
		chiptype = m_FileName.c_str() + index + 1;
		chiptype.resize(chiptype.length()-4);
	}
	return chiptype;
}

//////////////////////////////////////////////////////////////////////

void CCDFFileData::SetFileName(const char *str)
{
	std::string file = str;
	int index = (int) file.rfind(PATH_SEPARATOR);
	if (index != -1)
	{
		m_FileName = file.c_str() + index + 1;
		m_LibPath = file;
		m_LibPath.resize(index);
	}
	else
		m_FileName = file;
}

//////////////////////////////////////////////////////////////////////

bool CCDFFileData::Exists()
{
	// Format the full path name.
	m_FullPathName = m_LibPath;
	m_FullPathName += PATH_SEPARATOR;
	m_FullPathName += m_FileName;

	// Find the file stats.
	struct stat st;
	return (stat(m_FullPathName.c_str(), &st) == 0);
}

//////////////////////////////////////////////////////////////////////

bool CCDFFileData::ReadHeader()
{
	// Read the header, close if failed.
	if (Open(true) == false)
	{
		Close();
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

GeneChipProbeSetType CCDFFileData::GetProbeSetType(int index) const
{
#if defined(_USE_MEM_MAPPING_)
	if (m_bFileMapped == false)
		return m_ProbeSets[index].GetProbeSetType();
	else
	{
		// The type is the first item in the probe set object.
		int offset = m_ProbeSetPositions[index];
		unsigned short usval = *(unsigned short *)(m_lpData + offset);

#ifdef IS_BIG_ENDIAN
		unsigned short usvalOrig = usval;
		SwapBytes((char *)&usvalOrig, (char *)&usval, USHORT_SIZE);
#endif

		return (GeneChipProbeSetType)(usval);
	}
#else
	return m_ProbeSets[index].GetProbeSetType();
#endif
}

//////////////////////////////////////////////////////////////////////

void CCDFFileData::GetProbeSetInformation(int index, CCDFProbeSetInformation & info)
{
#if defined(_USE_MEM_MAPPING_)
	if (m_bFileMapped == false)
		info.MakeShallowCopy(m_ProbeSets[index]);
	else
	{
		int offset = m_ProbeSetPositions[index];
		char *lpData = m_lpData + offset;

		info.m_Index = index;
		info.m_bMapped = true;
		info.m_lpData = lpData;
		info.m_pGroups = NULL;

		info.m_ProbeSetType = *(unsigned short *)(lpData);
		lpData += SHORT_SIZE;

		info.m_Direction = *(unsigned char *)(lpData);
		lpData += CHAR_SIZE;

		info.m_NumLists = *(int *)(lpData);
		lpData += INT_SIZE;

		info.m_NumGroups = *(int *)(lpData);
		lpData += INT_SIZE;

		info.m_NumCells = *(int *)(lpData);
		lpData += INT_SIZE;

		info.m_ProbeSetNumber = *(int *)(lpData);
		lpData += INT_SIZE;

		info.m_NumCellsPerList = *(unsigned char *)(lpData);

#ifdef IS_BIG_ENDIAN
		int ival;
		unsigned short usval;

		usval = info.m_ProbeSetType;
		SwapBytes((char *)&usval, (char *)&info.m_ProbeSetType, USHORT_SIZE);

		ival = info.m_NumLists;
		SwapBytes((char *)&ival, (char *)&info.m_NumLists, INT_SIZE);

		ival = info.m_NumGroups;
		SwapBytes((char *)&ival, (char *)&info.m_NumGroups, INT_SIZE);

		ival = info.m_NumCells;
		SwapBytes((char *)&ival, (char *)&info.m_NumCells, INT_SIZE);

		ival = info.m_ProbeSetNumber;
		SwapBytes((char *)&ival, (char *)&info.m_ProbeSetNumber, INT_SIZE);
#endif

	}
#else
	info.MakeShallowCopy(m_ProbeSets[index]);
#endif
}

//////////////////////////////////////////////////////////////////////

void CCDFFileData::GetQCProbeSetInformation(GeneChipQCProbeSetType qcType, CCDFQCProbeSetInformation & info)
{
	bool bFound = false;
	for (int i=0; i<m_Header.GetNumQCProbeSets() && bFound == false; i++)
	{
		GetQCProbeSetInformation(i, info);
		if (info.GetQCProbeSetType() == qcType)
			bFound = true;
	}
	if (bFound == false)
		info.m_NumCells = 0;
}

//////////////////////////////////////////////////////////////////////

void CCDFFileData::GetQCProbeSetInformation(int index, CCDFQCProbeSetInformation & info)
{
#if defined(_USE_MEM_MAPPING_)
	if (m_bFileMapped == false)
		info.MakeShallowCopy(m_QCProbeSets[index]);
	else
	{
		int offset = m_QCProbeSetPositions[index];
		char *lpData = m_lpData + offset;

		info.m_bMapped = true;
		info.m_lpData = lpData;
		info.m_pCells = NULL;

		info.m_QCProbeSetType = *(unsigned short *)(lpData);
		lpData += SHORT_SIZE;

		info.m_NumCells = *(int *)(lpData);

#ifdef IS_BIG_ENDIAN
		int ival;
		unsigned short usval;

		usval = info.m_QCProbeSetType;
		SwapBytes((char *)&usval, (char *)&info.m_QCProbeSetType, USHORT_SIZE);

		ival = info.m_NumCells;
		SwapBytes((char *)&ival, (char *)&info.m_NumCells, INT_SIZE);
#endif

	}
#else
	info.MakeShallowCopy(m_QCProbeSets[index]);
#endif
}

//////////////////////////////////////////////////////////////////////

bool CCDFFileData::Read()
{
	// Open the file
	if (Open() == false)
	{
		Close();
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CCDFFileData::Open(bool bReadHeaderOnly)
{
	// First close the file.
	Close();

	// Format the full path name.
	m_FullPathName = m_LibPath;
	m_FullPathName += PATH_SEPARATOR;
	m_FullPathName += m_FileName;


	// Open the file.
	if (IsXDACompatibleFile())
	{
#if defined(_USE_MEM_MAPPING_)
		return ReadXDAFormatUsingMemMap(bReadHeaderOnly);
#else
		return ReadXDAFormat(bReadHeaderOnly);
#endif
	}
	else
		return ReadTextFormat(bReadHeaderOnly);
}

//////////////////////////////////////////////////////////////////////

bool CCDFFileData::ReadXDAFormatUsingMemMap(bool bReadHeaderOnly)
{
	// Open the file.
	std::ifstream instr;
	instr.open(m_FullPathName.c_str(), std::ios::in | std::ios::binary);


	// Check if open
	if (!instr)
	{
		m_strError = "Unable to open the file.";
		return false;
	}

	// Read the header.
	if (ReadXDAHeader(instr) == false)
		return false;


	// Stop if just reading the header.
	if (bReadHeaderOnly)
		return true;


	// Get the file position
	int offset = instr.tellg();

	// Close the file and reopen for mapping
	instr.close();

	// Initialize the bool flags.
	m_bFileOpen = false;
	m_bFileMapped = false;

#ifdef WIN32

	// Create the file.
	m_hFile = CreateFile(m_FullPathName.c_str(), GENERIC_READ, FILE_SHARE_READ,
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
	m_fp = fopen(m_FullPathName.c_str(), "r");
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
	if (stat(m_FullPathName.c_str(), &st) == 0)
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

	// Now that the file is mapped, set the file pointers of the members
	m_ProbeSetNames.m_bMapped = true;
	m_ProbeSetNames.m_lpData = m_lpData + offset;


	// Skip over the probe set names
	offset += (MAX_PROBE_SET_NAME_LENGTH * m_Header.m_NumProbeSets);


	// Read the qc probe set indicies
	int ival;
	m_QCProbeSetPositions.resize(m_Header.m_NumQCProbeSets);
	for (int iqcset=0; iqcset<m_Header.m_NumQCProbeSets; iqcset++)
	{
		READ_INT(m_lpData + offset, ival);
		m_QCProbeSetPositions[iqcset] = ival;
		offset += INT_SIZE;
	}

	// Read the probe set indicies.
	m_ProbeSetPositions.resize(m_Header.m_NumProbeSets);
	for (int iset=0; iset<m_Header.m_NumProbeSets; iset++)
	{
		READ_INT(m_lpData + offset, ival);
		m_ProbeSetPositions[iset] = ival;
		offset += INT_SIZE;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////

bool CCDFFileData::ReadXDAHeader(std::ifstream &instr)
{
	// Extact the magic and version numbers.
	READ_INT(instr, m_Header.m_Magic);
	READ_INT(instr, m_Header.m_Version);

	// Check the values for the right format file.
	if (m_Header.m_Magic != CDF_FILE_MAGIC_NUMBER || m_Header.m_Version > CDF_FILE_VERSION_NUMBER)
	{
		m_strError = "The file does not appear to be the correct format.";
		return false;
	}

	// Read the remaining header.
	char *sval=NULL;
	READ_USHORT(instr, m_Header.m_Cols);
	READ_USHORT(instr, m_Header.m_Rows);
	READ_INT(instr, m_Header.m_NumProbeSets);
	READ_INT(instr, m_Header.m_NumQCProbeSets);
	READ_STRING(instr, sval);
	m_Header.m_Reference = sval;
	delete[] sval;
	sval = NULL;

	return true;
}

//////////////////////////////////////////////////////////////////////

bool CCDFFileData::ReadXDAFormat(bool bReadHeaderOnly)
{
	// Open the file.
	std::ifstream instr;
	instr.open(m_FullPathName.c_str(), std::ios::in | std::ios::binary);


	// Check if open
	if (!instr)
	{
		m_strError = "Unable to open the file.";
		return false;
	}


	// Read the header.
	if (ReadXDAHeader(instr) == false)
		return false;


	// Stop if just reading the header.
	if (bReadHeaderOnly)
		return true;



	// Read the probe set names.
	int i;
	char name[MAX_PROBE_SET_NAME_LENGTH + 1];
	m_ProbeSetNames.Resize(m_Header.m_NumProbeSets);
	for (i=0; i<m_Header.m_NumProbeSets; i++)
	{
		READ_FIXED_STRING(instr, name, MAX_PROBE_SET_NAME_LENGTH);
		m_ProbeSetNames.SetName(i, name);
	}


	// Skip over the qc indicies
	int ival;
	for (i=0; i<m_Header.m_NumQCProbeSets; i++)
		READ_INT(instr, ival);

	// Skip over the probe set indicies
	for (i=0; i<m_Header.m_NumProbeSets; i++)
		READ_INT(instr, ival);

	// Read the qc ProbeSets.
	unsigned short usval;
	unsigned char ucval;
	int qcsize;
	CCDFQCProbeSetInformation *pQCProbeSet;
	m_QCProbeSets.resize(m_Header.m_NumQCProbeSets);
	for (i=0; i<m_Header.m_NumQCProbeSets; i++)
	{
		pQCProbeSet = &m_QCProbeSets[i];

		READ_USHORT(instr, usval);
		READ_INT(instr, qcsize);

		pQCProbeSet->m_NumCells = qcsize;
		pQCProbeSet->m_QCProbeSetType = usval;

		// Read the cells
		CCDFQCProbeInformation *pQCCell;
		pQCProbeSet->m_Cells.resize(qcsize);
		for (int j=0; j<qcsize; j++)
		{
			pQCCell = &pQCProbeSet->m_Cells[j];
			READ_USHORT(instr, usval);
			pQCCell->m_X = usval;

			READ_USHORT(instr, usval);
			pQCCell->m_Y = usval;

			READ_UCHAR(instr, ucval);
			pQCCell->m_PLen = ucval;

			READ_UCHAR(instr, ucval);
			pQCCell->m_PMProbe = ucval;

			READ_UCHAR(instr, ucval);
			pQCCell->m_Background = ucval;
		}
	}


	// Read the ProbeSets.
	CCDFProbeSetInformation *pProbeSet;
	m_ProbeSets.resize(m_Header.m_NumProbeSets);
	for (i=0; i<m_Header.m_NumProbeSets; i++)
	{
		pProbeSet = &m_ProbeSets[i];
		pProbeSet->m_Index = i;

		// ProbeSet info
		READ_USHORT(instr, usval);
		pProbeSet->m_ProbeSetType = usval;
		READ_UCHAR(instr, ucval);
		pProbeSet->m_Direction = ucval;
		READ_INT(instr, ival);
		pProbeSet->m_NumLists = ival;
		READ_INT(instr, ival);
		pProbeSet->m_NumGroups = ival;
		READ_INT(instr, ival);
		pProbeSet->m_NumCells = ival;
		READ_INT(instr, ival);
		pProbeSet->m_ProbeSetNumber = ival;
		READ_UCHAR(instr, ucval);
		pProbeSet->m_NumCellsPerList = ucval;


		// Read the Groups
		CCDFProbeGroupInformation *pBlk;
		pProbeSet->m_Groups.resize(pProbeSet->m_NumGroups);
		for (int j=0; j<pProbeSet->m_NumGroups; j++)
		{
			pBlk = &pProbeSet->m_Groups[j];
			pBlk->m_GroupIndex = j;

			// Group info
			READ_INT(instr, ival);
			pBlk->m_NumLists = ival;
			READ_INT(instr, ival);
			pBlk->m_NumCells = ival;
			READ_UCHAR(instr, ucval);
			pBlk->m_NumCellsPerList = ucval;
			READ_UCHAR(instr, ucval);
			pBlk->m_Direction = ucval;
			READ_INT(instr, ival);
			pBlk->m_Start = ival;
			READ_INT(instr, ival);
			pBlk->m_Stop = ival;
			memset(name, 0, MAX_PROBE_SET_NAME_LENGTH);
			READ_FIXED_STRING(instr, name, MAX_PROBE_SET_NAME_LENGTH);
			pBlk->m_Name = name;


			// Read the cells
			CCDFProbeInformation *pCell;
			pBlk->m_Cells.resize(pBlk->m_NumCells);
			for (int k=0; k<pBlk->m_NumCells; k++)
			{
				pCell = &pBlk->m_Cells[k];

				// Cell info.
				READ_INT(instr, ival);
				pCell->m_ListIndex = ival;
				READ_USHORT(instr, usval);
				pCell->m_X = usval;
				READ_USHORT(instr, usval);
				pCell->m_Y = usval;
				READ_INT(instr, ival);
				pCell->m_Expos = ival;
				READ_CHAR(instr, pCell->m_PBase);
				READ_CHAR(instr, pCell->m_TBase);

				if (k==0)
					pBlk->m_Start = pCell->m_ListIndex;
				else if (k == pBlk->m_NumCells-1)
					pBlk->m_Stop = pCell->m_ListIndex;
			}
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
bool __complementary(char pbase, char tbase) 
{
  switch (tbase) {
  
  case 'G' : 
    if (pbase == 'C')
      return false;
    else 
      return true;
  case 'C' :
    if (pbase == 'G')
      return false;
    else 
      return true;
  case 'T' : 
    if (pbase == 'A')
      return false;
    else 
      return true;
  case 'A' :
    if (pbase == 'T')
      return false;
    else 
      return true;
  }
}


bool CCDFFileData::ReadTextFormat(bool bReadHeaderOnly)
{
	// Open the file.
	std::ifstream instr;
	instr.open(m_FullPathName.c_str(), std::ios::in);

	// Check if open
	if (!instr)
	{
		m_strError = "Unable to open the file.";
		return false;
	}

	const int MAXLINELENGTH = 4096;
	char str[MAXLINELENGTH];
	char *subStr;
	const char *CDFVERSION1 = "GC1.0";
	const char *CDFVERSION2 = "GC2.0";
	const char *CDFVERSION3 = "GC3.0";

	// Get the CDF section.
	GET_NEXT_LINE(instr, str, MAXLINELENGTH);
	if (strncmp( str, "[CDF]", 5) != 0)
	{
		m_strError = "Unknown file format.";
		return false;
	}

	// Get the version number.
	GET_NEXT_LINE(instr, str, MAXLINELENGTH);
	subStr=strchr(str,'=')+1;
	if ( strncmp( subStr, CDFVERSION1, strlen(CDFVERSION1)) == 0)
		m_Header.m_Version = 1;
	else if ( strncmp( subStr, CDFVERSION2, strlen(CDFVERSION2)) == 0)
		m_Header.m_Version = 2;
	else if ( strncmp( subStr, CDFVERSION3, strlen(CDFVERSION3)) == 0)
		m_Header.m_Version = 3;


	// Get the next section.
	GET_NEXT_LINE(instr, str, MAXLINELENGTH); // [Chip]
	GET_NEXT_LINE(instr, str, MAXLINELENGTH); // name
	GET_NEXT_LINE(instr, str, MAXLINELENGTH); // rows
	subStr = strchr(str, '=')+1;
	m_Header.m_Rows = atoi(subStr);
	GET_NEXT_LINE(instr, str, MAXLINELENGTH); // cols
	subStr=strchr(str,'=')+1;
	m_Header.m_Cols = atoi(subStr);
	GET_NEXT_LINE(instr, str, MAXLINELENGTH); // #ProbeSets
	subStr=strchr(str,'=')+1;
	m_Header.m_NumProbeSets = atoi(subStr);
	GET_NEXT_LINE(instr, str, MAXLINELENGTH); // max ProbeSet number
	m_Header.m_NumQCProbeSets = 0;
	if (m_Header.m_Version > 1)
	{
		GET_NEXT_LINE(instr, str, MAXLINELENGTH); // #qc ProbeSets
		subStr=strchr(str,'=')+1;
		m_Header.m_NumQCProbeSets = atoi(subStr);
		char strref[65000];
		GET_NEXT_LINE(instr, strref, 65000);	// The reference string.
		subStr=strchr(strref,'=')+1;
		m_Header.m_Reference = subStr;
	}


	// Stop if just reading the header.
	if (bReadHeaderOnly)
		return true;

	// Allocate for the probe set names.
	m_ProbeSetNames.Resize(m_Header.m_NumProbeSets);

	// Allocate for the QCProbeSets.
	CCDFQCProbeSetInformation *pQCProbeSet;
	m_QCProbeSets.resize(m_Header.m_NumQCProbeSets);

	// Read the QC probe sets
	for (int iQCProbeSet=0; iQCProbeSet<m_Header.m_NumQCProbeSets; iQCProbeSet++)
	{
		pQCProbeSet = &m_QCProbeSets[iQCProbeSet];

		GET_NEXT_LINE(instr, str, MAXLINELENGTH);	// label [QCUnit...]
		GET_NEXT_LINE(instr, str, MAXLINELENGTH);	// type
		subStr=strchr(str,'=')+1;
		pQCProbeSet->m_QCProbeSetType = atoi( subStr);
		GET_NEXT_LINE(instr, str, MAXLINELENGTH);	// #cells
		subStr=strchr(str,'=')+1;
		pQCProbeSet->m_NumCells = atoi( subStr);
		GET_NEXT_LINE(instr, str, MAXLINELENGTH);	// cell header

		// Read the QC cells.
		int xqc;
		int yqc;
		int plenqc;
		CCDFQCProbeInformation *pQCCell;
		pQCProbeSet->m_Cells.resize(pQCProbeSet->m_NumCells);
		for (int iqccell=0; iqccell<pQCProbeSet->m_NumCells; iqccell++)
		{
			pQCCell = &pQCProbeSet->m_Cells[iqccell];

			GET_NEXT_LINE(instr, str, MAXLINELENGTH);
			subStr = strchr(str, '=')+1;

			sscanf(subStr, "%d %d %*s %d",
					&xqc,
					&yqc,
					&plenqc);

			pQCCell->m_X = xqc;
			pQCCell->m_Y = yqc;
			pQCCell->m_PLen = plenqc;
			pQCCell->m_Background = 0;
			pQCCell->m_PMProbe = 0;
		}
	}


	// Allocate for the ProbeSets.
	int iProbeSet=0;
	CCDFProbeSetInformation *pProbeSet;
	m_ProbeSets.resize(m_Header.m_NumProbeSets);


	//	printf("Number of probesets is: %d\n", m_Header.m_NumProbeSets);

	// Skip until the ProbeSet section is found
 NextProbeSet:
	while (1)
	{
		GET_NEXT_LINE(instr, str, MAXLINELENGTH);
		if (strlen(str) > 5 && strncmp(str, "[Unit", 5) == 0 && !strchr(str, '_'))
			break;

		if (instr.eof())
			return true;
	}

	// ProbeSet info.
	pProbeSet = &m_ProbeSets[iProbeSet];
	pProbeSet->m_Index = iProbeSet;
	GET_NEXT_LINE(instr, str, MAXLINELENGTH); // name (ignore)
	subStr=strchr(str,'=')+1;
	m_ProbeSetNames.SetName(iProbeSet, subStr);
	GET_NEXT_LINE(instr, str, MAXLINELENGTH); // direction
	subStr=strchr(str,'=')+1;
	pProbeSet->m_Direction = atoi(subStr);
	GET_NEXT_LINE(instr, str, MAXLINELENGTH); // # Lists
	subStr=strchr(str,'=')+1;
	int NumCellsPerList=0;
	if (sscanf(subStr, "%d %d", &pProbeSet->m_NumLists, &NumCellsPerList) != 2)
		NumCellsPerList = 0;
	pProbeSet->m_NumCellsPerList = NumCellsPerList;
	GET_NEXT_LINE(instr, str, MAXLINELENGTH); // # cells
	subStr=strchr(str,'=')+1;
	pProbeSet->m_NumCells = atoi(subStr);
	GET_NEXT_LINE(instr, str, MAXLINELENGTH); // ProbeSet number
	subStr=strchr(str,'=')+1;
	pProbeSet->m_ProbeSetNumber = atoi(subStr);
	GET_NEXT_LINE(instr, str, MAXLINELENGTH); // type
	subStr=strchr(str,'=')+1;
	int ival = atoi(subStr);

	typedef enum {
		UNKNOWN_TILE,
		STANDARD_TILE,
		BLOCK_TILE,
		GENE_EXPRESSION_TILE,
		CONTROL_TILE,
		STANDARD_ALTERNATE_TILE,
		STANDARD_VARIANT_TILE,
		UNIVERSAL_TILE
	} TilingTypes;

	switch (ival)
	{
	case STANDARD_TILE:
	case STANDARD_ALTERNATE_TILE:
	case STANDARD_VARIANT_TILE:
		pProbeSet->m_ProbeSetType = ResequencingProbeSetType;
		break;

	case BLOCK_TILE:
		pProbeSet->m_ProbeSetType = GenotypingProbeSetType;
		break;

	case GENE_EXPRESSION_TILE:
		pProbeSet->m_ProbeSetType = ExpressionProbeSetType;
		break;

	case UNIVERSAL_TILE:
		pProbeSet->m_ProbeSetType = TagProbeSetType;
		break;

	default:
		pProbeSet->m_ProbeSetType = UnknownProbeSetType;
		break;
	}

	GET_NEXT_LINE(instr, str, MAXLINELENGTH); // # blocks
	subStr=strchr(str,'=')+1;
	pProbeSet->m_NumGroups = atoi(subStr);

	// Determine the number of cells per List if not specified
	// in the CDF file.
	if (pProbeSet->m_NumCellsPerList == 0)
	{
		if (pProbeSet->m_ProbeSetType == GenotypingProbeSetType || 
		    pProbeSet->m_ProbeSetType == ResequencingProbeSetType || 
		    pProbeSet->m_ProbeSetType == TagProbeSetType || 
		    pProbeSet->m_ProbeSetType == UnknownProbeSetType)
		  
		  pProbeSet->m_NumCellsPerList = 4;
		
		else if (pProbeSet->m_ProbeSetType == ExpressionProbeSetType)
			pProbeSet->m_NumCellsPerList = 2;
		
		else
			pProbeSet->m_NumCellsPerList = 0;
	}

	// Get the mutation type if block tile. ignore.
	if (pProbeSet->m_ProbeSetType == GenotypingProbeSetType && m_Header.m_Version > 1)
		GET_NEXT_LINE(instr, str, MAXLINELENGTH);


	// Read the blocks.
	CCDFProbeGroupInformation *pBlk;
	pProbeSet->m_Groups.resize(pProbeSet->m_NumGroups);
	for (int iGroup=0; iGroup<pProbeSet->m_NumGroups; iGroup++)
	{
		pBlk = &pProbeSet->m_Groups[iGroup];
		pBlk->m_GroupIndex = iGroup;
		pBlk->m_ProbeSetIndex = iProbeSet;

		GET_NEXT_LINE(instr, str, MAXLINELENGTH); // section name - ignore
		GET_NEXT_LINE(instr, str, MAXLINELENGTH); // name
		subStr=strchr(str,'=')+1;
		pBlk->m_Name = subStr;

		if (pProbeSet->m_ProbeSetType == ExpressionProbeSetType)
			m_ProbeSetNames.SetName(iProbeSet, subStr);

		GET_NEXT_LINE(instr, str, MAXLINELENGTH); // block number - ignore.
		GET_NEXT_LINE(instr, str, MAXLINELENGTH); // number of Lists.
		subStr=strchr(str,'=')+1;
		pBlk->m_NumLists = atoi(subStr);
		GET_NEXT_LINE(instr, str, MAXLINELENGTH); // number of cells
		subStr=strchr(str,'=')+1;
		pBlk->m_NumCells = atoi(subStr);
		GET_NEXT_LINE(instr, str, MAXLINELENGTH); // start position.
		subStr=strchr(str,'=')+1;
		pBlk->m_Start = atoi(subStr);
		GET_NEXT_LINE(instr, str, MAXLINELENGTH); // stop position
		subStr=strchr(str,'=')+1;
		pBlk->m_Stop = atoi(subStr);
		pBlk->m_NumCellsPerList = pProbeSet->m_NumCellsPerList;

		if (pProbeSet->m_ProbeSetType == GenotypingProbeSetType && m_Header.m_Version > 2)
		{
			GET_NEXT_LINE(instr, str, MAXLINELENGTH);
			subStr=strchr(str,'=')+1;
			pBlk->m_Direction = atoi(subStr);
		}
		else
			pBlk->m_Direction = pProbeSet->m_Direction;


		// Read the cells.
		GET_NEXT_LINE(instr, str, MAXLINELENGTH); // header
		CCDFProbeInformation cell;
		pBlk->m_Cells.resize(pBlk->m_NumCells);
		char unusedstr[64];
		int unusedint;
		int cellIndex;
		int x,y;
		for (int iCell=0; iCell<pBlk->m_NumCells; iCell++)
		{
			GET_NEXT_LINE(instr, str, MAXLINELENGTH);
			subStr = strchr(str, '=')+1;
			sscanf(subStr, "%d %d %s %s %s %d %d %*c %c %c %d",
			       &x,
			       &y,
			       unusedstr,
			       unusedstr,
			       unusedstr,
			       &cell.m_Expos,
			       &unusedint,
			       &cell.m_PBase,
			       &cell.m_TBase,
			       &cell.m_ListIndex);

			cell.m_X = x;
			cell.m_Y = y;

			if (pProbeSet->m_ProbeSetType == ExpressionProbeSetType)
			{
			  cellIndex = (iCell / pProbeSet->m_NumCellsPerList) * pProbeSet->m_NumCellsPerList;
			  

			  /** JHB added to support alternate PM/MM logic. **/			 
			  if (!m_complementary_logic) {
			    if (cell.m_PBase == cell.m_TBase) { 
			      ++cellIndex;
			    } 
			  }
			  else {
			    if (__complementary(cell.m_PBase, cell.m_TBase)) {
			      ++cellIndex;
			    }
			  }
			  
			}
			else
			{
			  cellIndex = (iCell / pProbeSet->m_NumCellsPerList) * pProbeSet->m_NumCellsPerList;
			  cellIndex += (pProbeSet->m_NumCellsPerList - (iCell % pProbeSet->m_NumCellsPerList) - 1);
			}

			pBlk->m_Cells[cellIndex] = cell;

			if (iCell==0)
				pBlk->m_Start = cell.m_ListIndex;
			else if (iCell == pBlk->m_NumCells-1)
				pBlk->m_Stop = cell.m_ListIndex;

			
		}
	}
	
	++iProbeSet;
	goto NextProbeSet;
}

//////////////////////////////////////////////////////////////////////
