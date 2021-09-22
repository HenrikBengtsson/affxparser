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


#ifndef _AffymetrixFusionCELData_HEADER_
#define _AffymetrixFusionCELData_HEADER_

/*! \file FusionCELData.h This file defines the Fusion CEL Data classes
 */

#include "calvin_files/fusion/src/FusionCELDataAdapterInterface.h"
//

namespace affymetrix_fusion_io
{

////////////////////////////////////////////////////////////////////

/*! This defines the combined GCOS/Calvin CEL data interaction class. */
class FusionCELData  
{
public:
	/*! CEL file reading states */
	enum
	{
		CEL_ALL=1,
		CEL_DATA=2,
		CEL_OUTLIER=4,
		CEL_MASK=8
	};

public:
	/*! Constructor */
	FusionCELData();
	/*! Destructor */
	~FusionCELData();

public:
	/*! Set the file name.
	 *	@ value The file name.  Can have the full path or relative path
	 */
	void SetFileName(const char *value);

	/*! Get the file name.
	 *	@return File name
	 */
	std::string GetFileName() const;

	/*! Get the id of the file (only valid for Command Console "calvin" files)
	 * @return The unique file id.
	 */
	affymetrix_calvin_utilities::AffymetrixGuidType GetFileId() const;

	/*! Set the error string.
	 * @param value The error.
	 */
	void SetError(const wchar_t* value);

	/*! Retrieve error string
	 *  @return	Error string
	 */
	std::wstring GetError();

	/*! Accessors for header information. */
	std::wstring GetHeaderKey(const wchar_t* key);

	/*! Retrieve CEL file format version number.
	 *  @return CEL file format version number
	 */
	int GetVersion();

	/*!  Retrieve number of columns in array
	 *  @return Number of columns in array
	 */
	int GetCols();

	/*!	Retrieve number of rows in array
	 *	@return Number of rows in array
	 */
	int GetRows();

	/*! Retrieve number of cells in array
	 *	@return Number of cells in array
	 */
	int GetNumCells();

	/*!	Retrieve header in a single string
	 *	@return Header string
	 */
	std::wstring GetHeader();

	/*!	Retrieve algorithm name
	 *	@return Algorithm name
	 */
	std::wstring GetAlg();

	/*!	Retrieve algorithm version
	 *	@return Algorithm version
	 */
	std::wstring GetAlgVer();

	/*! Retrieve algorithm parameters
	 *  @return	Algorithm parameters
	 */
	std::wstring GetParams();

	/*!	Retrieve algorithm parameter of specified tag
	 *	@param tag Algorithm parameter tag
	 *	@return Algorithm parameter value
	 */
	std::wstring GetAlgorithmParameter(const wchar_t *tag);

	/*! Retrieves the algorithm parameter name (tag) for a given index position.
	 *	@param index The zero based index to the parameter array (0 to the number of alg parameters - 1).
	 *	@return The parameter name (tag).
	 */
	std::wstring GetAlgorithmParameterTag(int index);

	/*! Retrieves the number of algorithm parameters.
	 *	@return The number of algorithm parameters.
	 */
	int GetNumberAlgorithmParameters();

	/*! Retrieve algorithm parameters
	 *	@return Algorithm parameters
	 */
	std::wstring GetAlgorithmParameters();

	/*! Retreive the algorithm parameters in a list.
	 *	@return An STL list of algorithm parameters
	 */
	FusionTagValuePairTypeList &GetParameters();

	/*! Get the DatHeader string.
	 *	@return The DatHeader string.
	 */
	std::wstring GetDatHeader();

	/*!	Retrieve chip type
	 *	@return	Chip type
	 */
	std::wstring GetChipType();

	/*! Get the library package
	 *	@return library package name (blank for GCOS format CEL files)
	 */
	std::wstring GetLibraryPackageName();

	/*! Get the master file
	 *	@return master file name (blank for GCOS format CEL files)
	 */
	std::wstring GetMasterFileName();
    
	/*!	Retrieve cell margin
	 *	@return Cell margin
	 */
	int GetCellMargin();

	/*!	Retrieve number of outliers
	 *	@return Number of outliers
	 */
	unsigned int GetNumOutliers();

	/*!	Retrieve number of masked cells
	 *	@return Number of masked cells
	 */
	unsigned int GetNumMasked();

	/*! Get the grid coordinates.
	 *  @return Returns the grid coordinates.
	 */
	FGridCoords GetGridCorners();

	// Index/position conversions
	/*!	Get x coordinates from index
	 *  @return X coordinates
	 */
	int IndexToX(int index);

	/*!	Get y coordinates from index
	 *	@return Y coordinates
	 */
	int IndexToY(int index);

	/*!	Convert x, y coordinates to index
	 *	@return Index
	 */
	int XYToIndex(int x, int y);

	/*! Maps X/Y coordinates to CEL file index.
	 * @param x The x coordinate.
	 * @param y The y coordinate.
	 * @param r The number of rows.
	 * @param c The number of columns.
	 * @return The index to the intensity arrays.
	 */
	static int XYToIndex(int x, int y, int r, int c);


	// Accessors for intensity information.

	/*! Retrieve a CEL file entry.
	 * @param index The index to the CEL file entries.
	 * @param entry The CEL file entry.
	 */
	void GetEntry(int index, FusionCELFileEntryType &entry);

	/*! Retrieve a CEL file entry.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @param entry The CEL file entry.
	 */
	void GetEntry(int x, int y, FusionCELFileEntryType &entry);

	/*! Retrieve a CEL file intensity.
	 * @param index The index to the CEL file entries.
	 * @return The CEL file intensity.
	 */
	float GetIntensity(int index);

  /// @brief     Get a vector of intensities.
  /// @param     index         The index of where to start.
  /// @param     intensity_vec The vector to fill, its size is the number of intensities.
	int GetIntensities(int index,std::vector<float>& intensity_vec);

	/*! Retrieve a CEL file intensity.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @return The CEL file intensity.
	 */
	float GetIntensity(int x, int y);

	/*! Retrieve a CEL file stdv value.
	 * @param index The index to the CEL file entries.
	 * @return The CEL file stdv value.
	 */
	float GetStdv(int index);

	/*! Retrieve a CEL file stdv value.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @return The CEL file stdv value.
	 */
	float GetStdv(int x, int y);

	/*! Retrieve a CEL file pixel count.
	 * @param index The index to the CEL file entries.
	 * @return The CEL file pixel count.
	 */
	short GetPixels(int index);

	/*! Retrieve a CEL file pixel count.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @return The CEL file pixel count.
	 */
	short GetPixels(int x, int y);

	// Accessors for the mask/outlier flags
	/*! Retrieve a CEL file mask flag.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @return True if the feature is masked.
	 */
	bool IsMasked(int x, int y);

	/*! Retrieve a CEL file mask flag.
	 * @param index The index to the CEL file entries.
	 * @return True if the feature is masked.
	 */
	bool IsMasked(int index);

	/*! Retrieve a CEL file outlier flag.
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @return True if the feature is an outlier.
	 */
	bool IsOutlier(int x, int y);

	/*! Retrieve a CEL file outlier flag.
	 * @param index The index to the CEL file entries.
	 * @return True if the feature is an outlier.
	 */
	bool IsOutlier(int index);

	// For reading a file.
	/*! Closes the file */
	void Close();

	/*! Returns the file size.
	 * @return The file size.
	 */
	unsigned int GetFileSize();

	/*! Check if the file exists.
	 * @return True if the file exists.
	 */
	bool Exists();

	/*! Reads the header of the CEL file.
	 * @return True if successful.
	 */
	bool ReadHeader();

	/*! Reads the CEL file.
	 * @param bIncludeMaskAndOutliers Flag to indicate if the mask and outlier sections should also be read.
	 * @return True if successful.
	 */
	bool Read(bool bIncludeMaskAndOutliers=true);

	/*! Determine CEL file format and call appropriate function to read file using the specified file name.
	 * @param filename The name of the file.
	 * @param state Reading state 
	 * @return bool	true if success, false if fail
	 *
	 * \a state can be one or combination of the following values:
	 *       CEL_ALL      Read all information in file (default)
	 *       CEL_DATA     Read header and intensities only
	 *		 CEL_OUTLIER  Read header, intensities and outliers
	 *		 CEL_MASK     Read header, intensities and masked cells
	 */
	bool ReadEx(const char *filename=0, int state=CEL_ALL);

	/*! Returns the reading state.
	 * @return The reading state.
	 */
	int GetReadState();

	// For writing a new CEL file

	/*! Clears the members. */
	void Clear();

	/*! Returns the GenericData object associated with a Calvin file, NULL for GCOS files. */
	affymetrix_calvin_io::GenericData *GetGenericData();

	/*! Returns the list of parameters associated with a data set, empty for GCOS files
	 * @param setName The data set name
	 * @return The list of parameters
	 */
	affymetrix_calvin_parameter::ParameterNameValueTypeList GetDataSetParameters(const std::wstring &setName);

	/*! Sets the active data group for a multi-group CEL file. Default is the first group. */
	void SetActiveDataGroup(const std::wstring &groupName);

	/*! Is this a multi-color CEL file?
	 *  @return True if it is multi-color
	 */
	bool IsMultiColor();

	/*! Returns a list of the channel (ie data group) names
	 *	@return list of channel names
	 */
	WStringVector GetChannels();

protected:
	/*! Factory method to create the appropriate adapter class.
	 *	@exception UnableToOpenFileException
	 */
	void CreateAdapter();

	/*! Delete the adapter member */
	void DeleteAdapter();

	/*! Check the adapter and throw exception if not set
	 *	@exception FileNotOpenException
	 */
	void CheckAdapter() const;

	/*!  */
	void FillParameterList();

private:
	/*! Pointer to the adapter */
	IFusionCELDataAdapter* adapter;
	/*! CEL file name including a relative path or the full path. */
	std::string filename;
	/*! A separate parallel parameter list that is sync'ed with the list in the adapter */
	FusionTagValuePairTypeList parameterList;
	/*! Indicates whether the parameter list has been read from the adapter */
	bool parameterListRead;
};

}

#endif	//_AffymetrixFusionCELData_HEADER_
