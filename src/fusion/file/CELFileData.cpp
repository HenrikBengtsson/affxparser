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
#include "file/CELFileData.h"
//
#include "file/FileIO.h"
//
#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <istream>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
//

#ifdef _MSC_VER
#pragma warning(disable: 4996) // don't show deprecated warnings.
#ifdef HAVE_SNPRINTF // If not using visual c++'s _snprintf include snprintf.
extern "C" {
#include "snprintf.h"
} 
#else // otherwise use _snprintf where normally use snprintf.
#define snprintf _snprintf
#endif // HAVE_SNPRINTF
#endif // _MSC_VER

using namespace affxcel;

//////////////////////////////////////////////////////////////////////

/// Delimiter character in DAT header 
#define DELIMCHAR 0x14
/// Minimum number of characters in cell data row in text format
#define MIN_CELLSTR 2

/// Xda format identifier
#define CELL_FILE_MAGIC_NUMBER   0x00000040 // 64
/// Version number for xda format
#define CELL_FILE_VERSION_NUMBER 0x00000004 // 4

/// Trascriptome bcel format identifier
#define BCEL_HEADER_BYTES "BCEL\r\n\032\n"
/// Size of trascriptome bcel format identifier
#define BCEL_HEADER_LEN 8
/// Header information size in trascriptome bcel format
#define STRUCT_SIZE_BCEL_HEADER_INFO 66
/// Feature data size in trascriptome bcel format
#define STRUCT_SIZE_FEATURE_DATA 5
/// Masked cells and outlier data size in trascriptome bcel format
#define STRUCT_SIZE_XY_PAIR 8
/// Size of anonymous 4-byte identifier in trascriptome bcel format
#define BCEL_CHUNK_LEN_SIZE 4
/// Size of section name identifier in trascriptome bcel format
#define BCEL_CHUNK_NAME_SIZE 4
/// Size of section separator in trascriptome bcel format
#define BCEL_CHUNK_CRC_SIZE 4
/// Header section identifier in trascriptome bcel format
#define BCEL_CHUNK_HEAD "HEAD"
/// DAT header section identifier in trascriptome bcel format
#define BCEL_CHUNK_DTHD "DTHD"
/// Algorithm section identifier in trascriptome bcel format
#define BCEL_CHUNK_ALGM "ALGM"
/// Algorithm parameter section identifier in trascriptome bcel format
#define BCEL_CHUNK_ALPR "ALPR"
/// Intensity section identifier in trascriptome bcel format
#define BCEL_CHUNK_INTY "INTY"
/// Masked cell section identifier in trascriptome bcel format
#define BCEL_CHUNK_MASK "MASK"
/// Outlier section identifier in trascriptome bcel format
#define BCEL_CHUNK_OUTL "OUTL"
/// End of file identifier in trascriptome bcel format
#define BCEL_CHUNK_TAIL "TAIL"

/// Compact cel format identifier
#define CCEL_HEADER_BYTES "CCEL\r\n\128\n"
/// Unspported version of compact cel format identifier
#define OLD_CCEL_HEADER_BYTES "CCEL\r\n\064\n"
/// Size of compact cel format identifier
#define CCEL_HEADER_LEN 8

#ifndef WIN32
/// Line separator for unix/linux
#define LINE_SEPARATOR "\n"
#else
/// Line separator for Windows
#define LINE_SEPARATOR "\r\n"
#endif

#ifndef PAGE_SIZE
/// Page size used for memory mapping in non Windows environment
#define PAGE_SIZE (getpagesize())
#endif
#ifndef PAGE_MASK
/// Page mask used for memory mapping in non Windows environment
#define PAGE_MASK ~(PAGE_SIZE-1)
#endif
#ifndef PAGE_TRUNC
/// Page truncation pointer used for memory mapping in non Windows environment
#define PAGE_TRUNC(ptr) (ptr&(PAGE_MASK))
#endif 

///////////////////////////////////////////////////////////////////////////////
///  public constructor  CCELFileHeaderData
///  \brief Default constructor
///
///  @return void	
///////////////////////////////////////////////////////////////////////////////
CCELFileHeaderData::CCELFileHeaderData()
{
	m_Magic = 0;
	m_Version = CELL_FILE_VERSION_NUMBER;
	m_nCols = 0;
	m_nRows = 0;
	m_nCells = 0;
	m_Margin = 0;
	m_nOutliers = 0;
	m_nMasked = 0;
	m_Header = "";
	m_Alg = "";
	m_Params = "";
	m_DatHeader = "";
	m_ChipType = "";
	m_CellGrid.upperleft.x = 0;
	m_CellGrid.upperleft.y = 0;
	m_CellGrid.upperright.x = 0;
	m_CellGrid.upperright.y = 0;
	m_CellGrid.lowerleft.x = 0;
	m_CellGrid.lowerleft.y = 0;
	m_CellGrid.lowerright.x = 0;
	m_CellGrid.lowerright.y = 0;
}

///////////////////////////////////////////////////////////////////////////////
///  public destructor  ~CCELFileHeaderData
///  \brief Destructor
///
///  @return void	
///////////////////////////////////////////////////////////////////////////////
CCELFileHeaderData::~CCELFileHeaderData()
{
	Clear();
}

///////////////////////////////////////////////////////////////////////////////
///  public  Clear
///  \brief Clear and initialize content of member variables 
///
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileHeaderData::Clear()
{
	m_nRows = 0;
	m_nCols = 0;
	m_nCells = 0;
	if (m_ParameterIndices.size() > 0)
		m_ParameterIndices.clear();
	if (m_Parameters.size() > 0)
		m_Parameters.clear();
	m_Header = "";
	m_Alg = "";
	m_Params = "";
	m_DatHeader = "";
	m_ChipType = "";
	m_CellGrid.upperleft.x = 0;
	m_CellGrid.upperleft.y = 0;
	m_CellGrid.upperright.x = 0;
	m_CellGrid.upperright.y = 0;
	m_CellGrid.lowerleft.x = 0;
	m_CellGrid.lowerleft.y = 0;
	m_CellGrid.lowerright.x = 0;
	m_CellGrid.lowerright.y = 0;
	m_Margin = 0;
	m_nOutliers = 0;
	m_nMasked = 0;
	m_Magic = CELL_FILE_MAGIC_NUMBER;
	m_Version = CELL_FILE_VERSION_NUMBER;
}

///////////////////////////////////////////////////////////////////////////////
///  public  GetHeader
///  \brief Concatenate header data into a string and return it
///
///  @return std::string	Header string
///////////////////////////////////////////////////////////////////////////////
std::string CCELFileHeaderData::GetHeader()
{
	SetDatHeader();
	char paramString[2000];
	snprintf(paramString,sizeof(paramString), "Cols=%d\nRows=%d\nTotalX=%d\nTotalY=%d\nOffsetX=%d\nOffsetY=%d\n"
				"GridCornerUL=%d %d\nGridCornerUR=%d %d\nGridCornerLR=%d %d\nGridCornerLL=%d %d\n"
				"Axis-invertX=%d\nAxisInvertY=%d\nswapXY=%d\nDatHeader=%s\nAlgorithm=%s\n",
				m_nCols, m_nRows,
				m_nCols, m_nRows,
				0,0,
				m_CellGrid.upperleft.x, m_CellGrid.upperleft.y,
				m_CellGrid.upperright.x, m_CellGrid.upperright.y,
				m_CellGrid.lowerright.x, m_CellGrid.lowerright.y,
				m_CellGrid.lowerleft.x, m_CellGrid.lowerleft.y,
				0,0,0,
				m_DatHeader.c_str(),
				m_Alg.c_str());

	m_Header = paramString;
	m_Header += "AlgorithmParameters=";
	m_Header += GetAlgorithmParameters();
	m_Header += "\n";
	return m_Header;
}

///////////////////////////////////////////////////////////////////////////////
///  public overloaded  SetDatHeader
///  \brief Set default DAT header
///
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileHeaderData::SetDatHeader()
{
	if ((m_ChipType.length() > 0) && (m_DatHeader.length() == 0))
	{
		char scanInfo[2000];
		snprintf(scanInfo,sizeof(scanInfo), " %c %c %s.1sq %c %c %c %c %c %c %c %c %c ",
			DELIMCHAR,
			DELIMCHAR,
			m_ChipType.c_str(),
			DELIMCHAR,
			DELIMCHAR,
			DELIMCHAR,
			DELIMCHAR,
			DELIMCHAR,
			DELIMCHAR,
			DELIMCHAR,
			DELIMCHAR,
			DELIMCHAR);
		m_DatHeader = scanInfo;
	}
	else if ((m_ChipType.length() > 0) && (m_DatHeader.length() > 0))
	{
		std::string temp = "";
		std::string::size_type index = m_DatHeader.find(DELIMCHAR);
		if (index != std::string::npos)
		{
			index = m_DatHeader.find(DELIMCHAR, index + 1);
			if (index != std::string::npos)
			{
				std::string::size_type indexEnd = m_DatHeader.find(".1sq", index);
				if (indexEnd != std::string::npos)
				{
					if (m_DatHeader.substr(index + 2, indexEnd - index) != m_ChipType)
					{
						temp = m_DatHeader.substr(0, index + 2);
						temp += m_ChipType;
						temp += m_DatHeader.substr(indexEnd);
					}
					else 
						temp = m_DatHeader;
				}
				else
				{
					indexEnd = m_DatHeader.find(DELIMCHAR, index + 1);
					if (indexEnd != std::string::npos)
					{
						if (m_DatHeader.substr(index + 2, indexEnd - index - 1) != m_ChipType)
						{
							temp = m_DatHeader.substr(0, index + 2);
							temp += m_ChipType;
							temp += ".1sq";
							temp += m_DatHeader.substr(indexEnd);
						}
						else 
							temp = m_DatHeader;
					}
				}
			}
			else
				temp = m_DatHeader;
		}
		else
			temp = m_DatHeader;

		m_DatHeader = temp;
	}
}

///////////////////////////////////////////////////////////////////////////////
///  public  ParseDatHeader
///  \brief Parse DAT header from header string
///
///  @return void	
///////////////////////////////////////////////////////////////////////////////

void CCELFileHeaderData::ParseDatHeader()
{
	const char *pInfo = strstr(m_Header.c_str(), "DatHeader=");
	if (pInfo != NULL)
	{
		const char *pEnd = strchr(pInfo, '\n');
		if (pEnd != NULL)
		{
			char *str = new char [pEnd - pInfo - 10 + 1];
			strncpy(str, pInfo + 10, pEnd - pInfo - 10);
			str[pEnd - pInfo - 10] = '\0';
			m_DatHeader = str;
			delete [] str;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///  public  ParseChipType
///  \brief Parse chip type of array from header string
///
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileHeaderData::ParseChipType()
{
	// Determine the array type from the header.
	// chip type is between second and third delimiters
	const char *pInfo = strchr(m_Header.c_str(), DELIMCHAR);
	if (pInfo != NULL)
	{
		pInfo = strchr(pInfo + 1, DELIMCHAR);
		if ((pInfo != NULL) && *(++pInfo))  
		{
			pInfo++;
			const char *pEnd = strchr(pInfo, '.');
			if (pEnd == NULL)  
			{
				pEnd = strchr(pInfo, DELIMCHAR);
				if (pEnd != NULL)
					pEnd--;
			}
			if (pEnd != NULL)
			{
				char *str = new char [pEnd - pInfo + 1];
				strncpy(str, pInfo, pEnd - pInfo);
				str[pEnd - pInfo] = '\0';
				m_ChipType = str;
				delete [] str;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///  public  ParseAlgorithmParameters
///  \brief Parse algorithm parameters from single string into STL map
///
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileHeaderData::ParseAlgorithmParameters()
{
	if (m_Params.length() > 0)
	{
		int iStart = 0;
		int iEnd = 0;
		int iLen = (int) m_Params.length() - 1;
		std::string tag = "";
		std::string value = "";
		for (int i = 0; i < iLen; i++)
		{
			if ((m_Params.at(i) == ':') || (m_Params.at(i) == '='))
			{
				tag = m_Params.substr(iStart, (iEnd - iStart + 1));
				iStart = i + 1;
				iEnd = iStart;
			}
			else if ((m_Params.at(i) == ';') || (m_Params.at(i) == ' '))
			{
				value = m_Params.substr(iStart, (iEnd - iStart + 1));
				AddAlgorithmParameter(tag, value);
				iStart = i + 1;
				iEnd = iStart;
			}
			else
				iEnd = i;
		}
		value = m_Params.substr(iStart, (iEnd - iStart + 2));
		AddAlgorithmParameter(tag, value);
	}
}

///////////////////////////////////////////////////////////////////////////////
///  public overloaded  AddAlgorithmParameter
///  \brief Call AddAlgorithmParameter() to add algorithm paramter into STL map.  
///  If tag already exists, the addition will be ignored.
///
///  @param  tag const char * 	Algorithm parameter tag
///  @param  value const char * 	Algorithm parameter value
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileHeaderData::AddAlgorithmParameter(const char *tag, const char *value)
{
	assert(tag != NULL);
	assert(value != NULL);

	std::string strTag = tag;
	std::string strValue = value;

	AddAlgorithmParameter(strTag, strValue);
}

///////////////////////////////////////////////////////////////////////////////
///  public overloaded  AddAlgorithmParameter
///  \brief Add algorithm parameter into STL map. 
///  If tag already exists, the addition will be ignored.
///
///  @param  tag std::string & 	Algorithm parameter tag
///  @param  value std::string & 	Algorithm parameter value
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileHeaderData::AddAlgorithmParameter(std::string& tag, std::string& value)
{
	assert(tag != "");
	assert(value != "");

	std::map<std::string, std::string>::iterator pos = m_Parameters.find(tag);
	if (pos == m_Parameters.end())
	{
		int index = (int) m_Parameters.size() + 1;
		m_ParameterIndices.insert(std::make_pair(index, tag));
		m_Parameters.insert(std::make_pair(tag, value));
	}
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetAlgorithmParameter
///  \brief Set algorithm parameter value of existing specified tag
///
///  @param  tag const char * 	Algorithm parameter tag
///  @param  value const char * 	Algorithm parameter value
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileHeaderData::SetAlgorithmParameter(const char *tag, const char *value)
{
	assert(tag != NULL);
	assert(value != NULL);

	std::string strTag = tag;
	std::string strValue = value;
	std::map<std::string, std::string>::iterator pos = m_Parameters.find(strTag);
	if (pos != m_Parameters.end())
		pos->second = strValue;
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetAddAlgorithmParameter
///  \brief Set or Add algorithm parameter value
///
///  @param  tag const char * 	Algorithm parameter tag
///  @param  value const char * 	Algorithm parameter value
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileHeaderData::SetAddAlgorithmParameter(const char *tag, const char *value)
{
	assert(tag != NULL);
	assert(value != NULL);

	std::string strTag = tag;
	std::string strValue = value;

	SetAddAlgorithmParameter(strTag, strValue);
}
	

///////////////////////////////////////////////////////////////////////////////
///  public  SetAddAlgorithmParameter
///  \brief Set or Add algorithm parameter value
///
///  @param  tag const char * 	Algorithm parameter tag
///  @param  value const char * 	Algorithm parameter value
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileHeaderData::SetAddAlgorithmParameter(std::string& tag, std::string& value)
{
	assert(tag != "");
	assert(value != "");

	std::map<std::string, std::string>::iterator pos = m_Parameters.find(tag);
	if (pos != m_Parameters.end()) {
		pos->second = value;
	} else {
		int index = (int) m_Parameters.size() + 1;
		m_ParameterIndices.insert(std::make_pair(index, tag));
		m_Parameters.insert(std::make_pair(tag, value));
	}
}

///////////////////////////////////////////////////////////////////////////////
///  public  GetAlgorithmParameter
///  \brief Retrieve algorithm parameter value of specified tag
///
///  @param  tag const char * 	Algorithm parameter tag
///  @return std::string	Algorithm parameter value or NULL if tag is not found
///////////////////////////////////////////////////////////////////////////////
std::string CCELFileHeaderData::GetAlgorithmParameter(const char *tag)
{
	assert(tag != NULL);

	std::string strTag = tag;
	std::string strValue = "";
	std::map<std::string, std::string>::iterator pos = m_Parameters.find(strTag);
	if (pos != m_Parameters.end())
		strValue = pos->second;
	return strValue;
}

///////////////////////////////////////////////////////////////////////////////
///  public  GetAlgorithmParameterTag
///  \brief Retrieve algorithm parameter name of specified index
///
///  @param  index int Algorithm parameter index
///  @return std::string	Algorithm parameter name or NULL if not found
///////////////////////////////////////////////////////////////////////////////
std::string CCELFileHeaderData::GetAlgorithmParameterTag(int index)
{
	std::string tag="";
	std::map<int, std::string>::iterator pos = m_ParameterIndices.find(index+1);
	if (pos != m_ParameterIndices.end())
	{
		tag = pos->second;
	}
	return tag;
}

///////////////////////////////////////////////////////////////////////////////
///  public  GetAlgorithmParameters
///  \brief Retreive algorithm parameters from STL map and format them into a single string
///
///  @return std::string	Algorithm parameters
///////////////////////////////////////////////////////////////////////////////
std::string CCELFileHeaderData::GetAlgorithmParameters()
{
	std::string strBuffer = "";
	std::string strTag = "";
	std::string strValue = "";
	int iCount = 0;
	std::map<int, std::string>::iterator posIndex;
	std::map<std::string, std::string>::iterator pos;
	for (int i = 0; i < (int) m_ParameterIndices.size(); i++)
	{
		posIndex = m_ParameterIndices.find(i + 1);
		if (posIndex != m_ParameterIndices.end())
		{
			strTag = posIndex->second;
			pos = m_Parameters.find(strTag);
			if (pos != m_Parameters.end())
			{
				strValue = pos->second;
				if (iCount > 0) strBuffer.append(";"); 
				strBuffer.append(strTag);
				strBuffer.append(":"), 
				strBuffer.append(strValue);
				iCount++;
			}
		}
	}
	return strBuffer;
}

///////////////////////////////////////////////////////////////////////////////
///  inline public  SetMargin
///  \brief Set cell margin
///  @param  i int  	Cell margin
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileHeaderData::SetMargin(int i)
{
	m_Margin = i; 
	char sz[1024]; 
	snprintf(sz,sizeof(sz), "%d", i); 
	AddAlgorithmParameter("CellMargin", sz); 
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetGridCorners
///  \brief Set array grid coordinates
///
///  @param  grid GridCoordinatesType  	Grid coordinates
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileHeaderData::SetGridCorners(GridCoordinatesType grid)
{
	m_CellGrid.upperleft.x = grid.upperleft.x;
	m_CellGrid.upperleft.y = grid.upperleft.y;
	m_CellGrid.upperright.x = grid.upperright.x;
	m_CellGrid.upperright.y = grid.upperright.y;
	m_CellGrid.lowerleft.x = grid.lowerleft.x;
	m_CellGrid.lowerleft.y = grid.lowerleft.y;
	m_CellGrid.lowerright.x = grid.lowerright.x;
	m_CellGrid.lowerright.y = grid.lowerright.y;
}


///////////////////////////////////////////////////////////////////////////////
///  public  ParseCorners
///  \brief Parse array grid coordinates from header string
///
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileHeaderData::ParseCorners()
{
    // get grid coordinates and size from the header
	if (m_Header.length() > 0)
	{
		const char* pch = strstr(m_Header.c_str(), "GridCorner");
		int ulx,uly;
		int urx,ury;
		int llx,lly;
		int lrx,lry;
		if ((pch != NULL) &&
			(sscanf(pch, "GridCornerUL=%d %d GridCornerUR=%d %d GridCornerLR=%d %d GridCornerLL=%d %d",
						&ulx, &uly,
						&urx, &ury,
						&lrx, &lry,
						&llx, &lly) == 8))
		{
			m_CellGrid.upperleft.x = ulx;
			m_CellGrid.upperleft.y = uly;

			m_CellGrid.upperright.x = urx;
			m_CellGrid.upperright.y = ury;

			m_CellGrid.lowerright.x = lrx;
			m_CellGrid.lowerright.y = lry;

			m_CellGrid.lowerleft.x = llx;
			m_CellGrid.lowerleft.y = lly;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///  public  Exists
///  \brief Check if the CEL file exists by using the already set full path name
///
///  @return bool	true if exists; false otherwise
///////////////////////////////////////////////////////////////////////////////
bool CCELFileData::Exists()
{
  if (ResolveName()=="") {
    return false;
  }
  return true;
}

std::string CCELFileData::ResolveName()
{
	assert(m_FileName != "");

  std::string name;
	struct stat st;
  // name 
  name=m_FileName;
  if (stat(name.c_str(), &st) == 0) {
    return name;
  }
  //
#ifdef CELFILE_USE_ZLIB
  name=m_FileName+".gz";
  if (stat(name.c_str(), &st) == 0) {
    return name;
  }
#endif
  //
  return "";
}

///////////////////////////////////////////////////////////////////////////////
///  public  GetFileSize
///  \brief Retrieve file size
///
///  @return uint32_t	Size in bytes
///////////////////////////////////////////////////////////////////////////////
uint32_t CCELFileData::GetFileSize()
{
  std::string name=ResolveName();
	assert(name != "");

	int32_t lSize = 0;
	struct stat st;
	if (stat(name.c_str(), &st) == 0)
	{
		lSize = st.st_size;
	}
	return lSize;
}

///////////////////////////////////////////////////////////////////////////////
///  private  Open
///  \brief Determine the CEL file format and call corresponding function to read it
///
///  @param  bReadHeaderOnly bool  [=false] 	Flag to determine if reading header section only
///  @return bool	true if success; false if fail
///
///  \see DetermineFileFormat, ReadXDABCel, ReadTranscriptomeBCel, ReadCompactBCel, ReadTextCel
///////////////////////////////////////////////////////////////////////////////
bool CCELFileData::Open(bool bReadHeaderOnly)
{
	bool retVal = false;

	// First close the file.
	Close();

	DetermineFileFormat();
	if (IsXDACompatibleFile())
		retVal = ReadXDABCel(bReadHeaderOnly);
	else if (IsTranscriptomeBcelFile())
		retVal = ReadTranscriptomeBCel(bReadHeaderOnly);
	else if (IsCompactCelFile())
		retVal = ReadCompactBCel(bReadHeaderOnly);
	else if (IsUnsupportedCompactCelFile())
	{
		SetError("This version of compact cel file is no longer supported.");
		retVal = false;
	}
	else
		retVal = ReadTextCel(bReadHeaderOnly);

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
///  private  ReadXDABCel
///  \brief Read xda format CEL file using memory mapping
///
///  @param  bReadHeaderOnly bool  [=false] 	Flag to determine if reading header section only
///  @return bool	true if success; false if fail
///
///  \remark Header section, masked cells and outlier information are read in memory. 
///          The reading of masked cells and outlier information is determined by the reading state
///          which is set in ReadEx().
///////////////////////////////////////////////////////////////////////////////
bool CCELFileData::ReadXDABCel(bool bReadHeaderOnly)
{
	bool retVal = false;
  std::string tmp_FileName;

  tmp_FileName=ResolveName();
  if (tmp_FileName=="") {
    SetError("File '"+m_FileName+"' not found");
    return false;
  }

#define OPEN_ERR_MSG "Unable to open the file."

#ifdef CELFILE_USE_STDSTREAM
  // Open the file.
	std::ifstream instr;
  tmp_FileName=m_FileName;
	instr.open(tmp_FileName.c_str(), std::ios::in | std::ios::binary);
  // Check if open
  if (!instr) {
		SetError(OPEN_ERR_MSG " (ifstream)");
 		return false;
 	}
#endif

// #ifdef CELFILE_USE_STDIO
//   FILE* instr;
//   tmp_FileName=m_FileName;
//   instr=fopen(tmp_FileName.c_str(),"rb");
//   if (instr==NULL) {
//     SetError(OPEN_ERR_MSG " (stdio)");
//     return false;
//   }
// #endif

#ifdef CELFILE_USE_ZLIB
  gzFile instr;
  // try the UNgziped file first.
  tmp_FileName=ResolveName();
  instr=gzopen(tmp_FileName.c_str(),"rb");
  if (instr==NULL) {
    SetError(OPEN_ERR_MSG " (gzopen)");
    return false;
  }
  //printf("### CCELFileData::ReadXDABCel('%s'): open1\n",tmp_FileName.c_str());
#endif
  
	Clear();

	// Read the header
	int iHeaderBytes = 0;
	int32_t magic;

	int32_t version;
	int32_t nSubGrids;
	// char *sval=NULL;
  std::string tmp_str;

	// Read the magic number.
	ReadInt32_I(instr, magic);
	iHeaderBytes += INT_SIZE;
	m_HeaderData.SetMagic(magic);

	// Check if new type.
	if (!(magic==CELL_FILE_MAGIC_NUMBER))
	{
		SetError("The file does not appear to be the correct format.");
		return false;
	}

	// Read the version
	ReadInt32_I(instr, version);
	iHeaderBytes += INT_SIZE;
	m_HeaderData.SetVersion(version);

	// Read the dimensions of the array
	int32_t iValue = 0;
	uint32_t ulValue = 0;
	ReadInt32_I(instr, iValue);
	iHeaderBytes += INT_SIZE;
	m_HeaderData.SetRows(iValue);
	ReadInt32_I(instr, iValue);
	iHeaderBytes += INT_SIZE;
	m_HeaderData.SetCols(iValue);
	ReadInt32_I(instr, iValue);
	iHeaderBytes += INT_SIZE;
	m_HeaderData.SetCells(iValue);

	// Read the other members.
	ReadCString_I(instr, tmp_str);
	iHeaderBytes += INT_SIZE;
	iHeaderBytes += tmp_str.size();
	m_HeaderData.SetHeader(tmp_str.c_str());

	ReadCString_I(instr, tmp_str);
	iHeaderBytes += INT_SIZE;
	iHeaderBytes += tmp_str.size();
	m_HeaderData.SetAlg(tmp_str.c_str());

	ReadCString_I(instr, tmp_str);
	iHeaderBytes += INT_SIZE;
	iHeaderBytes += tmp_str.size();
	m_HeaderData.SetParams(tmp_str.c_str());

	ReadInt32_I(instr, iValue);
	iHeaderBytes += INT_SIZE;
	m_HeaderData.SetMargin(iValue);
	ReadUInt32_I(instr, ulValue);
	iHeaderBytes += UINT32_SIZE;
	m_HeaderData.SetOutliers(ulValue);
	ReadUInt32_I(instr, ulValue);
	iHeaderBytes += UINT32_SIZE;
	m_HeaderData.SetMasked(ulValue);
	ReadInt32_I(instr, nSubGrids);
	iHeaderBytes += INT_SIZE;

	// Set the chip type and DatHeader
	m_HeaderData.ParseChipType();
	m_HeaderData.ParseDatHeader();

	// Parse algorithm parameters into map
	m_HeaderData.ParseAlgorithmParameters();

	// Set grid coordinates
	m_HeaderData.ParseCorners();

#ifdef CELFILE_USE_STDSTREAM
	instr.close();
#endif
#ifdef CELFILE_USE_STDIO
  fclose(instr);
#endif
#ifdef CELFILE_USE_ZLIB
  gzclose(instr);
#endif

	// Read the remaining data.
	if (bReadHeaderOnly)
		return true;

#ifdef CELFILE_USE_MEMMAP

#ifdef _MSC_VER
	// Memory map file on windows...
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	m_hFile = CreateFile(m_FileName.c_str(), GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL/*FILE_FLAG_RANDOM_ACCESS*/);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		SetError("Failed to open the file for win memory mapping.");
		return false;
	}
	m_hFileMap = CreateFileMapping(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (m_hFileMap != NULL)
	{
		m_lpFileMap = MapViewOfFile(m_hFileMap, FILE_MAP_READ, 0, 0, 0);
		if (m_lpFileMap == NULL)
		{
			Close();
			SetError("Unable to map view for the win memory map file.");
			return false;
		}
		else
		{
			m_lpData = (char *)(m_lpFileMap) + iHeaderBytes;
		}
	}
#else
	// Memory map file on posix...
	int32_t lFileSize = GetFileSize();
	char* szBuffer = new char[iHeaderBytes + 1];
	m_File = fopen(m_FileName.c_str(), "r");
	if (m_File == NULL)
	{
		SetError("Failed to open the file for memory mapping.");
		return false;
	}
	fread(szBuffer, iHeaderBytes, 1, m_File);
  // printf("fpos=%u",ftell(m_File)); // debug
	delete [] szBuffer;
  
    size_t cellf_page_start = PAGE_TRUNC(ftell(m_File));
    size_t cellf_page_offset = ftell(m_File) - cellf_page_start;
    m_MapLen = lFileSize - cellf_page_start;
    m_lpFileMap = mmap(NULL, m_MapLen, PROT_READ, MAP_SHARED, fileno(m_File), cellf_page_start);
    //printf("=== mmap: %p\n",m_lpFileMap); //jhg
    if (m_lpFileMap == MAP_FAILED)
	{
		Close();
    static char buf[2048];
    sprintf(buf, "Unable to map view for the unix memory map file: %d", errno);
		SetError(buf);
		return false;
	}
	else
	{
		m_lpData = (char *)(m_lpFileMap) + cellf_page_offset;
	}
	if (m_File != NULL) 
	{
		fclose(m_File);
		m_File = NULL;
	}
#endif // _MSC_VER

#else
  // No memory mapping ...

#ifdef CELFILE_USE_ZLIB

  // take a guess at how big the uncompressed file is.
  int alloc_size=(20*1024) + // approx header
    ((FLOAT_SIZE + FLOAT_SIZE + SHORT_SIZE)*(GetRows()*GetCols())) + // row*col
    ((SHORT_SIZE + SHORT_SIZE)             * GetNumMasked()) + // masked
    ((SHORT_SIZE + SHORT_SIZE)             * GetNumOutliers()); // outliers
  // alloc it
  m_lpData = new char[alloc_size];

  // do the read -- must be the same file name as above.
  instr=gzopen(tmp_FileName.c_str(),"rb");
  if (instr==NULL) {
    SetError("gzip open failed - take2");
    return false;
  }
  gzseek(instr,iHeaderBytes,SEEK_SET);
  int read_size=gzread(instr,m_lpData,alloc_size);
  
  //printf("### CCELFileData::ReadXDABCel('%s')=%d (alloc=%d)\n",tmp_FileName.c_str(),read_size,alloc_size);
  gzclose(instr);

#else
#ifdef _MSC_VER
	// Memory map file on windows...
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	m_hFile = CreateFile(m_FileName.c_str(), GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL/*FILE_FLAG_RANDOM_ACCESS*/);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		SetError("Failed to open the file for win memory mapping.");
		return false;
	}
	m_hFileMap = CreateFileMapping(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (m_hFileMap != NULL)
	{
		m_lpFileMap = MapViewOfFile(m_hFileMap, FILE_MAP_READ, 0, 0, 0);
		if (m_lpFileMap == NULL)
		{
			Close();
			SetError("Unable to map view for the win memory map file.");
			return false;
		}
		else
		{
			m_lpData = (char *)(m_lpFileMap) + iHeaderBytes;
		}
	}
#else
  // printf("OPEN: non memory mapped.\n"); //
  m_lpFileMap=NULL;
  m_File=fopen(m_FileName.c_str(),"r");
  if (m_File==NULL) {
    SetError("Failed to open file for non-memap open.");
    return false;
  }
  // skip the header
  fseek(m_File,iHeaderBytes,SEEK_SET);
  // printf("fpos=%u",ftell(m_File)); // debug
  // read the entire file into memory...
  int32_t datasize = GetFileSize()-iHeaderBytes;
  m_lpData = new char[datasize];
  size_t read=fread(m_lpData,1,datasize,m_File);
  //printf("read=%u  iHeaderBytes=%u\n",read,iHeaderBytes);
  if (read!=datasize) {
    SetError("Unable to read the entire file.");
    return false;
  }
  //
  fclose(m_File);
#endif
#endif // CELFILE_USE_ZLIB

#endif // CELFILE_USE_MEMMAP

	int16_t x=0;
	int16_t y=0;
	int iCell;

	// "Read" the Mean data
	m_pEntries = (CELFileEntryType*)m_lpData;

	// Read the mask data
	int iOffset = m_HeaderData.GetCells() * (FLOAT_SIZE + FLOAT_SIZE + SHORT_SIZE);
	if (m_bReadMaskedCells)
	{
		for (iCell = 0; iCell < (int) m_HeaderData.GetMasked(); iCell++)
		{
			// Read the coordinate.
			x = ((int16_t)MmGetUInt16_I((uint16_t*)(m_lpData + iOffset + iCell * 2 * SHORT_SIZE)));
			//x = GetShort((short*) (m_lpData + iOffset + iCell * 2 * SHORT_SIZE), m_FileFormat);
			y = ((int16_t)MmGetUInt16_I((uint16_t*)(m_lpData + iOffset + iCell * 2 * SHORT_SIZE + SHORT_SIZE)));
//			y = GetShort((short*) (m_lpData + iOffset + iCell * 2 * SHORT_SIZE + SHORT_SIZE), m_FileFormat);
			m_MaskedCells.insert(std::make_pair(y * m_HeaderData.GetCols() + x, true));
		}
	}
		
	// Read the outlier data
	iOffset += m_HeaderData.GetMasked() * 2 * SHORT_SIZE;
	if (m_bReadOutliers)
	{
		for (iCell = 0; iCell < (int) m_HeaderData.GetOutliers(); iCell++)
		{
			// Read the coordinate.
			x = ((int16_t)MmGetUInt16_I((uint16_t*)(m_lpData + iOffset + iCell * 2 * SHORT_SIZE)));
			y = ((int16_t)MmGetUInt16_I((uint16_t*)(m_lpData + iOffset + iCell * 2 * SHORT_SIZE + SHORT_SIZE)));
//			x = GetShort((short*) (m_lpData + iOffset + iCell * 2 * SHORT_SIZE), m_FileFormat);
//			y = GetShort((short*) (m_lpData + iOffset + iCell * 2 * SHORT_SIZE + SHORT_SIZE), m_FileFormat);
			m_Outliers.insert(std::make_pair(y * m_HeaderData.GetCols() + x, true));
		}
	}
	else
		m_HeaderData.SetOutliers(0);
	if (!m_bReadMaskedCells) m_HeaderData.SetMasked(0);
	retVal = true;
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
///  private  ReadTranscriptomeBCel
///  \brief Read transcriptome binary CEL file using memory mapping
///
///  @param  bReadHeaderOnly bool  [=false] 	Flag to determine if reading header section only
///  @return bool	true if success; false if fail
///
///  \remark Header section, masked cells and outlier information are read in memory. 
///          The reading of masked cells and outlier information is determined by the reading state
///          which is set in ReadEx().
///////////////////////////////////////////////////////////////////////////////
bool CCELFileData::ReadTranscriptomeBCel(bool bReadHeaderOnly)
{
  std::string tmp_FileName=ResolveName();
	bool retVal = false;

#ifdef CELFILE_USE_STDSTREAM
  // Open the file.
	std::ifstream instr;
  tmp_FileName=m_FileName;
	instr.open(tmp_FileName.c_str(), std::ios::in | std::ios::binary);
  // Check if open
  if (!instr) {
		SetError(OPEN_ERR_MSG " (ifstream)");
 		return false;
 	}
#endif

#ifdef CELFILE_USE_ZLIB
  gzFile instr;
  // try the UNgziped file first.
  tmp_FileName=ResolveName();
  instr=gzopen(tmp_FileName.c_str(),"rb");
  if (instr==NULL) {
    SetError(OPEN_ERR_MSG " (gzopen)");
    return false;
  }
  //printf("### CCELFileData::ReadTranscriptomeBCel('%s'): open1\n",tmp_FileName.c_str());
#endif

	Clear();

	// Read the header
	int iHeaderBytes = 0;
	float version = 0;
	uint32_t ulValue = 0;
	uint32_t totalx = 0;
	uint32_t totaly = 0;
	uint32_t offsetx = 0;
	uint32_t offsety = 0;
	GridCoordinatesType grid;
	unsigned short axisinvertx = 0;
	unsigned short axisinverty = 0;
	unsigned short swapxy = 0;
	std::string scanInfo;
	std::string sval;

	// Read the magic number.
	ReadFixedString(instr, sval, BCEL_HEADER_LEN);
	iHeaderBytes += BCEL_HEADER_LEN;

	// Check if new type.
	if (strncmp(sval.c_str(), BCEL_HEADER_BYTES, BCEL_HEADER_LEN) != 0)
	{
		SetError("The file does not appear to be the correct format.");
		return false;
	}

	// Read the version
	ReadFloat_N(instr, version);
	iHeaderBytes += FLOAT_SIZE;
	m_HeaderData.SetVersion((int)version);

	// Read header information
	ReadUInt32_N(instr, ulValue);							// header struct size
	iHeaderBytes += (2 * UINT32_SIZE + ulValue + BCEL_CHUNK_NAME_SIZE);
	ReadFixedString(instr, sval, BCEL_CHUNK_NAME_SIZE);
	ReadFloat_N(instr, version);
	ReadUInt32_N(instr, ulValue);							// columns
	m_HeaderData.SetCols(ulValue);
	ReadUInt32_N(instr, ulValue);							// rows
	m_HeaderData.SetRows(ulValue);
	ReadUInt32_N(instr, totalx);							// totalx
	ReadUInt32_N(instr, totaly);							// totaly
	ReadUInt32_N(instr, offsetx);							// offsetx
	ReadUInt32_N(instr, offsety);							// offsety
	ReadUInt32_N(instr, ulValue);							// GridCornerUL.x
	grid.upperleft.x = ulValue;
	ReadUInt32_N(instr, ulValue);							// GridCornerUL.y
	grid.upperleft.y = ulValue;
	ReadUInt32_N(instr, ulValue);							// GridCornerUR.x
	grid.upperright.x = ulValue;
	ReadUInt32_N(instr, ulValue);							// GridCornerUR.y
	grid.upperright.y = ulValue;
	ReadUInt32_N(instr, ulValue);							// GridCornerLL.x
	grid.lowerleft.x = ulValue;
	ReadUInt32_N(instr, ulValue);							// GridCornerLL.y
	grid.lowerleft.y = ulValue;
	ReadUInt32_N(instr, ulValue);							// GridCornerLR.x
	grid.lowerright.x = ulValue;
	ReadUInt32_N(instr, ulValue);							// GridCornerLR.y
	grid.lowerright.y = ulValue;
	m_HeaderData.SetGridCorners(grid);
	ReadUInt16_N(instr, axisinvertx);						// Axis-invertX
	ReadUInt16_N(instr, axisinverty);						// AxisinvertY
	ReadUInt16_N(instr, swapxy);							// swapXY
	ReadUInt32_N(instr, ulValue);							

	// Read DatHeader
	ReadUInt32_N(instr, ulValue);							// DatHeader length
	iHeaderBytes += (2 * UINT32_SIZE + ulValue + BCEL_CHUNK_NAME_SIZE);
	ReadFixedString(instr, sval, BCEL_CHUNK_NAME_SIZE);
	ReadFixedString(instr, scanInfo, ulValue);
	if (scanInfo[ulValue - 1] == '\r') scanInfo[ulValue - 1] = '\0';
	m_HeaderData.SetDatHeader(scanInfo.c_str());
	ReadUInt32_N(instr, ulValue);							

	// Read Algorithm
	ReadUInt32_N(instr, ulValue);							// Algorithm length
	iHeaderBytes += (2 * UINT32_SIZE + ulValue + BCEL_CHUNK_NAME_SIZE);
	ReadFixedString(instr, sval, BCEL_CHUNK_NAME_SIZE);
	ReadFixedString(instr, sval, ulValue);
	if (sval[ulValue - 1] == '\r') sval[ulValue - 1] = '\0';
	m_HeaderData.SetAlg(sval.c_str());
	ReadUInt32_N(instr, ulValue);							

	// Read AlgorithmParameters
	ReadUInt32_N(instr, ulValue);							// AlgorithmParameters length
	iHeaderBytes += (2 * UINT32_SIZE + ulValue + BCEL_CHUNK_NAME_SIZE);
	ReadFixedString(instr, sval, BCEL_CHUNK_NAME_SIZE);
	ReadFixedString(instr, sval, ulValue);
	if (sval[ulValue - 1] == '\r') sval[ulValue - 1] = '\0';
	m_HeaderData.SetParams(sval.c_str());
	ReadUInt32_N(instr, ulValue);							

	char paramString[2000];
	snprintf(paramString,sizeof(paramString), 
          "Cols=%d\nRows=%d\n"
          "TotalX=%u\nTotalY=%u\nOffsetX=%u\nOffsetY=%u\n"
          "GridCornerUL=%d %d\nGridCornerUR=%d %d\n"
          "GridCornerLR=%d %d\nGridCornerLL=%d %d\n"
          "Axis-invertX=%d\nAxisInvertY=%d\nswapXY=%d\nDatHeader=%s\n"
          "Algorithm=%s\nAlgorithmParameters=",
          m_HeaderData.GetCols(),m_HeaderData.GetRows(),
          totalx,totaly,offsetx,offsety,
          grid.upperleft.x,grid.upperleft.y,
          grid.upperright.x,grid.upperright.y,
          grid.lowerright.x,grid.lowerright.y,
          grid.lowerleft.x,grid.lowerleft.y,
          axisinvertx,axisinverty,swapxy,
          scanInfo.c_str(),
          m_HeaderData.GetAlg().c_str());
	std::string param = paramString;
	param += m_HeaderData.GetParams();
	param += "\n";
	m_HeaderData.SetHeader(param.c_str());

	// Read cell size
	ReadUInt32_N(instr, ulValue);
	int iOffset = ulValue;
	m_HeaderData.SetCells(ulValue / STRUCT_SIZE_FEATURE_DATA);
	ReadFixedString(instr, sval, BCEL_CHUNK_NAME_SIZE);
	iHeaderBytes += (UINT32_SIZE + BCEL_CHUNK_NAME_SIZE);

	// Set the chip type
	m_HeaderData.ParseChipType();

	// Parse algorithm parameters into map
	m_HeaderData.ParseAlgorithmParameters();
	m_HeaderData.SetMargin(atoi(m_HeaderData.GetAlgorithmParameter("CellMargin").c_str()));

	// Set grid coordinates
	m_HeaderData.ParseCorners();

	// Read the remaining data.
	if (bReadHeaderOnly)
		return true;

#ifdef CELFILE_USE_STDSTREAM
	instr.close();
#endif
#ifdef CELFILE_USE_ZLIB
  gzclose(instr);
#endif


#ifdef CELFILE_USE_MEMMAP
	// Memory map file
#ifdef _MSC_VER
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	m_hFile = CreateFile(m_FileName.c_str(), GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL/*FILE_FLAG_RANDOM_ACCESS*/);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		SetError("Failed to open the file for memory mapping.");
		return false;
	}
	m_hFileMap = CreateFileMapping(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (m_hFileMap != NULL)
	{
		m_lpFileMap = MapViewOfFile(m_hFileMap, FILE_MAP_READ, 0, 0, 0);
		if (m_lpFileMap == NULL)
		{
			Close();
			SetError("Unable to map view for the win memory map file.");
			return false;
		}
		else
		{
			m_lpData = (char *)(m_lpFileMap) + iHeaderBytes;
		}
	}
#else // posix
	int32_t lFileSize = GetFileSize();
	char* szBuffer = new char[iHeaderBytes + 1];
	m_File = fopen(m_FileName.c_str(), "r");
	if (m_File == NULL)
	{
		SetError("Failed to open the file for memory mapping.");
		return false;
	}
	fread(szBuffer, iHeaderBytes, 1, m_File);
	delete [] szBuffer;

    size_t cellf_page_start = PAGE_TRUNC(ftell(m_File));
    size_t cellf_page_offset = ftell(m_File) - cellf_page_start;
#ifdef __CYGWIN__
	cellf_page_offset -= 1;
#endif
    m_MapLen = lFileSize - cellf_page_start;
    m_lpFileMap = mmap(NULL, m_MapLen, PROT_READ, MAP_SHARED, fileno(m_File), cellf_page_start);
    if (m_lpFileMap == MAP_FAILED)
	{
		Close();
    static char buf[2048];
    sprintf(buf, "Unable to map view for the unix memory map file: %d", errno);
		SetError(buf);
		return false;
	}
	else
	{
		m_lpData = (char *)(m_lpFileMap) + cellf_page_offset;
	}
	if (m_File != NULL) 
	{
		fclose(m_File);
		m_File = NULL;
	}

#endif // if _msc_ver
#endif // celfile_use_mmap

#ifdef CELFILE_USE_STDSTREAM
  int alloc_size=GetFileSize();
  m_lpData=new char[alloc_size];
  instr.open(tmp_FileName.c_str(),std::ios::in | std::ios::binary);
  instr.seekg(iHeaderBytes,std::ios::beg);
  instr.read(m_lpData,alloc_size-iHeaderBytes);
  instr.close();
#endif

#ifdef CELFILE_USE_ZLIB
  int alloc_size=
    ((SHORT_SIZE+SHORT_SIZE+CHAR_SIZE)*(GetRows()*GetCols())) + // row*col
    (100*1024) // add 100k for masked and outliers
    ;
  m_lpData=new char[alloc_size];
  instr=gzopen(tmp_FileName.c_str(),"rb");
  gzseek(instr,iHeaderBytes,SEEK_SET);
  int read_size=gzread(instr,m_lpData,alloc_size);
  //printf("### CCELFileData::ReadCompactCelFile('%s')=%d (alloc=%d)\n",tmp_FileName.c_str(),read_size,alloc_size);
  gzclose(instr);
  
#endif

	// Read the Mean data
	uint32_t x = 0;
	uint32_t y = 0;
	int iCell;
	m_pTransciptomeEntries = (CELFileTranscriptomeEntryType*) m_lpData;

	// Read mask size
	iOffset = m_HeaderData.GetCells() * STRUCT_SIZE_FEATURE_DATA + UINT32_SIZE;
	ulValue = MmGetUInt32_N((uint32_t*)(m_lpData + iOffset));
	m_HeaderData.SetMasked(ulValue / STRUCT_SIZE_XY_PAIR);
	iOffset += (UINT32_SIZE + BCEL_CHUNK_NAME_SIZE);

	// Read the mask data
	if (m_bReadMaskedCells)
	{
		for (iCell = 0; iCell < (int)m_HeaderData.GetMasked(); iCell++)
		{
			// Read the coordinate.
			x = MmGetUInt32_N((uint32_t*) (m_lpData + iOffset + iCell * 2 * UINT32_SIZE));
			y = MmGetUInt32_N((uint32_t*) (m_lpData + iOffset + iCell * 2 * UINT32_SIZE + UINT32_SIZE));
			m_MaskedCells.insert(std::make_pair(y * m_HeaderData.GetCols() + x, true));
		}
	}
	iOffset += (m_HeaderData.GetMasked() * STRUCT_SIZE_XY_PAIR + UINT32_SIZE);

	// Read outlier size
	ulValue = MmGetUInt32_N((uint32_t*)(m_lpData + iOffset));
	m_HeaderData.SetOutliers(ulValue / STRUCT_SIZE_XY_PAIR);
	iOffset += (UINT32_SIZE + BCEL_CHUNK_NAME_SIZE);

	// Read the outlier data
	if (m_bReadOutliers)
	{
		for (iCell = 0; iCell < (int)m_HeaderData.GetOutliers(); iCell++)
		{
			// Read the coordinate.
			x = MmGetUInt32_N((uint32_t*) (m_lpData + iOffset + iCell * 2 * UINT32_SIZE));
			y = MmGetUInt32_N((uint32_t*) (m_lpData + iOffset + iCell * 2 * UINT32_SIZE + UINT32_SIZE));
			m_Outliers.insert(std::make_pair(y * m_HeaderData.GetCols() + x, true));
		}
	}
	else
		m_HeaderData.SetOutliers(0);
	if (!m_bReadMaskedCells) m_HeaderData.SetMasked(0);

	retVal = true;
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
///  private  ReadCompactBCel
///  \brief Read compact binary CEL file using memory mapping
///
///  @param  bReadHeaderOnly bool  [=false] 	Flag to determine if reading header section only
///  @return bool	true if success; false if fail
///
///  \remark Header section is read in memory. 
///////////////////////////////////////////////////////////////////////////////
bool CCELFileData::ReadCompactBCel(bool bReadHeaderOnly)
{
	bool retVal = false;

  std::string tmp_FileName;
  tmp_FileName=ResolveName();

  if (tmp_FileName=="") {
    SetError("File '"+m_FileName+"' not found");
    return false;
  }

	// Open the file.
#ifdef CELFILE_USE_STDSTREAM
	std::ifstream instr;
	instr.open(tmp_FileName.c_str(), std::ios::in | std::ios::binary);
	// Check if open
	if (!instr)
	{
		SetError("Unable to open the file.");
		return false;
	}
#endif
#ifdef CELFILE_USE_ZLIB
  gzFile instr;
  instr=gzopen(tmp_FileName.c_str(),"rb");
  if (instr==NULL) {
    SetError("Unable to open the file." " (gzopen)");
    return false;
  }
  //printf("### CCELFileData::ReadCompactBCel('%s')\n",tmp_FileName.c_str());
#endif
  
	Clear();

	// Read the header
	int iHeaderBytes = 0;
	int32_t version;
	int32_t nSubGrids;
  std::string tmpstr;

	// Read the magic number.
	std::string magic;
	ReadFixedString(instr, magic, CCEL_HEADER_LEN);
	iHeaderBytes += CCEL_HEADER_LEN;

	// Check if new type.
	if (strncmp(magic.c_str(), CCEL_HEADER_BYTES, CCEL_HEADER_LEN) != 0)
	{
		SetError("The file does not appear to be the correct format.");
		return false;
	}

	// Read the version
	ReadInt32_I(instr, version);
	iHeaderBytes += INT_SIZE;
	m_HeaderData.SetVersion(version);

	// Read the dimensions of the array
	int32_t iValue = 0;
	uint32_t ulValue = 0;
	ReadInt32_I(instr, iValue);
	iHeaderBytes += INT_SIZE;
	m_HeaderData.SetRows(iValue);
	ReadInt32_I(instr, iValue);
	iHeaderBytes += INT_SIZE;
	m_HeaderData.SetCols(iValue);
	ReadInt32_I(instr, iValue);
	iHeaderBytes += INT_SIZE;
	m_HeaderData.SetCells(iValue);

	// Read the other members.
	ReadCString_I(instr, tmpstr);
	iHeaderBytes += INT_SIZE;
	iHeaderBytes += tmpstr.size();
	m_HeaderData.SetHeader(tmpstr.c_str());

	ReadCString_I(instr,tmpstr);
	iHeaderBytes += INT_SIZE;
	iHeaderBytes += tmpstr.size();
	m_HeaderData.SetAlg(tmpstr.c_str());

	ReadCString_I(instr, tmpstr);
	iHeaderBytes += INT_SIZE;
	iHeaderBytes += tmpstr.size();
	m_HeaderData.SetParams(tmpstr.c_str());

	ReadInt32_I(instr, iValue);
	iHeaderBytes += INT_SIZE;
	m_HeaderData.SetMargin(iValue);

	// Added -- causes backwards compatability issues w/ earlier pre-releases
	ReadUInt32_I(instr, ulValue);
	iHeaderBytes += UINT32_SIZE;
	m_HeaderData.SetMasked(ulValue);

	ReadInt32_I(instr, nSubGrids);
	iHeaderBytes += INT_SIZE;
	// This hack needs to be changed. In short, the application of ccel to date
	// is on chips with 0 sub grids reported. If we are dealing with a ccel file
	// from a pre inclusion of mask values we will get non-zero subgrids. Hence
	// the assert:
	assert(nSubGrids == 0);

	// Set the chip type and DatHeader
	m_HeaderData.ParseChipType();
	m_HeaderData.ParseDatHeader();

	// Parse algorithm parameters into map
	m_HeaderData.ParseAlgorithmParameters();

	// Set grid coordinates
	m_HeaderData.ParseCorners();

#ifdef CELFILE_USE_STDSTREAM
	instr.close();
#endif
#ifdef CELFILE_USE_ZLIB
  gzclose(instr);
#endif

	// Read the remaining data.
	if (bReadHeaderOnly)
		return true;

	// Memory map file
#ifdef CELFILE_USE_MEMMAP
#ifdef _MSC_VER
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	m_hFile = CreateFile(m_FileName.c_str(), GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL/*FILE_FLAG_RANDOM_ACCESS*/);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		SetError("Failed to open the file for memory mapping.");
		return false;
	}
	m_hFileMap = CreateFileMapping(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (m_hFileMap != NULL)
	{
		m_lpFileMap = MapViewOfFile(m_hFileMap, FILE_MAP_READ, 0, 0, 0);
		if (m_lpFileMap == NULL)
		{
			Close();
			SetError("Unable to map view for the win memory map file.");
			return false;
		}
		else
		{
			m_lpData = (char *)(m_lpFileMap) + iHeaderBytes;
		}
	}
#else
	int32_t lFileSize = GetFileSize();
	char* szBuffer = new char[iHeaderBytes + 1];
	m_File = fopen(m_FileName.c_str(), "r");
	if (m_File == NULL)
	{
		SetError("Failed to open the file for memory mapping.");
		return false;
	}
	fread(szBuffer, iHeaderBytes, 1, m_File);
	delete [] szBuffer;

    size_t cellf_page_start = PAGE_TRUNC(ftell(m_File));
    size_t cellf_page_offset = ftell(m_File) - cellf_page_start;
#ifdef __CYGWIN__
	cellf_page_offset -= 1;
#endif
    m_MapLen = lFileSize - cellf_page_start;
    m_lpFileMap = mmap(NULL, m_MapLen, PROT_READ, MAP_SHARED, fileno(m_File), cellf_page_start);
    if (m_lpFileMap == MAP_FAILED)
	{
		Close();
    static char buf[2048];
    sprintf(buf, "Unable to map view for the unix memory map file: %d", errno);
		SetError(buf);
		return false;
	}
	else
	{
		m_lpData = (char *)(m_lpFileMap) + cellf_page_offset;
	}
	if (m_File != NULL) 
	{
		fclose(m_File);
		m_File = NULL;
	}
#endif
#else
  // no-mem-map
  int alloc_size=(20*1024)+
    ((SHORT_SIZE)*(GetRows()*GetCols())) + // row*col
    ((SHORT_SIZE + SHORT_SIZE)             * GetNumMasked()) + // masked
    ((SHORT_SIZE + SHORT_SIZE)             * GetNumOutliers()); // outliers
  m_lpData=new char[alloc_size];

#ifdef CELFILE_USE_STDSTREAM
  instr.open(tmp_FileName.c_str(),std::ios::in | std::ios::binary);
  instr.seekg(iHeaderBytes,std::ios::beg);
  instr.read(m_lpData,alloc_size-iHeaderBytes);
#endif

#ifdef CELFILE_USE_ZLIB
  instr=gzopen(tmp_FileName.c_str(),"rb");
  gzseek(instr,iHeaderBytes,SEEK_SET);
  int read_size=gzread(instr,m_lpData,alloc_size);
  //printf("### CCELFileData::ReadCompactCelFile('%s')=%d (alloc=%d)\n",tmp_FileName.c_str(),read_size,alloc_size);
  gzclose(instr);
#endif
#endif

	// Read the Mean data
	m_pMeanIntensities = (uint16_t*)m_lpData;

	// Read the mask data
	int16_t x=0;
	int16_t y=0;
	int iOffset = m_HeaderData.GetCells() * USHORT_SIZE;
	if (m_bReadMaskedCells)
	{
		for (int iCell = 0; iCell < (int) m_HeaderData.GetMasked(); iCell++)
		{
			// Read the coordinate.
			x = ((int16_t)MmGetUInt16_I((uint16_t*)(m_lpData + iOffset + iCell * 2 * USHORT_SIZE)));
			//x = GetShort((short*) (m_lpData + iOffset + iCell * 2 * SHORT_SIZE), m_FileFormat);
			y = ((int16_t)MmGetUInt16_I((uint16_t*)(m_lpData + iOffset + iCell * 2 * USHORT_SIZE + USHORT_SIZE)));
//			y = GetShort((short*) (m_lpData + iOffset + iCell * 2 * SHORT_SIZE + SHORT_SIZE), m_FileFormat);
			m_MaskedCells.insert(std::make_pair(y * m_HeaderData.GetCols() + x, true));
		}
	}
	else
		m_HeaderData.SetMasked(0);
		
	retVal = true;
	return retVal;
}
  
///////////////////////////////////////////////////////////////////////////////
///  public  EnsureNotMmapped
///  \brief Store data in memory if CEL file is memory mapped
///
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::EnsureNotMmapped() 
{
	// skip if not mmapped...
	if (m_lpFileMap==NULL)
	{
		return;
	}

	// it is mmapped, so fix it.
	if (m_FileFormat == TRANSCRIPTOME_BCEL) // the (short,short,char) format
	{ 
		assert(m_pTransciptomeEntries != NULL);

		// duplicate the data
		size_t bytecnt = GetCols() * GetRows() * sizeof(CELFileTranscriptomeEntryType);
		CELFileTranscriptomeEntryType *tmpptr = (CELFileTranscriptomeEntryType *) malloc(bytecnt);
		memcpy(tmpptr, m_pTransciptomeEntries, bytecnt);

		// discard the old map
		Munmap();

		// put it in place
		m_pTransciptomeEntries = tmpptr;
	} 
	else if (m_FileFormat == XDA_BCEL) // the (float,float,short) format
	{ 
		assert(m_pEntries != NULL);

		// duplicate the data
		size_t bytecnt = GetCols() * GetRows() * sizeof(CELFileEntryType);
		CELFileEntryType *tmpptr = (CELFileEntryType *) malloc(bytecnt);
		memcpy(tmpptr, m_pEntries, bytecnt);

		// discard the old map
		Munmap();

		// put it in place
		m_pEntries = tmpptr;
	}
	else if (m_FileFormat == COMPACT_BCEL) // the (short) format
	{ 
		assert(m_pMeanIntensities != NULL);

		// duplicate the data
		size_t bytecnt = GetCols() * GetRows() * sizeof(unsigned short);
		unsigned short *tmpptr = (unsigned short *) malloc(bytecnt);
		memcpy(tmpptr, m_pMeanIntensities, bytecnt);

		// discard the old map
		Munmap();

		// put it in place
		m_pMeanIntensities = tmpptr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///  private  ReadTextCel
///  \brief Read text CEL file in memory
///
///  @param  bReadHeaderOnly bool  [=false] 	Flag to determine if reading header section only
///  @return bool	true if success; false if fail
///
///  \remark The reading of masked cells and outlier information is determined by the reading state
///          which is set in ReadEx().
///////////////////////////////////////////////////////////////////////////////
bool CCELFileData::ReadTextCel(bool bReadHeaderOnly)
{
	bool retVal = false;
  std::string tmp_FileName;
  tmp_FileName=ResolveName();
  if (tmp_FileName=="") {
    SetError("Cant find file: '"+m_FileName+"'");
    return false;
  }

  //#ifdef CELFILE_USE_STDSTREAM
	// Open the file.
	std::ifstream instr;
	instr.open(tmp_FileName.c_str(), std::ios::in);
	// Check if open
	if (!instr)
	{
		SetError("Unable to open the file." " (stdstream)");
		return false;
	}
  //#endif
//#ifdef CELFILE_USE_ZLIB
//  gzFile instr;
//  instr=gzopen(tmp_FileName.c_str(),"rb");
//  if (instr==NULL) {
//    SetError("Unable to open the file." " (gzopen)");
//  }
//#endif

	Clear();

	const int MAXLINELENGTH = 4096;
	char pszHeader[MAXLINELENGTH];
	char tempStr[MAXLINELENGTH];

	// Extract a line of header
  //#ifdef CELFILE_USE_STDSTREAM
	instr.getline(pszHeader, sizeof(pszHeader));
  //#endif
//#ifdef CELFILE_USE_ZLIB
//  gzgets(instr,pszHeader,sizeof(pszHeader));
//#endif

	//Determine the version number
	if (strncmp(pszHeader,"[CEL]",5)==0)
		m_HeaderData.SetVersion(3);
	else if (strncmp(pszHeader,"COLS/ROWS=",10)==0)
		m_HeaderData.SetVersion(2);
	else
	{
		SetError("Unrecognized CEL file format.");
		return false;
	}

	//read and store the header
	if (m_HeaderData.GetVersion() == 2)
	{
		m_HeaderData.SetHeader(pszHeader);
		instr.getline(pszHeader, MAXLINELENGTH);
		m_HeaderData.AppendHeader(pszHeader);
		instr.getline(pszHeader, MAXLINELENGTH);

		// Store rows and columns
		int iCols = 0;
		int iRows = 0;
		sscanf(m_HeaderData.GetHeader().c_str(), "COLS/ROWS=%d %d", &iCols, &iRows);
		m_HeaderData.SetCols(iCols);
		m_HeaderData.SetRows(iRows);
		m_HeaderData.SetCells(iCols*iRows);
	}
	else
	{
		bool moreSpace=true;
		//Read past [HEADER] to first line of header data
		while (moreSpace)
		{
			if (!instr.getline(pszHeader, MAXLINELENGTH))
				return false;
			if (strncmp(pszHeader,"[HEADER]",8)==0)
				moreSpace=false;
		}
		int iCols = 0;
		int iRows = 0;
		instr.getline(pszHeader, MAXLINELENGTH);
		sscanf(pszHeader,"Cols=%d",&iCols);
		m_HeaderData.SetCols(iCols);
		m_HeaderData.SetHeader(pszHeader);
		instr.getline(pszHeader, MAXLINELENGTH);
		sscanf(pszHeader,"Rows=%d",&iRows);
		m_HeaderData.SetRows(iRows);
		m_HeaderData.AppendHeader(pszHeader);
		m_HeaderData.SetCells(iRows*iCols);

		//Now read the rest of the header
		bool moreHeader=true;
		while (moreHeader)
		{
			if (!instr.getline(pszHeader, MAXLINELENGTH))
				return false;
			if (strncmp(pszHeader,"DatHeader=",10)==0) //Last line of the header
			{
                if (pszHeader[strlen(pszHeader) - 1] == '\r') pszHeader[strlen(pszHeader) - 1] = '\0';
				m_HeaderData.SetDatHeader(pszHeader + 10);
			}

			if (strncmp(pszHeader,"Algorithm=",10)==0) //Last line of the header
			{
				sscanf(pszHeader,"Algorithm=%s",tempStr);
				m_HeaderData.SetAlg(tempStr);
			}

			if (strncmp(pszHeader,"AlgorithmParameters=",20)==0) //Last line of the header
			{
				sscanf(pszHeader,"AlgorithmParameters=%s",tempStr);
				m_HeaderData.SetParams(tempStr);
				moreHeader=false;
			}
			m_HeaderData.AppendHeader(pszHeader);
		}
	}

	// Set the chip type
	m_HeaderData.ParseChipType();

	// Parse algorithm parameters into map
	m_HeaderData.ParseAlgorithmParameters();
	m_HeaderData.SetMargin(atoi(m_HeaderData.GetAlgorithmParameter("CellMargin").c_str()));

	// Set grid coordinates
	m_HeaderData.ParseCorners();

	char paramString[2000];
	GridCoordinatesType grid = m_HeaderData.GetGridCorners();
	snprintf(paramString,sizeof(paramString), "Cols=%d\nRows=%d\nTotalX=%d\nTotalY=%d\nOffsetX=%d\nOffsetY=%d\n"
				"GridCornerUL=%d %d\nGridCornerUR=%d %d\nGridCornerLR=%d %d\nGridCornerLL=%d %d\n"
				"Axis-invertX=%d\nAxisInvertY=%d\nswapXY=%d\nDatHeader=%s"
				"\nAlgorithm=%s\nAlgorithmParameters=",
				m_HeaderData.GetCols(),m_HeaderData.GetRows(),
				m_HeaderData.GetCols(),m_HeaderData.GetRows(),
				0,0,
				grid.upperleft.x, grid.upperleft.y,
				grid.upperright.x, grid.upperright.y,
				grid.lowerright.x, grid.lowerright.y,
				grid.lowerleft.x, grid.lowerleft.y,
				0,0,0,
				m_HeaderData.GetDatHeader().c_str(),
				m_HeaderData.GetAlg().c_str());
	std::string param = paramString;
	param += m_HeaderData.GetAlgorithmParameters();
	param += "\n";
	m_HeaderData.SetHeader(param.c_str());

	// Don't continue if just reading the header.
	if (bReadHeaderOnly)
		return true;

	// Create memory for Mean data.
	m_HeaderData.SetCells(m_HeaderData.GetRows() * m_HeaderData.GetCols());
	m_pEntries = new CELFileEntryType[m_HeaderData.GetCells()];

	int t_x,t_y,t_pixels;
	float t_mean,t_stdv;

	// Read v2 CEL files
	if (m_HeaderData.GetVersion() == 2)
	{
		// Write the Mean data
		const char *strCellEntryFormat="%d %d %f %f %d";
		for (int iCell=0; iCell < m_HeaderData.GetCells(); iCell++)
		{
			instr.getline(pszHeader, MAXLINELENGTH);
			sscanf(pszHeader, strCellEntryFormat,          
				&t_x,
				&t_y,
				&t_mean,
				&t_stdv,
				&t_pixels);
			SetIntensity(t_x,t_y,t_mean);
			SetStdv(t_x,t_y,t_stdv);
			SetPixels(t_x,t_y,t_pixels);
		}
		retVal = true;
	}
	else
	{
		//Advance to the beginning of the Mean data
		bool readMore=true;
		while(readMore)
		{
			if (!instr.getline(pszHeader, MAXLINELENGTH))
				return false;
			if (strncmp(pszHeader,"[INTENSITY]",11)==0)
				readMore=false;
		}
  		instr.getline(pszHeader, MAXLINELENGTH);//Data starts at 2 lines past [Mean]
  		instr.getline(pszHeader, MAXLINELENGTH);//Data starts at 2 lines past [Mean]

		//Read the Mean data
		int iCell=0;
		readMore=true;
		const char *strCellEntryFormat="%d\t%d\t%f\t%f\t%d";

		while (readMore)
		{
			if (!instr.getline(pszHeader, MAXLINELENGTH)) //end of file
				readMore=false;
			else if (strlen(pszHeader) < MIN_CELLSTR )// blank line at end of data
				readMore=false;
			else
			{
				sscanf(pszHeader, strCellEntryFormat,          
					&t_x,
					&t_y,
					&t_mean,
					&t_stdv,
					&t_pixels);
				SetIntensity(t_x,t_y,t_mean);
				SetStdv(t_x,t_y,t_stdv);
				SetPixels(t_x,t_y,t_pixels);
				++iCell;
			}
		}

		//Advance to the Masked data
		readMore=true;
		while(readMore)
		{
			if (!instr.getline(pszHeader, MAXLINELENGTH)) //end of file
				return false;
			if (strncmp(pszHeader,"[MASKS]",7)==0)
				readMore=false;
		}
		//Read number of masked cells
		instr.getline(pszHeader, MAXLINELENGTH);
		int nMasked=0;
		sscanf(pszHeader, "NumberCells=%d", &nMasked);

		m_HeaderData.SetMasked(nMasked);
		instr.getline(pszHeader, MAXLINELENGTH);//skip over the header

		//Read the masked data
		if (m_bReadMaskedCells)
		{
			readMore=true;
			while (readMore)
			{
				if (!instr.getline(pszHeader, MAXLINELENGTH)) //end of file
					readMore=false;
				else if (strlen(pszHeader) < MIN_CELLSTR )// blank line at end of data
					readMore=false;
				else
				{
					int x, y, iCellEntry;
					sscanf(pszHeader, "%d\t%d", &x, &y);
					iCellEntry = y * m_HeaderData.GetCols() + x;
					m_MaskedCells.insert(std::make_pair(iCellEntry, true));
				}
			}
		}
		else 
			m_HeaderData.SetMasked(0);

		//Advance to the outlier data
		readMore=true;
		while(readMore)
		{
			if (!instr.getline(pszHeader, MAXLINELENGTH)) //end of file
				return false;
			if (strncmp(pszHeader,"[OUTLIERS]",10)==0)
				readMore=false;
		}
		//Read number of outlier cells
		instr.getline(pszHeader, MAXLINELENGTH);
		int nOutliers=0;
		sscanf(pszHeader, "NumberCells=%d", &nOutliers);
		m_HeaderData.SetOutliers(nOutliers);
		instr.getline(pszHeader, MAXLINELENGTH);//skip over the header

		//Read the outlier data
		if (m_bReadOutliers)
		{
			readMore=true;
			while (readMore)
			{
				if (!instr.getline(pszHeader, MAXLINELENGTH)) //end of file
					readMore=false;
				else if (strlen(pszHeader) < MIN_CELLSTR )// blank line at end of data
					readMore=false;
				else
				{
					int x, y, iCellEntry;
					sscanf(pszHeader, "%d\t%d", &x, &y);
					iCellEntry = y * m_HeaderData.GetCols() + x;
					m_Outliers.insert(std::make_pair(iCellEntry, true));
				}
			}
		}
		else
			m_HeaderData.SetOutliers(0);
		retVal = true;
	}
	instr.close();
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
///  public  Clear
///  \brief Reset content and deallocate memory
///
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::Clear()
{
  // throw away our memmap on the way out.
  Munmap();

	m_HeaderData.Clear();
	m_MaskedCells.clear();
	m_Outliers.clear();

  delete [] m_pEntries; 
  m_pEntries=NULL;
  delete [] m_pTransciptomeEntries; 
	m_pTransciptomeEntries = NULL;
  delete [] m_pMeanIntensities; 
	m_pMeanIntensities = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///  public  Munmap
///  \brief Unmap file view
///
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::Munmap() 
{
	// If there isnt a mapping we dont have to munmap...
	if (m_lpFileMap == NULL) {
    // ...but we should get rid of memory which might have been alloced.
    delete [] m_pEntries;
    m_pEntries=NULL;
		return;
	}

	// zero out the pointers which are now invalid... 
  m_lpData = NULL;
	m_pTransciptomeEntries = NULL;
	m_pEntries = NULL;
	m_pMeanIntensities = NULL;

	// free the map
#ifdef _MSC_VER
	if (m_lpFileMap != NULL)
	{
		UnmapViewOfFile(m_lpFileMap);
		CloseHandle(m_hFileMap);
		m_hFileMap = NULL;
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
#else
	if ((m_lpFileMap != MAP_FAILED) && (m_lpFileMap != NULL))
	{
    //printf("=== unmap %p\n",m_lpFileMap); // jhg
		munmap(m_lpFileMap, m_MapLen);
		// m_lpFileMap = MAP_FAILED; // why?
		m_MapLen = 0;
		// should be closed already.
		if (m_File != NULL) 
		{
			fclose(m_File);
			m_File = NULL;
		}
	}
#endif
  m_lpFileMap=NULL;
}

///////////////////////////////////////////////////////////////////////////////
///  public  Close
///  \brief Unmap file view and reset data pointer 
///
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::Close()
{
	Clear();
}

///////////////////////////////////////////////////////////////////////////////
///  public  ReadHeader
///  \brief Determine CEL file format and call appropriate function to read header section only
///
///  @return bool	true if success, false if fail
///  
///  \remark File name has to be preset using SetFileName()
///  \see Read, Open
///////////////////////////////////////////////////////////////////////////////
bool CCELFileData::ReadHeader()
{
	// Read the header, close if failed.
	if (Open(true) == false)
	{
		Close();
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///  public  Read
///  \brief Determine CEL file format and call appropriate function to read file
///
///  @param bIncludeMaskAndOutliers bool Flag for including masked cells and outliers 
///  @return bool	true if success, false if fail
///  
///  \remark File name has to be preset using SetFileName()
///  \see ReadEx, Open
///////////////////////////////////////////////////////////////////////////////
bool CCELFileData::Read(bool bIncludeMaskAndOutliers)
{
	m_bReadMaskedCells = bIncludeMaskAndOutliers;
	m_bReadOutliers = bIncludeMaskAndOutliers;

	// Open the file
	if (Open() == false)
	{
		Close();
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///  public  ReadEx
///
///  \brief Determine CEL file format and call appropriate function to read file using the specified file name
///
///  @param  filename const char * [=0] 	File name of CEL file to be read
///  @param  nState int  [=CEL_ALL] 	Reading state 
///  @return bool	true if success, false if fail
///  
///  \a nState can be one or combination of the following values:\n\n
///          CEL_ALL      Read all information in file (default)\n
///          CEL_DATA     Read header and intensities only\n
///			 CEL_OUTLIER  Read header, intensities and outliers\n
///			 CEL_MASK     Read header, intensities and masked cells\n\n
///  File name previously set by using SetFileName() will be replaced by the current input.
///
///  \see Read, Open, GetReadingState
///////////////////////////////////////////////////////////////////////////////
bool CCELFileData::ReadEx(const char *filename, int nState)
{
	m_nReadState = nState;

	// Open the file
	SetFileName(filename);

	// Determine if outliers should be read
	if ((nState & (CEL_ALL | CEL_OUTLIER)))
		this->m_bReadOutliers = true;
	else
		this->m_bReadOutliers = false;

	// Determine if masks should be read
	if ((nState & (CEL_ALL | CEL_MASK)))
		this->m_bReadMaskedCells = true;
	else
		this->m_bReadMaskedCells = false;

	if (Open() == false)
	{
		Close();
		return false;
	}
	return true;
}


/// Length of buffer for GetHeaderKey.
#define SVALUE_LENGTH 50

///////////////////////////////////////////////////////////////////////////////
///  public  GetHeaderKey
///  \brief Retrieve header value in string by specifying header key
///
///  @param  key const char * 	Header section key
///  @return std::string	Header section value
///////////////////////////////////////////////////////////////////////////////

std::string CCELFileData::GetHeaderKey(const char* key)
{
	assert(key != NULL);
  // tmp buff for snprintf
  char buf[SVALUE_LENGTH+1];

	std::string strKey = key;
	std::transform(strKey.begin(), strKey.end(), strKey.begin(), toupper);

	if (strKey == "HEADER") {
		return GetHeaderString();
  }
	else if (strKey == "VERSION")
	{
		snprintf(buf,SVALUE_LENGTH, "%d", GetVersion());
		return std::string(buf);
	}
	else if (strKey == "COLS")
	{
		snprintf(buf,SVALUE_LENGTH, "%d", GetCols());
		return std::string(buf);
	}
	else if (strKey == "ROWS")
	{
		snprintf(buf,SVALUE_LENGTH, "%d", GetRows());
		return std::string(buf);
	}
	else if (strKey == "TOTALX")
	{
		snprintf(buf,SVALUE_LENGTH, "%d", GetCols());
		return std::string(buf);
	}
	else if (strKey == "TOTALY")
	{
		snprintf(buf,SVALUE_LENGTH, "%d", GetRows());
		return std::string(buf);
	}
	else if (strKey == "GRIDCORNERUL")
	{
		snprintf(buf,SVALUE_LENGTH, "(%d, %d)", GetGridCorners().upperleft.x, GetGridCorners().upperleft.y);
		return std::string(buf);
	}
	else if (strKey == "GRIDCORNERUR")
	{
		snprintf(buf,SVALUE_LENGTH, "(%d, %d)", GetGridCorners().upperright.x, GetGridCorners().upperright.y);
		return std::string(buf);
	}
	else if (strKey == "GRIDCORNERLL")
	{
		snprintf(buf,SVALUE_LENGTH, "(%d, %d)", GetGridCorners().lowerleft.x, GetGridCorners().lowerleft.y);
		return std::string(buf);
	}
	else if (strKey == "GRIDCORNERLR")
	{
		snprintf(buf,SVALUE_LENGTH, "(%d, %d)", GetGridCorners().lowerright.x, GetGridCorners().lowerright.y);
		return std::string(buf);
	}
	else if (strKey == "OFFSETX")
		return "0";
	else if (strKey == "OFFSETY")
		return "0";
	else if (strKey == "AXIS-INVERTX")
		return "0";
	else if (strKey == "AXISINVERTY")
		return "0";
	else if (strKey == "SWAPXY")
		return "0";
	else if (strKey == "DATHEADER")
		return GetDatHeader();
	else if (strKey == "ALGORITHM")
		return GetAlg();
	else if (strKey == "ALGORITHMPARAMETERS")
		return GetParams();
	else if (strKey == "NUMBERCELLS")
	{
		snprintf(buf,SVALUE_LENGTH, "%d", GetNumCells());
		return std::string(buf);
	}
	else if (strKey == "NUMBERMASKEDCELLS")
	{
		snprintf(buf,SVALUE_LENGTH, "%d", GetNumMasked());
		return std::string(buf);
	}
	else if (strKey == "NUMBEROUTLIERCELLS")
	{
		snprintf(buf,SVALUE_LENGTH, "%d", GetNumOutliers());
		return std::string(buf);
	}
  // unknown header...
	return "";
}

#undef SVALUE_LENGTH // dont need this any more.


///////////////////////////////////////////////////////////////////////////////
///  public  GetEntry
///  \brief Retrieve entry of specified cell for xda and text formats
///
///  @param  x int  X coordinates
///  @param  y int  Y coordinates
///  @param  entry CELFileEntryType& Cell entry
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::GetEntry(int x, int y, CELFileEntryType &entry) 
{ 
	assert((x >= 0) && (x <= m_HeaderData.GetCols()));
	assert((y >= 0) && (y <= m_HeaderData.GetRows()));

	GetEntry(XYToIndex(x,y), entry); 
}

///////////////////////////////////////////////////////////////////////////////
///  public  GetEntry
///  \brief Retrieve entry of specified cell for xda and text formats
///
///  @param  index int  Cell index
///  @param  entry CELFileEntryType& Cell entry
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::GetEntry(int index, CELFileEntryType &entry) 
{ 
	assert((index >= 0) && (index < m_HeaderData.GetCells()));

	entry.Intensity = GetIntensity(index);
	entry.Stdv = GetStdv(index);
	entry.Pixels = GetPixels(index);
}

///////////////////////////////////////////////////////////////////////////////
///  public  GetTranscriptomeEntry
///  \brief Retrieve entry of specified cell for transcriptome bcel format
///
///  @param  x int  X coordinates
///  @param  y int  Y coordinates
///  @param  entry CELFileTranscriptomeEntryType& Cell entry
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::GetTranscriptomeEntry(int x, int y, CELFileTranscriptomeEntryType &entry) 
{ 
	assert((x >= 0) && (x <= m_HeaderData.GetCols()));
	assert((y >= 0) && (y <= m_HeaderData.GetRows()));

	GetTranscriptomeEntry(XYToIndex(x,y), entry); 
}

///////////////////////////////////////////////////////////////////////////////
///  public  GetTranscriptomeEntry
///  \brief Retrieve entry of specified cell for transcriptome bcel format
///
///  @param  index int  Cell index
///  @param  entry CELFileTranscriptomeEntryType& Cell entry
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::GetTranscriptomeEntry(int index, CELFileTranscriptomeEntryType &entry) 
{ 
	assert((index >= 0) && (index < m_HeaderData.GetCells()));

	entry.Intensity = (uint16_t) RoundNumber(GetIntensity(index));
	entry.Stdv = (uint16_t) RoundNumber(GetStdv(index));
	entry.Pixels = (uint8_t) GetPixels(index);
}

///////////////////////////////////////////////////////////////////////////////
///  public  GetIntensity
///  \brief Retrieve intensity of specified cell
///
///  @param  x int  X coordinates
///  @param  y int  Y coordinates
///  @return float	intensity
///////////////////////////////////////////////////////////////////////////////
float CCELFileData::GetIntensity(int x, int y) 
{ 
	assert((x >= 0) && (x <= m_HeaderData.GetCols()));
	assert((y >= 0) && (y <= m_HeaderData.GetRows()));

	return GetIntensity(XYToIndex(x,y)); 
}

///////////////////////////////////////////////////////////////////////////////
///  public  GetIntensity
///  \brief Retrieve intensity of specified cell
///
///  @param  index int  Cell index
///  @return float	intensity
///////////////////////////////////////////////////////////////////////////////
float CCELFileData::GetIntensity(int index)
{
	float fIntensity = 0;

	assert((index >= 0) && (index < m_HeaderData.GetCells()));

	if (m_FileFormat == TEXT_CEL) 
	{
		//fIntensity = m_pEntries[index].Mean;
		fIntensity=MmGetFloat_I(&m_pEntries[index].Intensity);
	}
	else if (m_FileFormat == XDA_BCEL) 
	{
		//fIntensity = GetFloat(&(m_pEntries[index].Intensity), m_FileFormat);
		fIntensity=MmGetFloat_I(&(m_pEntries[index].Intensity));
	}
	else if (m_FileFormat == TRANSCRIPTOME_BCEL) 
	{
		//fIntensity = (float) (GetUShort(&(m_pTransciptomeEntries[index].Intensity), m_FileFormat));
		fIntensity=MmGetUInt16_N(&(m_pTransciptomeEntries[index].Intensity));
	}
	else if (m_FileFormat == COMPACT_BCEL) 
	{
		//fIntensity = (float) (GetUShort(m_pMeanIntensities + index, m_FileFormat));
		fIntensity=MmGetUInt16_I(&m_pMeanIntensities[index]);
	}
	else 
	{
		assert(0);
	}

	return fIntensity;
}

int CCELFileData::GetIntensities(int index,std::vector<float>& intensities)
{
  int idx_start=index;
  int idx_end=idx_start+intensities.size();
  // the start and end indexes must be valid...
	assert((idx_start >= 0) && (idx_end <= m_HeaderData.GetCells()));

  // determine the format once, then copy a vectors worth of data.
	if (m_FileFormat == TEXT_CEL) 
	{
    for (int idx=idx_start;idx<idx_end;idx++) {
      intensities[idx]=MmGetFloat_I(&m_pEntries[idx].Intensity);
    }
  }
	else if (m_FileFormat == XDA_BCEL) 
	{
    for (int idx=idx_start;idx<idx_end;idx++) {
      intensities[idx]=MmGetFloat_I(&m_pEntries[idx].Intensity);
    }
	}
	else if (m_FileFormat == TRANSCRIPTOME_BCEL) 
	{
    for (int idx=idx_start;idx<idx_end;idx++) {
      intensities[idx]=MmGetUInt16_N(&m_pTransciptomeEntries[idx].Intensity);
    }
	}
	else if (m_FileFormat == COMPACT_BCEL) 
	{
    for (int idx=idx_start;idx<idx_end;idx++) {
      intensities[idx]=MmGetUInt16_I(&m_pMeanIntensities[idx]);
    }
	}
	else {
    // Opps - we dont know what the format was.
    assert(0);
	}

  return 0;
}



///////////////////////////////////////////////////////////////////////////////
///  public  GetStdv
///  \brief Retrieve standard deviation of specified cell
///
///  @param  x int  	X coordinate of cell
///  @param  y int  	Y coordinate of cell
///  @return float	Standard deviation intensity
///////////////////////////////////////////////////////////////////////////////
float CCELFileData::GetStdv(int x, int y)
{
	assert((x >= 0) && (x <= m_HeaderData.GetCols()));
	assert((y >= 0) && (y <= m_HeaderData.GetRows()));

	return GetStdv(XYToIndex(x,y));
}

///////////////////////////////////////////////////////////////////////////////
///  public  GetStdv
///  \brief Retrieve standard deviation of specified cell
///
///  @param  index int  Cell index
///  @return float	Standard deviation intensity
///////////////////////////////////////////////////////////////////////////////
float CCELFileData::GetStdv(int index)
{
	assert((index >= 0) && (index < m_HeaderData.GetCells()));

	float fStdev = 0;
	if (m_FileFormat == TEXT_CEL)
		//fStdev = m_pEntries[index].Stdv;
		fStdev=MmGetFloat_I(&m_pEntries[index].Stdv);
	else if (m_FileFormat == XDA_BCEL)
		//fStdev = GetFloat(&(m_pEntries[index].Stdv), m_FileFormat);
		fStdev=MmGetFloat_I(&(m_pEntries[index].Stdv));
	else if (m_FileFormat == TRANSCRIPTOME_BCEL)
		//fStdev = (float) (GetUShort(&(m_pTransciptomeEntries[index].Stdv), m_FileFormat));
		fStdev=MmGetUInt16_N(&(m_pTransciptomeEntries[index].Stdv));
	else if (m_FileFormat == COMPACT_BCEL)
		fStdev=0;
	else 
		assert(0);

	return fStdev;
}

///////////////////////////////////////////////////////////////////////////////
///  public  GetPixels
///  \brief Retrieve number of pixels of specified cell
///
///  @param  x int  	X coordinate of cell
///  @param  y int  	Y coordinate of cell
///  @return short	Number of pixels
///////////////////////////////////////////////////////////////////////////////
short CCELFileData::GetPixels(int x, int y)
{
	assert((x >= 0) && (x <= m_HeaderData.GetCols()));
	assert((y >= 0) && (y <= m_HeaderData.GetRows()));

	return GetPixels(XYToIndex(x,y));
}

///////////////////////////////////////////////////////////////////////////////
///  public  GetPixels
///  \brief Retrieve number of pixels of specified cell
///
///  @param  index int  Cell index
///  @return short	Number of pixels
///////////////////////////////////////////////////////////////////////////////
short CCELFileData::GetPixels(int index)
{
	assert((index >= 0) && (index < m_HeaderData.GetCells()));

	short sPixels = 0;
	if (m_FileFormat == TEXT_CEL)
		//sPixels = m_pEntries[index].Pixels;
		sPixels=MmGetInt16_I(&m_pEntries[index].Pixels);
	else if (m_FileFormat == XDA_BCEL)
		//sPixels = GetShort(&(m_pEntries[index].Pixels), m_FileFormat);
		sPixels=MmGetInt16_I(&m_pEntries[index].Pixels);
	else if (m_FileFormat == TRANSCRIPTOME_BCEL)
		//sPixels = (short) m_pTransciptomeEntries[index].Pixels;
		sPixels=MmGetUInt8(&m_pTransciptomeEntries[index].Pixels);
	else if (m_FileFormat == COMPACT_BCEL)
		sPixels=0;
	else 
		assert(0);

	return sPixels;
}

///////////////////////////////////////////////////////////////////////////////
///  public  IsMasked
///  \brief Determine if specified cell is masked
///
///  @param  x int  	X coordinate of cell
///  @param  y int  	Y coordinate of cell
///  @return bool	true if masked cell; false otherwise
///////////////////////////////////////////////////////////////////////////////
bool CCELFileData::IsMasked(int x, int y)
{
	assert((x >= 0) && (x <= m_HeaderData.GetCols()));
	assert((y >= 0) && (y <= m_HeaderData.GetRows()));

	return IsMasked(XYToIndex(x,y));
}

///////////////////////////////////////////////////////////////////////////////
///  public  IsMasked
///  \brief Determine if specified cell is masked
///
///  @param  index int  Cell index
///  @return bool	true if masked cell; false otherwise
///////////////////////////////////////////////////////////////////////////////
bool CCELFileData::IsMasked(int index)
{
	assert((index >= 0) && (index < m_HeaderData.GetCells()));

	bool bReturn = false;
	std::map<int, bool>::iterator pos = m_MaskedCells.find(index);
	if (pos != m_MaskedCells.end())
		bReturn = true;
	return bReturn;
}

///////////////////////////////////////////////////////////////////////////////
///  public  IsOutlier
///  \brief Determine if specified cell is outlier
///
///  @param  x int  	X coordinate of cell
///  @param  y int  	Y coordinate of cell
///  @return bool	true if outlier; false otherwise
///////////////////////////////////////////////////////////////////////////////
bool CCELFileData::IsOutlier(int x, int y)
{
	assert((x >= 0) && (x <= m_HeaderData.GetCols()));
	assert((y >= 0) && (y <= m_HeaderData.GetRows()));

	return IsOutlier(XYToIndex(x,y));
}

///////////////////////////////////////////////////////////////////////////////
///  public  IsOutlier
///  \brief Determine if specified cell is outlier
///
///  @param  index int  Cell index
///  @return bool	true if outlier; false otherwise
///////////////////////////////////////////////////////////////////////////////
bool CCELFileData::IsOutlier(int index)
{
	assert((index >= 0) && (index < m_HeaderData.GetCells()));

	bool bReturn = false;
	std::map<int, bool>::iterator pos = m_Outliers.find(index);
	if (pos != m_Outliers.end())
		bReturn = true;
	return bReturn;
}

///////////////////////////////////////////////////////////////////////////////
///  public  IsXDACompatibleFile
///  \brief Determine if CEL file is in xda format
///
///  @return bool	true if xda; false otherwise
///////////////////////////////////////////////////////////////////////////////
bool CCELFileData::IsXDACompatibleFile()
{
  std::string tmp_FileName;
  tmp_FileName=ResolveName();

#ifdef CELFILE_USE_STDSTREAM
	// Open the file.
	std::ifstream instr(tmp_FileName.c_str(), std::ios::in | std::ios::binary);
	if (!instr)
		return 0;
#endif
#ifdef CELFILE_USE_ZLIB
  gzFile instr;
  instr=gzopen(tmp_FileName.c_str(),"rb");
  if (instr==NULL) {
    return 0;
  }
#endif

	// Read the magic number from the file.
	uint32_t magic=0;
	ReadUInt32_I(instr, magic);
	bool bXDAFile = ((magic == CELL_FILE_MAGIC_NUMBER) ? true : false);

#ifdef CELFILE_USE_STDSTREAM
	instr.close();
#endif
#ifdef CELFILE_USE_ZLIB
  gzclose(instr);
#endif

	return bXDAFile;
}

///////////////////////////////////////////////////////////////////////////////
///  public  IsVersion3CompatibleFile
///  \brief Determine if CEL file is in text (version 3) format
///
///  @return bool	true if version 3; false otherwise
///////////////////////////////////////////////////////////////////////////////
bool CCELFileData::IsVersion3CompatibleFile()
{
	// Open the file.
	std::ifstream instr(m_FileName.c_str(), std::ios::in);
	if (!instr)
		return false;

	bool status = false;
	const char *Version3Line = "[CEL]";
	const int MAXLINELENGTH = 16;
	char pszHeader[MAXLINELENGTH];

	// Extract the first line which should be [CEL] for a version 3 file
	instr.getline(pszHeader, MAXLINELENGTH);
	if (strncmp(pszHeader,Version3Line,strlen(Version3Line))==0)
		status = true;
	instr.close();
	return status;
}

///////////////////////////////////////////////////////////////////////////////
///  public  IsTranscriptomeBcelFile
///  \brief Determine if CEL file is in transcriptome binary format
///
///  @return bool	true if transcriptome binary; false otherwise
///////////////////////////////////////////////////////////////////////////////
bool CCELFileData::IsTranscriptomeBcelFile()
{
  std::string tmp_FileName;
  tmp_FileName=ResolveName();

	// Open the file.
#ifdef CELFILE_USE_STDSTREAM
	std::ifstream instr(tmp_FileName.c_str(), std::ios::in | std::ios::binary);
	if (!instr) {
		return 0;
  }
#endif
#ifdef CELFILE_USE_ZLIB
  gzFile instr;
  instr=gzopen(tmp_FileName.c_str(),"rb");
  if (instr==NULL) {
    return 0;
  }
#endif

	// Read the header marker from the file.
	char szMarker[BCEL_HEADER_LEN];
	ReadFixedCString(instr, szMarker, BCEL_HEADER_LEN);
	bool bTranscriptomeFile = false;
	if (strncmp(szMarker, BCEL_HEADER_BYTES, BCEL_HEADER_LEN) == 0)
		bTranscriptomeFile = true;
#ifdef CELFILE_USE_STDSTREAM
	instr.close();
#endif
#ifdef CELFILE_USE_ZLIB
  gzclose(instr);
#endif

	return bTranscriptomeFile;
}

///////////////////////////////////////////////////////////////////////////////
///  public  IsCompactCelFile
///  \brief Determine if CEL file is in compact binary format
///
///  @return bool	true if compact; false otherwise
///////////////////////////////////////////////////////////////////////////////
bool CCELFileData::IsCompactCelFile()
{
  std::string tmp_FileName;
  tmp_FileName=ResolveName();

	// Open the file.
#ifdef CELFILE_USE_STDSTREAM
	std::ifstream instr(tmp_FileName.c_str(), std::ios::in | std::ios::binary);
	if (!instr) {
		return 0;
  }
#endif
#ifdef CELFILE_USE_ZLIB
  gzFile instr;
  tmp_FileName=ResolveName();
  instr=gzopen(tmp_FileName.c_str(),"rb");
  if (instr==NULL) {
    return 0;
  }
#endif

	// Read the header marker from the file.
	char szMarker[CCEL_HEADER_LEN];
	ReadFixedCString(instr, szMarker, CCEL_HEADER_LEN);
	bool bCompactCelFile = false;
	if (strncmp(szMarker, CCEL_HEADER_BYTES, CCEL_HEADER_LEN) == 0)
		bCompactCelFile = true;

#ifdef CELFILE_USE_STDSTREAM
	instr.close();
#endif
#ifdef CELFILE_USE_ZLIB
  gzclose(instr);
#endif

	return bCompactCelFile;
}

///////////////////////////////////////////////////////////////////////////////
///  public  IsUnsupportedCompactCelFile
///  \brief Determine if CEL file is in unsupported compact binary format
///
///  @return bool	true if unsupported compact; false otherwise
///////////////////////////////////////////////////////////////////////////////
bool CCELFileData::IsUnsupportedCompactCelFile()
{
  std::string tmp_FileName;
  tmp_FileName=ResolveName();

#ifdef CELFILE_USE_STDSTREAM
	// Open the file.
	std::ifstream instr(tmp_FileName.c_str(), std::ios::in | std::ios::binary);
	if (!instr)
		return 0;
#endif
#ifdef CELFILE_USE_ZLIB
  gzFile instr;
  tmp_FileName=ResolveName();
  instr=gzopen(tmp_FileName.c_str(),"rb");
  if (instr==NULL) {
    return 0;
  }
#endif

	// Read the header marker from the file.
	char szMarker[CCEL_HEADER_LEN];
	ReadFixedCString(instr, szMarker, CCEL_HEADER_LEN);
	bool bCompactCelFile = false;
	if (strncmp(szMarker, OLD_CCEL_HEADER_BYTES, CCEL_HEADER_LEN) == 0)
		bCompactCelFile = true;

#ifdef CELFILE_USE_STDSTREAM
	instr.close();
#endif
#ifdef CELFILE_USE_ZLIB
  gzclose(instr);
#endif

	return bCompactCelFile;
}

///////////////////////////////////////////////////////////////////////////////
///  private  DetermineFileFormat
///  \brief Determine the CEL file format by checking teh file against each of the format type
///
///  @return void	
///  
///  \see IsXDACompatibleFile, IsTranscriptomeBcelFile, IsCompactCelFile
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::DetermineFileFormat()
{
	if (IsXDACompatibleFile())
		m_FileFormat = XDA_BCEL;
	else if (IsTranscriptomeBcelFile())
		m_FileFormat = TRANSCRIPTOME_BCEL;
	else if (IsCompactCelFile())
		m_FileFormat = COMPACT_BCEL;
	else if (IsUnsupportedCompactCelFile())
		m_FileFormat = 0;
	else
		m_FileFormat = TEXT_CEL;
}


///////////////////////////////////////////////////////////////////////////////
///  private  RoundNumber
///
///  \brief Round floating point to integer 
///
///  @param  f float  	Floating point
///  @return unsigned short	Rounded integer
///
///  RoundNumber(3.3) = 3\n
///  RoundNumber(3.5) = 4
///////////////////////////////////////////////////////////////////////////////
unsigned short CCELFileData::RoundNumber(float f)
{
	assert(f >= 0);
	unsigned short us = (unsigned short) f;
	if((f - (float) us) >= 0.5)
		us++;
	return us;
}

///////////////////////////////////////////////////////////////////////////////
///  public constructor  CCELFileData
///  \brief Default constructor
///
///  @return void	
///////////////////////////////////////////////////////////////////////////////
CCELFileData::CCELFileData()
{
	m_strError = "";
	m_FileName = "";
	m_pEntries = NULL;
	m_pTransciptomeEntries = NULL;
	m_pMeanIntensities = NULL;
	m_FileFormat = XDA_BCEL;
	m_lpFileMap = NULL;
	m_lpData = NULL;
	m_bReadMaskedCells = true;
	m_bReadOutliers = true;
	m_nReadState = CEL_ALL;
}

///////////////////////////////////////////////////////////////////////////////
///  public destructor  ~CCELFileData
///  \brief Destructor
///
///  @return void	
///////////////////////////////////////////////////////////////////////////////
CCELFileData::~CCELFileData()
{
	Clear();
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetError
///  \brief Set error string
///
///  @param  str const char * 	Error string
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetError(const std::string& str)
{
	m_strError = str;
}
void CCELFileData::SetError(const char* str)
{
  m_strError=str;
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetGridCorners
///  \brief Call CCELFileHeaderData::SetGridCorners() to set grid coordinates
///
///  @param  grid GridCoordinatesType 	Algorithm name
///  @return void	
///  \see CCELFileHeaderData::SetGridCorners
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetGridCorners(GridCoordinatesType grid)
{
	m_HeaderData.SetGridCorners(grid);
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetAlgorithmName
///  \brief Call CCELFileHeaderData::SetAlg() to set algorithm name
///
///  @param  str const char * 	Algorithm name
///  @return void	
///  \see CCELFileHeaderData::SetAlg
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetAlgorithmName(const char *str)
{
	m_HeaderData.SetAlg(str);
}

///////////////////////////////////////////////////////////////////////////////
///  public  AddAlgorithmParameter
///  \brief Call CCELFileHeaderData::AddAlgorithmParameter() to add algorithm parameter
///
///  @param  tag const char * 	Algorithm parameter tag
///  @param  value const char * 	Algorithm paramter value
///  @return void	
///  \see CCELFileHeaderData::AddAlgorithmParameter
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::AddAlgorithmParameter(const char *tag, const char *value)
{
	m_HeaderData.AddAlgorithmParameter(tag, value);
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetAlgorithmParameter
///  \brief Call CCELFileHeaderData::SetAlgorithmParameter() to add algorithm parameter
///
///  @param  tag const char * 	Algorithm parameter tag
///  @param  value const char * 	Algorithm paramter value
///  @return void	
///  \see CCELFileHeaderData::SetAlgorithmParameter
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetAlgorithmParameter(const char *tag, const char *value)
{
	m_HeaderData.SetAlgorithmParameter(tag, value);
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetAddAlgorithmParameter
///  \brief Call CCELFileHeaderData::SetAddAlgorithmParameter() to add algorithm parameter
///
///  @param  tag const char * 	Algorithm parameter tag
///  @param  value const char * 	Algorithm paramter value
///  @return void	
///  \see CCELFileHeaderData::SetAddAlgorithmParameter
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetAddAlgorithmParameter(const char *tag, const char *value)
{
	m_HeaderData.SetAddAlgorithmParameter(tag, value);
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetFileFormat
///
///  \brief Set CEL file format to determine the appropriate data storage in cell
///
///  @param  i int  	File format
///  @return void	
///
///  \a i can be one of the following values:\n
///  TEXT_CEL\n
///  XDA_BCEL\n
///  TRANSCRIPTOME_BCEL\n
///  COMPACT_BCEL\n
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetFileFormat(int i) 
{ 
	assert((i >= TEXT_CEL) && (i <= COMPACT_BCEL)); 
	m_FileFormat = i; 
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetDimensions
///  \brief Set array dimensions in header section and allocate memory for intensity data
///
///  @param  rows int  	Number of rows in array
///  @param  cols int  	Number of columns in array
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetDimensions(int rows, int cols)
{
	m_HeaderData.SetRows(rows);
	m_HeaderData.SetCols(cols);
	m_HeaderData.SetCells(rows * cols);
	GridCoordinatesType grid;
	grid.upperleft.x = 1;
	grid.upperleft.y = 1;
	grid.upperright.x = cols;
	grid.upperright.y = 1;
	grid.lowerleft.x = 1;
	grid.lowerleft.y = rows;
	grid.lowerright.x = cols;
	grid.lowerright.y = rows;
	m_HeaderData.SetGridCorners(grid);

  /// @todo UnMap
  // delete [] m_lpData;
  // m_lpData=NULL;
	delete [] m_pEntries;
  m_pEntries=NULL;
	delete [] m_pTransciptomeEntries;
  m_pTransciptomeEntries=NULL;
  
	if (m_FileFormat == TRANSCRIPTOME_BCEL)
		m_pTransciptomeEntries = new CELFileTranscriptomeEntryType[rows*cols];
	else if ((m_FileFormat == XDA_BCEL) || (m_FileFormat == TEXT_CEL))
		m_pEntries = new CELFileEntryType[rows*cols];
	else if (m_FileFormat == COMPACT_BCEL)
		m_pMeanIntensities = new unsigned short[rows*cols];
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetChipType
///  \brief Set chip type and regenerate header string
///
///  @param  str const char * 	Chip type
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetChipType(const char *str)
{
	assert(str != NULL);
	m_HeaderData.SetChipType(str);
	
	std::string strHeader = m_HeaderData.GetHeader();

	// Set the chip type in the header string.
	int endIndex = (int) strHeader.find(".1sq");
	int startIndex = (int) strHeader.rfind(" ", endIndex);
	int length = (int) strHeader.length();

	std::string start = strHeader.substr(0, startIndex + 1);
	std::string end = strHeader.substr(endIndex, length - endIndex);
	std::string newHeader =	start + m_HeaderData.GetChipType() + end;
	m_HeaderData.SetHeader(newHeader.c_str());
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetMargin
///  \brief Set cell margin
///
///  @param  margin int  	Cell margin
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetMargin(int margin)
{
	m_HeaderData.SetMargin(margin);
}

///////////////////////////////////////////////////////////////////////////////
///  public AllocateEntries
///  \brief Allocate memory for cell entries 
///
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::AllocateEntries()
{
	delete [] m_pEntries;
	m_pEntries = new CELFileEntryType[m_HeaderData.GetCells()];
}

///////////////////////////////////////////////////////////////////////////////
///  public overloaded  SetCellEntry
///  \brief Set cell file entry 
///
///  @param  x int  	X coordinates
///  @param  y int  	Y coordinates
///  @param  pEntry CELFileEntryType * 	Pointer to CEL File entry for xda and text formats
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetCellEntry(int x, int y, CELFileEntryType *pEntry)
{
	SetCellEntry(XYToIndex(x,y), pEntry);
}

///////////////////////////////////////////////////////////////////////////////
///  public overloaded  SetCellEntry
///  \brief Set cell file entry 
///
///  @param  index int		Cell index
///  @param  pEntry CELFileEntryType * 	Pointer to CEL File entry for xda and text formats
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetCellEntry(int index, CELFileEntryType *pEntry)
{
	m_pEntries[index] = *pEntry;
}

///////////////////////////////////////////////////////////////////////////////
///  public AllocateEntries
///  \brief Allocate memory for cell entries 
///
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::AllocateTranscriptomeEntries()
{
	delete [] m_pTransciptomeEntries;
	m_pTransciptomeEntries = new CELFileTranscriptomeEntryType[m_HeaderData.GetCells()];
}

///////////////////////////////////////////////////////////////////////////////
///  public overloaded  SetCellEntry
///  \brief Set cell file entry 
///
///  @param  x int  	X coordinates
///  @param  y int  	Y coordinates
///  @param  pEntry CELFileEntryType * 	Pointer to CEL File entry for transcriptome bcel format
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetTranscriptomeCellEntry(int x, int y, CELFileTranscriptomeEntryType *pEntry)
{
	SetTranscriptomeCellEntry(XYToIndex(x,y), pEntry);
}

///////////////////////////////////////////////////////////////////////////////
///  public overloaded  SetCellEntry
///  \brief Set cell file entry 
///
///  @param  index int		Cell index
///  @param  pEntry CELFileEntryType * 	Pointer to CEL File entry for transcriptome bcel format
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetTranscriptomeCellEntry(int index, CELFileTranscriptomeEntryType *pEntry)
{
	m_pTransciptomeEntries[index] = *pEntry;
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetIntensity
///  \brief Set intensity of specified cell
///
///  @param  x int  	X coordinate of cell
///  @param  y int  	Y coordinate of cell
///  @param  intensity float  	Mean intensity
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetIntensity(int x, int y, float intensity)
{
	assert((x >= 0) && (x <= m_HeaderData.GetCols()));
	assert((y >= 0) && (y <= m_HeaderData.GetRows()));

	SetIntensity(XYToIndex(x,y), intensity);
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetIntensity
///  \brief Set intensity of specified cell
///
///  @param  index int  Cell index
///  @param  intensity float  	Mean intensity
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetIntensity(int index, float intensity)
{
	assert((index >= 0) && (index < m_HeaderData.GetCells()));

	if (m_FileFormat == TEXT_CEL) 
	{
		MmSetFloat_I(&m_pEntries[index].Intensity,intensity);
	}
	else if (m_FileFormat == XDA_BCEL) 
	{
		//m_pEntries[index].Mean = GetFloat(&intensity, m_FileFormat);
		MmSetFloat_I(&m_pEntries[index].Intensity,intensity);
	}
	else if (m_FileFormat == TRANSCRIPTOME_BCEL)	
	{
		unsigned short s = RoundNumber(intensity);
		MmSetUInt16_N(&m_pTransciptomeEntries[index].Intensity,s);
	}
	else if (m_FileFormat == COMPACT_BCEL) 
	{
		unsigned short s = RoundNumber(intensity);
		//m_pMeanIntensities[index] = GetUShort(&s, m_FileFormat);
		MmSetUInt16_I(&m_pMeanIntensities[index],s);
	}
	else 
	{
		assert(0);
	}
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetStdv
///  \brief Set standard deviation intensity of specified cell
///
///  @param  x int  	X coordinate of cell
///  @param  y int  	Y coordinate of cell
///  @param  stdev float  	Standard deviation intensity
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetStdv(int x, int y, float stdev)
{
	assert((x >= 0) && (x <= m_HeaderData.GetCols()));
	assert((y >= 0) && (y <= m_HeaderData.GetRows()));

	SetStdv(XYToIndex(x,y), stdev); 
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetStdv
///  \brief Set standard deviation intensity of specified cell
///
///  @param  index int  	Cell index
///  @param  stdev float  	Standard deviation intensity
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetStdv(int index, float stdev)
{
	assert((index >= 0) && (index < m_HeaderData.GetCells()));

	if (m_FileFormat == TRANSCRIPTOME_BCEL)
	{
		unsigned short s = RoundNumber(stdev);
		//m_pTransciptomeEntries[index].Stdv = GetUShort(&s, m_FileFormat);
		MmSetUInt16_N(&m_pTransciptomeEntries[index].Stdv,s);
	}
	else if ((m_FileFormat == TEXT_CEL) || (m_FileFormat == XDA_BCEL))
		//m_pEntries[index].Stdv = GetFloat(&stdev, m_FileFormat);
		MmSetFloat_I(&m_pEntries[index].Stdv,stdev);
	else if (m_FileFormat != COMPACT_BCEL) 
	    assert(0);
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetPixels
///  \brief Set number of pixels of specified cell
///
///  @param  x int  	X coordinate of cell
///  @param  y int  	Y coordinate of cell
///  @param  pixels short  	Number of pixels
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetPixels(int x, int y, short pixels)
{
	assert((x >= 0) && (x <= m_HeaderData.GetCols()));
	assert((y >= 0) && (y <= m_HeaderData.GetRows()));

	SetPixels(XYToIndex(x,y), pixels); 
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetPixels
///  \brief Set number of pixels of specified cell
///
///  @param  index int  	Cell index
///  @param  pixels short  	Number of pixels
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetPixels(int index, short pixels)
{
	assert((index >= 0) && (index < m_HeaderData.GetCells()));

	if (m_FileFormat == TRANSCRIPTOME_BCEL)
		//m_pTransciptomeEntries[index].Pixels = (unsigned char) pixels;
    MmSetUInt8(&m_pTransciptomeEntries[index].Pixels,(uint8_t)pixels);
	else if ((m_FileFormat == TEXT_CEL) || (m_FileFormat == XDA_BCEL))
		//m_pEntries[index].Pixels = GetShort(&pixels, m_FileFormat);
		MmSetUInt16_I((uint16_t*)&m_pEntries[index].Pixels,(uint16_t)pixels);
	else if (m_FileFormat != COMPACT_BCEL) 
	    assert(0);
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetMask
///  \brief Set if specified cell is masked
///
///  @param  x int  	X coordinate of cell
///  @param  y int  	Y coordinate of cell
///  @param  masked bool  	true if masked; false otherwise
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetMask(int x, int y, bool masked)
{
	assert((x >= 0) && (x <= m_HeaderData.GetCols()));
	assert((y >= 0) && (y <= m_HeaderData.GetRows()));

	SetMask(XYToIndex(x,y), masked); 
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetMask
///  \brief Set if specified cell is masked
///
///  @param  index int  	Cell index
///  @param  masked bool  	true if masked; false otherwise
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetMask(int index, bool masked)
{
	assert((index >= 0) && (index < m_HeaderData.GetCells()));

	if (masked)
	{
		m_MaskedCells.insert(std::make_pair(index, true));
		m_HeaderData.IncrementMasked();
	}
	else
	{
		std::map<int, bool>::iterator pos = m_MaskedCells.find(index);
		if (pos != m_MaskedCells.end())
		{
			m_MaskedCells.erase(pos);
			m_HeaderData.DecrementMasked();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetOutlier
///  \brief Set if specified cell is outlier
///
///  @param  x int  	X coordinate of cell
///  @param  y int  	Y coordinate of cell
///  @param  outlier bool  	true if outlier; false otherwise
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetOutlier(int x, int y, bool outlier)
{
	assert((x >= 0) && (x <= m_HeaderData.GetCols()));
	assert((y >= 0) && (y <= m_HeaderData.GetRows()));

	SetOutlier(XYToIndex(x,y), outlier); 
}

///////////////////////////////////////////////////////////////////////////////
///  public  SetOutlier
///  \brief Set if specified cell is outlier
///
///  @param  index int  	Cell index
///  @param  outlier bool  	true if outlier; false otherwise
///  @return void	
///////////////////////////////////////////////////////////////////////////////
void CCELFileData::SetOutlier(int index, bool outlier)
{
	assert((index >= 0) && (index < m_HeaderData.GetCells()));

	if (outlier)
	{
		m_Outliers.insert(std::make_pair(index, true));
		m_HeaderData.IncrementOutliers();
	}
	else
	{
		std::map<int, bool>::iterator pos = m_Outliers.find(index);
		if (pos != m_Outliers.end())
		{
			m_Outliers.erase(pos);
			m_HeaderData.DecrementOutliers();
		}
	}
}

//////////////////////////////////////////////////////////////////////
