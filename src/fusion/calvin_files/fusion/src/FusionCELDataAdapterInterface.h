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


#ifndef _AffymetrixFusionCELDataInterface_HEADER_
#define _AffymetrixFusionCELDataInterface_HEADER_

#include "calvin_files/data/src/GenericData.h"
#include "calvin_files/fusion/src/FusionCoords.h"
#include "calvin_files/fusion/src/FusionTagValuePairType.h"
#include "calvin_files/parameter/src/ParameterNameValueType.h"
#include "calvin_files/utils/src/AffymetrixGuid.h"
//
#include <cstring>
#include <list>
#include <string>
//

/*! \file FusionCELDataAdapterInterface.h This file defines the interface between
 *	the FusionCELData class and the Calvin and GCOS adapter classes.
 */

namespace affymetrix_fusion_io
{

/*! Fusion Entry Type */
typedef struct _FusionCELFileEntryType
{
	/*! Intensity value */
	float Intensity;
	/*! standard deviation value */
	float Stdv;
	/*! pixels */
	short Pixels;
} FusionCELFileEntryType;

/*! \brief Adapter that wraps both GCOS and Calvin cell file readers and contains the interfaces to the data.*/
class IFusionCELDataAdapter
{
public:

	/*! \brief Virtual destructor. */
	virtual ~IFusionCELDataAdapter() {};

	/*! \brief Can this object read the file 
	 *	\return If the cell file can be read.
	 */
	virtual bool CanReadFile() = 0;
	/*! \brief Set the cell file name. 
	 *	\param value The cell file name to be set
	 */
	virtual void SetFileName(const std::string& value) = 0;
	/*! \brief Get the cell file name. 
	 *	\return The currently set cell file name.
	 */
	virtual std::string GetFileName() const = 0;
	/*! \brief Get the id of the file (only valid for Command Console "calvin" files)
	 * \return The unique file id.
	 */
	virtual affymetrix_calvin_utilities::AffymetrixGuidType GetFileId() = 0;
	/*! \brief Set the error message.
	 *	\param value The error message to be set.
	 */
	virtual void SetError(const wchar_t* value) = 0;
	/*! \brief Get the currently set error message string. 
	 *	\return The error message.
	 */
	virtual std::wstring GetError() = 0;
	/*! \brief Get the header key.
	 *	\param key Name of header value.
	 *  \return The header key.
	 */
	virtual std::wstring GetHeaderKey(const wchar_t* key) = 0;
	/*! \brief Get the version of the cell file.
	 *	\return The cell file version.
	 */
	virtual int GetVersion() = 0;
	/*! \brief Get the number of columns.
	 *	\return The number of columns
	 */
	virtual int GetCols() = 0;
	/*! \brief Get the number of rows.
	 *	\return The number of rows.
	 */
	virtual int GetRows() = 0;
	/*! \brief Get number of cells
	 *	\return The number of cells
	 */
	virtual int GetNumCells() = 0;
	/*! \brief Get the header string.
	 *	\return The header as a string.
	 */
	virtual std::wstring GetHeader() = 0;
	/*! \brief Get the algorithm name.
	 *	\return The algorithm name.
	 */
	virtual std::wstring GetAlg() = 0;
	/*! \brief Get the algorithm version.
	 *	\return The algorithm version.
	 */
	virtual std::wstring GetAlgVer() = 0;

	/*! \brief Get the parameters.
	 *	\return The parameters used for creating the cell file.
	 */
	virtual std::wstring GetParams() = 0;
	/*!	\brief Get a parameter.
	 *	\param tag Parameter name associated with a parameter value.
	 *	\return The parameter value.
	 */
	virtual std::wstring GetAlgorithmParameter(const wchar_t *tag) = 0;
	/*! \brief Gets a parameter.
	 *	\param index The index to the parameter array.
	 *	\return The parammeter value.
	 */
	virtual std::wstring GetAlgorithmParameterTag(int index) = 0;
	/*! \brief Get number of parameters.
	 *	\return The Parameter name.
	 */
	virtual int GetNumberAlgorithmParameters() = 0;
	/*! \brief Get the number of parameters.
	 *	\return The number of parameters.
	*/
	virtual std::wstring GetAlgorithmParameters() = 0;
	/*! \brief Get parameters.
	 *	\param values Collection of name/value type parameter list.
	 */
	virtual void GetParameters(FusionTagValuePairTypeList& values) = 0;
	/*! \brief Get the DatHeader string.
	 *	\return The DatHeader string.
	 */
	virtual std::wstring GetDatHeader() = 0;
	/*! \brief Get chip type.
	 *	\return The chip type of the cell.
	 */
	virtual std::wstring GetChipType() = 0;
	/*! \brief Get the master file
	 *	\return master file name
	 */
	virtual std::wstring GetMasterFileName() = 0;
    /*! \brief Get the library package
	 *	\return library package name
	 */
	virtual std::wstring GetLibraryPackageName() = 0;
	/*! \brief Get cell margin.
		\return The cell margin.
	 */
	virtual int GetCellMargin() = 0;
	/*! \brief Get number of outliers.
	 *	\return The number of outliers.
	 */
	virtual unsigned int GetNumOutliers() = 0;
	/*! \brief Get number of masked cells.
	 *	\return The number of masked cells.
	 */
	virtual unsigned int GetNumMasked() = 0;

	/*! \brief Get the grid coordinates.
	 *  \return Returns the grid coordinates.
	 */
	virtual FGridCoords GetGridCorners() = 0;

	// Index/position conversions
	/*! \brief Translate index to X.
	 *	\param index The index to translate for x.
	 *	\return The translated index to x value.
	 */
	virtual int IndexToX(int index) = 0;
	/*! \brief Translate index to Y.
	 *	\param index The index to translate for y.
	 *	\return The translated index to y value.
	 */
	virtual int IndexToY(int index) = 0;
	/*! \brief Translate X and Y to an index.
	 *	\param x X coordinate.
	 *	\param y Y coordinate.
	 *	\return The translated index from x and y.
	 */
	virtual int XYToIndex(int x, int y) = 0;	
	
	// Accessors for intensity information.
	/*! \brief Get entry by index.
	 *	\param index Entry index.
	 *	\param entry Entry to be filled from index.
	 */
	virtual void GetEntry(int index, FusionCELFileEntryType &entry) = 0;
	/*! \brief Get entry by x and y.
	 *	\param x X position.
	 *	\param y Y position.
	 *	\param entry Entry to be filled from x and y.
	 */
	virtual void GetEntry(int x, int y, FusionCELFileEntryType &entry) = 0;
	/*! \brief Get intensity by index position.
	 *	\param index Location of intensity
	 *	\return The intensity value.
	 */
	virtual float GetIntensity(int index) = 0;

	/*! @brief  Get a vector of intensities
   *  @param    index       index of the starting intensity.
	 *	@param    intensities vector to fill, its size is the count.
	 *	@return   non-zero on error.
	 */
	virtual int GetIntensities(int index, std::vector<float>& intensities) = 0;

	/*! \brief Get intensity by x, y position.
	 *	\param x X position.
	 *	\param y Y position.
	 *	\return The intensity value.
	 */
	virtual float GetIntensity(int x, int y) = 0;
	/*! \brief Get standard deviation by index position.
	 *	\param index Location of stdv.
	 *	\return The standard deviation value.
	 */
	virtual float GetStdv(int index) = 0;
	/*! \brief Get standard deviation by x, y position.
	 *	\param x X position.
	 *	\param y Y position.
	 *	\return The standard deviation value.
	 */
	virtual float GetStdv(int x, int y) = 0;
	/*! \brief Get pixel by index position.
	 *	\param index Location of pixel.
	 *	\return The pixel value.
	 */
	virtual short GetPixels(int index) = 0;
	/*! \brief Get pixel x, y position.
	 *	\param x X position.
	 *	\param y Y position.
	 *	\return The pixel value.
	 */
	virtual short GetPixels(int x, int y) = 0;

	// Accessors for the mask/outlier flags
	/*! \brief Get masked x, y position.
	 *	\param x X position.
	 *	\param y Y position.
	 *	\return Is index position masked.
	 */
	virtual bool IsMasked(int x, int y) = 0;
	/*! \brief Check if masked by index position.
	 *	\param index Location to check.
	 *	\return Is index position masked.
	 */
	virtual bool IsMasked(int index) = 0;
	/*! \brief Check if outlier by index position.
	 *	\param x X position.
	 *	\param y Y position.
	 *	\return Is index position an outlier.
	 */
	virtual bool IsOutlier(int x, int y) = 0;
	/*! \brief Check if outlier by index position.
	 *	\param index Location to check.
	 *	\return Is index position an outlier.
	 */
	virtual bool IsOutlier(int index) = 0;

	// For reading a file.
	/*! Close the cell file. */
	virtual void Close() = 0;
	/*! \brief Close cell file. */
	virtual bool ReadHeader() = 0;
	/*! \brief Read the cell file.
	 *	\param bIncludeMaskAndOutliers Flag indicates whether to include in the read, the reading of outliers and masked items.
	 *	\return If the read completed successfully.
	 */
	virtual bool Read(bool bIncludeMaskAndOutliers) = 0;
	/*! \brief read cell file.
	 *
	 *	The state flag is used for GCOS files only.
	 *
	 *	\param filename Cell file name to read.
	 *  \param  state [=CEL_ALL] Reading state 
	 *	\return If the read completed successfully.
	 *  \a nState can be one or combination of the following values:\n\n
	 *          CEL_ALL      Read all information in file (default)\n
	 *          CEL_DATA     Read header and intensities only\n
	 *			CEL_OUTLIER  Read header, intensities and outliers\n
	 *			CEL_MASK     Read header, intensities and masked cells\n\n
	*/
	virtual bool ReadEx(const char *filename, int state) = 0;
	/*! \brief Get the reading state 
	 *	\return The reading state.
	 */
	virtual int GetReadState() = 0;
	/*! \brief clears the members. */
	virtual void Clear() = 0;

	/*! Returns the GenericData object associated with a Calvin file, NULL for GCOS files. */
	virtual affymetrix_calvin_io::GenericData *GetGenericData() = 0;

	/*! Returns the list of parameters associated with a data set, empty for GCOS files
	 * @param setName The data set name
	 * @return The list of parameters
	 */
	virtual affymetrix_calvin_parameter::ParameterNameValueTypeList GetDataSetParameters(const std::wstring &setName) = 0;

	/*! Sets the active data group for a multi-group CEL file. Default is the first group. */
	virtual void SetActiveDataGroup(const std::wstring &groupName) = 0;

	/*! Is this a multi-color CEL file?
	 *  @return True if it is multi-color
	 */
	virtual bool IsMultiColor() = 0;

	/*! Returns a list of the channel (ie data group) names
	 *	@return list of channel names
	 */
	virtual WStringVector GetChannels() = 0;
};

}

#endif //_AffymetrixFusionCELDataInterface_HEADER_
