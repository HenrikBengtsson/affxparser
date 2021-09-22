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


#ifndef _AffymetrixCalvinCELDataAdapter_HEADER_
#define _AffymetrixCalvinCELDataAdapter_HEADER_

/*! \file CalvinCELDataAdapter.h This file defines the Calvin Fusion CEL Data adapter classes
 */

#include "calvin_files/data/src/CELData.h"
#include "calvin_files/fusion/src/FusionCELDataAdapterInterface.h"
//

namespace affymetrix_fusion_io
{

////////////////////////////////////////////////////////////////////

/*!  */
class CalvinCELDataAdapter : public IFusionCELDataAdapter
{
public:
	/*! \brief Constructor */
	CalvinCELDataAdapter();
	/*! \brief Destructor */
	virtual ~CalvinCELDataAdapter();

public:
	/*! \brief Can this object read the file 
	 *	\return If the cell file can be read.
	 */
	virtual bool CanReadFile();

	/*! \brief Set the cell file name. 
	 *	\param value The cell file name to be set
	 */
	virtual void SetFileName(const std::string& value);
	/*! \brief Get the cell file name. 
	 *	\return The currently set cell file name.
	 */
	virtual std::string GetFileName() const;
	/*! \brief Get the id of the file (only valid for Command Console "calvin" files)
	 * \return The unique file id.
	 */
	virtual affymetrix_calvin_utilities::AffymetrixGuidType GetFileId();
	/*! \brief Set the error message.
	 *	\param value The error message to be set.
	 */
	virtual void SetError(const wchar_t* value) {}	// do nothing
	/*! \brief Get the currently set error message string. 
	 * \return The error message.
	 */
	virtual std::wstring GetError() { return L""; }
	/*! \brief Get the header key.
	 *  \return The header key.
	 */
	virtual std::wstring GetHeaderKey(const wchar_t* key) { return L""; }
	/*! \brief Get the version of the cell file.
	 *	\return The cell file version.
	 */
	virtual int GetVersion() { return calvinCel.GetVersion(); }
	/*! \brief Get the number of columns.
	 *	\return The number of columns
	 */
	virtual int GetCols() { return calvinCel.GetCols(); }
	/*! \brief Get the number of rows.
	 *	\return The number of rows.
	 */
	virtual int GetRows() { return calvinCel.GetRows(); }
	/*! \brief Get number of cells
	 *	\return The number of cells
	 */
	virtual int GetNumCells() { return calvinCel.GetNumCells(); }
	/*! \brief Get the header string.
	 *	\return The header as a string.
	 */
	virtual std::wstring GetHeader() { return L""; }
	/*! \brief Get the algorithm name.
	 *	\return The algorithm name.
	 */
	virtual std::wstring GetAlg() { return calvinCel.GetAlgorithmName(); }
	/*! \brief Get the algorithm version.
	 *	\return The algorithm version.
	 */
	virtual std::wstring GetAlgVer() { return calvinCel.GetAlgorithmVersion(); }

	/*! \brief Get the parameters.
	 *	\return The parameters used for creating the cell file.
	 */
	virtual std::wstring GetParams();
	/*!	\brief Get a parameter.
	 *	\param tag Parameter name associated with a parameter value.
	 *	\return The parameter value.
	 */
	virtual std::wstring GetAlgorithmParameter(const wchar_t *tag);
	/*! \brief Get parameter.
	 *	\param index The index to the parameter array.
	 *	\return The parameter value.
	 */
	virtual std::wstring GetAlgorithmParameterTag(int index);
	/*! \brief Get number of parameters.
	 *	\return The number of parameters.
	 */
	virtual int GetNumberAlgorithmParameters() ;
	/*! \brief Get the number of parameters.
	 *	\return The number of parameters.
	*/
	virtual std::wstring GetAlgorithmParameters();
	/*! \brief Get parameters.
	 *	\param values Collection of name/value type parameter list.
	 */
	virtual void GetParameters(FusionTagValuePairTypeList& values);
	/*! \brief Get the DatHeader string.
	 *	Attempts to read the DatHeader parameter of the DAT generic data header.
	 *	\return The DatHeader parameter value if found, otherwise an empty string.
	 */
	virtual std::wstring GetDatHeader();
	/*! \brief Get chip type.
	 *	\return The chip type of the cell.
	 */
	virtual std::wstring GetChipType() { return calvinCel.GetArrayType(); }
	/*! \brief Get the library package
	 *	\return library package name
	 */
    std::wstring GetLibraryPackageName() { return calvinCel.GetLibraryPackageName(); }
	/*! \brief Get the master file
	 *	\return master file name
	 */
    std::wstring GetMasterFileName() { return calvinCel.GetMasterFileName(); }
	/*! \brief Get cell margin.
		\return The cell margin.
	 */
	virtual int GetCellMargin();
	/*! \brief Get number of outliers.
	 *	\return The number of outliers.
	 */
	virtual unsigned int GetNumOutliers();
	/*! \brief Get number of masked cells.
	 *	\return The number of masked cells.
	 */
	virtual unsigned int GetNumMasked();
	/*! \brief Get the grid coordinates.
	 *  \return Returns the grid coordinates.
	 */
	virtual FGridCoords GetGridCorners();

	// Index/position conversions
	/*! \brief Translate index to X.
	 *	\param index The index to translate for x.
	 *	\return The translated index to x value.
	 */
	virtual int IndexToX(int index);
	/*! \brief Translate index to Y.
	 *	\param index The index to translate for y.
	 *	\return The translated index to y value.
	 */
	virtual int IndexToY(int index);
	/*! \brief Translate X and Y to an index.
	 *	\param x X coordinate.
	 *	\param y Y coordinate.
	 *	\return The translated index from x and y.
	 */
	virtual int XYToIndex(int x, int y);

	// Accessors for intensity information.
	/*! \brief Get entry by index.
	 *	\param index Entry index.
	 *	\param entry Entry to be filled from index.
	 */
	virtual void GetEntry(int index, FusionCELFileEntryType &entry);
	/*! \brief Get entry by x and y.
	 *	\param x X position.
	 *	\param y Y position.
	 *	\param entry Entry to be filled from x and y.
	 */
	virtual void GetEntry(int x, int y, FusionCELFileEntryType &entry);
	/*! \brief Get intensity by index position.
	 *	\param index Location of intensity
	 *	\return The intensity value.
	 */
	virtual float GetIntensity(int index);
	/*! @brief   Get a vector of intensities
	 *	@param   index        Index of first intensity
   *  @param   intensities  vector of intensities to fill
	 *	@return  non-zero on error.
	 */
	virtual int GetIntensities(int index,std::vector<float>& intensities);
	/*! \brief Get intensity by x, y position.
	 *	\param x X position.
	 *	\param y Y position.
	 *	\return The intensity value.
	 */
	virtual float GetIntensity(int x, int y);
	/*! \brief Get standard deviation by index position.
	 *	\param index Location of stdv.
	 *	\return The standard deviation value.
	 */
	virtual float GetStdv(int index);
	/*! \brief Get standard deviation by x, y position.
	 *	\param x X position.
	 *	\param y Y position.
	 *	\return The standard deviation value.
	 */
	virtual float GetStdv(int x, int y);
	/*! \brief Get pixel by index position.
	 *	\param index Location of pixel.
	 *	\return The pixel value.
	 */
	virtual short GetPixels(int index);
	/*! \brief Get pixel x, y position.
	 *	\param x X position.
	 *	\param y Y position.
	 *	\return The pixel value.
	 */
	virtual short GetPixels(int x, int y);

	// Accessors for the mask/outlier flags
	/*! \brief Get masked x, y position.
	 *	\param x X position.
	 *	\param y Y position.
	 *	\return Is index position masked.
	 */
	virtual bool IsMasked(int x, int y);
	/*! \brief Check if masked by index position.
	 *	\param index Location to check.
	 *	\return Is index position masked.
	 */
	virtual bool IsMasked(int index);
	/*! \brief Check if outlier by index position.
	 *	\param x X position.
	 *	\param y Y position.
	 *	\return Is index position an outlier.
	 */
	virtual bool IsOutlier(int x, int y);
	/*! \brief Check if outlier by index position.
	 *	\param index Location to check.
	 *	\return Is index position an outlier.
	 */
	virtual bool IsOutlier(int index);

	// For reading a file.
	/*! Close the cell file. */
	virtual void Close() {}
	/*! \brief Close cell file. */
	virtual bool ReadHeader();
	/*! \brief Read the cell file.
	 *	\param bIncludeMaskAndOutliers Flag indicates whether to include in the read, the reading of outliers and masked items.
	 *	\return If the read completed successfully.
	 */
	virtual bool Read(bool bIncludeMaskAndOutliers);
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
	virtual bool ReadEx(const char *filename, int state);
	/*! \brief Get the reading state 
	 *	\return The reading state.
	 */
	virtual int GetReadState() { return 1; /* CEL_ALL, always*/}
	/*! \brief clears the members. */
	virtual void Clear() { calvinCel.Clear(); }

	/*! Returns the GenericData object associated with a Calvin file, NULL for GCOS files. */
	virtual affymetrix_calvin_io::GenericData *GetGenericData();

	/*! Returns the list of parameters associated with a data set.
	 * @param setName The data set name
	 * @return The list of parameters
	 */
	virtual affymetrix_calvin_parameter::ParameterNameValueTypeList GetDataSetParameters(const std::wstring &setName);

	/*! Sets the active data group for a multi-group CEL file. Default is the first group. */
	virtual void SetActiveDataGroup(const std::wstring &groupName);

	/*! Is this a multi-color CEL file?
	 *  @return True if it is multi-color
	 */
	virtual bool IsMultiColor();

	/*! Returns a list of the channel (ie data group) names
	 *	@return list of channel names
	 */
	virtual WStringVector GetChannels();

	// Sets the data values
	/*! \brief Sets the name of the algorithm used to create the CEL file. 
	 *	\param str The algorithm name.
	 */
//	virtual void SetAlgorithmName(const wchar_t *str) { calvinCel.SetAlgorithmName(str); }
	/*! \brief Add algorithm parameter.
	 *	\param tag Parameter name.
	 *	\param value Parameter value.
	 */
//	virtual void AddAlgorithmParameter(const wchar_t *tag, const wchar_t *value);
	/*! \brief Set the number of rows and columns.
	 *	\param rows Number of rows.
	 *	\param cols Number of columns.
	 */
//	virtual void SetDimensions(int rows, int cols);
	/*! \brief Set the chip type.
	 *	\param str Chip type.
	 */
//	virtual void SetChipType(const wchar_t *str) { calvinCel.SetArrayType(str); }
	/*! \brief Set the margin.
	 *	\param margin Margin value to set.
	 */
//	virtual void SetMargin(int margin) {}

protected:
	/*! The underlying data access object */
	affymetrix_calvin_io::CelFileData calvinCel;
};

}

#endif	//_AffymetrixCalvinCELDataAdapter_HEADER_
