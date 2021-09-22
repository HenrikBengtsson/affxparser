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
#include "file/MDLFileData.h"
//
#include "file/FileIO.h"
#include "file/FileWriter.h"
//
#include <cassert>
#include <cstring>
#include <ctime>
#include <fstream>
#include <istream>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
//

#ifdef _MSC_VER
#pragma warning(disable: 4996) // don't show deprecated warnings.
#endif

using namespace std;
using namespace affxmdl;

//////////////////////////////////////////////////////////////////////

static void SwapToLittleEndian(MDLData &data)
{
  // @todo: This breaks type punning.
  // This is a double (64bit) 
  // The high and low words need to be flipped.
	float *f = (float *)&data.Affinity;
	f[0] = MmGetFloat_I(&f[0]);
	f[1] = MmGetFloat_I(&f[1]);
#if BYTE_ORDER == BIG_ENDIAN
	float temp = f[0];
	f[0] = f[1];
	f[1] = temp;
#endif
	data.RelativeBkg = MmGetFloat_I(&data.RelativeBkg);
	data.Saturation = MmGetFloat_I(&data.Saturation);
	data.Offset = MmGetFloat_I(&data.Offset);
}

//////////////////////////////////////////////////////////////////////

static void SwapFromLittleEndian(MDLData &data)
{
	SwapToLittleEndian(data);
}

//////////////////////////////////////////////////////////////////////

static void SwapToLittleEndian(MDLHeader &header)
{
	header.nVersion = MmGetInt32_I(&header.nVersion);
	header.nNumProbes = MmGetInt32_I(&header.nNumProbes);
	header.nAnalysisType = MmGetInt32_I(&header.nAnalysisType);
	header.bMasked = MmGetInt32_I(&header.bMasked);
}

//////////////////////////////////////////////////////////////////////

static void SwapFromLittleEndian(MDLHeader &header)
{
	SwapToLittleEndian(header);
}

//////////////////////////////////////////////////////////////////////

CMDLFileHeader::CMDLFileHeader(int32_t nProbes, int32_t analysisType) :
	m_nVersion(PLIER_MODEL_FILE_VERSION),
	m_nNumProbes(nProbes),
	m_nAnalysisType(MDL_PLIER),
	m_bMasked(false)
{
}

/////////////////////////////////////////////////////////////////

CMDLFileHeader &CMDLFileHeader::operator=( CMDLFileHeader &rhs )
{
	m_nVersion		= rhs.GetVersion();
	m_nNumProbes		= rhs.GetNumProbes();
	m_nAnalysisType	= rhs.GetAnalysisType();
	m_bMasked			= rhs.GetMasked();
	m_strChipType		= rhs.GetChipType();
	m_strModifiedDate	= rhs.GetModifiedDate();
	m_strReserve		= rhs.GetReserve();

	return *this;
}

/////////////////////////////////////////////////////////////////

CMDLProbeData::CMDLProbeData() :
	m_dAffinity(0.0f),
	m_fRelativeBkg(0.0f),
	m_fSaturation(0.0f),
	m_fOffset(0.0f),
  m_nIndex(0),
	m_bMapped(false),
	m_lpData(NULL)
{
}

/////////////////////////////////////////////////////////////////

void CMDLProbeData::MakeShallowCopy(CMDLProbeData &orig)
{
	m_dAffinity = orig.m_dAffinity;
	m_fRelativeBkg = orig.m_fRelativeBkg;
	m_fSaturation = orig.m_fSaturation;
	m_fOffset = orig.m_fOffset;
	m_bMapped = orig.m_bMapped;
	m_lpData = orig.m_lpData;
}


/////////////////////////////////////////////////////////////////

CMDLFileData::CMDLFileData
(
	int32_t nProbes, 
	double defAffinity, 
	int32_t analysisType
) :	m_Header(nProbes, analysisType)
{
	// Allocate probe data
	if (nProbes > 0)
	{
		m_ProbeData.resize(nProbes);
		CMDLProbeData probeData;
		probeData.SetAffinity(defAffinity);
		for (int32_t i=0; i<nProbes; i++)
			m_ProbeData[i] = probeData;
	}
  
#if defined(_USE_MEM_MAPPING_) && defined(_MSC_VER)
	m_bFileOpen = false;
	m_lpFileMap = NULL;
	m_hFileMap = INVALID_HANDLE_VALUE;
	m_hFile = INVALID_HANDLE_VALUE;
	m_lpData = NULL;
	m_bFileMapped = false;
	m_nHeaderOffset = 0;
#endif
}

/////////////////////////////////////////////////////////////////

CMDLFileData::~CMDLFileData()
{
	Close();
}

/////////////////////////////////////////////////////////////////

void CMDLFileData::InitializeForWriting
(
	int32_t nProbes, 
	double defAffinity, 
	int32_t analysisType
)
{
	// Set header information
	m_Header.SetNumProbes(nProbes);
	m_Header.SetAnalysisType(analysisType);

	// Allocate probe data
	if (nProbes > 0)
	{
		m_ProbeData.resize(nProbes);
		CMDLProbeData probeData;
		probeData.SetAffinity(defAffinity);
		for (int32_t i=0; i<nProbes; i++)
			m_ProbeData[i] = probeData;
	}
}

/////////////////////////////////////////////////////////////////

void CMDLFileData::SetHeaderInfo(MDLHeader* pHeader)
{
	m_Header.SetVersion(pHeader->nVersion);
	m_Header.SetNumProbes(pHeader->nNumProbes);
	m_Header.SetChipType(pHeader->sChipType);
	m_Header.SetAnalysisType(pHeader->nAnalysisType);
	m_Header.SetModifiedDate(pHeader->sModifiedDate);
	m_Header.SetMasked((pHeader->bMasked == 1 ? true : false));
}

/////////////////////////////////////////////////////////////////

void CMDLFileData::GetHeaderInfo(MDLHeader* pHeader)
{
	pHeader->nVersion = m_Header.GetVersion();
	pHeader->nNumProbes = m_Header.GetNumProbes();
	strcpy(pHeader->sChipType, m_Header.GetChipType().c_str());
	pHeader->nAnalysisType = m_Header.GetAnalysisType();
	strcpy(pHeader->sModifiedDate, m_Header.GetModifiedDate().c_str());
	pHeader->bMasked = m_Header.GetMasked();
	memset(pHeader->sReserve, 0, RESERVED_SIZE);
}

/////////////////////////////////////////////////////////////////

void CMDLFileData::SetDataInfo(MDLData* pData, CMDLProbeData *data)
{
	data->SetAffinity(pData->Affinity);
	data->SetOffset(pData->Offset);
	data->SetRelativeBkg(pData->RelativeBkg);
	data->SetSaturation(pData->Saturation);
}

/////////////////////////////////////////////////////////////////

void CMDLFileData::GetDataInfo(MDLData* pData, CMDLProbeData &data)
{
	pData->Affinity = data.GetAffinity();
	pData->Offset = data.GetOffset();
	pData->RelativeBkg = data.GetRelativeBkg();
	pData->Saturation = data.GetSaturation();
}

/////////////////////////////////////////////////////////////////

void CMDLFileData::GetProbeData(int32_t index, CMDLProbeData& data)
{
	if (index < 0 || index >= m_Header.GetNumProbes())
		assert(index >= 0 && index < m_Header.GetNumProbes());

#if defined(_USE_MEM_MAPPING_) && defined(_MSC_VER)
	if (m_bFileMapped == false)
		data.MakeShallowCopy(m_ProbeData[index]);
	else
	{
		int32_t offset = index*MDLDATASIZE;		
		char *lpData = m_lpData + m_nHeaderOffset + offset;

		data.m_nIndex = index;
		data.m_bMapped = true;
		data.m_lpData = lpData;
				
		data.m_dAffinity = *(double *)(lpData);
		lpData += (2*FLOAT_SIZE);

		data.m_fRelativeBkg = *(float *)(lpData);
		lpData += FLOAT_SIZE;

		data.m_fSaturation = *(float *)(lpData);
		lpData += FLOAT_SIZE;

		data.m_fOffset = *(float *)(lpData);
	}
#else
	data.MakeShallowCopy(m_ProbeData[index]);
#endif
}

/////////////////////////////////////////////////////////////////

void CMDLFileData::SetProbeData(int32_t index, CMDLProbeData data)
{
	if (index < 0 || index >= m_Header.GetNumProbes() || m_ProbeData.size() <= 0)
		assert(index >= 0 && index < m_Header.GetNumProbes() && m_ProbeData.size() > 0);
	m_ProbeData[index] = data;
}

/////////////////////////////////////////////////////////////////

bool CMDLFileData::Read()
{
	// Check if file exists
	if (Exists() == false)
		return false;

	// Open the file
	if (Open() == false)
	{
		Close();
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////

bool CMDLFileData::ReadHeader()
{
	// Check if file exists
	if (Exists() == false)
		return false;

	// Open the file
	if (Open(true) == false)
	{
		Close();
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////

bool CMDLFileData::Open(bool bReadHeaderOnly)
{
	// First close the file.
	Close();

#if defined(_USE_MEM_MAPPING_) && defined(_MSC_VER)
	return ReadFileUsingMemMap(bReadHeaderOnly);
#else
	return ReadFile(bReadHeaderOnly);
#endif
}

/////////////////////////////////////////////////////////////////

void CMDLFileData::Close()
{
	m_ProbeData.clear();

#if defined(_USE_MEM_MAPPING_) && defined(_MSC_VER)
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
#endif
}

/////////////////////////////////////////////////////////////////

bool CMDLFileData::ReadFile(bool bReadHeaderOnly)
{
	try
	{
		// Open the file.
		ifstream instr;
		instr.open(m_strFileName.c_str(), ios::in | ios::binary);

		// Check if open
		if (!instr)
		{
			m_strError = "Unable to open the MDL file for reading.";
			return false;
		}

		// Read the header.
		if (ReadFileHeader(instr) == false)
			return false;

		// Stop if just reading the header.
		if (bReadHeaderOnly)
			return true;
		
		// Read Data
		if (m_Header.GetNumProbes() > 0)
		{
			// Allocate for the probe data.
			CMDLProbeData *pProbeData;
			m_ProbeData.resize(m_Header.GetNumProbes());

			// Loop over number of probes
			char strData[MDLDATASIZE];
			MDLData data;
			for (int32_t i=0; i<m_Header.GetNumProbes(); i++)
			{
				// Read next probe data structure
				if (instr.read(strData, MDLDATASIZE))
				{					
					// Copy into probe data structor
					memcpy(&data, strData, MDLDATASIZE);
					SwapFromLittleEndian(data);

					// Copy probe data structure into probe data object
					pProbeData = &m_ProbeData[i];
					SetDataInfo(&data, pProbeData);
				}
				else
				{
					return false;
				}
			}
		}

		instr.close();
	}
	catch(...)
	{
		m_strError = "Unspecified read error.";
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////

#if defined(_USE_MEM_MAPPING_) && defined(_MSC_VER)
bool CMDLFileData::ReadFileUsingMemMap(bool bReadHeaderOnly)
{
	try
	{
		// Open the file.
		ifstream instr;
		instr.open(m_strFileName.c_str(), ios::in | ios::binary);

		// Check if open
		if (!instr)
		{
			m_strError = "Unable to open the MDL file for reading.";
			return false;
		}

		// Read the header.
		if (ReadFileHeader(instr) == false)
			return false;

		// Stop if just reading the header.
		if (bReadHeaderOnly)
			return true;

		// Get the file position at the end of the header section
		// This will be used to determine where to begin reading
		// the probe data.
		m_nHeaderOffset = instr.tellg();

		// Close the file and reopen for mapping
		instr.close();

		// Initialize the bool flags.
		m_bFileOpen = false;
		m_bFileMapped = false;

		// Create the file.
		m_hFile = CreateFile(m_strFileName.c_str(), GENERIC_READ, FILE_SHARE_READ,
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
	}
	catch(...)
	{
		m_strError = "Unspecified read error.";
		return false;
	}

	return true;
}
#endif

/////////////////////////////////////////////////////////////////

bool CMDLFileData::ReadFileHeader(std::ifstream &instr)
{
	// Read Header
	char strHeader[MDLHEADERSIZE];
	MDLHeader header;
	if (instr.read(strHeader, MDLHEADERSIZE))
	{
		memcpy(&header, strHeader, MDLHEADERSIZE);
		SwapFromLittleEndian(header);
	}
	else
	{
		m_strError = "Unable to read the file header.";
		return false;
	}

	// Copy structure read into class member
	SetHeaderInfo(&header);

	return true;
}

/////////////////////////////////////////////////////////////////

bool CMDLFileData::Write()
{
	try
	{
		ofstream outstr;
		outstr.open(m_strFileName.c_str(), ios::out | ios::binary);

		// Check if open
		if (!outstr)
		{
			m_strError = "Unable to open the MDL file for writing.";
			return false;
		}

		// Set the current time
		char buffer[256];
		time_t curtime;
		struct tm *loctime;

		// Get the current time
		curtime = time(NULL);

		// Convert it to local time representation.
		loctime = localtime (&curtime);

		// Print it out in a nice format
		strftime(buffer, 256, "%B %d %Y %I:%M%p", loctime);

		// Store date
		m_Header.SetModifiedDate(buffer);

		// Write Header
		char strHeader[MDLHEADERSIZE];
		MDLHeader header;
		GetHeaderInfo(&header);
		SwapToLittleEndian(header);
		memcpy(strHeader, &header, MDLHEADERSIZE);
		outstr.write(strHeader, MDLHEADERSIZE);

		// Make sure probe data has been allocated
		if (m_ProbeData.size() == 0)
		{
			m_strError = "Probe data has not been allocated";
			outstr.close();
			return false;
		}

		// Write Data
		char strData[MDLDATASIZE];
		MDLData data;
		for (int32_t i=0; i<m_Header.GetNumProbes(); i++)
		{
			GetDataInfo(&data, m_ProbeData[i]);
			SwapToLittleEndian(data);
			memcpy(strData, &data, MDLDATASIZE);
			outstr.write(strData, MDLDATASIZE);
		}

		outstr.close();
	}
	catch(...)
	{
		m_strError = "Unspecified write error.";
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////

bool CMDLFileData::Exists()
{
	// Find the file stats.
	struct stat st;
	return (stat(m_strFileName.c_str(), &st) == 0);
}

/////////////////////////////////////////////////////////////////

bool CMDLFileData::PrintMDLToTextFile(const char* sFile, int32_t nProbes)
{
	string strFileName = sFile;

	try
	{
		// Open file
		ofstream outstr;
		outstr.open(strFileName.c_str(), ios::out);

		// Check if open
		if (!outstr)
		{
			m_strError = "Unable to open file for writing.";
			return false;
		}

		// Print header
		outstr << "[HEADER]" << endl;
		outstr << "Version=" << m_Header.GetVersion() << endl;
		outstr << "ModifiedDate=" << m_Header.GetModifiedDate() << endl;
		outstr << "ChipType=" << m_Header.GetChipType() << endl;
		outstr << "AnalysisType=" << m_Header.GetAnalysisType() << endl;
		outstr << "Masked=" << m_Header.GetMasked() << endl;
		outstr << "NumProbes=" << m_Header.GetNumProbes() << endl;
		outstr << endl;

		// Print data
		outstr << "[DATA]" << endl;
		CMDLProbeData probeData;
		for (int32_t iprobe=0; iprobe<nProbes; iprobe++)
		{
			GetProbeData(iprobe, probeData);
			outstr << "Affinity" << iprobe << "=" << probeData.GetAffinity() << endl;
			outstr << "RelativeBkg" << iprobe << "=" << probeData.GetRelativeBkg() << endl;
			outstr << "Saturation" << iprobe << "=" << probeData.GetSaturation() << endl;
			outstr << "Offset" << iprobe << "=" << probeData.GetOffset() << endl;
			outstr << endl;
		}

		// Close file
		outstr.close();
	}
	catch(...)
	{
		m_strError = "Unspecified write error.";
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////
