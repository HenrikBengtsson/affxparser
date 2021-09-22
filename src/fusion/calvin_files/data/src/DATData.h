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
#ifndef _DATData_HEADER_
#define _DATData_HEADER_

/*! \file DATData.h This file defines a data container class for DAT data.
 */

#include "calvin_files/data/src/FileHeader.h"
#include "calvin_files/data/src/GenericData.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
#include "calvin_files/utils/src/Coords.h"
//
#include <cstring>
#include <string>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

using namespace affymetrix_calvin_utilities;

namespace affymetrix_calvin_io
{

#define DAT_DATAGROUP std::wstring(L"Default Cube")
#define DAT_PIXEL std::wstring(L"Pixel")
#define DAT_STATS std::wstring(L"Stats")
#define DAT_GLOBAL_GRID std::wstring(L"GlobalGrid")
#define DAT_SUBGRID std::wstring(L"SubGrid")

/*! This class is a data container object for DAT data. */
class DATData
{
public:

	/*! Constructor */
	DATData();
	
	/*! Constructor
	 *	@param filename Name of the DAT file
	 */
	DATData(const std::string &filename);
	
	/*! Destructor */
	~DATData();

	/*! The grid and subgrid status flags.  May be combined in an OR fashion. */
	enum GridStatus { GridOK = 0x1, GridError = 0x2, GridManualAdjust = 0x4 };

private:

	/*! generic file object */
	GenericData genericData;
	bool setPixelMetaData;
	bool setStatsMetaData;

	/*! keep rows from being read from the header all the time */
	int32_t cachedRows;
	/*! keep cols from being read from the header all the time */
	int32_t cachedCols;
	/*! Pixel intensity DataSet */
	DataSet* dsPixel;
	/*! Small memory pixel DataSet */
	DataSet* dsSMPixel;
	/*! Stats DataSet */
	DataSet* dsStats;
	/*! Global grid */
	FRegion globalGrid;
	/*! Global grid status flag - indicates the state of the global grid */
	u_int32_t globalGridStatus;
	/*! Subgrids */
	FRegionVector subgrids;
	/*! Subgrids status flag - indicates the state of each subgrid */
	Uint32Vector subgridsStatus;
	/*! Grid Alignment Algorithm Parameters */
	ParameterNameValueTypeVector gridAlignParams;

public:

	/*! Clear the class members */
	void Clear();
	/*! Set the file name.
	 *	@param p Name of the file.
	 */
	void SetFilename(const std::string &p);
	/*! Get the file name.
	 *	@return Returns the file name.
	 */
	std::string GetFilename();
	/*! Set the total number of pixels in the DAT.
	 *	@param ln Number of pixels.
	 */
	void SetPixelCount(int32_t ln);
	/*! Set the total number of statistic rows in the DAT.
	 *	@param ln Number of rows of statistic rows.
	 */
	void SetStatsCount(int32_t ln);
	/*! Get the FileHeader.
	 *	@return Returns a pointer to the FileHeader.
	 */
	FileHeader* GetFileHeader() { return &genericData.Header(); }
	/*! Get the GenericData object.
	 *	@return Returns a reference to the GenericData object.
	 */
	GenericData& GetGenericData() { return genericData; }	// should be a friend method only
	/*! Get the array type.
	 *	@return Returns the name of the array type of the DAT
	 */
	std::wstring GetArrayType();
	/*! Set the array type.
	 *	@param value The name of the DAT array type.
	 */
	void SetArrayType(const std::wstring& value);
	/*! Get the array id.
	 *	@return The array id.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType GetArrayId();
	/*! Set the array id.
	 *	@param value The array id.
	 */
	void SetArrayId(affymetrix_calvin_utilities::AffymetrixGuidType& value);
	/*! Get the array barcode.
	 *	@return The barcode.
	 */
	std::wstring GetArrayBarcode();
	/*! Set the array barcode.
	 *	@param value The barcode.
	 */
	void SetArrayBarcode(std::wstring& value);
	/*! Get the pixel size.
	 *	@return Returns the pixel size in microns.
	 */
	float GetPixelSize();
	/*! Set the pixel size.
	 *	@param value The pixel size in microns.
	 */
	void SetPixelSize(float value);
	/*! Get the type of scanner used to generate the DAT.
	 *	@return Returns the type of scanner used to generate the DAT.
	 */
	std::wstring GetScannerType();
	/*! Set the type of scanner used to generate the DAT.
	 *	@param value The type of scanner used to generate the DAT.
	 */
	void SetScannerType(const std::wstring& value);
	/*! Get the ID of the scanner used to generate the DAT.
	 *	@return Returns the ID of the scanner used to generate the DAT.
	 */
	std::wstring GetScannerID();
	/*! Set the ID of the scanner used to generate the DAT.
	 *	@param value The ID of the scanner used to generate the DAT.
	 */
	void SetScannerID(const std::wstring& value);

	std::wstring GetExposureTime();
	void SetExposureTime(std::wstring& value);

	/*! Get the index of the subimage of the peg.
	 *	@return Return the index of the subimage of the peg.
	 */
	int32_t GetPegSubImageIndex();

	/*! Set the index of the subimage of the peg.
	 *	@return void and set the subimage of the peg to the DAT file.
	 */
	void SetPegSubImageIndex(int32_t value);

	/*! Get the date of the scan.
	 *	@return Returns the date of the scan.
	 */
	affymetrix_calvin_utilities::DateTime GetScanDate();
	/*! Set the date of the scan.
	 *	@param value The date of the scan.
	 */
	void SetScanDate(affymetrix_calvin_utilities::DateTime value);
	/*! Get the number of rows of pixels in the DAT.
	 *	@return Returns the number of pixel rows in the DAT.
	 */
	int32_t GetRows();
	/*! Set the number of rows of pixels in the DAT.
	 *	@param value The number of pixel rows in the DAT.
	 */
	void SetRows(int32_t value);
	/*! Get the number of columns of pixels in the DAT.
	 *	@return Returns the number of pixel columns in the DAT.
	 */
	int32_t GetCols();
	/*! Set the number of colums of pixels in the DAT.
	 *	@param value The number of pixel columns in the DAT.
	 */
	void SetCols(int32_t value);

	/*! Fills the array with pixel values
	 *	@param pixelBuffer Pointer to the buffer to receive pixel intensities
	 *	@param startRow The row from which to start copying pixel intensities
	 *	@param rowCnt The number of rows of pixel intensities to copy to the buffer
	 *	@return true if the method succeeded
	 */
	bool GetPixels(u_int16_t* pixelBuffer, u_int32_t startRow, u_int32_t rowCnt);

	/*! Fills the array with requested pixel values. Uses a small-memory footprint access.  Good for retrieving small parts of the image.
	 *	@param pixelBuffer Pointer to the buffer to receive the pixel intensities
	 *	@param startRow The row from which to start copying pixel intensities
	 *	@param startCol The col from which to start copying pixel intensities
	 *	@param rowCnt The number of rows of pixel intensities to copy to the buffer
	 *	@param colCnt The number of columns of pixel intensities to copy to the buffer
	 *	@return true if the method succeeded
	 */
	bool GetPixels(u_int16_t* pixelBuffer, u_int32_t startRow, u_int32_t startCol, u_int32_t rowCnt, u_int32_t colCnt);

	/*! Gets the range of pixel intensities.
	 *	@param min Reference to a u_int16_t to receive the min pixel intensity
	 *	@param max Reference to a u_int16_t to receive the max pixel intensity
	 *	@return	true if the max and min intensities can be returned.
	 */
	bool GetRange(u_int16_t& min, u_int16_t& max);

	/*! Get the global grid positions.  Use FGridCoords::FGridCoords(FRegion&) cast constructor to convert to FGridCoords.
	 *	@return Global grid position.
	 */
	FRegion GetGlobalGrid() const { return globalGrid; }

	/*! Get the global grid status.
	 *	@return Returns the status of the global grid.  Can be a combination of GridStatus values.
	 */
	u_int32_t GetGlobalGridStatus() const { return globalGridStatus; }

	/*! Has global grid data been set.
	 *	@return True if there is global grid data.
	 */
	bool HasGlobalGridData() const { return globalGrid.pts.size() > 0; }

	/*! Set the global grid positions. Use FRegion::FRegion(FGridCoords&) cast constructor to convert from FGridCoords.
	 *	@param gridStatus A combination of the GridStatus flags indicating the status of the global grid.
	 *	@param grid Global grid position
	 */
	void SetGlobalGrid(u_int32_t gridStatus, FRegion& grid){ globalGridStatus = gridStatus, globalGrid = grid; }

	/*! Get the number of subgrids.
	 *	@return Number of subgrids
	 */
	int32_t GetSubgridCnt()const { return (int32_t)subgrids.size(); }
	/*! Return a subgrid by index.
	 *	@param index Index of the subgrid to get
	 *	@return A region defining the subgrid.
	 */
	FRegion GetSubgrid(int32_t index)const { return subgrids.at(index); }
	/*! Get the status of a subgrid.
	 *	@param index Index of the subgrid status to get.
	 *	@return Returns the status of the subgrid. Can be a combination of GridStatus values.
	 */
	u_int32_t GetSubgridStatus(int32_t index) const { return subgridsStatus.at(index); }

	/*! Add a subgrid.
	 *	@param status A combination of the GridStatus flags indicating the status of the subgrid grid.
	 *	@param subgrid Subgrid region to be added.
	 */
	void AddSubgrid(u_int32_t status, const FRegion& subgrid);
	/*! Clear subgrids.
	 */
	void ClearSubgrids(){ subgrids.clear(); subgridsStatus.clear(); }

	/*! Add a grid alignment algorithm parameter.
	 *	If the parameter already exists, it will be overwritten.
	 *	@param nvt Algorithm parameter to add to the list.
	 */
	void AddGridAlignmentAlgorithmParameter(const ParameterNameValueType& nvt);

	/*! Remove all grid alignment algorithm parameters.
	 */
	void ClearGridAlignmentAlgorithmParameters();

	/*! Return a grid alignment algorithm parameter given a name.
	 *	@param name Name of the grid alignment algorithm parameter to find.
	 *	@param param The found parameter.
	 *	@return True if the parameter was found.
	 */
	bool FindGridAlignmentAlgorithmParameter(const std::wstring& name, ParameterNameValueType& param);

	/*! Return the grid alignment algorithm parameters.
	 *	@param algParams Vector with grid alignment algorithm parameters.
	 */
	void GetGridAlignmentAlgorithmParameters(ParameterNameValueTypeVector& algParams);

	/*! Get a pointer to the parent array GenericDataHeader object.
	 *	@return A pointer to the parent array GenericDataHeader.  Never null.
	 */
	GenericDataHeader* GetParentArrayGenericDataHeader();

private:

	void InsertDataSetHeader(const DataSetHeader &hdr);

	void UpdateDataSetRowCount(const DataSetHeader &hdr);

	std::wstring GetWStringFromGenericHdr(const std::wstring& name);

	void SetWStringToGenericHdr(const std::wstring& name, const std::wstring value, int32_t reserve=-1);

	int32_t GetInt32FromGenericHdr(const std::wstring& name);

	void SetInt32ToGenericHdr(const std::wstring& name, int32_t value);

	void PreparePixelSet();

	void PrepareSmallMemoryPixelSet();

	void PrepareStatsSet();

	void PrepareSet(const std::wstring& name, DataSet*& ds);
};

}

#endif // _FileHeader_HEADER_
