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

#include "file/CELFileWriter.h"
//
#include "file/FileIO.h"
#include "file/FileWriter.h"
//
#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <istream>
#include <string.h>
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

#ifdef _MSC_VER
/// Line separator for _MSC_VER
#define LINE_SEPARATOR "\n"
#else
/// Line separator for unix/linux
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
///  public constructor  CCELFileWriter
///  \brief Default constructor
///
///  @return void
///////////////////////////////////////////////////////////////////////////////
CCELFileWriter::CCELFileWriter() : CCELFileData()
{
}

///////////////////////////////////////////////////////////////////////////////
///  public destructor  ~CCELFileWriter
///  \brief Destructor
///
///  @return void
///////////////////////////////////////////////////////////////////////////////
CCELFileWriter::~CCELFileWriter()
{
}

///////////////////////////////////////////////////////////////////////////////
///  public  WriteTextCel
///  \brief <Write CEL file in text format
///
///  @return bool	true if success; false otherwise
///////////////////////////////////////////////////////////////////////////////
bool CCELFileWriter::WriteTextCel()
{
	if (m_FileName.length() == 0)
	{
		SetError("No file name is set for file creation.");
		return false;
	}

	std::ofstream newCelFile;
	newCelFile.open(m_FileName.c_str(), std::ios::out);
	if (!newCelFile)
	{
		SetError("Unable to open the file.");
		return false;
	}

	m_HeaderData.SetDatHeader();

	const int MAXLINELENGTH = 4096;
	char szBuffer[MAXLINELENGTH] = "\0";
	int length = 0;

	// Write CEL
	length = snprintf(szBuffer,sizeof(szBuffer), "[CEL]%s", LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "Version=3%s%s", LINE_SEPARATOR, LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);

	// Write the header.
	GridCoordinatesType grid = m_HeaderData.GetGridCorners();
	length = snprintf(szBuffer,sizeof(szBuffer), "[HEADER]%s", LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "Cols=%d%sRows=%d%s", m_HeaderData.GetCols(), LINE_SEPARATOR, m_HeaderData.GetRows(), LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "TotalX=%d%sTotalY=%d%s", m_HeaderData.GetCols(), LINE_SEPARATOR, m_HeaderData.GetRows(), LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "OffsetX=0%sOffsetY=0%s", LINE_SEPARATOR, LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "GridCornerUL=%d %d%s", grid.upperleft.x, grid.upperleft.y, LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "GridCornerUR=%d %d%s", grid.upperright.x, grid.upperright.y, LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "GridCornerLR=%d %d%s", grid.lowerright.x, grid.lowerright.y, LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "GridCornerLL=%d %d%s", grid.lowerleft.x, grid.lowerleft.y, LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "Axis-invertX=0%sAxisInvertY=0%sswapXY=0%s", LINE_SEPARATOR, LINE_SEPARATOR, LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "DatHeader=%s%s", m_HeaderData.GetDatHeader().c_str(), LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "Algorithm=%s%s", m_HeaderData.GetAlg().c_str(), LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "AlgorithmParameters=%s%s%s", m_HeaderData.GetAlgorithmParameters().c_str(), LINE_SEPARATOR, LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);

	// Write the Mean header
	length = snprintf(szBuffer,sizeof(szBuffer), "[INTENSITY]%s", LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "NumberCells=%d%s", m_HeaderData.GetCells(), LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "CellHeader=X\tY\tMEAN\tSTDV\tNPIXELS%s", LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);

  //
	for (int iCell=0; iCell < m_HeaderData.GetCells(); iCell++)
	{
    int   t_x=IndexToX(iCell);
    int   t_y=IndexToY(iCell);
    float t_mean=GetIntensity(t_x,t_y);
    float t_stdv=GetStdv(t_x,t_y);
    int   t_pixel=GetPixels(t_x,t_y);

    snprintf(szBuffer,sizeof(szBuffer), "%3d\t%3d\t%.1f\t%.1f\t%3d",t_x,t_y,t_mean,t_stdv,t_pixel);
    newCelFile<<szBuffer<<LINE_SEPARATOR;
	}
	newCelFile<<LINE_SEPARATOR;


	// Write the mask data
	length = snprintf(szBuffer,sizeof(szBuffer), "[MASKS]%s", LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "NumberCells=%d%s", m_HeaderData.GetMasked(), LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "CellHeader=X\tY%s", LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	for(std::map<int, bool>::iterator pos = m_MaskedCells.begin(); pos != m_MaskedCells.end(); pos++)
	{
		length = snprintf(szBuffer,sizeof(szBuffer), "%d\t%d%s", IndexToX(pos->first), IndexToY(pos->first), LINE_SEPARATOR);
		newCelFile.write(szBuffer, length);
	}
	length = snprintf(szBuffer,sizeof(szBuffer), "%s", LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);

	// Write the outlier data
	length = snprintf(szBuffer,sizeof(szBuffer), "[OUTLIERS]%s", LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "NumberCells=%d%s", m_HeaderData.GetOutliers(), LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "CellHeader=X\tY%s", LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	for(std::map<int, bool>::iterator pos = m_Outliers.begin(); pos != m_Outliers.end(); pos++)
	{
		length = snprintf(szBuffer,sizeof(szBuffer), "%d\t%d%s", IndexToX(pos->first), IndexToY(pos->first), LINE_SEPARATOR);
		newCelFile.write(szBuffer, length);
	}

	// Write the blank modified section.
	length = snprintf(szBuffer,sizeof(szBuffer), "%s", LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "[MODIFIED]%s", LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "NumberCells=0%s", LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);	
	length = snprintf(szBuffer,sizeof(szBuffer), "CellHeader=X\tY\tORIGMEAN%s", LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);
	length = snprintf(szBuffer,sizeof(szBuffer), "%s", LINE_SEPARATOR);
	newCelFile.write(szBuffer, length);



	// Close the file and check the status.
	newCelFile.close();

	return !newCelFile.fail();
}

///////////////////////////////////////////////////////////////////////////////
///  public  WriteXDABCel
///  \brief Write CEL file in xda binary format
///
///  @return bool	true if success; false otherwise
///
///  \remark All multi-byte numeric values are stored in little endian byte order
///////////////////////////////////////////////////////////////////////////////
bool CCELFileWriter::WriteXDABCel()
{
	if (m_FileName.length() == 0)
	{
		SetError("No file name is set for file creation.");
		return false;
	}

	std::ofstream newCelFile;
	newCelFile.open(m_FileName.c_str(), std::ios::out | std::ios::binary);
	if (!newCelFile)
	{
		SetError("Unable to open the file.");
		return false;
	}

	m_HeaderData.SetDatHeader();

	// Write the header.
	WriteUInt32_I(newCelFile, CELL_FILE_MAGIC_NUMBER);
	WriteUInt32_I(newCelFile, CELL_FILE_VERSION_NUMBER);

	// Write the dimensions of the array
	WriteUInt32_I(newCelFile, m_HeaderData.GetRows());
	WriteUInt32_I(newCelFile, m_HeaderData.GetCols());
	WriteUInt32_I(newCelFile, m_HeaderData.GetCells());

	// Write the other members.
	WriteString_I(newCelFile, m_HeaderData.GetHeader());
	WriteString_I(newCelFile, m_HeaderData.GetAlg());
	WriteString_I(newCelFile, m_HeaderData.GetAlgorithmParameters());
	WriteUInt32_I(newCelFile, m_HeaderData.GetMargin());
	WriteUInt32_I(newCelFile, m_HeaderData.GetOutliers());
	WriteUInt32_I(newCelFile, m_HeaderData.GetMasked());
	WriteUInt32_I(newCelFile, 0);

	// Write the Mean data
	for (int iCell=0; iCell < m_HeaderData.GetCells(); iCell++) 
	{
		int   t_x=IndexToX(iCell);
		int   t_y=IndexToY(iCell);
		float t_mean=GetIntensity(t_x,t_y);
		float t_stdv=GetStdv(t_x,t_y);
		uint16_t t_pixel=GetPixels(t_x,t_y);

		WriteFloatLowPrecision(newCelFile,t_mean);
		WriteFloatLowPrecision(newCelFile,t_stdv);
		WriteUInt16_I(newCelFile,t_pixel);
	}

	// Write the mask data
	for(std::map<int, bool>::iterator pos = m_MaskedCells.begin(); pos != m_MaskedCells.end(); pos++)
	{
		WriteUInt16_I(newCelFile, (uint16_t) IndexToX(pos->first));
		WriteUInt16_I(newCelFile, (uint16_t) IndexToY(pos->first));
	}

	// Write the outlier data
	for(std::map<int, bool>::iterator pos = m_Outliers.begin(); pos != m_Outliers.end(); pos++)
	{
		WriteUInt16_I(newCelFile, (uint16_t) IndexToX(pos->first));
		WriteUInt16_I(newCelFile, (uint16_t) IndexToY(pos->first));
	}

	// Close the file and check the status.
	newCelFile.close();

	return !newCelFile.fail();
}

///////////////////////////////////////////////////////////////////////////////
///  public  WriteTranscriptomeBCel
///  \brief Write CEL file in transcriptome binary format
///
///  @return bool	true if success; false otherwise
///
///  \remark All multi-byte numeric values are stored in big endian byte order
///////////////////////////////////////////////////////////////////////////////
bool CCELFileWriter::WriteTranscriptomeBCel()
{
	if (m_FileName.length() == 0)
	{
		SetError("No file name is set for file creation.");
		return false;
	}

	std::ofstream newCelFile;
	newCelFile.open(m_FileName.c_str(), std::ios::out | std::ios::binary);
	if (!newCelFile)
	{
		SetError("Unable to open the file.");
		return false;
	}

	m_HeaderData.SetDatHeader();

	// Write the header
	WriteFixedString(newCelFile, BCEL_HEADER_BYTES, BCEL_HEADER_LEN);
	WriteFloat_N(newCelFile, (float)m_HeaderData.GetVersion());

	WriteUInt32_N(newCelFile, STRUCT_SIZE_BCEL_HEADER_INFO);
	WriteFixedString(newCelFile, BCEL_CHUNK_HEAD, BCEL_CHUNK_NAME_SIZE);
	WriteFloat_N(newCelFile, (float)m_HeaderData.GetVersion());
	WriteUInt32_N(newCelFile, m_HeaderData.GetCols());		// columns
	WriteUInt32_N(newCelFile, m_HeaderData.GetRows());		// rows
	WriteUInt32_N(newCelFile, m_HeaderData.GetCols());		// totalx
	WriteUInt32_N(newCelFile, m_HeaderData.GetRows());		// totaly
	WriteUInt32_N(newCelFile, 0);							// offsetx
	WriteUInt32_N(newCelFile, 0);							// offsety

	GridCoordinatesType grid = m_HeaderData.GetGridCorners();
	WriteUInt32_N(newCelFile, grid.upperleft.x);			// GridCornerUL.x
	WriteUInt32_N(newCelFile, grid.upperleft.y);			// GridCornerUL.y
	WriteUInt32_N(newCelFile, grid.upperright.x);			// GridCornerUR.x
	WriteUInt32_N(newCelFile, grid.upperright.y);			// GridCornerUR.y
	WriteUInt32_N(newCelFile, grid.lowerleft.x);			// GridCornerLL.y
	WriteUInt32_N(newCelFile, grid.lowerleft.y);			// GridCornerLL.x
	WriteUInt32_N(newCelFile, grid.lowerright.x);			// GridCornerLR.x
	WriteUInt32_N(newCelFile, grid.lowerright.y);			// GridCornerLR.x
	WriteUInt16_N(newCelFile, 0);							// Axis-invertX
	WriteUInt16_N(newCelFile, 0);							// AxisInvertY
	WriteUInt16_N(newCelFile, 0);							// swapXY
	WriteUInt32_N(newCelFile, 0);

	int length = (int) m_HeaderData.GetDatHeader().length();		// DatHeader
	WriteUInt32_N(newCelFile, length);
	WriteFixedString(newCelFile, BCEL_CHUNK_DTHD, BCEL_CHUNK_NAME_SIZE);
	if (length > 0)
		WriteFixedString(newCelFile, m_HeaderData.GetDatHeader(), length);
	else
	{
		SetError("Missing DatHeader.");
		Close();
		return 0;
	}
	WriteUInt32_N(newCelFile, 0);

	length = (int) m_HeaderData.GetAlg().length();					// Algorithm
	WriteUInt32_N(newCelFile, length);
	WriteFixedString(newCelFile, BCEL_CHUNK_ALGM, BCEL_CHUNK_NAME_SIZE);
	if (length > 0)
		WriteFixedString(newCelFile, m_HeaderData.GetAlg(), length);
	else
	{
		SetError("Missing Algorithm.");
		Close();
		return 0;
	}
	WriteUInt32_N(newCelFile, 0);

	length = (int) m_HeaderData.GetAlgorithmParameters().length();	// AlgorithmParameters
	WriteUInt32_N(newCelFile, length);
	WriteFixedString(newCelFile, BCEL_CHUNK_ALPR, BCEL_CHUNK_NAME_SIZE);
	if (length > 0)
		WriteFixedString(newCelFile, m_HeaderData.GetAlgorithmParameters(), length);
	else
	{
		SetError("Missing Algorithm Parameters.");
		Close();
		return 0;
	}
	WriteUInt32_N(newCelFile, 0);

	// Write the Mean data
	WriteUInt32_N(newCelFile,(uint32_t)(m_HeaderData.GetCells() * STRUCT_SIZE_FEATURE_DATA));
	WriteFixedString(newCelFile, BCEL_CHUNK_INTY, BCEL_CHUNK_NAME_SIZE);
	for (int iCell=0; iCell < m_HeaderData.GetCells(); iCell++) {
    int   t_x=IndexToX(iCell);
    int   t_y=IndexToY(iCell);
    float t_mean=GetIntensity(t_x,t_y);
    float t_stdv=GetStdv(t_x,t_y);
    uint16_t t_pixel=GetPixels(t_x,t_y);

    WriteUInt16_N(newCelFile,(uint16_t)RoundNumber(t_mean));
    WriteUInt16_N(newCelFile,(uint16_t)RoundNumber(t_stdv));
    WriteUInt8(newCelFile,(uint8_t)t_pixel);
	}
	WriteUInt32_N(newCelFile, 0); // crc

	// Write the mask data
	WriteUInt32_N(newCelFile,(uint32_t)(m_HeaderData.GetMasked() * STRUCT_SIZE_XY_PAIR));
	WriteFixedString(newCelFile, BCEL_CHUNK_MASK, BCEL_CHUNK_NAME_SIZE);
	for(std::map<int, bool>::iterator pos = m_MaskedCells.begin(); pos != m_MaskedCells.end(); pos++)
	{
		WriteUInt32_N(newCelFile, (uint32_t) IndexToX(pos->first));
		WriteUInt32_N(newCelFile, (uint32_t) IndexToY(pos->first));
	}
	WriteUInt32_N(newCelFile, 0);

	// Write the outlier data
	WriteUInt32_N(newCelFile, (uint32_t) (m_HeaderData.GetOutliers() * STRUCT_SIZE_XY_PAIR));
	WriteFixedString(newCelFile, BCEL_CHUNK_OUTL, BCEL_CHUNK_NAME_SIZE);
	for(std::map<int, bool>::iterator pos = m_Outliers.begin(); pos != m_Outliers.end(); pos++)
	{
		WriteUInt32_N(newCelFile, (uint32_t) IndexToX(pos->first));
		WriteUInt32_N(newCelFile, (uint32_t) IndexToY(pos->first));
	}
	WriteUInt32_N(newCelFile, 0);

	// Write tail
	WriteUInt32_N(newCelFile, 0);
	WriteFixedString(newCelFile, BCEL_CHUNK_TAIL, BCEL_CHUNK_NAME_SIZE);
	WriteUInt32_N(newCelFile, 0);

	// Close the file and check the status.
	newCelFile.close();

	return !newCelFile.fail();
}

///////////////////////////////////////////////////////////////////////////////
///  public  WriteCompactBCel
///  \brief Write CEL file in compact binary format
///
///  @return bool	true if success; false otherwise
///
///  \remark All multi-byte numeric values are stored in little endian byte order
///////////////////////////////////////////////////////////////////////////////
bool CCELFileWriter::WriteCompactBCel()
{
	if (m_FileName.length() == 0)
	{
		SetError("No file name is set for file creation.");
		return false;
	}

	std::ofstream newCelFile;
	newCelFile.open(m_FileName.c_str(), std::ios::out | std::ios::binary);
	if (!newCelFile)
	{
		SetError("Unable to open the file.");
		return false;
	}

	m_HeaderData.SetDatHeader();

	// Write the header
	WriteFixedString(newCelFile, CCEL_HEADER_BYTES, CCEL_HEADER_LEN);
	WriteUInt32_I(newCelFile, m_HeaderData.GetVersion());

	// Write the dimensions of the array
	WriteUInt32_I(newCelFile, m_HeaderData.GetRows());
	WriteUInt32_I(newCelFile, m_HeaderData.GetCols());
	WriteUInt32_I(newCelFile, m_HeaderData.GetCells());

	// Write the other members.
	WriteString_I(newCelFile, m_HeaderData.GetHeader().c_str());
	WriteString_I(newCelFile, m_HeaderData.GetAlg().c_str());
	WriteString_I(newCelFile, m_HeaderData.GetAlgorithmParameters().c_str());
	WriteUInt32_I(newCelFile, m_HeaderData.GetMargin());
	WriteUInt32_I(newCelFile, m_HeaderData.GetMasked());
	WriteUInt32_I(newCelFile, 0);

	// Write the Mean data
	for (int iCell=0; iCell < m_HeaderData.GetCells(); iCell++)
	{
		int   t_x=IndexToX(iCell);
		int   t_y=IndexToY(iCell);
		float t_mean=GetIntensity(t_x,t_y);

		WriteUInt16_I(newCelFile,(uint16_t)RoundNumber(t_mean));
	}

	// Write the mask data
	for(std::map<int, bool>::iterator pos = m_MaskedCells.begin(); pos != m_MaskedCells.end(); pos++)
	{
		WriteUInt16_I(newCelFile, (uint16_t) IndexToX(pos->first));
		WriteUInt16_I(newCelFile, (uint16_t) IndexToY(pos->first));
	}

	// Close the file and check the status.
	newCelFile.close();

	return !newCelFile.fail();
}

//////////////////////////////////////////////////////////////////////
