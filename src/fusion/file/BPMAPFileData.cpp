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

#include "file/BPMAPFileData.h"
#include "file/FileIO.h"
//
#include <cmath>
#include <cstdio>
#include <cstdlib>
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

using namespace affxbpmap;

//////////////////////////////////////////////////////////////////////

// The older bpmap files had a corruption in the way the floating point
// values were written. This flag is used to toggle the way the values are
// read. The older files should use ReadFloatFromOldBPMAP.
static bool g_OlderBPMAPFileFormat=true;


///////////////////////////////////////////////////////////////////////////////

bool GDACSequenceHitItemType::operator<(const GDACSequenceHitItemType &rhs) const
{
  // numeric sort on position, then strand, then PM X, then PM Y, then MM X, then MM Y
   if( this->Position == rhs.Position && this->TopStrand == rhs.TopStrand && this->PMX == rhs.PMX && this->PMY == rhs.PMY && this->MMX == rhs.MMX && this->MMY < rhs.MMY) return true;

   if( this->Position == rhs.Position && this->TopStrand == rhs.TopStrand && this->PMX == rhs.PMX && this->PMY == rhs.PMY && this->MMX < rhs.MMX) return true;

   if( this->Position == rhs.Position && this->TopStrand == rhs.TopStrand && this->PMX == rhs.PMX && this->PMY < rhs.PMY) return true;

   if( this->Position == rhs.Position && this->TopStrand == rhs.TopStrand && this->PMX < rhs.PMX) return true;

   if( this->Position == rhs.Position && this->TopStrand< rhs.TopStrand) return true;

   if( this->Position < rhs.Position ) return true;

   return false;
}

//////////////////////////////////////////////////////////////////////

CBPMAPFileData::CBPMAPFileData() :
	m_NumberSequences(0),
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

bool CBPMAPFileData::Exists()
{
	// Find the file stats.
	struct stat st;
	return (stat(m_FileName.c_str(), &st) == 0);
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

  n_bytes = (unsigned short) ::ceil(n_d/(float)4); /* number of bytes we need to process */
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
	instr.open(m_FileName.c_str(), std::ios::in | std::ios::binary);

	// Check if open
	if (!instr)
	{
		m_strError = "Unable to open the file.";
		return false;
	}

	// Read the magic number
	std::string magic;
	const int magic_size = BPMAP_FILE_HEADER_LEN;
	ReadFixedString(instr, magic, magic_size);

	// check the header is correct
	if(magic != BPMAP_FILE_HEADER_BYTES){
	  m_strError = "File header is not of BPMAP type. ";
	  return false;
	}

	// Get the version number
	m_Version = 0.0f;
	ReadFloatFromOldBPMAP_N(instr, m_Version);
	g_OlderBPMAPFileFormat = true;

	// Some of the BPMAP files had the version number written incorrectly.
	// If invalid version number from the first read attempt then try again.
	if (m_Version > 3.5f || m_Version < 0.5f)
	{
		g_OlderBPMAPFileFormat = false;
		instr.seekg(magic_size, std::ios_base::beg);
		ReadFloat_N(instr, m_Version);
	}


	// Get the number of sequences
	uint32_t uval;
	ReadUInt32_N(instr, uval);
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
	instr.open(m_FileName.c_str(), std::ios::in | std::ios::binary);

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
	uint32_t uval;
	for (iSeq=0; iSeq<m_NumberSequences; iSeq++)
	{
		ReadUIntLenString_N(instr, m_SequenceItems[iSeq].m_Name);

		if (m_Version >= 3.0f)
		{
			ReadUInt32_N(instr, uval);
			m_SequenceItems[iSeq].m_ProbePairs = uval;
			ReadUInt32_N(instr, uval);
			m_SequenceItems[iSeq].m_HitStartPosition = uval;
		}

		ReadUInt32_N(instr, uval);
		m_SequenceItems[iSeq].m_NumberHits = uval;

		if ((int) (m_Version+0.1) >= 2)
		{
			ReadUIntLenString_N(instr, m_SequenceItems[iSeq].m_GroupName);
			ReadUIntLenString_N(instr, m_SequenceItems[iSeq].m_SeqVersion);

			ReadUInt32_N(instr, uval);
			m_SequenceItems[iSeq].m_Parameters.resize(uval);
			for (int iParam=0; iParam<(int)uval; iParam++)
			{
				ReadUIntLenString_N(instr, m_SequenceItems[iSeq].m_Parameters[iParam].Tag);
				ReadUIntLenString_N(instr, m_SequenceItems[iSeq].m_Parameters[iParam].Value);
			}
		}
	}


// WAS always declared regardless of _USE_MEM_MAPPING_, leading to
// a compile warning on unused variables 'probeIn' and 'probeOut'.
#ifndef _USE_MEM_MAPPING_
	// Read more sequence data
	const int probeBufSize=64;
	unsigned char probeIn[PROBE_STORAGE_BUFFER_LENGTH]="";
	char probeOut[probeBufSize]="";
#endif
	for (iSeq=0; iSeq<m_NumberSequences; iSeq++)
	{
		// Read the sequence values.
		if(m_Version > 3.0f){
		  instr.seekg(m_SequenceItems[iSeq].m_HitStartPosition);
		  ReadUInt32_N(instr, uval);
		  m_SequenceItems[iSeq].m_Number = uval;
		}
		else {
		  ReadUInt32_N(instr, uval);
		  m_SequenceItems[iSeq].m_Number = uval;
		  m_SequenceItems[iSeq].m_HitStartPosition = instr.tellg();
		}

#ifndef _USE_MEM_MAPPING_

		m_SequenceItems[iSeq].m_Hits.resize(m_SequenceItems[iSeq].m_NumberHits);
		m_SequenceItems[iSeq].m_pHits = &m_SequenceItems[iSeq].m_Hits;

		// Read the hit data (probe pair data).
		for (int iHit=0; iHit<m_SequenceItems[iSeq].m_NumberHits; iHit++)
		{
			ReadUInt32_N(instr, uval);
			m_SequenceItems[iSeq].m_Hits[iHit].PMX = uval;
			ReadUInt32_N(instr, uval);
			m_SequenceItems[iSeq].m_Hits[iHit].PMY = uval;
			if (m_SequenceItems[iSeq].m_ProbePairs == PM_MM)
			{
				ReadUInt32_N(instr, uval);
				m_SequenceItems[iSeq].m_Hits[iHit].MMX=uval;
				ReadUInt32_N(instr, uval);
				m_SequenceItems[iSeq].m_Hits[iHit].MMY=uval;
			}
			ReadUInt8(instr, m_SequenceItems[iSeq].m_Hits[iHit].ProbeLength);
			ReadFixedUCString(instr, probeIn, PROBE_STORAGE_BUFFER_LENGTH);
			byte_to_dna((char *)probeIn, PROBE_STORAGE_BUFFER_LENGTH, probeOut, m_SequenceItems[iSeq].m_Hits[iHit].ProbeLength);
			m_SequenceItems[iSeq].m_Hits[iHit].PMProbe = probeOut;
			if (g_OlderBPMAPFileFormat == true)
				ReadFloatFromOldBPMAP_N(instr, m_SequenceItems[iSeq].m_Hits[iHit].MatchScore);
			else
				ReadFloat_N(instr, m_SequenceItems[iSeq].m_Hits[iHit].MatchScore);
			ReadUInt32_N(instr, uval);
			m_SequenceItems[iSeq].m_Hits[iHit].Position = uval;
			uint8_t uval8;
			ReadUInt8(instr, uval8);
			m_SequenceItems[iSeq].m_Hits[iHit].TopStrand=uval8;
		}

#else

		// Determine the size of each hit.
		int hitSize=(m_SequenceItems[iSeq].m_ProbePairs == PM_MM ? HIT_ITEM_SIZE_WITH_PROBE_PAIRS : HIT_ITEM_SIZE_WITH_PM_ONLY);

		m_SequenceItems[iSeq].m_pHits = NULL;
		int offset = m_SequenceItems[iSeq].m_NumberHits * hitSize;
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
	m_ProbePairs(PM_MM),
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
	m_ProbePairs = orig.m_ProbePairs;
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
		// Determine the size of each hit
		int hitSize=(m_ProbePairs == PM_MM ? HIT_ITEM_SIZE_WITH_PROBE_PAIRS : HIT_ITEM_SIZE_WITH_PM_ONLY);

		// Get the offset and then the data.
		char *lpData = m_lpData + (index*hitSize);

		int offset = 0;
		hit.PMX = MmGetUInt32_N((uint32_t *)(lpData + offset));
		offset += INT_SIZE;

		hit.PMY = MmGetUInt32_N((uint32_t *)(lpData + offset));
		offset += INT_SIZE;

		if (m_ProbePairs == PM_MM)
		{
			hit.MMX = MmGetUInt32_N((uint32_t *)(lpData + offset));
			offset += INT_SIZE;

			hit.MMY = MmGetUInt32_N((uint32_t *)(lpData + offset));
			offset += INT_SIZE;
		}

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
			hit.MatchScore = MmGetFloatFromOldBPMAP_N((float *)(lpData + offset));
		else
			hit.MatchScore = MmGetFloat_N((float *)(lpData + offset));
		offset += FLOAT_SIZE;

		hit.Position = MmGetUInt32_N((uint32_t *)(lpData + offset));
		offset += INT_SIZE;

		hit.TopStrand = *(unsigned char *)(lpData+offset);
	}
}

//////////////////////////////////////////////////////////////////////

TagValuePairType CGDACSequenceItem::GetParameter(int index) { 
	  return m_Parameters[index];
 }

//////////////////////////////////////////////////////////////////////

std::string CGDACSequenceItem::FullName() const
{
	if (m_SeqVersion.length() > 0)
	{
		return (m_GroupName + ":" + m_SeqVersion + ";" + m_Name);
	}
	else
		return m_Name;
}

//////////////////////////////////////////////////////////////////////
