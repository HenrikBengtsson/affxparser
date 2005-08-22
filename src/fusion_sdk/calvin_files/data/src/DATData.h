/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
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
#ifndef _DATData_HEADER_
#define _DATData_HEADER_

#include <string>
#include "FileHeader.h"
#include "GenericData.h"
#include "Coords.h"
#include "AffymetrixBaseTypes.h"

#ifdef WIN32
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

#define DAT_DATAGROUP std::wstring(L"Default Cube")
#define DAT_PIXEL std::wstring(L"Pixel")
#define DAT_STATS std::wstring(L"Stats")
#define DAT_GLOBAL_GRID std::wstring(L"GlobalGrid")
#define DAT_SUBGRID std::wstring(L"Subgrid")

class DATData
{
public:

	DATData();
	DATData(const std::string &filename);
	~DATData();

private:

	GenericData genericData;
	bool setPixelMetaData;
	bool setStatsMetaData;

	/*! keep rows from being read from the header all the time */
	int32_t cachedRows;
	/*! keep cols from being read from the header all the time */
	int32_t cachedCols;
	/*! Pixel intensity DataSet */
	DataSet* dpPixel;
	/*! Stats DataSet */
	DataSet* dpStats;
	/*! Global grid */
	affymetrix_calvin_utilities::FRegion globalGrid;
	/*! Subgrids */
	affymetrix_calvin_utilities::FRegionVector subgrids;


public:

	/*!  */
	void Clear();
	/*!  */
	void SetFilename(const std::string &p);
	/*!  */
	std::string GetFilename();
	/*!  */
	void SetPixelCount(int32_t ln);
	/*!  */
	void SetStatsCount(int32_t ln);
	/*!  */
	FileHeader* GetFileHeader() { return &genericData.Header(); }
	/*!  */
	GenericData& GetGenericData() { return genericData; }	// should be a friend method only
	/*!  */
	std::wstring GetArrayType();
	/*!  */
	void SetArrayType(const std::wstring& value);
	/*!  */
	float GetPixelSize();
	/*!  */
	void SetPixelSize(float value);
	/*!  */
	std::wstring GetScannerType();
	/*!  */
	void SetScannerType(const std::wstring& value);
	/*!  */
	std::wstring GetScannerID();
	/*!  */
	void SetScannerID(const std::wstring& value);
	/*!  */
	affymetrix_calvin_utilities::DateTime GetScanDate();
	/*!  */
	void SetScanDate(affymetrix_calvin_utilities::DateTime value);
	/*!  */
	int32_t GetRows();
	/*!  */
	void SetRows(int32_t value);
	/*!  */
	int32_t GetCols();
	/*!  */
	void SetCols(int32_t value);

	/*! Fills the array with pixel values
	 *	@param pixelBuffer Pointer to the buffer to receive the pixel intensities
	 *	@param startRow The row from which to start copying the pixel intensities
	 *	@param rowCnt The number of rows of pixel intensities to copy to the buffer
	 *	@return true if the method succeeded
	 */
	bool GetPixels(u_int16_t* pixelBuffer, u_int32_t startRow, u_int32_t rowCnt);

	/*! Gets the range of pixel intensities.
	 *	@param min Reference to a u_int16_t to receive the min pixel intensity
	 *	@param max Reference to a u_int16_t to receive the max pixel intensity
	 *	@return	true if the max and min intensities can be returned.
	 */
	bool GetRange(u_int16_t& min, u_int16_t& max);

	/*! Get the global grid positions.  Use FRect::FRect(FRegion&) cast constructor to convert to FRect.
	 *	@return Global grid position.
	 */
	affymetrix_calvin_utilities::FRegion GetGlobalGrid() const { return globalGrid; }
	/*! Set the global grid positions. Use FRegion::FRegion(FRect&) cast constructor to convert from Rect.
	 *	@param value Global grid position
	 */
	void SetGlobalGrid(affymetrix_calvin_utilities::FRegion& value){ globalGrid = value; }

	/*! Get the number of subgrids.
	 *	@return Number of subgrids
	 */
	int32_t GetSubgridCnt()const { return (int32_t)subgrids.size(); }
	/*! Return a subgrid by index.
	 *	@param index Index of the subgrid to get
	 *	@return A region defining the subgrid.
	 */
	affymetrix_calvin_utilities::FRegion GetSubgrid(int32_t index)const { return subgrids.at(index); }
	/*! Add a subgrid.
	 *	@param value Subgrid region to be added.
	 */
	void AddSubgrid(const affymetrix_calvin_utilities::FRegion& value){ subgrids.push_back(value); }
	/*! Clear subgrids.
	 */
	void ClearSubgrids(){ subgrids.clear(); }

private:

	void InsertDataSetHeader(const DataSetHeader &hdr);

	void UpdateDataSetRowCount(const DataSetHeader &hdr);

	std::wstring GetWStringFromGenericHdr(const std::wstring& name);

	void SetWStringToGenericHdr(const std::wstring& name, const std::wstring value);

	int32_t GetInt32FromGenericHdr(const std::wstring& name);

	void SetInt32ToGenericHdr(const std::wstring& name, int32_t value);

	void PreparePixelPlane();

	void PrepareStatsPlane();

	void PreparePlane(const std::wstring& name, DataSet*& dp);
};

}

#endif // _FileHeader_HEADER_
