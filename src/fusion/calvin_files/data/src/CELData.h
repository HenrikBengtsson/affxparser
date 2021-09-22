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

#ifndef _CelFileData_HEADER_
#define _CelFileData_HEADER_

/*! \file CELData.h This file provides methods to access CEL data.
 */

#include "calvin_files/data/src/GenericData.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
#include "calvin_files/utils/src/Coords.h"
//
#include <cstring>
#include <set>
#include <string>
//


#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

/*! Data Group name */
#define CelDataGroupName std::wstring(L"Default Group")

/*! Name of the intensity data set */
#define CelIntensityLabel std::wstring(L"Intensity")

/*! Name of the standard deviation data set */
#define CelStdDevLabel std::wstring(L"StdDev")

/*! Name of the pixel data set */
#define CelPixelLabel std::wstring(L"Pixel")

/*! Name of the outlier data set */
#define CelOutlierLabel std::wstring(L"Outlier")

/*! Name of the mask data set */
#define CelMaskLabel std::wstring(L"Mask")

/*! Cel file version */
#define CurrentCelFileVersion u_int8_t(1)

/*! This is the container class for CEL data. */
class CelFileData
{
public:
	
	/*! Default Constructor */
	CelFileData();

	/*! Constructor 
	 *	@param filename Name of the cel file.
	 */
	CelFileData(const std::string &filename);

	/*! Destructor */
	~CelFileData();

private:

	/*! Generic layer object */
	GenericData genericData;
	bool setIntensityMetaData;
	bool setStdDevMetaData;
	bool setPixelMetaData;
	bool setOutlierMetaData;
	bool setMaskMetaData;
	int32_t intensityColumnType;

	/*! current wavelength which is the datagroup in this file */
	std::wstring activeChannel;

	// DataSet cache - initialized on first use and Delete in destructor
	/*! Intensity DataSet */
	DataSet* dpInten;
	/*! Stdev DataSet */
	DataSet* dpStdev;
	/*! NumPixels DataSet */
	DataSet* dpPixels;
	/*! Indicates whether an attempt to read the outlier data set has been made. */
	bool outlierPlaneRead;
	/*! A set of outlier cell coordinates. */
	std::set<XYCoord> outliers;
	/*! Indicates whether an attempt to read the mask data set has been made. */
	bool maskPlaneRead;
	/*! A set of masked cell coordinates. */
	std::set<XYCoord> masked;
	/*! keep rows from being read from the header all the time */
	int32_t cachedRows;
	/*! keep cols from being read from the header all the time */
	int32_t cachedCols;

public:

	/*! Clear the object members
	 */
	void Clear();

	/*! Check if the file exists
	 */
	bool Exists();

	/*! Sets the active channel for a multi-channel CEL file.  Default is single channel. */
	void SetActiveChannel(const std::wstring &channel);

	/*! Set the file name
	 *	@param p file name
	 */
	void SetFilename(const std::string &p);

	/*! Get the file name
	 *	@return file name
	 */
	std::string GetFilename() const;

	/*!  */
	void SetIntensityCount(int32_t ln);
	/*!  */
	void SetStdDevCount(int32_t ln);
	/*!  */
	void SetPixelCount(int32_t ln);
	/*!  */
	void SetOutlierCount(int32_t ln);
	/*!  */
	void SetMaskCount(int32_t ln);
	/*!  */
	FileHeader* GetFileHeader()  { return &genericData.Header(); }

	/*! Get the version of the CEL file
	 *	@return CEL file version
	 */
	u_int8_t GetVersion();

	/*! Set the array type
	 *	@param value array type name
	 */
	void SetArrayType(const std::wstring& value);

	/*! Get the array type
	 *	@return array type name
	 */
	std::wstring GetArrayType();

	/*! Set the master file
	 *	@param value master file name
	 */
	void SetMasterFileName(const std::wstring& value);

	/*! Get the master file
	 *	@return master file name
	 */
	std::wstring GetMasterFileName();
    
	/*! Set the library package
	 *	@param value library package name
	 */
	void SetLibraryPackageName(const std::wstring& value);

	/*! Get the library package
	 *	@return library package name
	 */
	std::wstring GetLibraryPackageName();

	/*! Set name of the algorithm used to generate the results.
	 *	@param value algorithm name
	 */
	void SetAlgorithmName(const std::wstring& value);

	/*! Get the name of the algorithm used to generate the results.
	 *	@return Algorithm name
	 */
	std::wstring GetAlgorithmName();
	/*! Set version of the algorithm used to generate the results.
	 *	@param value algorithm name
	 */
	void SetAlgorithmVersion(const std::wstring& value);
	/*
	 * Get the version of the algorithm used to generate the results.
	 */
	std::wstring GetAlgorithmVersion();

	/*! Set the number of rows of cells on the array.
	 *	@param value Number of rows of cells.
	 */
	void SetRows(int32_t value);

	/*! Get the number of rows of cells on the array.
	 *	@return Number of rows of cells.
	 */
	int32_t GetRows();

	/*! Set the number of columns of cells on the array.
	 *	@param value Number of columns of cells.
	 */
	void SetCols(int32_t value);

	/*! Get the number of columns of cells on the array.
	 *	@return Number of columns of cells.
	 */
	int32_t GetCols();

	/*! Return the number of cells on the array.
	 *	This is the number of the intensity data elements and is == GetRows() * GetCols()
	 *	Stdev, NumPixels, Outlier and Masked data is optional, but if present they will have GetNumCells elements.
	 *	@return Number of cells in the array.
	 */
	int32_t GetNumCells();

	/*! Return the algorithm parameters.  The algorithm parameter prefix is removed from the name.
	 *	@param algParams Vector with algorithm parameters.
	 */
	void GetAlgorithmParameters(ParameterNameValueTypeVector& algParams);

	/*! Return an algorithm parameter given a name.
	 *	@param name Name of the parameter to find.
	 *	@param param The found parameter.
	 *	@return True if the parameter was found.
	 */
	bool FindAlgorithmParameter(const std::wstring& name, ParameterNameValueType& param);

	/*! Add an algorithm parameter.  The algorithm parameter prefix will be added by the method.
	 *	@param nvt Algorithm parameter to add to the list.
	 */
	void AddAlgorithmParameter(ParameterNameValueType& nvt);

	/*! Get the cell intensity, standard deviation, number of pixels, outlier flag and mask flag.
	 *	@param cellIdx Index of the cell.
	 *	@param intensity The intensity of the cell.
	 *	@param stdev The standard deviation of the cell.
	 *	@param numPixels The number of pixels in the cell.
	 *	@param outlier The outlier flag of the cell.
	 *	@param masked The masked flag of the cell.
	 */
	void GetData(int32_t cellIdx, float& intensity, float& stdev, int16_t& numPixels, bool& outlier, bool& masked);

	/*! Indicates whether there are standard deviation values.
	 *	@return True if there are standard deviation values.
	 */
	bool HasStdev();

	/*! Indicates whether there are number of pixel values.
	 *	@return True if there are number of pixels values.
	 */
	bool HasNumPixels();

	/*! Get the intensity for a cell index.
	 *	@param cell index of the intensity to retrieve.
	 *	@return float of a particular intensity value.
	 */
	float GetIntensity(int index);

	/*! Get the intensities for a range of cell indexes.
	 *	@param cellIdxStart Cell index of the first intensity to retrieve.
	 *	@param count Number of intensities to retrieve.
	 *	@param values Retrieved cell intensities.
	 *	@return True if all intensities were retrieved.
	 */
	bool GetIntensities(int32_t cellIdxStart, int32_t count, FloatVector& values);

	/*! Get the standard deviations for a range of cell indexes.
	 *	@param cellIdxStart Cell index of the first standard deviation to retrieve.
	 *	@param count Number of cell standard deviations to retrieve.
	 *	@param values Retrieved cell standard deviations.
	 *	@return True if all standard deviations were retrieved.
	 */
	bool GetStdev(int32_t cellIdxStart, int32_t count, FloatVector& values);

	/*! Get the number of pixels for a range of cell indexes.
	 *	@param cellIdxStart Cell index of the first pixel count to retrieve.
	 *	@param count Number of cell pixels to retrieve.
	 *	@param values Retrieved cell pixel counts
	 *	@return True if all pixel count (number of pixels) were retrieved.
	 */
	bool GetNumPixels(int32_t cellIdxStart, int32_t count, Int16Vector& values);

	/*! Get the outlier flags for a range of cell indexes.
	 *	@param cellIdxStart Cell index of the first outlier flag to retrieve.
	 *	@param count Number of cell outlier flags to retrieve.
	 *	@param values Retrieved cell outlier flags
	 *	@return True if all outlier flags were retrieved.
	 */
	bool GetOutliers(int32_t cellIdxStart, int32_t count, BoolVector& values);

	/*! Get the mask flags for a range of cell indexes.
	 *	@param cellIdxStart Cell index of the first mask flag to retrieve.
	 *	@param count Number of cell mask flags to retrieve.
	 *	@param values Retrieved cell mask flags
	 *	@return True if all mask flags were retrieved.
	 */
	bool GetMasked(int32_t cellIdxStart, int32_t count, BoolVector& values);

	/*! Get the coordinates of all outlier cells (i.e. outlier flag is true).
	 *	@param coords Cell coordinates.
	 */
	void GetOutlierCoords(XYCoordVector& coords);

	/*! Get the coordinates of all masked cells (i.e. mask flag is true).
	 *	@param coords Cell coordinates.
	 */
	void GetMaskedCoords(XYCoordVector& coords);

	/*! Return a reference to the generic layer object
	 *	@return Generic layer object
	 */
	GenericData& GetGenericData() { return genericData; }

	/*! Return a wstring for DatHeader
	 *	@return DatHeader
	 */
	std::wstring GetDatHeader();

	/*! Determine the x-y coordinate given a cell index.
	 *	@param cellIdx Cell index
	 *	@param x Cell x-coordinate
	 *	@param y Cell y-coordinate
	 */
	void ComputeXY(int32_t cellIdx, int16_t& x, int16_t& y);

	/*! Returns the list of parameters associated with a data set.
	 * @param setName The data set name
	 * @return The list of parameters
	 */
	ParameterNameValueTypeList GetDataSetParameters(const std::wstring &setName);

	/*! Is this a multi-color CEL file?
	 *  @return True if it is multi-color
	 */
	bool IsMultiColor();

	/*! Returns a list of the channel (ie data group) names
	 *	@return list of channel names
	 */
	WStringVector GetChannels();

	protected:

	/* Close the data set pointers. Used when switching channels. */
	void CloseDataSets();

	/* Clear the mask and outlier lists and reset to unread state.  Used when switching channels. */
	void ResetMaskAndOutliers();

private:
	/*!  */
	void SetFileCreationTime(const std::wstring &f);
	/*!  */
	std::wstring GetFileCreationTime() const;

	void InsertDataSetHeader(const DataSetHeader &hdr);

	void UpdateDataSetRowCount(const DataSetHeader &hdr);

	/*! Set the version of the CEL file. Used by the reader.
	 *	@param value CEL file version
	 */
	void SetVersion(u_int8_t value);
	/*! Prepare to read intensity data */
	void PrepareIntensityPlane();
	/*! Prepare to read the standard deviation data */
	void PrepareStdevPlane();
	/*! Prepare to read the number of pixel data */
	void PrepareNumPixelPlane();
	/*! Prepare to read the outlier data */
	void PrepareOutlierPlane();
	/*! Prepare to read the mask data */
	void PrepareMaskedPlane();
	/*! Prepare to read all data */
	void PrepareAllPlanes();
	/*! Read an int32_t value from the GenericDataHeader parameter list.
	 *	@param name Name of the parameter to read
	 *	@return int32_t value of the named parameter
	 */
	int32_t GetInt32FromGenericHdrParameterList(const std::wstring& name);
	/*! Set a int32_t value into the GenericDataHeader parameter list.
	 *	@param name of the parameter to set
	 *	@param value Int32_t value to set
	*/
	void SetInt32ToGenericHdrParameterList(const std::wstring& name, int32_t value);
	/*! Read an wstring value from the GenericDataHeader parameter list.
	 *	@param name Name of the parameter to read
	 *	@return wstring value of the named parameter
	 */
	std::wstring GetWStringFromGenericHdr(const std::wstring& name);
	/*! Set a wstring value into the GenericDataHeader parameter list.
	 *	@param name of the parameter to set
	 *	@param value wstring value to set
	 *	@param reserve The total number of characters to reserve for the value. -1 indicates not to reserve any extra space.
	*/
	void SetWStringToGenericHdr(const std::wstring& name, const std::wstring value, int32_t reserve=-1);
	/*! Check if the cell is an outlier (outlier flag is true)
	 *	@param cellIdx Cell index
	 *	@return True if the cell outlier flag is true
	 */
	bool IsOutlier(int32_t cellIdx);
	/*! Check if the cell is masked (mask flag is true)
	 *	@param cellIdx cell index
	 *	@return True if the cell mask flag is true
	 */
	bool IsMasked(int32_t cellIdx);
	/*! Find a DataSetHeader by name.
	 *	@param name DataSetHeader name
	 *	@return Pointer to a DataSetHeader with name parameter, otherwise 0
	 */
	DataSetHeader* FindDataSetHeader(const std::wstring& name);

	friend class CelFileReader;
	friend class CelFileWriter;
	friend class MultiChannelCelFileWriter;
};

}

#endif // _CelFileData_HEADER_
