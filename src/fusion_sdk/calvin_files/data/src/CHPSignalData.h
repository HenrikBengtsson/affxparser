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

#ifndef _CHPSignalData_HEADER_
#define _CHPSignalData_HEADER_

#include <string>
#include "FileHeader.h"
#include "GenericData.h"
#include "AffymetrixBaseTypes.h"
#include "ProbeSetSignalData.h"

namespace affymetrix_calvin_io
{

/*! The identifier to identify a signal CHP file. */
#define CHP_SIGNAL_TYPE std::string("affymetrix-signal-analysis")

/*! The group and data set and column name to store the signal data. */
#define SIGNAL_SIGNAL_NAME std::wstring(L"Signal")

/*! The column name for the probe set name. */
#define SIGNAL_PROBE_SET_NAME std::wstring(L"ProbeSetName")

/*! Holds data associated with signal array CHP files. */
class CHPSignalData
{
public:
	/*! Constructor */
	CHPSignalData();

	/*! Constructor with file name.
	 * @param filename The name of the CHP file.
	 */
	CHPSignalData(const std::string& filename);

	/*! Destructor */
	~CHPSignalData();

private:
	/*! Flag indicating if the probe set names were stored in wide character format. */
	bool wideProbeSetNames;

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
	
	/*! Sets the number of entries (probe sets)
	 * @param ln The number of probe sets.
	 * @param maxln The maximum length of a probe set name.
	 */
	void SetEntryCount(int32_t ln, int32_t maxln);

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
	 * @param entry The signal value.
	 */
	void GetSignalEntry(int index, affymetrix_calvin_data::ProbeSetSignalData &entry);

	/*! Opens a group for reading. */
	void OpenSignalDataSet();

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
	 * @param maxProbeSetName The max probe set name length.
	 */
	void AddColumns(DataSetHeader& hdr);

};

}

#endif
