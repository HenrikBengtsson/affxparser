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

#include "BPMAPFileData.h"
#include "FileIO.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <istream>
#include <fstream>
#include <math.h>
#include <stdio.h>

#ifndef WIN32
#include <sys/mman.h>
#endif

#ifdef _INCLUDE_UNISTD_HEADER_
#include <unistd.h>
#endif

//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

//////////////////////////////////////////////////////////////////////

using namespace affxbpmap;

//////////////////////////////////////////////////////////////////////

// The older bpmap files had a corruption in the way the floating point
// values were written. This flag is used to toggle the way the values are
// read. The older files should use ReadFloatFromOldBPMAP.
static bool g_OlderBPMAPFileFormat=true;

//////////////////////////////////////////////////////////////////////

CBPMAPFileData::CBPMAPFileData() :
	m_NumberSequences(0),
	m_bFileOpen(false),
	m_lpFileMap(NULL),
	m_lpData(NULL),
	m_bFileMapped(false)
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

CBPMAPFileData::~CBPMAPFileData()
{
	Close();
}

//////////////////////////////////////////////////////////////////////

void CBPMAPFileData::Close()
{
	m_DataStartPosition = 0;
	m_NumberSequences = 0;
	m_SequenceItems.erase(m_SequenceItems.begin(), m_SequenceItems.end());

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
}

//////////////////////////////////////////////////////////////////////

void CBPMAPFileData::SetFileName(const char *name)
{
	std::string file = name;
	int index = (int)file.rfind(PATH_SEPARATOR);
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

bool CBPMAPFileData::Exists()
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

bool CBPMAPFileData::ReadHeader()
{
	// Read the header, Close memory if failed.
	if (ReadFile(true) == false)
	{
		Close();
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CBPMAPFileData::Read()
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

#define DNA_BASE_A     0
#define DNA_BASE_C     1
#define DNA_BASE_G     2
#define DNA_BASE_T     3
#define DNA_BASE_OTHER 4

static int int_to_acgt(unsigned int i, char *c) {
  switch(i) {
    case DNA_BASE_A:
      *c = 'A';
      break;
    case DNA_BASE_C:
      *c = 'C';
      break;
    case DNA_BASE_G:
      *c = 'G';
      break;
    case DNA_BASE_T:
      *c = 'T';
      break;
    default:
      *c = 'N';
      break;
  }

  if(*c == 'N')
    return(EXIT_FAILURE);
  else
    return(EXIT_SUCCESS);
}

//////////////////////////////////////////////////////////////////////

static int byte_to_dna(const char *b, unsigned short n_b, char *d, unsigned short n_d)
/*
** b is the array of bytes to be converted
** n_b is the length of the byte array
** d is the char array where the converted dna will be written
** n_d is the number of bases which should be extracted.
*/
{
  unsigned short n_bytes,i,j,k;

  /* Do we have enough bytes for the requested dna? */
  if(n_d > 4*n_b) {
	//fprintf(stderr,"byte_to_dna: request for %d bases from %d bytes, not possible.\n",n_d,n_b);
	return(EXIT_FAILURE);
  }

  n_bytes = ::ceil(n_d/(float)4); /* number of bytes we need to process */
  for(i=0,j=0; i<n_bytes; i++) {
	/* printf("byte_to_dna: n_bytes = %d, n_d=%d, about to translate b[%d] = %u\n",n_bytes,n_d,i,(unsigned char)b[i]); */
	for(k=0; k<4 && j<n_d; k++,j++) {
	  if(int_to_acgt((b[i] >> (2*(3-k))) & 03,d+j)==EXIT_FAILURE)
		return EXIT_FAILURE; //err_bcel("byte_to_dna: problem translating int: %d, b[%u]=%u\n",(b[i] >> (2*(3-j))) & 03,i,b[i]);
	}
  }
  d[j] = '\0';

  return(EXIT_SUCCESS);
}

//////////////////////////////////////////////////////////////////////

bool CBPMAPFileData::ReadFile(bool bReadHeaderOnly)
{
	// First Close the memory.
	Close();

	// Format the full path name.
	m_FullPathName = m_LibPath;
	m_FullPathName += PATH_SEPARATOR;
	m_FullPathName += m_FileName;

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

bool CBPMAPFileData::ReadHeaderSection()
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

	// Read the magic number
	std::string magic;
	const int magic_size = 8;
	ReadFixedString(instr, magic, magic_size);

	// Get the version number
	m_Version = 0;
	ReadFloatFromOldBPMAP(instr, m_Version);
	g_OlderBPMAPFileFormat = true;

	// Some of the BPMAP files had the version number written incorrectly.
	// If invalid version number from the first read attempt then try again.
	if (m_Version > 3.5f || m_Version < 0.5f)
	{
		g_OlderBPMAPFileFormat = false;
		instr.seekg(magic_size, std::ios_base::beg);
		ReadFloat(instr, m_Version);
	}


	// Get the number of sequences
	unsigned int uval;
	ReadUInt(instr, uval);
	m_NumberSequences = uval;


	// Allocate memory for the sequence data
	m_SequenceItems.resize(m_NumberSequences);

	// Determine the position of the start of the data
	m_DataStartPosition = instr.tellg();

	// Close the file
	instr.close();

	return true;
}

//////////////////////////////////////////////////////////////////////

bool CBPMAPFileData::ReadDataSection()
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


	// Skip to the data section
	instr.seekg(m_DataStartPosition);

	// Read the sequence data.
	int iSeq;
	unsigned int uval;
	for (iSeq=0; iSeq<m_NumberSequences; iSeq++)
	{
		ReadUIntLenString(instr, m_SequenceItems[iSeq].m_Name);
		ReadUInt(instr, uval);
		m_SequenceItems[iSeq].m_NumberHits = uval;

		if ((int) (m_Version+0.1) == 2)
		{
			ReadUIntLenString(instr, m_SequenceItems[iSeq].m_GroupName);
			ReadUIntLenString(instr, m_SequenceItems[iSeq].m_SeqVersion);
			ReadUInt(instr, uval);
			m_SequenceItems[iSeq].m_Parameters.resize(uval);
			for (int iParam=0; iParam<(int)uval; iParam++)
			{
				ReadUIntLenString(instr, m_SequenceItems[iSeq].m_Parameters[iParam].Tag);
				ReadUIntLenString(instr, m_SequenceItems[iSeq].m_Parameters[iParam].Value);
			}
		}
	}


	// Read more sequence data
	const int probeBufSize=64;
	unsigned char probeIn[PROBE_STORAGE_BUFFER_LENGTH]="";
	char probeOut[probeBufSize]="";
	for (iSeq=0; iSeq<m_NumberSequences; iSeq++)
	{
		ReadUInt(instr, uval);
		m_SequenceItems[iSeq].m_Number = uval;
		m_SequenceItems[iSeq].m_HitStartPosition = instr.tellg();

#ifndef _USE_MEM_MAPPING_

		m_SequenceItems[iSeq].m_Hits.resize(m_SequenceItems[iSeq].m_NumberHits);
		m_SequenceItems[iSeq].m_pHits = &m_SequenceItems[iSeq].m_Hits;

		// Read the hit data (probe pair data).
		for (int iHit=0; iHit<m_SequenceItems[iSeq].m_NumberHits; iHit++)
		{
			ReadUInt(instr, m_SequenceItems[iSeq].m_Hits[iHit].PMX);
			ReadUInt(instr, m_SequenceItems[iSeq].m_Hits[iHit].PMY);
			ReadUInt(instr, m_SequenceItems[iSeq].m_Hits[iHit].MMX);
			ReadUInt(instr, m_SequenceItems[iSeq].m_Hits[iHit].MMY);
			ReadUChar(instr, m_SequenceItems[iSeq].m_Hits[iHit].ProbeLength);
			ReadFixedUChar(instr, probeIn, PROBE_STORAGE_BUFFER_LENGTH);
			byte_to_dna((char *)probeIn, PROBE_STORAGE_BUFFER_LENGTH, probeOut, m_SequenceItems[iSeq].m_Hits[iHit].ProbeLength);
			m_SequenceItems[iSeq].m_Hits[iHit].PMProbe = probeOut;
			if (g_OlderBPMAPFileFormat == true)
				ReadFloatFromOldBPMAP(instr, m_SequenceItems[iSeq].m_Hits[iHit].MatchScore);
			else
				ReadFloat(instr, m_SequenceItems[iSeq].m_Hits[iHit].MatchScore);
			ReadUInt(instr, m_SequenceItems[iSeq].m_Hits[iHit].Position);
			ReadUChar(instr, m_SequenceItems[iSeq].m_Hits[iHit].TopStrand);
		}

#else

		m_SequenceItems[iSeq].m_pHits = NULL;
		int offset = m_SequenceItems[iSeq].m_NumberHits * HIT_ITEM_SIZE;
		instr.seekg(offset, std::ios_base::cur);

#endif
	}


	// Close the file
	instr.close();


#ifdef _USE_MEM_MAPPING_
	
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

	// Set the data pointer for each sequence item.
	for (iSeq=0; iSeq<m_NumberSequences; iSeq++)
	{
		m_SequenceItems[iSeq].m_bMapped = true;
		m_SequenceItems[iSeq].m_lpData = m_lpData + m_SequenceItems[iSeq].m_HitStartPosition;
	}

#endif

	return true;
}

//////////////////////////////////////////////////////////////////////

void CBPMAPFileData::GetSequenceItem(int index, CGDACSequenceItem &seq)
{
	seq.MakeShallowCopy(m_SequenceItems[index]);
}

//////////////////////////////////////////////////////////////////////

CGDACSequenceItem::CGDACSequenceItem() :
	m_Number(0),
	m_NumberHits(0),
	m_pHits(NULL),
	m_bMapped(false),
	m_lpData(NULL),
	m_HitStartPosition(0)
{
}

//////////////////////////////////////////////////////////////////////

CGDACSequenceItem::~CGDACSequenceItem()
{
	m_Hits.erase(m_Hits.begin(), m_Hits.end());
}

//////////////////////////////////////////////////////////////////////

void CGDACSequenceItem::MakeShallowCopy(CGDACSequenceItem &orig)
{
	m_Name = orig.m_Name;
	m_GroupName = orig.m_GroupName;
	m_SeqVersion = orig.m_SeqVersion;
	m_Number = orig.m_Number;
	m_NumberHits = orig.m_NumberHits;
	m_pHits = &orig.m_Hits;
	m_Parameters = orig.m_Parameters;
	m_bMapped = orig.m_bMapped;
	m_lpData = orig.m_lpData;
	m_HitStartPosition = orig.m_HitStartPosition;
}

//////////////////////////////////////////////////////////////////////

void CGDACSequenceItem::GetHitItem(int index, GDACSequenceHitItemType &hit, bool readProbe)
{
	if (m_bMapped == false)
	{
		hit = (*m_pHits)[index];
	}
	else
	{
		char *lpData = m_lpData + (index*HIT_ITEM_SIZE);

		int offset = 0;
		ReadUInt(lpData + offset, hit.PMX);
		offset += UINT_SIZE;

		ReadUInt(lpData + offset, hit.PMY);
		offset += UINT_SIZE;

		ReadUInt(lpData + offset, hit.MMX);
		offset += UINT_SIZE;

		ReadUInt(lpData + offset, hit.MMY);
		offset += UINT_SIZE;

		hit.ProbeLength = *(unsigned char *)(lpData+offset);
		offset += UCHAR_SIZE;

		if (readProbe == true)
		{
			const int probeBufSize=64;
			char probeOut[probeBufSize];
			const char *probeIn = lpData + offset;
			byte_to_dna(probeIn, PROBE_STORAGE_BUFFER_LENGTH, probeOut, hit.ProbeLength);
			hit.PMProbe = probeOut;
		}
		offset += PROBE_STORAGE_BUFFER_LENGTH;

		if (g_OlderBPMAPFileFormat == true)
			ReadFloatFromOldBPMAP(lpData + offset, hit.MatchScore);
		else
			ReadFloat(lpData + offset, hit.MatchScore);
		offset += FLOAT_SIZE;

		ReadUInt(lpData + offset, hit.Position);
		offset += UINT_SIZE;

		hit.TopStrand = *(unsigned char *)(lpData+offset);
	}
}

//////////////////////////////////////////////////////////////////////
