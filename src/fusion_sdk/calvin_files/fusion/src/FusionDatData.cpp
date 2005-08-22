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

#include "FusionDatData.h"
#include "DevelopmentException.h"

using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_exceptions;
using namespace affymetrix_calvin_parameter;


/*
 * Sets the pixel size in microns
 */
void ScanParameters::SetPixelSize(float value)
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the pixel size in microns
 */
float ScanParameters::GetPixelSize() const
{
	NotImplementedException e;
	throw e;
}

/*
 * Sets the array orientation
 */
void ScanParameters::SetOrientation(ArrayLabelPosition value)
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the array orientation
 */
ArrayLabelPosition ScanParameters::GetOrientation() const
{
	NotImplementedException e;
	throw e;
}

/*
 * Sets the scanner filter
 */
void ScanParameters::SetFilter(int32_t value)
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the scanner filter
 */
int32_t ScanParameters::GetFilter() const
{
	NotImplementedException e;
	throw e;
}

/*
 * Sets the scan date
 */
void ScanParameters::SetScanDate(affymetrix_calvin_utilities::DateTime& value)
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the scan date
 */
affymetrix_calvin_utilities::DateTime ScanParameters::GetScanDate () const
{
	NotImplementedException e;
	throw e;
}

/*
 * Sets the scanner ID
 */
void ScanParameters::SetScannerID (const std::wstring& scannerID)
{
	NotImplementedException e;
	throw e;
}

/*
 * Sets the scanner ID
 */
std::wstring ScanParameters::GetScannerID () const
{
	NotImplementedException e;
	throw e;
}

/*
 * Sets the scanner type
 */
void ScanParameters::SetScannerType(const std::wstring& scannerType)
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the scanner type
 */
std::wstring ScanParameters::GetScannerType () const
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the global grid coordinates
 */
const FRect& GridParameters::GetGlobalGrid()
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the number of subgrids
 */
u_int32_t GridParameters::GetSubgridCnt () const
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the subgrid information based on index
 */
bool GridParameters::GetSubgrid(/*[in]*/ int32_t nIndex, Subgrid& subgrid)
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the min pixel intensity
 */
u_int16_t ImageParameters::GetMinData() const
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the max pixel intensity
 */
u_int16_t ImageParameters::GetMaxData() const
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the average pixel intensity
 */
float ImageParameters::GetAvgData() const
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the standard deviation of the pixel intensities
 */
float ImageParameters::GetStdev() const
{
	NotImplementedException e;
	throw e;
}

/*
 * Sets the number of rows of pixels in the image
 */
void ImageParameters::SetRows(int32_t value)
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the number of rows of pixels in the image
 */
int32_t ImageParameters::GetRows() const
{
	NotImplementedException e;
	throw e;
}

/*
 * Sets the number of columns of pixels in the image
 */
void ImageParameters::SetCols(int32_t value)
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the number of columns of pixels in the image
 */
int32_t ImageParameters::GetCols() const
{
	NotImplementedException e;
	throw e;
}

/*
 * True if the image has been flipped about the y-axis
 */
void ImageParameters::SetFlipFlag(bool value)
{
	NotImplementedException e;
	throw e;
}

/*
 * True if the image has been flipped about the y-axis
 */
bool ImageParameters::GetFlipFlag() const
{
	NotImplementedException e;
	throw e;
}

/*
 * Deletes the FusionDatData object 
 */
void FusionDatData::Delete()
{
	NotImplementedException e;
	throw e;
}

/*
 * Sets the DAT file version
 */
void FusionDatData::SetVersion(int32_t value)
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the DAT file version
 */
int32_t FusionDatData::GetVersion() const
{
	NotImplementedException e;
	throw e;
}

/*
 * Sets the name of the DAT to be opened read or written
 */
void FusionDatData::SetFileName (const std::string& name)
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the name of the DAT file
 */
std::string FusionDatData::GetFileName () const
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the file id of the DAT file
 */
affymetrix_calvin_utilities::AffymetrixGuidType FusionDatData::GetFileId() const
{
	NotImplementedException e;
	throw e;
}

/*
 * Sets the id of the array associated with this DAT
 */
void FusionDatData::SetArrayId(const affymetrix_calvin_utilities::AffymetrixGuidType arrayId)
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the id of the array associated with this DAT
 */
affymetrix_calvin_utilities::AffymetrixGuidType FusionDatData::GetArrayId() const
{
	NotImplementedException e;
	throw e;
}

/*
 * Sets the array type
 */
void FusionDatData::SetArrayType(const std::wstring& arrayType)
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets the array type
 */
std::wstring FusionDatData::GetArrayType() const
{
	NotImplementedException e;
	throw e;
}

/*
 * Access the scan parameters
 */
ScanParameters& FusionDatData::ScanParams()
{
	NotImplementedException e;
	throw e;
}

/*
 * Access the grid parameters
 */
GridParameters& FusionDatData::GridParams()
{
	NotImplementedException e;
	throw e;
}

/*
 * Access the image paramters
 */
ImageParameters& FusionDatData::ImageParams()
{
	NotImplementedException e;
	throw e;
}

/*
 * Allocates a array of hold pixel values
 */
u_int16_t* FusionDatData::AllocPixelBuffer(u_int32_t rowCount)
{
	NotImplementedException e;
	throw e;
}

/*
 * Fills the array with pixel values
 */
bool FusionDatData::GetPixels(u_int16_t* pixelBuffer, u_int32_t startRow, u_int32_t rowCnt)
{
	NotImplementedException e;
	throw e;
}

/*
 * Releases the allocated buffer
 */
void FusionDatData::DeallocPixelBuffer(u_int16_t* pixelBuffer)
{
	NotImplementedException e;
	throw e;
}

/*
 * Adds a parameter to the parameter list
 */
void FusionDatData::AddParameter(const ParameterNameValueType& value)
{
	NotImplementedException e;
	throw e;
}

/*
 * Finds a parameter from the parameter list by name
 */
bool FusionDatData::FindParameter(const std::wstring& name, ParameterNameValueType& found)
{
	NotImplementedException e;
	throw e;
}

/*
 * Gets parameter list iterators
 */

void FusionDatData::GetParameterIts(ParameterNameValueTypeConstIt& begin, ParameterNameValueTypeConstIt& end)
{
	NotImplementedException e;
	throw e;
}

/*
 * Clears (empties) the parameters list
 */
void FusionDatData::ClearParameters()
{
	NotImplementedException e;
	throw e;
}

