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
#ifndef _CHPTilingData_HEADER_
#define _CHPTilingData_HEADER_

#include "calvin_files/data/src/CHPTilingEntry.h"
#include "calvin_files/data/src/FileHeader.h"
#include "calvin_files/data/src/GenericData.h"
#include "calvin_files/data/src/TilingResultData.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <string>
//

namespace affymetrix_calvin_io
{

/*! The identifier to identify a tiling CHP file. */
#define CHP_TILING_TYPE std::string("affymetrix-tiling-analysis")

/*! The identifier for the type of data stored in the file. */
#define TILING_DATA_TYPE std::wstring(L"file_type")

/*! The identifier for the scale of the data. */
#define TILING_PARAM_SUFFIX std::wstring(L"Param-")

/*! The identifier for the algorithms tail type. */
#define TILING_NUM_SEQS std::wstring(L"NumberSequences")

/*! The identifier for the algorithm name. */
#define TILING_ALG_NAME std::wstring(L"Algorithm-Name")

/*! The identifier for the algorithm version. */
#define TILING_ALG_VERSION std::wstring(L"Algorithm-Version")

/*! The name of the tiling data group. */
#define CHP_TILING_GROUP std::wstring(L"Tiling Results")

/*! The id for the sequence name. */
#define TILING_SEQ_NAME std::wstring(L"Name")

/*! The id for the sequence group name. */
#define TILING_SEQ_GROUP_NAME std::wstring(L"GroupName")

/*! The id for the sequence version. */
#define TILING_SEQ_VERSION std::wstring(L"Version")

/*! The value to indicate signal values are stored in the CHP file. */
#define TILING_SIGNAL_VALUES std::wstring(L"Signal")

/*! The value to indicate p-values are stored in the CHP file. */
#define TILING_PVALUE_VALUES std::wstring(L"p-value")

/*! Holds data associated with tiling array CHP files. */
class CHPTilingData
{
public:
	/*! Constructor */
	CHPTilingData();

	/*! Constructor with file name.
	 * @param filename The name of the CHP file.
	 */
	CHPTilingData(const std::string& filename);

	/*! Destructor */
	~CHPTilingData();

private:
	/*! The generic data item. */
	GenericData genericData;

	/*! keep number of sequences from being read from the header all the time */
	int32_t cachedNumSequences;

	/*! chp data sets */
	DataSet* entries;

public:

	/*! Clears the members. */
	void Clear();

	/*! Sets the file name.
	 * @param p The name of the CHP file
	 */
	void SetFilename(const std::string &p);

	/*! Gets the file name.
	 * @return The file name.
	 */
	std::string GetFilename() const;

	/*! Gets the number of sequences.
	 * @return The number of sequences.
	 */
	int32_t GetNumberSequences();

	/*! Sets the number of sequences.
	 * @param value The number of sequences.
	 */
	void SetNumberSequences(int32_t value);

	/*! Gets the name of the algorithm.
	 * @return The algorithm name.
	 */
	std::wstring GetAlgName();

	/*! Sets the name of the algorithm.
	 * @param value The algorithm name.
	 */
	void SetAlgName(const std::wstring& value);

	/*! Gets the algorithm version.
	 * @return The version.
	 */
	std::wstring GetAlgVersion();

	/*! Sets the algorithm version.
	 * @param value The version.
	 */
	void SetAlgVersion(const std::wstring& value);

	/*! Gets the algorithm parameters
	 * @return The algoirhtm parameters.
	 */
	ParameterNameValueTypeList GetAlgParams();

	/*! Adds the algorithm parameters.
	 * @param params The algorithm parameters.
	 */
	void AddAlgParams(const ParameterNameValueTypeList& params);

	/*! Gets the file header.
	 * @return The file header.
	 */
	FileHeader* GetFileHeader() { return &genericData.Header(); }

	/*! Gets the generic data object.
	 * @return The data object.
	 */
	GenericData& GetGenericData() { return genericData; }

	/*! Adds a data group to the file.
	 * @param numEntries The number of entries in the sequence.
	 * @param data The data associated with the sequence.
	 */
	void AddTilingSequenceData(int32_t numEntries, const TilingSequenceData &data);

	/*! Gets the sequence data.
	 * @return The data associated with the sequence.
	 */
	TilingSequenceData GetTilingSequenceData();

	/*! Gets the number of entries in a tiling sequence.
	 * @param index The sequence index.
	 * @return The number of entries in the sequence.
	 */
	int32_t GetTilingSequenceEntryCount(int index);

	/*! Opens a group for reading.
	 * @param index The index to the sequence.
	 */
	void OpenTilingSequenceDataSet(int index);

	/*! Returns the entry for the given row. The data set must be open.
	 * @param row The row index.
	 * @param e The entry.
	 */
	void GetTilingSequenceEntry(int row, CHPTilingEntry& e);

private:

	/*! Gets a parameter value as a string.
	 * @param name The name of the parameter.
	 * @return The string representation.
	 */
	std::wstring GetWStringFromGenericHdr(const std::wstring& name);

	/*! Adds a parameter name/value pair to the parameters section.
	 * @param name The name of the parameter.
	 * @param value The value of the parameter.
	 */
	void SetWStringToGenericHdr(const std::wstring& name, const std::wstring value);

	/*! Gets a parameter value as an integer.
	 * @param name The name of the parameter.
	 * @return The integer representation.
	 */
	int32_t GetInt32FromGenericHdr(const std::wstring& name);

	/*! Adds a parameter name/value pair to the parameters section.
	 * @param name The name of the parameter.
	 * @param value The value of the parameter.
	 */
	void SetInt32ToGenericHdr(const std::wstring& name, int32_t value);

	/*! Adds the columns to the data set.
	 * @param hdr The data set header.
	 */
	void AddColumns(DataSetHeader& hdr);

};

}

#endif
