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

#ifndef _GCOSDatData_HEADER_
#define _GCOSDatData_HEADER_

/*! \file GCOSDatData.h This file defines the GCOS DAT Data class to that implements the DAT fusion interface.
 */

#include "../FusionDatData.h"
#include <string>

namespace affymetrix_fusion_io
{
	// DATIO7 parameter list parameters
	// - ScanSpeed (int) - Shantanu says to remove
	// - Power (int)- Shantanu says to remove
	// - Temp (int) - Shantanu says to remove
	// - ChipInfo (string)
	// - ScanInfo (string)
	// - AvgOffset (double) - is this a statistic
	// - OffsetStdev (double) - is this a statistic
	// - OffsetSamples (int)
	// - LaserSpotSize (double) - not general, does not apply to the CCD scanner
	// - LaserSportSize (double) - ditto



/*! Defines the GCOS scan parameters */
class GCOSScanParameters : public ScanParameters
{
public:
	GCOSScanParameters();

	/*! Sets the pixel size
	 *	@param value Pixel size in microns
	 *	@exception affymetrix_calvin_exceptions::OutOfRangeParameterException
	 */
	virtual void SetPixelSize(float value);
	/*! Gets the pixel size
	 *	@return pixel size in microns
	 */
	virtual float GetPixelSize() const { return pixelSize; }

	/*! Sets the array orientation
	 *	@param value Position of the text label on the array
	 */
	virtual void SetOrientation(ArrayLabelPosition value) { orientation = value; }
	/*! Gets the array orientation
	 *	@return Position of the text on the array
	 */
	virtual ArrayLabelPosition GetOrientation() const { return orientation; }

	/*! Sets the scanner filter
	 *	@param value Filter wavelength
	 *	@exception affymetrix_calvin_exceptions::OutOfRangeParameterException
	 */
	virtual void SetFilter(int32_t value);
	/*! Gets the scanner filter
	 *	@return Filter wavelength
	 */
	virtual int32_t GetFilter() const { return filter; }

	/*! Sets the scan date
	 *	@param value The current date and time. Use DateTime::GetCurrentDateTime()
	 */
	virtual void SetScanDate(affymetrix_calvin_utilities::DateTime& value) { date = value; }
	/*! Gets the scan date
	 *	@return DateTime of the scan
	 */
	virtual affymetrix_calvin_utilities::DateTime GetScanDate () const { return date; }

	/*! Sets the scanner ID
	 *	@param value Scanner ID
	 */
	virtual void SetScannerID (const std::wstring& value) { scannerID = value; }
	/*! Gets the scanner ID
	 *	@return ScannerID
	 */
	virtual std::wstring GetScannerID () const { return scannerID; }

	/*! Sets the scanner type
	 *	@param value Scanner type
	 */
	virtual void SetScannerType(const std::wstring& value) { scannerType = value; }
	/*! Gets the scanner type
	 *	@return Scanner type
	 */
	virtual std::wstring GetScannerType () const { return scannerType; }

private:
	float pixelSize;
	ArrayLabelPosition orientation;
	int32_t filter;
	affymetrix_calvin_utilities::DateTime date;
	std::wstring scannerID;
	std::wstring scannerType;
};

/*! Defines the GCOS grid parameters */
class GCOSGridParameters : public GridParameters
{
public:
	GCOSGridParameters();

	/*! Gets the global grid coordinates
	 *	@return A reference to the global grid coordinates
	 */
	virtual const affymetrix_calvin_utilities::FRect& GetGlobalGrid() { return globalGrid; }
	/*! Sets the global grid coordinates - not part of the fusion interface */
	void SetGlobalGrid(const affymetrix_calvin_utilities::FRect& value) { globalGrid = value; }

	/*! Gets the number of subgrids
	 *	@return Number of subgrids
	 */
	virtual u_int32_t GetSubgridCnt () const { return (u_int32_t)subgrids.size(); }

	/*! Gets the subgrid information based on index
	 *	@param index Index of the subgrid
	 *	@param subgrid Reference to a Subgrid object that will receive the subgrid information
	 *	@return true if successful
	 */
	virtual bool GetSubgrid(int32_t nIndex, affymetrix_calvin_utilities::Subgrid& subgrid);
	/*! Adds a subgrid - not part of the fusion interface */
	void AddSubgrid(const affymetrix_calvin_utilities::Subgrid& subgrid) { subgrids.push_back(subgrid); }

private:
	affymetrix_calvin_utilities::FRect globalGrid;
	affymetrix_calvin_utilities::SubgridVector subgrids;
};

/*! Defines the GCOS image parameters */
class GCOSImageParameters : public ImageParameters
{
public:
	GCOSImageParameters();

	// Set during the write operation - written to statistics data plane.
	/*! Gets the min pixel intensity
	 *	@return Min pixel intensity
	 */
	virtual u_int16_t GetMinData() const { return minData; }
	/*! Sets the min pixel intensity - not part of the fusion interface */
	void SetMinData(u_int16_t value) { minData = value; }

	/*! Gets the max pixel intensity
	 *	@return Max pixel intensity
	 */
	virtual u_int16_t GetMaxData() const { return maxData; }
	/*! Sets the max pixel intensity - not part fo the fusion interface */
	void SetMaxData(u_int16_t value) { maxData = value; }

	/*! Gets the average pixel intensity
	 *	@return Average pixel intensity
	 */
	virtual float GetAvgData() const { return avgData; }
	/*! Sets the average pixel intensity - not part of the fusion interface */
	void SetAvgData(float value) { avgData = value; }

	/*! Gets the standard deviation of the pixel intensities
	 *	@return Standard deviation of the pixel intensities
	 */
	virtual float GetStdev() const { return stdev; }
	/*! Sets the standard deviation of the pixel intensities - not part of the fusion interface */
	void SetStdev(float value) { stdev = value; }

	/*! Sets the number of rows of pixels in the image
	 *	@param value Number of rows of pixels in the image
	 *	@exception affymetrix_calvin_exceptions::OutOfRangeParameterException
	 */
	virtual void SetRows(int32_t value);
	/*! Gets the number of rows of pixels in the image
	 * @return Number of rows of pixels in the image
	 */
	virtual int32_t GetRows() const { return rows; }

	/*! Sets the number of columns of pixels in the image
	 *	@param value Number of columns of pixels in the image
	*/
	virtual void SetCols(int32_t value);
	/*! Gets the number of columns of pixels in the image
	 *	@return Number of columns of pixels in the image
	 *	@exception affymetrix_calvin_exceptions::OutOfRangeParameterException
	 */
	virtual int32_t GetCols() const { return cols; }

	/*! Sets the flip flag
	 *	@param value true indicates that the image has been flipped about the y-axis.
	 */
	virtual void SetFlipFlag(bool value) { flipped = value; }
	/*! Gets the flip flag
	 *	@return True indicates that the image has been flipped about the y-axis.
	 */
	virtual bool GetFlipFlag() const { return flipped; }

private:
	u_int16_t minData;
	u_int16_t maxData;
	float avgData;
	float stdev;
	int32_t rows;
	int32_t cols;
	bool flipped;
};

/*! This class defines the fusion DAT data object model interface */
class GCOSDatData : public FusionDatData
{
	/*! Constructor */
	GCOSDatData();
	/*! Destructor */
	~GCOSDatData() {}

public:
	/*! Create a GCOSDatData object
	 *	@return A GCOSDatData pointer
	 */
	static GCOSDatData* Create() { return new GCOSDatData; }

	/*! Deletes the FusionDatData object.  Call this when done with the object.
	 */
	virtual void Delete() { delete this; }

	/*! Sets the DAT file version
	 *	@param value The DAT file version
	 */
	virtual void SetVersion(int32_t value) { version = value; }
	/*! Gets the DAT file version
	 *	@return The DAT file version
	 */
	virtual int32_t GetVersion() const { return version; }

	/*! Sets the name of the DAT to be opened read or written
	 *	@param value DAT file name
	 */
	virtual void SetFileName(const std::string& value) { fileName = value; }
	/*! Gets the name of the DAT file
	 *	@return DAT file name
	 */
	virtual std::string GetFileName() const { return fileName; }

	/*! Gets the file identifier of the DAT file
	 *	@return Unique file identifier
	 */
	virtual affymetrix_calvin_utilities::AffymetrixGuidType GetFileId() const { return ""; }

	/*! Sets the identifier of the array associated with this DAT
	 *	@param value The unique identifier of the array file
	 */
	virtual void SetArrayId(const affymetrix_calvin_utilities::AffymetrixGuidType value) { arrayId = value; }
	/*! Gets the id of the array associated with this DAT
	 *	@return The unique identifier of the array file
	 */
	virtual affymetrix_calvin_utilities::AffymetrixGuidType GetArrayId() const { return arrayId; }

	/*! Sets the probe array type
	 *	@param value The array type
	 */
	virtual void SetArrayType (const std::wstring& value) { arrayType = value; }
	/*! Gets the probe array type
	 *	@return The array type
	 */
	virtual std::wstring GetArrayType () const { return arrayType; }

	/*! Access the scan parameters
	 *	@return A reference to the scan parameters
	 */
	virtual ScanParameters& ScanParams() { return scanParams; }
	/*! Access the grid parameters
	 *	@return A reference to the grid parameters
	 */
	virtual GridParameters& GridParams() { return gridParams; }
	/*! Access the image paramters
	 *	@return A reference to the image parameters
	 */
	virtual ImageParameters& ImageParams() { return imageParams; }

	/*! Allocates a buffer to receive pixel data
	 *	@param rowCount The number of rows of pixel for which to allocate memory
	 *	@return A pointer to the allocated buffer
	 */
	virtual u_int16_t* AllocPixelBuffer(u_int32_t rowCount);
	/*! Fills the array with pixel values
	 *	@param pixelBuffer Pointer to the buffer to receive the pixel intensities
	 *	@param startRow The row from which to start copying the pixel intensities
	 *	@param rowCnt The number of rows of pixel intensities to copy to the buffer
	 *	@return true if the method succeeded
	 */
	virtual bool GetPixels(u_int16_t* pixelBuffer, u_int32_t startRow, u_int32_t rowCnt);
	/*! Deallocates the allocated buffer
	 *	@param pixelBuffer Pointer to the pixel buffer to deallocate
	 */
	virtual void DeallocPixelBuffer(u_int16_t* pixelBuffer);

	// Additional Parameters - parameter list
	/*! Adds a parameter to the parameter list.  If the parameter is already in the list then the value is updated.
	 *	@param value A name-value-type to add to the parameter list
	 */
	virtual void AddParameter(const affymetrix_calvin_parameter::ParameterNameValueType& value);
	/*! Finds a parameter from the parameter list by name
	 *	@param name The name of the parameter to find in the parameter list
	 *	@param found A reference to an object that receives a copy of the found parameter
	 *	@return true if the parameter is found.
	 */
	virtual bool FindParameter(const std::wstring& name, affymetrix_calvin_parameter::ParameterNameValueType& found);
	/*! Gets parameter list iterators.  begin == end if the list is empty
	 *	@param begin A reference to an object that receives the begin iterator
	 *	@param end A reference to an object that receive the end iterator
	 */
	virtual void GetParameterIts(affymetrix_calvin_parameter::ParameterNameValueTypeConstIt& begin, affymetrix_calvin_parameter::ParameterNameValueTypeConstIt& end);
	/*! Clears (empties) the parameters list
	 */
	virtual void ClearParameters();

	// DATIO7 parameter list parameters
	// - ScanSpeed (int) - Removed - not used
	// - Power (int) - Removed - not used
	// - Temp (int) - Removed - not used
	// - ChipInfo (string)
	// - ScanInfo (string)
	// - AvgOffset (double) - Removed - not used
	// - OffsetStdev (double - Removed - not used
	// - OffsetSamples (int) - Removed - not used
	// - LaserSpotSize (double) - not general, does not apply to the CCD scanner
	// - LaserSportSize (double) - ditto

	// Support methods
protected:
	/*! Return an iterator referencing the found parameter.
	 *	@param nvt A ParameterNameValueType with the name set to the parameter name to be found
	 *	@return Iterator referencing the found parameter, if the parameter could not be found == param.end()
	 */
	affymetrix_calvin_parameter::ParameterNameValueTypeIt FindParameter(const affymetrix_calvin_parameter::ParameterNameValueType& nvt);
	/*! Check the type and range of expected parameters.
	 *	@param value ParameterNameValueType to check.
	 *	@exception - The parameter is not the expected type.
	 *	@exception affymetrix_calvin_exceptions::UnexpectedParameterException- The parameter value is out-of-range.
	 */
	void CheckParameterTypeAndRange(const affymetrix_calvin_parameter::ParameterNameValueType& value);

private:
	int32_t version;
	std::string fileName;
	affymetrix_calvin_utilities::AffymetrixGuidType arrayId;
	std::wstring arrayType;
	GCOSScanParameters scanParams;
	GCOSGridParameters gridParams;
	GCOSImageParameters imageParams;
	affymetrix_calvin_parameter::ParameterNameValueTypeVector params;
};

}

#endif // _AffymetrixFusionDatData_HEADER_