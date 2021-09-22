////////////////////////////////////////////////////////////////
//
// Copyright (C) 2004 Affymetrix, Inc.
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

/*! \file DATFileData.h This file provides interfaces to read GCOS DAT files. */

#ifndef _DATFileData_HEADER_
#define _DATFileData_HEADER_

//////////////////////////////////////////////////////////////////////

#include "file/GridCoordinates.h"
//
#include "portability/affy-base-types.h"
//
#include <cstring>
#include <fstream>
#include <string>
//
#ifdef sun
#include <stdint.h>
#endif

//////////////////////////////////////////////////////////////////////

namespace affxdat
{

/*! This class provides storage for the header of a GCOS DAT file. */
class CDATFileHeaderData
{
public:
	/*! Constructor */
	CDATFileHeaderData();

	/*! Destructor */
	~CDATFileHeaderData();

protected:
	/*! The number of pixel columns. */
	int32_t m_nCols;

	/*! The number of pixel rows. */
	int32_t m_nRows;

	/*! The minimum pixel value. */
	uint16_t m_MinValue;

	/*! The maximum pixel value. */
	uint16_t m_MaxValue;

	/*! The grid coordinates. */
	GridCoordinatesType m_Grid;

	/*! The probe array type. */
	std::string m_ArrayType;

	/*! The scanner ID */
	std::string m_ScannerID;

public:
	/*! Gets the number of pixel columns.
	 * @return The number of pixel columns.
	 */
	int32_t GetCols() const { return m_nCols; }

	/*! Sets the number of pixel columns.
	 * @param cols The number of pixel columns.
	 */
	void SetCols(int32_t cols) { m_nCols = cols; }

	/*! Gets the number of pixel rows.
	 * @return The number of pixel rows.
	 */
	int32_t GetRows() const { return m_nRows; }

	/*! Sets the number of pixel rows.
	 * @param rows The number of pixel rows.
	 */
	void SetRows(int32_t rows) { m_nRows = rows; }

	/*! Gets the minimum pixel value.
	 * @return The minimum pixel value.
	 */
	uint16_t GetMinValue() const { return m_MinValue; }

	/*! Sets the minimum pixel value.
	 * @param val The minimum pixel value.
	 */
	void SetMinValue(uint16_t val) { m_MinValue = val; }

	/*! Gets the maximum pixel value.
	 * @return The maximum pixel value.
	 */
	uint16_t GetMaxValue() const { return m_MaxValue; }

	/*! Sets the maximum pixel value.
	 * @param val The maximum pixel value.
	 */
	void SetMaxValue(uint16_t val) { m_MaxValue = val; }

	/*! Returns the total number of pixels.
	 * @return The total number of pixels.
	 */
	int32_t GetNumPixels() const { return m_nCols * m_nRows; }

	/*! Gets the grid coordinates.
	 * @return The grid coordinates.
	 */
	const GridCoordinatesType &GetGridCorners() { return m_Grid; }

	/*! Sets the grid coordinates.
	 * @param grid The grid coordinates.
	 */
	void SetGridCorners(const GridCoordinatesType &grid) { m_Grid = grid; }

	/*! Gets the probe array type.
	 * @return The probe array type.
	 */
	std::string GetArrayType() const { return m_ArrayType; }

	/*! Sets the array type.
	 * @param arrayType The probe array type.
	 */
	void SetArrayType(const char *arrayType) { m_ArrayType = arrayType; }

	/*! Gets the scanner ID.
	 * @return The scanner ID.
	 */
	std::string GetScannerID() const { return m_ScannerID; }

	/*! Sets the array type.
	 * @param scannerID The scanner ID.
	 */
	void SetScannerID(const char *scannerID) { m_ScannerID = scannerID; }
};

//////////////////////////////////////////////////////////////////////

/*! This class provides parsing and access interfaces to a GCOS DAT file. */
class CDATFileData  
{
public:
	/*! Constructor */
	CDATFileData();

	/*! Destructor */
	~CDATFileData();

protected:
	/*! The full path of the DAT file. */
	std::string m_FileName;

	/*! The DAT file header */
	CDATFileHeaderData m_HeaderData;

	/*! The file handle. */
	std::ifstream m_File;

public:
	/*! Sets the full path of the DAT file.
	 * @param str The full path of the DAT file.
	 */
	void SetFileName(const char *str) { m_FileName = str; }

	/*! Gets the full path of the DAT file.
	 * @return The full path of the DAT file.
	 */
	std::string GetFileName() const { return m_FileName; }

	/*! Gets the DAT file header object.
	 * @return The header.
	 */
	CDATFileHeaderData &GetHeader() { return m_HeaderData; }

	/*! Gets row of data.
	 * @param rowIndex The index to the row to extract.
	 * @param pixels An array to store the pixel data.
	 */
	void GetPixels(int rowIndex, uint16_t *pixels);

	/*! Gets row of data.
	 * @param rowIndex The index to the row to extract.
	 * @param rowCount The number of rows to extract.
	 * @param pixels An array to store the pixel data.
	 */
	void GetPixels(int rowIndex, int rowCount, uint16_t *pixels);

	/*! Gets a single pixel value.
	 * @param x The x coordinate.
	 * @param y The y coordinate.
	 * @return The pixel value at the coordinate.
	 */
	uint16_t GetPixel(int x, int y);

	/*! Checks if the file exists.
	 * @return True if exists.
	 */
	bool Exists();

	/*! Reads the file.
	 * @return True if successful.
	 */
	bool Read();

	/*! Closes the file when finished accessing it. */
	void Close();

	/*! Update the grid coordinates in a DAT file.
	 * @param datFile The full path name of the DAT file to update.
	 * @param grid The new grid coordinates.
	 * @return True if successful.
	 */
	static bool UpdateGridCorners(const char *datFile, const GridCoordinatesType &grid);

	/*! Determines if the file is a GCOS DAT file.
	 * @param datFile The full path name of the DAT file to update.
	 * @return True if the file is a GCOS DAT file.
	 */
	static bool IsGCOSDATFile(const char *datFile);
};

//////////////////////////////////////////////////////////////////////

} // namespace

////////////////////////////////////////////////////////////////////

#endif // !defined(_DATFileData_HEADER_)
