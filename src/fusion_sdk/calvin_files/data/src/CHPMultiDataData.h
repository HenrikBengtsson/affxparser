////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007 Affymetrix, Inc.
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


#ifndef _CHPMultiDataData_HEADER_
#define _CHPMultiDataData_HEADER_

#include <string>
#include <map>
#include "FileHeader.h"
#include "GenericData.h"
#include "AffymetrixBaseTypes.h"
#include "ProbeSetMultiDataData.h"
#include "ColumnInfo.h"

namespace affymetrix_calvin_io
{

/*! The identifier to identify a multi-data CHP file. */
#define CHP_MULTI_DATA_TYPE "affymetrix-multi-data-type-analysis"

/*! An enumerant to store the types of data stored in the file. */
typedef enum MultiDataType { ExpressionMultiDataType, ExpressionControlMultiDataType, GenotypeMultiDataType, GenotypeControlMultiDataType, CopyNumberMultiDataType, CytoMultiDataType };

/*! The data set name. */
const static std::wstring MultiDataDataSetNames[] = 
{
    L"Expression",
    L"ExpressionControl",
    L"Genotype",
    L"GenotypeControl",
    L"CopyNumber",
    L"Cyto"
};

/*! The data types. */
const static MultiDataType MultiDataDataTypes[] = 
{
    ExpressionMultiDataType,
    ExpressionControlMultiDataType,
    GenotypeMultiDataType,
    GenotypeControlMultiDataType,
    CopyNumberMultiDataType,
    CytoMultiDataType
};

/*! Holds data set information. */
class DataSetInfo
{
public:
    /*! The data type. */
    MultiDataType dataType;

    /*! chp data sets */
    DataSet* entries;

    /*! The maximum length of a probe set name. */
    int maxProbeSetName;

    /*! The data set index. */
    int dataSetIndex;

    /*! An array of extra metric columns. */
    std::vector<ColumnInfo> metricColumns;

    /*! constructor */
    DataSetInfo() { entries = NULL; maxProbeSetName = -1; dataSetIndex=-1; }
};

/*! Holds data associated with genotype or expression CHP files. */
class CHPMultiDataData
{
public:
	/*! Constructor */
	CHPMultiDataData();

	/*! Constructor with file name.
	 * @param filename The name of the CHP file.
	 */
	CHPMultiDataData(const std::string& filename);

	/*! Destructor */
	~CHPMultiDataData();

private:

	/*! The generic data item. */
	GenericData genericData;

	/*! chp data sets */
    std::map<MultiDataType, DataSetInfo> dataSetInfo;

public:

    /*! The data set information */
    std::map<MultiDataType, DataSetInfo> &GetDataSetInfo() { return dataSetInfo; }

	/*! The maximum length of a probe set name.
     * @param dataType The data type
     * @return The maximum probe set name length
     */
	int GetMaxProbeSetName(MultiDataType dataType);

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
	
	/*! Sets the number of entries (probe sets).
     * @param dataType The data type
	 * @param ln The number of probe sets.
	 * @param maxln The maximum length of a probe set name.
	 * @param columns An array of metric columns
	 */
	void SetEntryCount(MultiDataType dataType, int32_t ln, int32_t maxln, const std::vector<ColumnInfo> &columns);

	/*! Sets the number of entries (probe sets).
     * @param dataType The data type
	 * @param ln The number of probe sets.
	 * @param maxln The maximum length of a probe set name.
	 */
	void SetEntryCount(MultiDataType dataType, int32_t ln, int32_t maxln);

	/*! Gets the number of entries (probe sets) 
     * @param dataType The data type
     */
	int32_t GetEntryCount(MultiDataType dataType);

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

	/*! Gets the probe set data.
     * @param dataType The data type
	 * @param index The row index.
	 * @param entry The genotype results.
	 */
	void GetGenotypeEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataGenotypeData &entry);

	/*! Gets the probe set data.
     * @param dataType The data type
	 * @param index The row index.
	 * @param entry The copy number results.
	 */
	void GetCopyNumberEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCopyNumberData &entry);

	/*! Gets the probe set data.
     * @param dataType The data type
	 * @param index The row index.
	 * @param entry The cyto results.
	 */
    void GetCytoEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCytoRegionData &entry);

	/*! Gets the probe set data.
     * @param dataType The data type
	 * @param index The row index.
	 * @param entry The expression results.
	 */
	void GetExpressionEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataExpressionData &entry);

	/*! Gets the call of the probe set.
     * @param dataType The data type
	 * @param index The row index.
	 * @return The call.
	 */
	u_int8_t GetGenoCall(MultiDataType dataType, int index);

	/*! Gets the confidence in the call of the probe set.
     * @param dataType The data type
	 * @param index The row index.
	 * @return The confidence.
	 */
	float GetGenoConfidence(MultiDataType dataType, int index);

    /*! Gets the quantification of the probe set.
     * @param dataType The data type
	 * @param index The row index.
	 * @return The quantification.
	 */
	float GetExpressionQuantification(MultiDataType dataType, int index);

	/*! Get the probe set name.
     * @param dataType The data type
	 * @param index The row index.
	 * @return The probe set name.
	 */
	std::string GetProbeSetName(MultiDataType dataType, int index);

    /*! Returns the data set header.
     * @param dataType The data type.
     */
    DataSetHeader *GetDataSetHeader(MultiDataType dataType);

    /*! Get the length of the metric columns.
     * @param dataType The data type
     * @param col The column index (of the metric columns)
     * @return The length.
     */
    int32_t GetMetricColumnLength(MultiDataType dataType, int col);

    /*! Get the length of the metric columns.
     * @param dataType The data type
     * @return The number of columns.
     */
    int32_t GetNumMetricColumns(MultiDataType dataType);

    /*! Get the metric column name.
     * @param dataType The data type
     * @param colIndex the metric column index
     * @return The column name
     */
    std::wstring GetMetricColumnName(MultiDataType dataType, int colIndex);

private:
    /*! Get the extra metric columns.
     * @param ds The data set info.
     * @param rowIndex The row index.
     * @param colIndex The column index
     * @param metrics The results.
     */
    void GetExtraMetricEntries(DataSetInfo *ds, int rowIndex, int colIndex, std::vector<affymetrix_calvin_parameter::ParameterNameValueType> &metrics);

	/*! Gets the probe set data.
     * @param dataType The data type
	 * @param index The row index.
	 * @param entry The genotype results.
	 */
	void GetGenericGenotypeEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataGenotypeData &entry);

	/*! Gets the probe set data.
     * @param dataType The data type
	 * @param index The row index.
	 * @param entry The copy number results.
	 */
	void GetGenericCopyNumberEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCopyNumberData &entry);

	/*! Gets the probe set data.
     * @param dataType The data type
	 * @param index The row index.
	 * @param entry The cyto region results.
	 */
	void GetGenericCytoRegionEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCytoRegionData &entry);

	/*! Gets the probe set data.
     * @param dataType The data type
	 * @param index The row index.
	 * @param entry The expression results.
	 */
	void GetGenericExpressionEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataExpressionData &entry);

	/*! Opens a group for reading.
     * @param dataType The data type
     */
	DataSetInfo *OpenMultiDataDataSet(MultiDataType dataType);

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
     * @param info The data info
	 * @param hdr The data set header.
	 */
	void AddColumns(DataSetInfo &info, DataSetHeader& hdr);

};

}

#endif
