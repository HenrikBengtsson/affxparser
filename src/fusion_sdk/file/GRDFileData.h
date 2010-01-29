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

#ifndef _GRDFileData_HEADER_
#define _GRDFileData_HEADER_

//////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#include <windows.h>
#endif

#include "portability/affy-base-types.h"
//
#include <cstring>
#include <map>
#include <string>
#include <vector>
//

//////////////////////////////////////////////////////////////////////

namespace affxgrd
{

//////////////////////////////////////////////////////////////////////

/*! The name of the parent DAT file parameter. */
// const char SZ_PARENT_DAT_PROP_NAME[] = "Parent DAT File";
  extern const char SZ_PARENT_DAT_PROP_NAME[];

/*! The name of the scan date-time parameter. */
  //const char SZ_SCAN_DATE_TIME_PROP_NAME[] = "Scan Date-Time";
  extern const char SZ_SCAN_DATE_TIME_PROP_NAME[];

/*! The name of the scanner id parameter. */
// const char SZ_SCANNER_ID_PROP_NAME[] = "Scanner ID";
  extern const char SZ_SCANNER_ID_PROP_NAME[];

/*! Floating point coordinates */
struct FCOORD {
	float fx; /*! The X coordinate */
	float fy; /*! The Y coordinate */
};

/*! Coordinates to define a grid */
struct FRECT {
	FCOORD ul; /*! Upper left coordinate */
	FCOORD ur; /*! Upper right coordinate */
	FCOORD ll; /*! Lower left coordinate */
	FCOORD lr; /*! Lower right coordinate */
};

/*! Name value parameters. */
typedef std::map<std::string,std::string> StrStrMap;

/*! An STL vector of grid coordinates. */
typedef std::vector<FRECT> FRECTArray;

/*! This class provides storage capabilities for the GRD file header. */
class CGRDFileHeaderData
{
protected:
	/*! The file version number. */
	float m_Version;

	/*! The number of columns of features on the array. */
	uint32_t m_nCols;

	/*! The number of rows of features on the array. */
	uint32_t m_nRows;

	/*! The feature pitch in the X direction. */
	float m_fFeaturePitchX;

	/*! The feature pitch in the Y direction. */
	float m_fFeaturePitchY;

	/*! The feature setback in the X direction. */
	float m_fFeatureSetbackX;

	/*! The feature setback in the Y direction. */
	float m_fFeatureSetbackY;

	/*! A map of parameter name/value pairs. */
	StrStrMap m_props;

	/*! The number of columns of features on the array. */
	FRECTArray m_optSubgridCoords;

	/*! The size of the header in bytes. */
	uint32_t m_nHeaderBytes;

	/*! Friend to the high level file class. This allows the class
	 * to set the protected members thus making this a read-only
	 * class.
	 */
	friend class CGRDFileData;

public:
	/*! Constructor */
	CGRDFileHeaderData();

	/*! Gets the version number of the file.
	 * @return The version number of the file.
	 */
	float GetVersion() const { return m_Version; }

	/*! Sets the version number of the file.
	 *	@param value The version number of the file.
	 */
	void SetVersion(float value) { m_Version = value; }

	/*! Gets the number of columns of features on the array.
	 * @return The number of columns of features on the array.
	 */
	uint32_t GetCols() const { return m_nCols; }

	/*! Sets the number of columns of features on the array.
	 *	@param value The number of columns of features on the array.
	 */
	void SetCols(uint32_t value) { m_nCols = value; }

	/*! Gets the number of rows of features on the array.
	 * @return The number of rows of features on the array.
	 */
	uint32_t GetRows() const { return m_nRows; }

	/*! Sets the number of rows of features on the array.
	 *	@param value The number of rows of features on the array.
	 */
	void SetRows(uint32_t value) { m_nRows = value; }

	/*! Gets the number of features on the array.
	 * @return The number of features on the array.
	 */
	uint32_t GetNumCells() const { return m_nRows*m_nCols; }

	/*! Gets the feature pitch in the X direction.
	 * @return The feature pitch in the X direction.
	 */
	float GetFeaturePitchX() const { return m_fFeaturePitchX; }

	/*! Sets the feature pitch in the X direction.
	 *	@param value The feature pitch in the X direction.
	 */
	void SetFeaturePitchX(float value) { m_fFeaturePitchX = value; }

	/*! Gets the feature pitch in the Y direction.
	 * @return The feature pitch in the Y direction.
	 */
	float GetFeaturePitchY() const { return m_fFeaturePitchY; }

	/*! Sets the feature pitch in the Y direction.
	 *	@param value The feature pitch in the Y direction.
	 */
	void SetFeaturePitchY(float value) { m_fFeaturePitchY = value; }

	/*! Gets the feature setback in the X direction.
	 * @return The feature setback in the X direction.
	 */
	float GetFeatureSetbackX() const { return m_fFeatureSetbackX; }

	/*! Sets the feature setback in the X direction.
	 *	@param value The feature setback in the X direction.
	 */
	void SetFeatureSetbackX(float value) { m_fFeatureSetbackX = value; }

	/*! Gets the feature setback in the Y direction.
	 * @return The feature setback in the Y direction.
	 */
	float GetFeatureSetbackY() const { return m_fFeatureSetbackY; }

	/*! Sets the feature setback in the Y direction.
	 *	@param value The feature setback in the Y direction.
	 */
	void SetFeatureSetbackY(float value) { m_fFeatureSetbackY = value; }

	/*! Gets the number of sub-grids.
	 * @return The number of sub-grids.
	 */
	uint32_t GetNumSubgrids() const { return (uint32_t) m_optSubgridCoords.size(); }

	/*! Gets the optimized coordinates of a sub-grid.
	 * @param nIndex The index to the sub-grid of interest.
	 * @return The sub-grid.
	 */
	FRECT GetOptSubgrid(int nIndex) { return m_optSubgridCoords[nIndex]; }

	/*! Add the optimized coordinates of a sub-grid.
	 *	@param subgrid The optimized subgrid coordinates.
	 *	@return Returns true if the subgrid was successfully added.
	 */
	 bool AddOptSubgrid(FRECT* subgrid) { if (subgrid == NULL) return false; m_optSubgridCoords.push_back(*subgrid); return true;}

	 /*! Remove all optimized subgrid coordinates.
	  */
	 void RemoveOptSubgrids() { m_optSubgridCoords.clear(); }

	/*! Gets the parameter name/value pairs as a map.
	 * @return The parameters.
	 */
	StrStrMap &GetParameters() { return m_props; }
};

/*! This class provide parsing and storage capabilities for the GRD files. */
class CGRDFileData  
{
public:
	/*! Constructor */
	CGRDFileData();

	/*! Destructor */
	~CGRDFileData();

	/*! Sets the file name parameters.
	 * @param str The full path to the GRD file.
	 */
	void SetFileName(const char *str) { m_FileName = str; }

	/*! Gets the file name.
	 * @return The full path of the GRD file.
	 */
	std::string GetFileName() const { return m_FileName; }

	/*! Gets the error message associated with a read error.
	 *  @return The error message.
	 */ 
	std::string GetError() const { return m_strError; }

	/*! Gets the header.
	 * @return The header object.
	 */
	CGRDFileHeaderData &GetHeader() { return m_HeaderData; }

	/*! Gets the center position for the given feature.
	 * @param x The feature X coordinate.
	 * @param y The feature Y coordinate.
	 * @return The center position.
	 */
	FCOORD GetCenter( int x, int y );

	/*! Checks if the file exists.
	 * @return True if exists.
	 */
	bool Exists();

	/*! Reads the contents of the GRD file.
	 * @return True if successful.
	 */
	bool Read();

	/*! Closes the file. The data is not accessible after the file is closed. */
	void Close();

protected:
	/*! Opens the file.
	 * @return True if successful.
	 */
	bool Open();

	/*! Reads the header of the file using memory mapping. 
	 * @param pData The memory mapped data pointer.
	 * @return True if successful.
	 */
	bool ReadHeader(char *pData);

	/*! Reads the the fixed part of the header.
	 * @param pData The memory mapped data pointer.
	 * @return True if successful.
	 */
	bool ReadFixedHeader(char* pData );

	/*! Reads the name/value parameters from the header.
	 * @param pData The memory mapped data pointer.
	 * @return True if successful.
	 */
	bool ReadTagHeader(char *pData);

	/*! Reads the optimum corners from the header.
	 * @param pData The memory mapped data pointer.
	 * @return True if successful.
	 */
	bool ReadOptimumCornerHeader(char* pData);

	/*! A pointer to the memory mapped file. */
	void  *m_lpFileMap;

	/*! A pointer to the memory mapped file. */
	char  *m_lpData;

#ifdef _MSC_VER

	/*! A file handle. */
	HANDLE m_hFileMap;

	/*! Windows handle to the file. */
	HANDLE m_hFile;

#else

	/*! File pointer for memory mapping. */
	FILE *m_fp;

	/*! The size of the file. */
	int m_MapLen;

#endif

	/*! Flag indicating if the file is open. */
	bool m_bFileOpen;

	/*! Flag indicating if the file is memory mapped. */
	bool m_bFileMapped;

	/*! An string to hold error message upon read failures. */ 
	std::string m_strError;

	/*! The file name. */
	std::string m_FileName;

	/*! The file header. */
	CGRDFileHeaderData m_HeaderData;
};

};

//////////////////////////////////////////////////////////////////////

#endif // _GRDFileData_HEADER_

