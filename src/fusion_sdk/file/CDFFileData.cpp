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
#include "file/CDFFileData.h"
//
#include "file/FileIO.h"
//
#include "portability/affy-base-types.h"
//
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <istream>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
//

#ifndef _MSC_VER
#include <sys/mman.h>
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4996) // don't show deprecated warnings.
#endif

#ifdef _INCLUDE_UNISTD_HEADER_
#include <unistd.h>
#endif

using namespace affxcdf;

//////////////////////////////////////////////////////////////////////

// ascii(67) == 0x43 ==> 'C'
#define CDF_FILE_MAGIC_NUMBER 67
#define CDF_FILE_VERSION_NUMBER 4

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

CCDFFileData::CCDFFileData()
{
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
    m_TBase(' '),
    m_ProbeLength(0),
    m_ProbeGrouping(0)
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
    m_WobbleSituation(0),
    m_AlleleCode(0),
    m_Channel(0),
    m_RepType(0),
    m_NumCellsPerList(0),
    m_Direction(0),
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
    m_WobbleSituation = orig.m_WobbleSituation;
    m_AlleleCode = orig.m_AlleleCode;
    m_Channel = orig.m_Channel;
    m_RepType = orig.m_RepType;
    m_pCells = &orig.m_Cells;
}

//////////////////////////////////////////////////////////////////////

void CCDFProbeGroupInformation::GetCell(int cell_index, CCDFProbeInformation & info)
{
    info = (*m_pCells)[cell_index];
}

//////////////////////////////////////////////////////////////////////

CCDFQCProbeInformation::CCDFQCProbeInformation() :
    m_X(0),
    m_Y(0),
    m_PLen(0),
    m_PMProbe(0),
    m_Background(0)
{
}

//////////////////////////////////////////////////////////////////////

CCDFQCProbeSetInformation::CCDFQCProbeSetInformation() :
    m_NumCells(0),
    m_QCProbeSetType(UnknownQCProbeSetType),
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
}

//////////////////////////////////////////////////////////////////////

void CCDFQCProbeSetInformation::GetProbeInformation(int index, CCDFQCProbeInformation & info)
{
    info = (*m_pCells)[index];;
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
}

//////////////////////////////////////////////////////////////////////

void CCDFProbeSetInformation::GetGroupInformation(int index, CCDFProbeGroupInformation & info)
{
    info.MakeShallowCopy((*m_pGroups)[index]);
}

//////////////////////////////////////////////////////////////////////

CCDFProbeSetNames::CCDFProbeSetNames()
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
    return m_ProbeSetNames[index];
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

std::string CCDFFileData::GetProbeSetName(int index)
{
  if (iteratorReader.is_open() == false) {
    return m_ProbeSetNames.GetName(index);
  }

  int loc = (int)probeSetNamePos + (index*MAX_PROBE_SET_NAME_LENGTH);
  seekg(loc, std::ios::beg);
  char name[MAX_PROBE_SET_NAME_LENGTH + 1];
  ReadFixedCString(iteratorReader, name, MAX_PROBE_SET_NAME_LENGTH);
  return name;
}

//////////////////////////////////////////////////////////////////////

void CCDFFileData::Close()
{
    if (iteratorReader.is_open() == true)
        iteratorReader.close();
    m_ProbeSets.clear();
    m_QCProbeSets.clear();
    m_ProbeSetNames.Clear();
}

//////////////////////////////////////////////////////////////////////

bool CCDFFileData::IsXDACompatibleFile()
{
    // Open the file.
    std::ifstream instr(m_FileName.c_str(), std::ios::in | std::ios::binary);
    if (!instr)
        return false;

    // Read the magic number from the file.
    int32_t magic=0;
    ReadInt32_I(instr, magic);
    bool bXDAFile = (magic == CDF_FILE_MAGIC_NUMBER);
    instr.close();

    return bXDAFile;
}

//////////////////////////////////////////////////////////////////////

std::string CCDFFileData::GetGUID()
{
    std::string guid = "";
		
	bool bXDAFile = IsXDACompatibleFile();

	if (m_Header.GetFormatVersion() == 0)
	{
		if (!ReadHeader())
			return guid;
	}
	if ((bXDAFile && (m_Header.GetFormatVersion() >= 4)) || (!bXDAFile && (m_Header.GetFormatVersion() >= 6)))
	{
		guid = m_Header.GetGUID();
	}

	return guid;
}

//////////////////////////////////////////////////////////////////////

std::string CCDFFileData::GetIntegrityMd5()
{
    std::string integrityMd5 = "";
		
	bool bXDAFile = IsXDACompatibleFile();

	if (m_Header.GetFormatVersion() == 0)
	{
		if (!ReadHeader())
			return integrityMd5;
	}
	if ((bXDAFile && (m_Header.GetFormatVersion() >= 4)) || (!bXDAFile && (m_Header.GetFormatVersion() >= 6)))
	{
		integrityMd5 = m_Header.GetIntegrityMd5();
	}

	return integrityMd5;
}

//////////////////////////////////////////////////////////////////////

std::string CCDFFileData::GetChipType()
{
    std::string chiptype;
    if (m_FileName.empty() == false)
    {
		bool bXDAFile = IsXDACompatibleFile();

		if (m_Header.GetFormatVersion() == 0)
		{
			if (!ReadHeader())
				return chiptype;
		}
		if ((bXDAFile && (m_Header.GetFormatVersion() < 4)) || (!bXDAFile && (m_Header.GetFormatVersion() < 6)))
		{
			int index = (int) m_FileName.rfind('\\');
			if (index == -1)
				index = (int) m_FileName.rfind('/');
			chiptype = m_FileName.c_str() + index + 1;
			chiptype.resize(chiptype.length()-4);
		}
		else
		{
			chiptype = m_Header.GetChipType();
		}
    }
    return chiptype;
}

//////////////////////////////////////////////////////////////////////

std::vector<std::string> CCDFFileData::GetChipTypes()
{
    std::vector<std::string> chiptypes;
    std::string chiptype;
    if (m_FileName.empty() == false)
    {
		bool bXDAFile = IsXDACompatibleFile();

		if (m_Header.GetFormatVersion() == 0)
		{
			if (!ReadHeader())
				return chiptypes;
		}
		if ((bXDAFile && (m_Header.GetFormatVersion() < 4)) || (!bXDAFile && (m_Header.GetFormatVersion() < 6)))
		{
			int index = (int) m_FileName.rfind('\\');
			if (index == -1)
				index = (int) m_FileName.rfind('/');
			chiptype = m_FileName.c_str() + index + 1;
			chiptype.resize(chiptype.length()-4);

			// The full file name (minus .cdf extension) is the default (1st)
			// chip type. This matches what GetChipType() returns.
			// ie: foo.bar.v1.r2.cdf -> foo.bar.v1.r2
			chiptypes.push_back(chiptype);

			//We then add all substrings starting at zero and ending at '.'
			// ie: foo.bar.v1.r2.cdf -> foo.bar.v1, foo.bar, foo
		    // ie: chiptypes contain foo.bar.v1.r2, foo.bar.v1, foo.bar, foo
			std::string::size_type pos = chiptype.rfind(".",chiptype.size()-1);
			while (pos != std::string::npos){
				if(pos>0)
					chiptypes.push_back(chiptype.substr(0,pos));
				pos = chiptype.rfind(".",pos-1);
			}
		}
		else
		{
			chiptypes = m_Header.GetChipTypes();
		}
	}
    return chiptypes;
}

//////////////////////////////////////////////////////////////////////

bool CCDFFileData::Exists()
{
    // Find the file stats.
    struct stat st;
    return (stat(m_FileName.c_str(), &st) == 0);
}

//////////////////////////////////////////////////////////////////////

bool CCDFFileData::ReadHeader()
{
    // Read the header, close if failed.
    readHeaderOnly = true;
    if (Open() == false)
    {
        Close();
        return false;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////

GeneChipProbeSetType CCDFFileData::GetProbeSetType(int index)
{
    if (iteratorReader.is_open() == false) {
        return m_ProbeSets[index].GetProbeSetType();
  }

  uint32_t i_pos;
  uint32_t p_pos;

  // Get the probe set position from the index part of the file then seek to it.
  i_pos=(uint32_t) probeSetIndexPos + (index*sizeof(uint32_t));
  iteratorReader.seekg(i_pos, std::ios::beg); // the seekg method will be called later.
  ReadUInt32_I(iteratorReader,p_pos);
  // skip to the record
  seekg(p_pos, std::ios::beg);

  // Now grab the probeset type.
  uint16_t usval;
  ReadUInt16_I(iteratorReader, usval);
  return (GeneChipProbeSetType)(usval);
}

//////////////////////////////////////////////////////////////////////

void CCDFFileData::GetProbeSetInformation(int index, CCDFProbeSetInformation & info)
{
    if (iteratorReader.is_open() == false) {
        info.MakeShallowCopy(m_ProbeSets[index]);
    return;
  }

  uint32_t i_pos;
  uint32_t p_pos;

  // Is this a sequential read?
  // Are we just reading the next record?
  // If so no, need to seek around.
  // NOTE: Of course this expects the records to be next to each other.
  //       |Rec:1......|Rec:2......|Rec:.....
  if ((m_probeSetIndex_last_valid!=1)||(index!=m_probeSetIndex_last+1)) {
    // Nope! Look it it in the index.
    i_pos = (uint32_t)probeSetIndexPos + (index*sizeof(uint32_t));
    iteratorReader.seekg(i_pos, std::ios::beg);
    ReadUInt32_I(iteratorReader, p_pos);
    // now seek to the probe pos.
    iteratorReader.seekg(p_pos,std::ios::beg);
  }
  // remember which index we just read.
  m_probeSetIndex_last_valid=1;
  m_probeSetIndex_last=index;

  // Read the data
  uint16_t usval;
  uint8_t ucval;
  int32_t ival;

  info.m_Index = index;
  ReadUInt16_I(iteratorReader, usval);
  info.m_ProbeSetType = usval;
  ReadUInt8(iteratorReader, ucval);
  info.m_Direction = ucval;
  ReadInt32_I(iteratorReader, ival);
  info.m_NumLists = ival;
  ReadInt32_I(iteratorReader, ival);
  info.m_NumGroups = ival;
  ReadInt32_I(iteratorReader, ival);
  info.m_NumCells = ival;
  ReadInt32_I(iteratorReader, ival);
  info.m_ProbeSetNumber = ival;
  ReadUInt8(iteratorReader, ucval);
  info.m_NumCellsPerList = ucval;

  // Read the Groups
  CCDFProbeGroupInformation *pBlk;
  info.m_Groups.resize(info.m_NumGroups);
  info.m_pGroups = &info.m_Groups;
  for (int j=0; j<info.m_NumGroups; j++)
        {
            pBlk = &info.m_Groups[j];
            pBlk->m_GroupIndex = j;

            // Group info
            ReadInt32_I(iteratorReader, ival);
            pBlk->m_NumLists = ival;
            ReadInt32_I(iteratorReader, ival);
            pBlk->m_NumCells = ival;
            ReadUInt8(iteratorReader, ucval);
            pBlk->m_NumCellsPerList = ucval;
            ReadUInt8(iteratorReader, ucval);
            pBlk->m_Direction = ucval;
            ReadInt32_I(iteratorReader, ival);
            pBlk->m_Start = ival;
            ReadInt32_I(iteratorReader, ival);
            pBlk->m_Stop = ival;
            ReadFixedString(iteratorReader, pBlk->m_Name, MAX_PROBE_SET_NAME_LENGTH);
            if (m_Header.m_Version >= 2)
            {
                ReadUInt16_I(iteratorReader, usval);
                pBlk->m_WobbleSituation = usval;
                ReadUInt16_I(iteratorReader, usval);
                pBlk->m_AlleleCode = usval;
            }
            if (m_Header.m_Version >= 3)
            {
                ReadUInt8(iteratorReader, ucval);
                pBlk->m_Channel = ucval;
                ReadUInt8(iteratorReader, ucval);
                pBlk->m_RepType = ucval;
            }

            // Read the cells
            CCDFProbeInformation *pCell;
            pBlk->m_Cells.resize(pBlk->m_NumCells);
            pBlk->m_pCells = &pBlk->m_Cells;
            for (int k=0; k<pBlk->m_NumCells; k++)
            {
                pCell = &pBlk->m_Cells[k];

                // Cell info.
                ReadInt32_I(iteratorReader, ival);
                pCell->m_ListIndex = ival;
                ReadUInt16_I(iteratorReader, usval);
                pCell->m_X = usval;
                ReadUInt16_I(iteratorReader, usval);
                pCell->m_Y = usval;
                ReadInt32_I(iteratorReader, ival);
                pCell->m_Expos = ival;
                ReadUInt8(iteratorReader,ucval);
                pCell->m_PBase = ucval;
                ReadUInt8(iteratorReader,ucval);
                pCell->m_TBase = ucval;

                if (k==0)
                    pBlk->m_Start = pCell->m_ListIndex;
                else if (k == pBlk->m_NumCells-1)
                    pBlk->m_Stop = pCell->m_ListIndex;

                if (m_Header.m_Version >= 2)
                {
                    ReadUInt16_I(iteratorReader, usval);
                    pCell->m_ProbeLength = usval;
                    ReadUInt16_I(iteratorReader, usval);
                    pCell->m_ProbeGrouping = usval;
                }
            }
        }
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
    if (iteratorReader.is_open() == false)
        info.MakeShallowCopy(m_QCProbeSets[index]);
    else
    {
        // Get the QC position from the index part of the file then seek to it.
        int32_t pos = (int32_t)qcSetIndexPos + (index*sizeof(int32_t));
        seekg(pos, std::ios::beg);
        ReadInt32_I(iteratorReader, pos);
        seekg(pos, std::ios::beg);

        // Read the data
        uint16_t usval;
        int32_t ival;
        unsigned char ucval;

        ReadUInt16_I(iteratorReader, usval);
        ReadInt32_I(iteratorReader, ival);

        info.m_NumCells = ival;
        info.m_QCProbeSetType = usval;
        info.m_Cells.resize(info.m_NumCells);
        info.m_pCells = &info.m_Cells;

        // Read the cells
        for (int j=0; j<info.m_NumCells; j++)
        {
            ReadUInt16_I(iteratorReader, usval);
            info.m_Cells[j].m_X = usval;

            ReadUInt16_I(iteratorReader, usval);
            info.m_Cells[j].m_Y = usval;

            ReadUInt8(iteratorReader, ucval);
            info.m_Cells[j].m_PLen = ucval;

            ReadUInt8(iteratorReader, ucval);
            info.m_Cells[j].m_PMProbe = ucval;

            ReadUInt8(iteratorReader, ucval);
            info.m_Cells[j].m_Background = ucval;
        }
    }
}

//////////////////////////////////////////////////////////////////////

bool CCDFFileData::Read()
{
    // Read the file
    readHeaderOnly = false;
    if (Open() == false)
    {
        Close();
        return false;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////

bool CCDFFileData::Open()
{
    // First close the file.
    Close();

    // Open the file.
    if (IsXDACompatibleFile())
        return ReadXDAFormat();
    else
        return ReadTextFormat();
}

//////////////////////////////////////////////////////////////////////

bool CCDFFileData::ReadXDAHeader()
{
    // Extact the magic and version numbers.
    int32_t ival;
    ReadInt32_I(iteratorReader, ival);
    m_Header.m_Magic = ival;
    ReadInt32_I(iteratorReader, ival);
    m_Header.m_Version = ival;

    // Check the values for the right format file.
    if (m_Header.m_Magic != CDF_FILE_MAGIC_NUMBER || m_Header.m_Version > CDF_FILE_VERSION_NUMBER)
    {
        m_strError = "The file does not appear to be the correct format.";
        return false;
    }

	// Read guid, integrity md5, and chip type if format version is 4 or above
	if (m_Header.m_Version >= 4)
	{
		ReadString_I(iteratorReader, m_Header.m_GUID);
		ReadFixedString(iteratorReader, m_Header.m_IntegrityMd5, INTEGRITY_MD5_LENGTH);

		uint8_t numChipTypes;
		std::string chiptype;
		m_Header.m_ChipType = "";
		m_Header.m_ChipTypes.clear();
		ReadUInt8(iteratorReader, numChipTypes);
		for (uint8_t count = 0; count < numChipTypes; count++)
		{
			ReadString_I(iteratorReader, chiptype);
			m_Header.m_ChipTypes.push_back(chiptype);
			if ((m_Header.m_ChipType.empty() == true) && (chiptype.find(".") == std::string::npos))
			{
				m_Header.m_ChipType = chiptype;
			}
		}
		if (m_Header.m_ChipTypes.size() == 0)
		{
			m_strError = "The file does not contain chip types.";
			return false;
		}
		else if (m_Header.m_ChipType.empty() == true)
		{
			chiptype = m_Header.m_ChipTypes.at(0);
			std::string::size_type pos = chiptype.rfind(".", chiptype.size() - 1);
			while (pos != std::string::npos)
			{
				if(pos > 0)
					m_Header.m_ChipType = chiptype.substr(0, pos);
				pos = chiptype.rfind(".", pos - 1);
			}
		}
	}

    // Read the remaining header.
    uint16_t uval;
    ReadUInt16_I(iteratorReader, uval);
    m_Header.m_Cols = uval;
    ReadUInt16_I(iteratorReader, uval);
    m_Header.m_Rows = uval;
    ReadInt32_I(iteratorReader, ival);
    m_Header.m_NumProbeSets = ival;
    ReadInt32_I(iteratorReader, ival);
    m_Header.m_NumQCProbeSets = ival;
    ReadString_I(iteratorReader,m_Header.m_Reference);
    return true;
}

//////////////////////////////////////////////////////////////////////

bool CCDFFileData::ReadXDAFormat()
{
    // Open the file.
    iteratorReader.open(m_FileName.c_str(), std::ios::in | std::ios::binary);

    // Check if open
    if (!iteratorReader)
    {
        m_strError = "Unable to open the file.";
        return false;
    }

    // Read the header.
    if (ReadXDAHeader() == false)
        return false;

    // Save the probe set name position
    probeSetNamePos = iteratorReader.tellg();
    // Skip the probe set names
    seekg(MAX_PROBE_SET_NAME_LENGTH * m_Header.m_NumProbeSets, std::ios::cur);

    // remember the start of the qc index
    qcSetIndexPos = iteratorReader.tellg();
    // Skip it
    seekg(m_Header.m_NumQCProbeSets * sizeof(uint32_t), std::ios::cur);

    // remember the start of the probeset index
    probeSetIndexPos = iteratorReader.tellg();
    // invalidate
    m_probeSetIndex_last_valid=0;

    return true;
}

//////////////////////////////////////////////////////////////////////

bool CCDFFileData::ReadTextFormat()
{
    // Open the file.
    std::ifstream instr;
    instr.open(m_FileName.c_str(), std::ios::in);

    // Check if open
    if (!instr)
    {
        m_strError = "Unable to open the file.";
        return false;
    }

    const int MAXLINELENGTH = 4096;
    char str[MAXLINELENGTH];
    char *subStr;
    char *buffer = NULL;
    const char *CDFVERSION1 = "GC1.0";
    const char *CDFVERSION2 = "GC2.0";
    const char *CDFVERSION3 = "GC3.0";
    const char *CDFVERSION4 = "GC4.0";
    const char *CDFVERSION5 = "GC5.0";
    const char *CDFVERSION6 = "GC6.0";

    // Get the CDF section.
    ReadNextLine(instr, str, MAXLINELENGTH);
    if (strncmp( str, "[CDF]", 5) != 0)
    {
        m_strError = "Unknown file format.";
        return false;
    }

    // Get the version number.
    ReadNextLine(instr, str, MAXLINELENGTH);
    subStr=strchr(str,'=')+1;
    if ( strncmp( subStr, CDFVERSION1, strlen(CDFVERSION1)) == 0)
        m_Header.m_Version = 1;
    else if ( strncmp( subStr, CDFVERSION2, strlen(CDFVERSION2)) == 0)
        m_Header.m_Version = 2;
    else if ( strncmp( subStr, CDFVERSION3, strlen(CDFVERSION3)) == 0)
        m_Header.m_Version = 3;
    else if ( strncmp( subStr, CDFVERSION4, strlen(CDFVERSION4)) == 0)
        m_Header.m_Version = 4;
    else if ( strncmp( subStr, CDFVERSION5, strlen(CDFVERSION5)) == 0)
        m_Header.m_Version = 5;
    else if ( strncmp( subStr, CDFVERSION6, strlen(CDFVERSION6)) == 0)
        m_Header.m_Version = 6;

	if (m_Header.m_Version >= 6)
	{
		// Get the guid.
		ReadNextLine(instr, str, MAXLINELENGTH);
		subStr=strchr(str,'=')+1;
		m_Header.m_GUID = subStr;

		// Get the integrity md5.
		ReadNextLine(instr, str, MAXLINELENGTH);
		subStr=strchr(str,'=')+1;
		m_Header.m_IntegrityMd5 = subStr;
	}

	// Get the next section.
    ReadNextLine(instr, str, MAXLINELENGTH); // [Chip]
    ReadNextLine(instr, str, MAXLINELENGTH); // name
	if (m_Header.m_Version >= 6)
	{
		std::string chiptype;
		m_Header.m_ChipType = "";
		m_Header.m_ChipTypes.clear();
		ReadNextLine(instr, str, MAXLINELENGTH); // chiptype
		while (strncmp(str, "ChipType=", 9) == 0)
		{
			chiptype = strchr(str, '=')+1;
			m_Header.m_ChipTypes.push_back(chiptype);
			if ((m_Header.m_ChipType.empty() == true) && (chiptype.find(".") == std::string::npos))
			{
				m_Header.m_ChipType = chiptype;
			}
			ReadNextLine(instr, str, MAXLINELENGTH); 
		}
		if (m_Header.m_ChipType.empty() == true)
		{
			chiptype = m_Header.m_ChipTypes.at(0);
			std::string::size_type pos = chiptype.rfind(".", chiptype.size() - 1);
			while (pos != std::string::npos)
			{
				if(pos > 0)
					m_Header.m_ChipType = chiptype.substr(0, pos);
				pos = chiptype.rfind(".", pos - 1);
			}
		}
	}
	else
		ReadNextLine(instr, str, MAXLINELENGTH); // rows
    subStr = strchr(str, '=')+1;
    m_Header.m_Rows = atoi(subStr);
    ReadNextLine(instr, str, MAXLINELENGTH); // cols
    subStr=strchr(str,'=')+1;
    m_Header.m_Cols = atoi(subStr);
    ReadNextLine(instr, str, MAXLINELENGTH); // #ProbeSets
    subStr=strchr(str,'=')+1;
    m_Header.m_NumProbeSets = atoi(subStr);
    ReadNextLine(instr, str, MAXLINELENGTH); // max ProbeSet number
    m_Header.m_NumQCProbeSets = 0;
    if (m_Header.m_Version > 1)
    {
        ReadNextLine(instr, str, MAXLINELENGTH); // #qc ProbeSets
        subStr=strchr(str,'=')+1;
        m_Header.m_NumQCProbeSets = atoi(subStr);
        char strref[400000];
        ReadNextLine(instr, strref, 400000);    // The reference string.
        subStr=strchr(strref,'=')+1;
        m_Header.m_Reference = subStr;
    }


    // Stop if just reading the header.
    if (readHeaderOnly)
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

        ReadNextLine(instr, str, MAXLINELENGTH);    // label [QCUnit...]
        ReadNextLine(instr, str, MAXLINELENGTH);    // type
        subStr=strchr(str,'=')+1;
        pQCProbeSet->m_QCProbeSetType = atoi( subStr);
        ReadNextLine(instr, str, MAXLINELENGTH);    // #cells
        subStr=strchr(str,'=')+1;
        pQCProbeSet->m_NumCells = atoi( subStr);
        ReadNextLine(instr, str, MAXLINELENGTH);    // cell header

        // Read the QC cells.
        int xqc;
        int yqc;
        int plenqc;
        CCDFQCProbeInformation *pQCCell;
        pQCProbeSet->m_Cells.resize(pQCProbeSet->m_NumCells);
        for (int iqccell=0; iqccell<pQCProbeSet->m_NumCells; iqccell++)
        {
            pQCCell = &pQCProbeSet->m_Cells[iqccell];

            ReadNextLine(instr, str, MAXLINELENGTH);
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


    // Skip until the ProbeSet section is found
NextProbeSet:
    while (1)
    {
        ReadNextLine(instr, str, MAXLINELENGTH);
        if (strlen(str) > 5 && strncmp(str, "[Unit", 5) == 0 && !strchr(str, '_'))
            break;

        if (instr.eof())
            return true;
    }
        bool expectMisMatch = false;
    // ProbeSet info.
    pProbeSet = &m_ProbeSets[iProbeSet];
    pProbeSet->m_Index = iProbeSet;
    ReadNextLine(instr, str, MAXLINELENGTH); // name (ignore)
    subStr=strchr(str,'=')+1;
    m_ProbeSetNames.SetName(iProbeSet, subStr);
    ReadNextLine(instr, str, MAXLINELENGTH); // direction
    subStr=strchr(str,'=')+1;
    pProbeSet->m_Direction = atoi(subStr);
    ReadNextLine(instr, str, MAXLINELENGTH); // # Lists
    subStr=strchr(str,'=')+1;
    int NumCellsPerList=0;
    if (sscanf(subStr, "%d %d", &pProbeSet->m_NumLists, &NumCellsPerList) != 2)
        NumCellsPerList = 0;
    pProbeSet->m_NumCellsPerList = NumCellsPerList;
    ReadNextLine(instr, str, MAXLINELENGTH); // # cells
    subStr=strchr(str,'=')+1;
    pProbeSet->m_NumCells = atoi(subStr);
    ReadNextLine(instr, str, MAXLINELENGTH); // ProbeSet number
    subStr=strchr(str,'=')+1;
    pProbeSet->m_ProbeSetNumber = atoi(subStr);
    ReadNextLine(instr, str, MAXLINELENGTH); // type
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
        UNIVERSAL_TILE,
        COPY_NUMBER_TILE,
        GENOTYPE_CONTROL_TILE,
        EXPRESSION_CONTROL_TILE,
        MARKER_TILE,
        MULTICHANNEL_MARKER_TILE
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

    case COPY_NUMBER_TILE:
        pProbeSet->m_ProbeSetType = CopyNumberProbeSetType;
        break;

    case GENOTYPE_CONTROL_TILE:
        pProbeSet->m_ProbeSetType = GenotypeControlProbeSetType;
        break;

    case EXPRESSION_CONTROL_TILE:
        pProbeSet->m_ProbeSetType = ExpressionControlProbeSetType;
        break;

    case MARKER_TILE:
        pProbeSet->m_ProbeSetType = MarkerProbeSetType;
        break;

    case MULTICHANNEL_MARKER_TILE:
        pProbeSet->m_ProbeSetType = MultichannelMarkerProbeSetType;
        break;

    default:
        pProbeSet->m_ProbeSetType = UnknownProbeSetType;
        break;
    }

    ReadNextLine(instr, str, MAXLINELENGTH); // # blocks
    subStr=strchr(str,'=')+1;
    pProbeSet->m_NumGroups = atoi(subStr);

    // Determine the number of cells per List if not specified
    // in the CDF file.
    if (pProbeSet->m_NumCellsPerList == 0)
    {
        if ((pProbeSet->m_ProbeSetType == GenotypingProbeSetType) ||
            (pProbeSet->m_ProbeSetType == ResequencingProbeSetType) ||
            (pProbeSet->m_ProbeSetType == TagProbeSetType) ||
            ((pProbeSet->m_ProbeSetType == UnknownProbeSetType) &&
            ((pProbeSet->m_NumLists != 0) && ((pProbeSet->m_NumCells / pProbeSet->m_NumLists) == 4))) )
            {
                pProbeSet->m_NumCellsPerList = 4;
            }
        else if (pProbeSet->m_ProbeSetType == ExpressionProbeSetType ||
            pProbeSet->m_ProbeSetType == CopyNumberProbeSetType ||
            pProbeSet->m_ProbeSetType == GenotypeControlProbeSetType ||
            pProbeSet->m_ProbeSetType == ExpressionControlProbeSetType)
        {
            if(pProbeSet->m_NumLists != 0 &&
               pProbeSet->m_NumCells / pProbeSet->m_NumLists < 255)
                pProbeSet->m_NumCellsPerList = pProbeSet->m_NumCells / pProbeSet->m_NumLists;
            else
                pProbeSet->m_NumCellsPerList = 1;
        }
        else
        {
            pProbeSet->m_NumCellsPerList = 1;
        }
    }
    // Sanity check for relationship of m_NumCellsPerList, m_NumCells and m_NumLists
    if(pProbeSet->m_NumLists != 0 &&
       pProbeSet->m_NumCells / pProbeSet->m_NumLists < 255 &&
       pProbeSet->m_NumCellsPerList != pProbeSet->m_NumCells / pProbeSet->m_NumLists) {
        assert(0 &&
             "CCDFFileData::ReadTextFormat(): m_NumCellsPerList != pProbeSet->m_NumCells / pProbeSet->m_NumLists");
    }

    // If this is an expression probe set and we have 2 cells per list set expectMisMatch flag.
    if(pProbeSet->m_ProbeSetType == ExpressionProbeSetType &&
       pProbeSet->m_NumCellsPerList == 2)
        expectMisMatch = true;

    // Get the mutation type if block tile. ignore.
    if (pProbeSet->m_ProbeSetType == GenotypingProbeSetType && m_Header.m_Version > 1)
        ReadNextLine(instr, str, MAXLINELENGTH);


    // Read the blocks.
    CCDFProbeGroupInformation *pBlk;
    pProbeSet->m_Groups.resize(pProbeSet->m_NumGroups);
    for (int iGroup=0; iGroup<pProbeSet->m_NumGroups; iGroup++)
    {
        pBlk = &pProbeSet->m_Groups[iGroup];
        pBlk->m_GroupIndex = iGroup;
        pBlk->m_ProbeSetIndex = iProbeSet;

        ReadNextLine(instr, str, MAXLINELENGTH); // section name - ignore
        ReadNextLine(instr, str, MAXLINELENGTH); // name
        subStr=strchr(str,'=')+1;
        pBlk->m_Name = subStr;

        if (pProbeSet->m_ProbeSetType == ExpressionProbeSetType)
            m_ProbeSetNames.SetName(iProbeSet, subStr);

        ReadNextLine(instr, str, MAXLINELENGTH); // block number - ignore.
        if (pProbeSet->m_ProbeSetType == MarkerProbeSetType && m_Header.m_Version > 3)
        {
            ReadNextLine(instr, str, MAXLINELENGTH);
            subStr=strchr(str,'=')+1;
            pBlk->m_WobbleSituation = (uint16_t) strtoul(subStr, &buffer, 10);
            ReadNextLine(instr, str, MAXLINELENGTH);
            subStr=strchr(str,'=')+1;
            pBlk->m_AlleleCode = (uint16_t) strtoul(subStr, &buffer, 10);
        }
        if (pProbeSet->m_ProbeSetType == MultichannelMarkerProbeSetType && m_Header.m_Version > 4)
        {
            ReadNextLine(instr, str, MAXLINELENGTH);
            subStr=strchr(str,'=')+1;
            pBlk->m_WobbleSituation = (uint16_t) strtoul(subStr, &buffer, 10);
            ReadNextLine(instr, str, MAXLINELENGTH);
            subStr=strchr(str,'=')+1;
            pBlk->m_AlleleCode = (uint16_t) strtoul(subStr, &buffer, 10);
            ReadNextLine(instr, str, MAXLINELENGTH);
            subStr=strchr(str,'=')+1;
            pBlk->m_Channel = (uint8_t) strtoul(subStr, &buffer, 10);
            ReadNextLine(instr, str, MAXLINELENGTH);
            subStr=strchr(str,'=')+1;
            pBlk->m_RepType = (uint8_t) strtoul(subStr, &buffer, 10);
        }
        ReadNextLine(instr, str, MAXLINELENGTH); // number of Lists.
        subStr=strchr(str,'=')+1;
        pBlk->m_NumLists = atoi(subStr);
        ReadNextLine(instr, str, MAXLINELENGTH); // number of cells
        subStr=strchr(str,'=')+1;
        pBlk->m_NumCells = atoi(subStr);
        ReadNextLine(instr, str, MAXLINELENGTH); // start position.
        subStr=strchr(str,'=')+1;
        pBlk->m_Start = atoi(subStr);
        ReadNextLine(instr, str, MAXLINELENGTH); // stop position
        subStr=strchr(str,'=')+1;
        pBlk->m_Stop = atoi(subStr);
        pBlk->m_NumCellsPerList = pProbeSet->m_NumCellsPerList;
        if ((pProbeSet->m_ProbeSetType == GenotypingProbeSetType && m_Header.m_Version > 2) ||
            ((pProbeSet->m_ProbeSetType == MarkerProbeSetType || pProbeSet->m_ProbeSetType == CopyNumberProbeSetType) && m_Header.m_Version > 3) ||
            (pProbeSet->m_ProbeSetType == MultichannelMarkerProbeSetType && m_Header.m_Version > 4))
        {
            ReadNextLine(instr, str, MAXLINELENGTH);
            subStr=strchr(str,'=')+1;
            pBlk->m_Direction = atoi(subStr);
        }
        else
            pBlk->m_Direction = pProbeSet->m_Direction;

        // Read the cells.
        ReadNextLine(instr, str, MAXLINELENGTH); // header
        CCDFProbeInformation cell;
        pBlk->m_Cells.resize(pBlk->m_NumCells);
        char unusedstr[64];
        int unusedint;
        unsigned int cellIndex;
        int x,y;
        for (int iCell=0; iCell<pBlk->m_NumCells; iCell++)
        {
            ReadNextLine(instr, str, MAXLINELENGTH);
            subStr = strchr(str, '=')+1;
            int scanCount = 0;
            if (m_Header.m_Version > 3)
            {
                scanCount = sscanf(subStr, "%d %d %s %s %s %d %hu %d %*c %c %c %d %d %hu",
                                               &x,
                                               &y,
                                               unusedstr,
                                               unusedstr,
                                               unusedstr,
                                               &cell.m_Expos,
                                               &cell.m_ProbeLength,
                                               &unusedint,
                                               &cell.m_PBase,
                                               &cell.m_TBase,
                                               &cell.m_ListIndex,
                                               &unusedint,
                                               &cell.m_ProbeGrouping);
                if(scanCount != 13) {
                  m_strError = "Didn't get 13 entries in scan.";
                  return false;
                }
            }
            else
            {
                scanCount = sscanf(subStr, "%d %d %s %s %s %d %d %*c %c %c %d",
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
                if(scanCount != 10) {
                  m_strError = "Didn't get 10 entries in scan.";
                  return false;
                }
            }
            cell.m_X = x;
            cell.m_Y = y;

            if (pProbeSet->m_ProbeSetType == ExpressionProbeSetType)
            {
                cellIndex = (iCell / pProbeSet->m_NumCellsPerList) * pProbeSet->m_NumCellsPerList;
                                // If we are expecting pairs of PM/MM then we want the order
                                // in m_Cells to be PM first and MM second.
                if (expectMisMatch && cell.m_PBase == cell.m_TBase)
                    ++cellIndex;
            }
            else
            {
                cellIndex = (iCell / pProbeSet->m_NumCellsPerList) * pProbeSet->m_NumCellsPerList;
                cellIndex += (pProbeSet->m_NumCellsPerList - (iCell % pProbeSet->m_NumCellsPerList) - 1);
            }

            if(cellIndex >= pBlk->m_Cells.size()) {
              assert(0 &&
                     "CCDFFileData::ReadTextFormat(): cellIndex cannot be larger that pBlk->m_Cells.size()" );
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
