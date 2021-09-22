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

//
#include "file/GRDFileData.h"
//
#include "file/FileIO.h"
//
#include <cstdio>
#include <cstring>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
//

#ifndef _MSC_VER
#include <sys/mman.h>
#endif

/*! The file identifier size */
#define IDENTIFIER_SIZE 8

/*! The file identifier */
#define GRD_FILE_IDENTIFIER "\211GRD\r\n\032\n"

/*! The file version expected. */
#define FILE_VERSION 1.0f

/*! Use the GRD namespace. */
using namespace affxgrd;

//////////////////////////////////////////////////////////////////////

const char affxgrd::SZ_PARENT_DAT_PROP_NAME[]="Parent DAT File";
const char affxgrd::SZ_SCAN_DATE_TIME_PROP_NAME[]= "Scan Date-Time";
const char affxgrd::SZ_SCANNER_ID_PROP_NAME[]="Scanner ID";

void affxgrd_use_SZ_arrays_above_to_supress_warnings() {
  const char* junk;
  junk=affxgrd::SZ_PARENT_DAT_PROP_NAME;
  junk=affxgrd::SZ_SCAN_DATE_TIME_PROP_NAME;
  junk=affxgrd::SZ_SCANNER_ID_PROP_NAME;
}

/*
 * Initialize the header object to zero values.
 */
CGRDFileHeaderData::CGRDFileHeaderData()
{
	m_Version = 0.0f;
	m_nCols = 0;
	m_nRows = 0;
	m_fFeaturePitchX = 0.0f;
	m_fFeaturePitchY = 0.0f;
	m_fFeatureSetbackX = 0.0f;
	m_fFeatureSetbackY = 0.0f;
}

/*
 * Read each of the header sections.
 */
bool CGRDFileData::ReadHeader(char *pData)
{
	// Initialize the header size
	m_HeaderData.m_nHeaderBytes = 0;

	if( ReadFixedHeader( pData ) == false )
		return false;

	if( ReadTagHeader( pData ) == false )
		return false;

	if( ReadOptimumCornerHeader( pData ) == false )
		return false;
	return true;
}

/*
 * Read the fixed header section.
 */
bool CGRDFileData::ReadFixedHeader(char* pData)
{
	// Check that this is a GRD file
	if( memcmp(pData + m_HeaderData.m_nHeaderBytes, GRD_FILE_IDENTIFIER, IDENTIFIER_SIZE) != 0 )
	{
		m_strError = "File does not match GRD format";
		return false;
	}
	m_HeaderData.m_nHeaderBytes += IDENTIFIER_SIZE;

	// Extract version number
	m_HeaderData.m_Version = MmGetFloat_N((float *)(pData + m_HeaderData.m_nHeaderBytes));
	m_HeaderData.m_nHeaderBytes += FLOAT_SIZE;

	// Extract the remainder of the header.
	m_HeaderData.m_nRows = MmGetUInt32_N((uint32_t *)(pData + m_HeaderData.m_nHeaderBytes));
	m_HeaderData.m_nHeaderBytes += INT32_SIZE;

	m_HeaderData.m_nCols = MmGetUInt32_N((uint32_t *)(pData + m_HeaderData.m_nHeaderBytes));
	m_HeaderData.m_nHeaderBytes += INT32_SIZE;

	m_HeaderData.m_fFeaturePitchX = MmGetFloat_N((float *)(pData + m_HeaderData.m_nHeaderBytes));
	m_HeaderData.m_nHeaderBytes += FLOAT_SIZE;

	m_HeaderData.m_fFeaturePitchY = MmGetFloat_N((float *)(pData + m_HeaderData.m_nHeaderBytes));
	m_HeaderData.m_nHeaderBytes += FLOAT_SIZE;

	m_HeaderData.m_fFeatureSetbackX = MmGetFloat_N((float *)(pData + m_HeaderData.m_nHeaderBytes));
	m_HeaderData.m_nHeaderBytes += FLOAT_SIZE;

	m_HeaderData.m_fFeatureSetbackY = MmGetFloat_N((float *)(pData + m_HeaderData.m_nHeaderBytes));
	m_HeaderData.m_nHeaderBytes += FLOAT_SIZE;

	return true;
}

/*
 * Read the parameter header section.
 */
bool CGRDFileData::ReadTagHeader(char* pData)
{
	// Extract the number of bytes in the tag header section.
	// uint32_t numBtyes = MmGetUInt32_N((uint32_t *)(pData + m_HeaderData.m_nHeaderBytes));
	m_HeaderData.m_nHeaderBytes += INT32_SIZE;

	// Extract the number of name-value pairs
	uint32_t numNVP = MmGetUInt32_N((uint32_t *)(pData + m_HeaderData.m_nHeaderBytes));
	m_HeaderData.m_nHeaderBytes += INT32_SIZE;

	m_HeaderData.m_props.clear();
	
	std::string sName;
	std::string sValue;
	int32_t len;
	char *buf;

	// Extract the name-value pairs
	for( uint32_t iNVP=0; iNVP<numNVP; ++iNVP )
	{
		len = MmGetInt32_N((int32_t *)(pData + m_HeaderData.m_nHeaderBytes));
		m_HeaderData.m_nHeaderBytes += INT32_SIZE;
		buf = new char[len+1];
		memcpy(buf, pData + m_HeaderData.m_nHeaderBytes, len);
		buf[len]=0;
		sName = buf;
		delete[] buf;
		m_HeaderData.m_nHeaderBytes += len;

		len = MmGetInt32_N((int32_t *)(pData + m_HeaderData.m_nHeaderBytes));
		m_HeaderData.m_nHeaderBytes += INT32_SIZE;
		buf = new char[len+1];
		memcpy(buf, pData + m_HeaderData.m_nHeaderBytes, len);
		buf[len]=0;
		sValue = buf;
		delete[] buf;
		m_HeaderData.m_nHeaderBytes += len;

		m_HeaderData.m_props[sName] = sValue;
	}

	return true;
}

/*
 * Read the grid corners from the header.
 */
bool CGRDFileData::ReadOptimumCornerHeader(char* pData)
{
	// Extract the number of bytes in the optimum corner header section.
	//uint32_t numBtyes = MmGetUInt32_N((uint32_t *)(pData + m_HeaderData.m_nHeaderBytes));
	m_HeaderData.m_nHeaderBytes += INT32_SIZE;

	// Extract the number of subgrids
	uint32_t numSG = MmGetUInt32_N((uint32_t *)(pData + m_HeaderData.m_nHeaderBytes));
	m_HeaderData.m_nHeaderBytes += INT32_SIZE;

	m_HeaderData.m_optSubgridCoords.clear();
	
	// Extract the subgrid coordinates
	for( uint32_t iSG=0; iSG<numSG; ++iSG )
	{
		FRECT r;
		r.ul.fx = MmGetFloat_N((float *)(pData + m_HeaderData.m_nHeaderBytes));
		m_HeaderData.m_nHeaderBytes += FLOAT_SIZE;
		r.ul.fy = MmGetFloat_N((float *)(pData + m_HeaderData.m_nHeaderBytes));
		m_HeaderData.m_nHeaderBytes += FLOAT_SIZE;
		r.ur.fx = MmGetFloat_N((float *)(pData + m_HeaderData.m_nHeaderBytes));
		m_HeaderData.m_nHeaderBytes += FLOAT_SIZE;
		r.ur.fy = MmGetFloat_N((float *)(pData + m_HeaderData.m_nHeaderBytes));
		m_HeaderData.m_nHeaderBytes += FLOAT_SIZE;
		r.ll.fx = MmGetFloat_N((float *)(pData + m_HeaderData.m_nHeaderBytes));
		m_HeaderData.m_nHeaderBytes += FLOAT_SIZE;
		r.ll.fy = MmGetFloat_N((float *)(pData + m_HeaderData.m_nHeaderBytes));
		m_HeaderData.m_nHeaderBytes += FLOAT_SIZE;
		r.lr.fx = MmGetFloat_N((float *)(pData + m_HeaderData.m_nHeaderBytes));
		m_HeaderData.m_nHeaderBytes += FLOAT_SIZE;
		r.lr.fy = MmGetFloat_N((float *)(pData + m_HeaderData.m_nHeaderBytes));
		m_HeaderData.m_nHeaderBytes += FLOAT_SIZE;

		m_HeaderData.m_optSubgridCoords.push_back(r);
	}

	return true;
}

/*
 * Null the fields.
 */
CGRDFileData::CGRDFileData() :
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

/*
 * Close the file.
 */
CGRDFileData::~CGRDFileData()
{
	Close();
}

/*
 * Check if the file exists.
 */
bool CGRDFileData::Exists()
{
	// Find the file stats.
	struct stat st;
	return (stat(m_FileName.c_str(), &st) == 0);
}

/*
 * Open the file using memory mapping.
 */
bool CGRDFileData::Open()
{
	// First close the file.
	Close();

	// Initialize the bool flags.
	m_bFileOpen = false;
	m_bFileMapped = false;

#ifdef _MSC_VER

	// Create the file.
	m_hFile = CreateFileA(m_FileName.c_str(), GENERIC_READ, FILE_SHARE_READ,
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

	return true;
}

/*
 * Read the file.
 */
bool CGRDFileData::Read()
{
	// Open the file
	if (Open() == false)
	{
		Close();
		return false;
	}

	// Read the header.
	if (ReadHeader(m_lpData) == false)
	{
		m_strError = "Unable to read the file.";
		Close();
		return false;
	}
	return true;
}

/*
 * Close the file and unmap it.
 */
void CGRDFileData::Close()
{

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

/*
 * Gets a center position from the file given the X/Y coordinate
 */
FCOORD CGRDFileData::GetCenter( int x, int y )
{
	uint32_t index = y*m_HeaderData.m_nCols + x;
	FCOORD fc;
	char* pData = m_lpData + m_HeaderData.m_nHeaderBytes + (index * (FLOAT_SIZE + FLOAT_SIZE));
	fc.fx = MmGetFloat_N((float *)pData);
	pData += FLOAT_SIZE;
	fc.fy = MmGetFloat_N((float *)pData);
	return fc;
}

