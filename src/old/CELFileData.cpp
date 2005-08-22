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

#include "CELFileData.h"
#include "FileIO.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <istream>
#include <fstream>

#ifndef WIN32
#include <sys/mman.h>
#endif

#ifdef _INCLUDE_UNISTD_HEADER_
#include <unistd.h>
#endif

using namespace affxcel;

//////////////////////////////////////////////////////////////////////

#define DELIMCHAR 0x14
#define MIN_CELLSTR 2
#define CELL_FILE_MAGIC_NUMBER 64
#define CELL_FILE_VERSION_NUMBER 4

#ifdef WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

//////////////////////////////////////////////////////////////////////

void CCELFileHeaderData::SetChipType()
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
				str[pEnd - pInfo] = NULL;
				m_ChipType = str;
				delete [] str;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////

CCELFileHeaderData::~CCELFileHeaderData()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////

CCELFileHeaderData::CCELFileHeaderData()
{
	Clear();
	m_Magic = 0;
	m_Version = 0;
	m_nCols = 0;
	m_nRows = 0;
	m_nCells = 0;
	m_Margin = 0;
	m_nOutliers = 0;
	m_nMasked = 0;
}

//////////////////////////////////////////////////////////////////////

void CCELFileData::SetFileName(const char *str)
{
	std::string file = str;
	int index = (int) file.rfind(PATH_SEPARATOR);
	if (index != -1)
	{
		m_FileName = file.c_str() + index + 1;
		m_DataPath = file;
		m_DataPath.resize(index);
	}
	else
		m_FileName = file;
}

//////////////////////////////////////////////////////////////////////

bool CCELFileData::Exists()
{
	// Format the full path name.
	m_FullPathName = m_DataPath;
	m_FullPathName += PATH_SEPARATOR;
	m_FullPathName += m_FileName;

	// Find the file stats.
	struct stat st;
	return (stat(m_FullPathName.c_str(), &st) == 0);
}

//////////////////////////////////////////////////////////////////////

bool CCELFileData::ReadFile(bool bReadHeaderOnly)
{
	// First close the file.
	Close();

	// Format the full path name.
	m_FullPathName = m_DataPath;
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
	{
		return ReadTextFormat(bReadHeaderOnly);
	}
}

//////////////////////////////////////////////////////////////////////

bool CCELFileData::ReadXDAFormat(bool bReadHeaderOnly)
{
	// Open the file.
	std::ifstream instr;
	instr.open(m_FullPathName.c_str(), std::ios::in | std::ios::binary);
	if (!instr)
	{
		m_strError = "Unable to open the file.";
		return false;
	}

	// Read the header.
	if (ReadXDAHeader(instr) == false)
		return false;

	// Read the remaining data.
	if (bReadHeaderOnly)
		return true;

	// Read the intensity data
	m_pEntries = new CELFileEntryType[m_HeaderData.m_nCells];
	for (int iCell=0; iCell<m_HeaderData.m_nCells; iCell++)
	{
		CELFileEntryType *pEntry = m_pEntries + iCell;
		READ_FLOAT(instr, pEntry->Intensity);
		READ_FLOAT(instr, pEntry->Stdv);
		READ_SHORT(instr, pEntry->Pixels);
	}

	// Read the footer.
	ReadXDAFooter(instr);

	return true;
}

//////////////////////////////////////////////////////////////////////

void CCELFileData::ReadXDAFooter(std::ifstream &instr)
{
	// Skip if excluding the mask and outliers.
	if (m_bIncludeMaskAndOutliers == false)
		return;

	// Read the mask data
	int iCell;
	short x;
	short y;
	for (iCell=0; iCell<m_HeaderData.m_nMasked; iCell++)
	{
		// Read the coordinate.
		READ_SHORT(instr, x);
		READ_SHORT(instr, y);

		// Set the mask flag.
		m_Masked[XYToIndex(x,y)] = 1;
	}

	// Read the outlier data
	for (iCell=0; iCell<m_HeaderData.m_nOutliers; iCell++)
	{
		// Read the coordinate.
		READ_SHORT(instr, x);
		READ_SHORT(instr, y);

		// Set the outlier flag
		m_Outliers[XYToIndex(x,y)] = 1;
	}
}

//////////////////////////////////////////////////////////////////////

bool CCELFileData::ReadXDAHeader(std::ifstream &instr)
{
	// Read the magic number.
	int magic;
	READ_INT(instr, magic);
	m_HeaderData.m_Magic = magic;

	// Check if new type.
	if (magic != CELL_FILE_MAGIC_NUMBER)
	{
		m_strError = "The file does not appear to be the correct format.";
		return false;
	}

	// Read the version
	int version;
	READ_INT(instr, version);
	m_HeaderData.m_Version = version;

	// Read the dimensions of the array
	READ_INT(instr, m_HeaderData.m_nRows);
	READ_INT(instr, m_HeaderData.m_nCols);
	READ_INT(instr, m_HeaderData.m_nCells);

	// Read the other members.
	char *sval=NULL;
	READ_STRING(instr, sval);
	m_HeaderData.m_Header = sval;
	delete[] sval;
	sval = NULL;
	READ_STRING(instr, sval);
	m_HeaderData.m_Alg = sval;
	delete[] sval;
	sval = NULL;
	READ_STRING(instr, sval);
	m_HeaderData.m_Params = sval;
	delete[] sval;
	sval = NULL;
	READ_INT(instr, m_HeaderData.m_Margin);
	unsigned int uval;
	READ_UINT(instr, uval);
	m_HeaderData.m_nOutliers = (m_bIncludeMaskAndOutliers == true ? uval : false);
	READ_UINT(instr, uval);
	m_HeaderData.m_nMasked = (m_bIncludeMaskAndOutliers == true ? uval : false);
	int nSubGrids;
	READ_INT(instr, nSubGrids);

	// Parse the grid corners from the header.
	m_HeaderData.ParseCorners();

	// Set the chip type
	m_HeaderData.SetChipType();

	return true;
}

//////////////////////////////////////////////////////////////////////

bool CCELFileData::ReadXDAFormatUsingMemMap(bool bReadHeaderOnly)
{
	// Open the file.
	std::ifstream instr;
	instr.open(m_FullPathName.c_str(), std::ios::in | std::ios::binary);
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


	// Read the footer.
	instr.seekg(sizeof(CELFileEntryType) * m_HeaderData.m_nCells, std::ios_base::cur);
	ReadXDAFooter(instr);


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
			m_lpData = ((char *)m_lpFileMap) + offset;
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
		m_lpData = ((char *)m_lpFileMap) + offset;
		m_bFileMapped = true;
	}

#endif

	return true;
}

//////////////////////////////////////////////////////////////////////

bool CCELFileData::ReadTextFormat(bool bReadHeaderOnly)
{
	// Open the file
	std::ifstream instr;
	instr.open(m_FullPathName.c_str(), std::ios::in);
	if (!instr)
	{
		m_strError = "Unable to open the file.";
		return false;
	}

	const int MAXLINELENGTH = 4096;
	char pszHeader[MAXLINELENGTH];
	char tempStr[MAXLINELENGTH];

	// Extract a line of header
	instr.getline(pszHeader, MAXLINELENGTH);

	//Determine the version number
	if (strncmp(pszHeader,"[CEL]",5)==0)
		m_HeaderData.m_Version=3;
	else if (strncmp(pszHeader,"COLS/ROWS=",10)==0)
		m_HeaderData.m_Version=2;
	else
	{
		m_strError = "Unrecognized CEL file format.";
		return false;
	}

	//read and store the header
	if (m_HeaderData.m_Version==2)
	{
		m_HeaderData.m_Header = pszHeader;
		instr.getline(pszHeader, MAXLINELENGTH);
		m_HeaderData.m_Header += pszHeader;
		instr.getline(pszHeader, MAXLINELENGTH);

		// Store rows and columns
		sscanf(m_HeaderData.m_Header.c_str(), "COLS/ROWS=%d %d", &m_HeaderData.m_nCols, &m_HeaderData.m_nRows);
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
		instr.getline(pszHeader, MAXLINELENGTH);
		sscanf(pszHeader,"Cols=%d",&m_HeaderData.m_nCols);
		m_HeaderData.m_Header = pszHeader;		//Initialize the header to this line of text
		instr.getline(pszHeader, MAXLINELENGTH);
		sscanf(pszHeader,"Rows=%d",&m_HeaderData.m_nRows);
		m_HeaderData.m_Header += pszHeader;

		//Now read the rest of the header
		bool moreHeader=true;
		while (moreHeader)
		{
			if (!instr.getline(pszHeader, MAXLINELENGTH))
				return false;
			if (strncmp(pszHeader,"Algorithm=",10)==0) //Last line of the header
			{
				sscanf(pszHeader,"Algorithm=%s",tempStr);
				m_HeaderData.m_Alg = tempStr;
			}

			if (strncmp(pszHeader,"AlgorithmParameters=",20)==0) //Last line of the header
			{
				sscanf(pszHeader,"AlgorithmParameters=%s",tempStr);
				m_HeaderData.m_Params = tempStr;
				moreHeader=false;
			}
			m_HeaderData.m_Header += pszHeader;
		}
	}

	// Store the total number of cells
	m_HeaderData.m_nCells = m_HeaderData.m_nRows * m_HeaderData.m_nCols;

	// Parse the grid corners from the header.
	m_HeaderData.ParseCorners();

	// Set the chip type
	m_HeaderData.SetChipType();

	// Don't continue if just reading the header.
	if (bReadHeaderOnly)
		return true;


	// Create memory for intensity data.
	m_pEntries = new CELFileEntryType[m_HeaderData.m_nCells];


	// Read v2 CEL files
	if (m_HeaderData.m_Version == 2)
	{
		// Write the intensity data
		const char *strCellEntryFormat="%d %d %f %f %d";
		int x,y,pix;
		for (int iCell=0; iCell<m_HeaderData.m_nCells; iCell++)
		{
			instr.getline(pszHeader, MAXLINELENGTH);
			CELFileEntryType *pEntry = m_pEntries + iCell;
			sscanf(pszHeader, strCellEntryFormat,          
				&x,
				&y,
				&pEntry->Intensity,
				&pEntry->Stdv,
				&pix);
			pEntry->Pixels = pix;
		}
	}
	else
	{

		//Advance to the beginning of the intensity data
		bool readMore=true;
		while(readMore)
		{
			if (!instr.getline(pszHeader, MAXLINELENGTH))
				return false;
			if (strncmp(pszHeader,"[INTENSITY]",11)==0)
				readMore=false;
		}
  		instr.getline(pszHeader, MAXLINELENGTH);//Data starts at 2 lines past [INTENSITY]
  		instr.getline(pszHeader, MAXLINELENGTH);//Data starts at 2 lines past [INTENSITY]


		//Read the intensity data
		int iCell=0;
		readMore=true;
		const char *strCellEntryFormat="%d %d %f %f %d";
		int x,y,pix;
		while (readMore)
		{
			if (!instr.getline(pszHeader, MAXLINELENGTH)) //end of file
				readMore=false;
			else if (strlen(pszHeader) < MIN_CELLSTR )// blank line at end of data
				readMore=false;
			else
			{
				CELFileEntryType *pEntry = m_pEntries + iCell;
				sscanf(pszHeader, strCellEntryFormat,          
					&x,
					&y,
					&pEntry->Intensity,
					&pEntry->Stdv,
					&pix);
				pEntry->Pixels = pix;
				++iCell;
			}
		}

		// Read the mask and outliers.
		if (m_bIncludeMaskAndOutliers == true)
		{

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
			m_HeaderData.m_nMasked = nMasked;
			instr.getline(pszHeader, MAXLINELENGTH);//skip over the header

			//Read the masked data
			readMore=true;
			while (readMore)
			{
				if (!instr.getline(pszHeader, MAXLINELENGTH)) //end of file
					readMore=false;
				else if (strlen(pszHeader) < MIN_CELLSTR )// blank line at end of data
					readMore=false;
				else
				{
					int x,y;
					sscanf(pszHeader, "%d\t%d",&x,&y);
					m_Masked[XYToIndex(x, y)] = 1;
				}
			}

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
			m_HeaderData.m_nOutliers = nOutliers;
			instr.getline(pszHeader, MAXLINELENGTH);//skip over the header

			//Read the outlier data
			readMore=true;
			while (readMore)
			{
				if (!instr.getline(pszHeader, MAXLINELENGTH)) //end of file
					readMore=false;
				else if (strlen(pszHeader) < MIN_CELLSTR )// blank line at end of data
					readMore=false;
				else
				{
					int x,y;
					sscanf(pszHeader, "%d\t%d",&x,&y);
					m_Outliers[XYToIndex(x, y)] = 1;
				}
			}
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////

void CCELFileData::Close()
{
	delete[] m_pEntries;
	m_pEntries = NULL;

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

bool CCELFileData::ReadHeader()
{
	// Read the header, close if failed.
	if (ReadFile(true) == false)
	{
		Close();
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CCELFileData::Read(bool bIncludeMaskAndOutliers)
{
	// Determine if the outliers or masked flags should be read.
	m_bIncludeMaskAndOutliers = bIncludeMaskAndOutliers;

	// Read the file
	if (ReadFile(false) == false)
	{
		Close();
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

void CCELFileHeaderData::ParseCorners()
{
    // get grid coordinates and size from the header
    const char * pch = strstr(m_Header.c_str(), "GridCorner");
	int ulx,uly;
	int urx,ury;
	int llx,lly;
	int lrx,lry;
    if ((pch == NULL) ||
          (sscanf(pch, "GridCornerUL=%d %d GridCornerUR=%d %d GridCornerLR=%d %d GridCornerLL=%d %d",
					&ulx, &uly,
					&urx, &ury,
					&lrx, &lry,
					&llx, &lly) != 8))
		return;

	m_CellGrid.upperleft.x = ulx;
	m_CellGrid.upperleft.y = uly;

	m_CellGrid.upperright.x = urx;
	m_CellGrid.upperright.y = ury;

	m_CellGrid.lowerright.x = lrx;
	m_CellGrid.lowerright.y = lry;

	m_CellGrid.lowerleft.x = llx;
	m_CellGrid.lowerleft.y = lly;
}

//////////////////////////////////////////////////////////////////////

void CCELFileHeaderData::SetGridCorners(GridCoordinatesType &grid)
{
	m_CellGrid.upperleft.x = grid.upperleft.x;
	m_CellGrid.upperleft.y = grid.upperleft.y;

	m_CellGrid.upperright.x = grid.upperright.x;
	m_CellGrid.upperright.y = grid.upperright.y;

	m_CellGrid.lowerright.x = grid.lowerright.x;
	m_CellGrid.lowerright.y = grid.lowerright.y;

	m_CellGrid.lowerleft.x = grid.lowerleft.x;
	m_CellGrid.lowerleft.y = grid.lowerleft.y;
}

//////////////////////////////////////////////////////////////////////

bool CCELFileData::IsXDACompatibleFile()
{
	// Format the full path name.
	m_FullPathName = m_DataPath;
	m_FullPathName += PATH_SEPARATOR;
	m_FullPathName += m_FileName;

	// Open the file.
	std::ifstream instr(m_FullPathName.c_str(), std::ios::in | std::ios::binary);
	if (!instr)
		return false;

	// Read the magic number from the file.
	int magic=0;
	READ_INT(instr, magic);
	bool bXDAFile = (magic == CELL_FILE_MAGIC_NUMBER);
	instr.close();

	return bXDAFile;
}

//////////////////////////////////////////////////////////////////////

CCELFileData::CCELFileData() : 
	m_pEntries(NULL), 
	m_bIncludeMaskAndOutliers(true),
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

CCELFileData::~CCELFileData()
{
	Close();
}

//////////////////////////////////////////////////////////////////////

void CCELFileData::Clear()
{
	m_HeaderData.Clear();
	delete[] m_pEntries;
	m_pEntries = NULL;
	m_Outliers.clear();
	m_Masked.clear();
}

//////////////////////////////////////////////////////////////////////

void CCELFileHeaderData::Clear()
{
	m_nRows = 0;
	m_nCols = 0;
	m_nCells = 0;
	m_Header = "";
	m_Alg = "";
	m_Params = "";
	m_ChipType = "";
	m_Margin = 1;
	m_nOutliers = 0;
	m_nMasked = 0;
	m_Magic = CELL_FILE_MAGIC_NUMBER;
	m_Version = CELL_FILE_VERSION_NUMBER;
	m_Parameters.erase(m_Parameters.begin(), m_Parameters.end());
}

//////////////////////////////////////////////////////////////////////

void CCELFileHeaderData::AddAlgorithmParameter(const char *tag, const char *value)
{
	TagValuePairType param;
	param.Tag = tag;
	param.Value = value;
	m_Parameters.push_back(param);
}

//////////////////////////////////////////////////////////////////////

void CCELFileHeaderData::SetDimensions(int rows, int cols)
{
	m_nRows = rows;
	m_nCols = cols;
	m_nCells = rows * cols;
}

//////////////////////////////////////////////////////////////////////

void CCELFileData::AllocateEntries()
{
	delete [] m_pEntries;
	m_pEntries = new CELFileEntryType[m_HeaderData.m_nCells];
}

//////////////////////////////////////////////////////////////////////

void CCELFileData::SetCellEntry(int index, CELFileEntryType *pEntry)
{
	m_pEntries[index] = *pEntry;
}

//////////////////////////////////////////////////////////////////////

void CCELFileData::SetCellEntry(int x, int y, CELFileEntryType *pEntry)
{
	SetCellEntry(XYToIndex(x,y), pEntry);
}

//////////////////////////////////////////////////////////////////////

bool CCELFileData::IsMasked(int index)
{
	return (m_Masked.count(index) == 0 ? false : true);
}

//////////////////////////////////////////////////////////////////////

bool CCELFileData::IsOutlier(int index)
{
	return (m_Outliers.count(index) == 0 ? false : true);
}

//////////////////////////////////////////////////////////////////////

float CCELFileData::GetIntensity(int index)
{
	CELFileEntryType entry;
	GetEntry(index, entry);
	return entry.Intensity;
}

//////////////////////////////////////////////////////////////////////

float CCELFileData::GetStdv(int index)
{
	CELFileEntryType entry;
	GetEntry(index, entry);
	return entry.Stdv;
}

//////////////////////////////////////////////////////////////////////

short CCELFileData::GetPixels(int index)
{
	CELFileEntryType entry;
	GetEntry(index, entry);
	return entry.Pixels;
}

//////////////////////////////////////////////////////////////////////

void CCELFileData::GetEntry(int index, CELFileEntryType &entry)
{
#if defined(_USE_MEM_MAPPING_)
	if (m_bFileMapped)
	{
		int offset = sizeof(CELFileEntryType) * index;
		entry = *(CELFileEntryType *) (m_lpData + offset);

#ifdef IS_BIG_ENDIAN
		float intensity=entry.Intensity;
		SwapBytes((char *)&intensity, (char *)&entry.Intensity, FLOAT_SIZE);

		float stdv=entry.Stdv;
		SwapBytes((char *)&stdv, (char *)&entry.Stdv, FLOAT_SIZE);

		short pix=entry.Pixels;
		SwapBytes((char *)&pix, (char *)&entry.Pixels, SHORT_SIZE);
#endif
	}
	else
	{
		entry = *(m_pEntries + index);
	}
#else
	entry = *(m_pEntries + index);
#endif
}

//////////////////////////////////////////////////////////////////////

void CCELFileData::SetMask(int index, bool mask)
{
	bool found = IsMasked(index);
	if (mask == true && found == false)
	{
		m_Masked[index] = 1;
	}
	else if (mask == false && found == true)
	{
		std::map<int, char>::iterator iter = m_Masked.find(index);
		m_Masked.erase(iter);
	}
	m_HeaderData.m_nMasked = (int) m_Masked.size();
}

//////////////////////////////////////////////////////////////////////

void CCELFileData::SetOutlier(int index, bool outlier)
{
	bool found = IsOutlier(index);
	if (outlier == true && found == false)
	{
		m_Outliers[index] = 1;
	}
	else if (outlier == false && found == true)
	{
		std::map<int, char>::iterator iter = m_Outliers.find(index);
		m_Outliers.erase(iter);
	}
	m_HeaderData.m_nOutliers = (int) m_Outliers.size();
}

//////////////////////////////////////////////////////////////////////
