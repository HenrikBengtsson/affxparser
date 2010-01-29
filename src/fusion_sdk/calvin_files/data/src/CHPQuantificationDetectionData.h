////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006 Affymetrix, Inc.
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


#ifndef _CHPQuantificationDetectionData_HEADER_
#define _CHPQuantificationDetectionData_HEADER_

#include "calvin_files/data/src/FileHeader.h"
#include "calvin_files/data/src/GenericData.h"
#include "calvin_files/data/src/ProbeSetQuantificationDetectionData.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <string>
//

namespace affymetrix_calvin_io
{

/*! The identifier to identify a quantification/detection CHP file. */
#define CHP_QUANTIFICATION_DETECTION_TYPE "affymetrix-quantification-detection-analysis"

/*! Holds data associated with quantification/detection CHP files. */
class CHPQuantificationDetectionData
{
public:
	/*! Constructor */
	CHPQuantificationDetectionData();

	/*! Constructor with file name.
	 * @param filename The name of the CHP file.
	 */
	CHPQuantificationDetectionData(const std::string& filename);

	/*! Destructor */
	~CHPQuantificationDetectionData();

private:
	/*! Flag indicating if the probe set names were stored in wide character format. */
    DataSetColumnTypes firstColumnType;

	/*! The generic data item. */
	GenericData genericData;

	/*! chp data sets */
	DataSet* entries;

	/*! The maximum length of a probe set name. */
	int maxProbeSetName;

public:

	/*! The maximum length of a probe set name. */
	int GetMaxProbeSetName() const { return maxProbeSetName; }

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

	/*! Sets the array type */
	std::wstring GetArrayType();

	/*! Sets the array type.  */
	void SetArrayType(const std::wstring& value);
	
	/*! Sets the number of entries (probe sets). Use this function if names are to be stored.
	 * @param ln The number of probe sets.
	 * @param maxln The maximum length of a probe set name.
	 */
	void SetEntryCount(int32_t ln, int32_t maxln);
	
	/*! Sets the number of entries (probe sets). Use this function if ids are to be stored.
	 * @param ln The number of probe sets.
	 */
	void SetEntryCount(int32_t ln);

	/*! Gets the number of entries (probe sets) */
	int32_t GetEntryCount();

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

	/*! Gets the summary parameters
	 * @return The summary parameters.
	 */
	ParameterNameValueTypeList GetSummaryParams();

	/*! Adds the summary parameters.
	 * @param params The summary parameters.
	 */
	void AddSummaryParams(const ParameterNameValueTypeList& params);

	/*! Gets the file header.
	 * @return The file header.
	 */
	FileHeader* GetFileHeader() { return &genericData.Header(); }

	/*! Gets the generic data object.
	 * @return The data object.
	 */
	GenericData& GetGenericData() { return genericData; }

	/*! Gets the sequence data.
	 * @param index The row index.
	 * @param entry The quantification/detection value.
	 */
	void GetQuantificationDetectionEntry(int index, affymetrix_calvin_data::ProbeSetQuantificationDetectionData &entry);

	/*! Opens a group for reading. */
	void OpenQuantificationDetectionDataSet();

    /*! Returns the data set header. */
    DataSetHeader &GetDataSetHeader();

private:

	/*! Gets a parameter value as a string.
	 * @param name The name of the parameter.
	 * @return The string representation.
	 */
	std::wstring GetWStringFromGenericHdr(const std::wstring& name);

	/*! Adds a parameter name/value pair to the parameters section.
	 * @param name The name of the parameter.
	 * @param value The value of the parameter.
	 * @param reserve The length of the string to reserve.
	 */
	void SetWStringToGenericHdr(const std::wstring& name, const std::wstring value, int32_t reserve=-1);

	/*! Adds the columns to the data set.
	 * @param hdr The data set header.
	 * @param keyIsID True if the probe set ids are to be stored, false for probe set names.
	 */
	void AddColumns(DataSetHeader& hdr, bool keyIsID);

};

}

#endif
