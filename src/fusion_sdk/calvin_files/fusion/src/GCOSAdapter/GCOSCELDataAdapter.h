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


#ifndef _AffymetrixGCOSCELDataAdapter_HEADER_
#define _AffymetrixGCOSCELDataAdapter_HEADER_

/*! \file GCOSCELDataAdapter.h This file defines the GCOS Fusion CEL Data adapter classes
 */

#include "FusionCELDataAdapterInterface.h"
#include "CELFileData.h"
#include "StringUtils.h"
#include <string>

namespace affymetrix_fusion_io
{

////////////////////////////////////////////////////////////////////

/*! \brief GCOS Adapter that wraps GCOS cell file reader for Fusion and contains the interfaces to the data.*/
class GCOSCELDataAdapter : public IFusionCELDataAdapter
{
public:
	/*! \brief Constructor */
	GCOSCELDataAdapter();
	/*! \brief Destructor */
	virtual ~GCOSCELDataAdapter();

public:
	/*! \brief Can this object read the file 
	 *	\return If the cell file can be read.
	 */
	virtual bool CanReadFile();

	/*! Set the cell file name. 
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
	virtual affymetrix_calvin_utilities::AffymetrixGuidType GetFileId() { return ""; }
	/*! \brief Set the error message.
	 *	\param value The error message to be set.
	 */
	virtual void SetError(const wchar_t* value);
	/*! \brief Get the currently set error message string. 
	 * \return The error message.
	 */
	virtual std::wstring GetError();

	/*! \brief Get the header key.
	 *  \return The header key.
	 */
	virtual std::wstring GetHeaderKey(const wchar_t* key);
	/*! \brief Get the version of the cell file.
	 *	\return The cell file version.
	 */
	virtual int GetVersion() { return gcosCel.GetVersion(); }
	/*! \brief Get the number of columns.
	 *	\return The number of columns
	 */
	virtual int GetCols() { return gcosCel.GetCols(); }
	/*! \brief Get the number of rows.
	 *	\return The number of rows.
	 */
	virtual int GetRows() { return gcosCel.GetRows(); }
	/*! \brief Get number of cells
	 *	\return The number of cells
	 */
	virtual int GetNumCells() { return gcosCel.GetNumCells(); }
	/*! \brief Get the header string.
	 *	\return The header as a string.
	 */
	virtual std::wstring GetHeader();
	/*! \brief Get the algorithm name.
	 *	\return The algorithm name.
	 */
	virtual std::wstring GetAlg();
	/*! \brief Get the parameters.
	 *	\return The parameters used for creating the cell file.
	 */
	virtual std::wstring GetParams();
	/*!	\brief Get a parameter.
	 *	\param tag Parameter name associated with a parameter value.
	 *	\return The parameter value.
	 */
	virtual std::wstring GetAlgorithmParameter(const wchar_t *tag);
	/*! \brief Get a parameter.
	 *	\param index The index to the parameter.
	 *	\return The parameter value.
	 */
	virtual std::wstring GetAlgorithmParameterTag(int index);
	/*! \brief Get number of parameters.
	 *	\return The number of parameters.
	 */
	virtual int GetNumberAlgorithmParameters() { return gcosCel.GetNumberAlgorithmParameters(); }
	/*! \brief Get the number of parameters.
	 *	\return The parameters.
	*/
	virtual std::wstring GetAlgorithmParameters();
	/*! \brief Get parameters.
	 *	\param values Collection of name/value type parameter list.
	 */
	virtual void GetParameters(FusionTagValuePairTypeList& values);
	/*! \brief Get the DatHeader string.
	 *	\return The DatHeader string.
	 */
	virtual std::wstring GetDatHeader() { return affymetrix_calvin_utilities::StringUtils::ConvertMBSToWCS(gcosCel.GetDatHeader()); }
	/*! \brief Get chip type.
	 *	\return The chip type of the cell.
	 */
	virtual std::wstring GetChipType();
	/*! \brief Get the library package
	 *	\return blank value
	 */
    virtual std::wstring GetLibraryPackageName() { return L""; }
	/*! \brief Get the master file
	 *	\return blank value
	 */
	virtual std::wstring GetMasterFileName() { return L""; }
	/*! \brief Get cell margin.
		\return The cell margin.
	 */
	virtual int GetCellMargin() { return gcosCel.GetCellMargin(); }
	/*! \brief Get number of outliers.
	 *	\return The number of outliers.
	 */
	virtual unsigned int GetNumOutliers() { return gcosCel.GetNumOutliers(); }
	/*! \brief Get number of masked cells.
	 *	\return The number of masked cells.
	 */
	virtual unsigned int GetNumMasked() { return gcosCel.GetNumMasked(); }
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
	virtual void Close();
	/*! \brief Close cell file. */
	virtual bool ReadHeader();
	/*! \brief Read the cell file.
	 *	\param bIncludeMaskAndOutlier Flag indicates whether to include in the read, the reading of outliers and masked items.
	 *	\return If the read completed successfully.
	 */
	virtual bool Read(bool bIncludeMaskAndOutlier);
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
	virtual bool ReadEx(const char *filename, int state) { return gcosCel.ReadEx(filename, state); }
	/*! \brief Get the reading state 
	 *	\return The reading state.
	 */
	virtual int GetReadState() { return gcosCel.GetReadState(); }
	/*! \brief clears the members. */
	virtual void Clear();

	/*! Returns the GenericData object associated with a Calvin file, NULL for GCOS files. */
	virtual affymetrix_calvin_io::GenericData *GetGenericData() { return NULL; }

	/*! \brief Sets the name of the algorithm used to create the CEL file. 
	 *	\param str The algorithm name.
	 */
//	virtual void SetAlgorithmName(const wchar_t *str);
	/*! \brief Add algorithm parameter.
	 *	\param tag Parameter name.
	 *	\param value Parameter value.
	 */
//	virtual void AddAlgorithmParameter(const wchar_t *tag, const wchar_t *value);
	/*! \brief Set the number of rows and columns.
	 *	\param rows Number of rows.
	 *	\param cols Number of columns.
	 */
//	virtual void SetDimensions(int rows, int cols) { gcosCel.SetDimensions(rows, cols); }
	/*! \brief Set the chip type.
	 *	\param str Chip type.
	 */
//	virtual void SetChipType(const wchar_t *str);
	/*! \brief Set the margin.
	 *	\param margin Margin value to set.
	 */
//	virtual void SetMargin(int margin) { gcosCel.SetMargin(margin); }

protected:
	/*! \brief Covert an GCOS entry type to an Calvin entry type.
	 *	\param source GCOS entry type to convert from.
	 *	\param target Calvin entry type to convert to.
	 */
	static void CopyGCOSEntryToCalvin(const affxcel::CELFileEntryType& source, FusionCELFileEntryType& target);

protected:
	/*! The underlying data access object */
	affxcel::CCELFileData gcosCel;
};

}

#endif	//_AffymetrixGCOSCELDataAdapter_HEADER_
