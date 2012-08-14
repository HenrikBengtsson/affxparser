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

#include "calvin_files/data/src/AllelePeaks.h"
#include "calvin_files/data/src/ChromosomeMultiDataSummaryData.h"
#include "calvin_files/data/src/ChromosomeSegmentMultiDataData.h"
#include "calvin_files/data/src/ColumnInfo.h"
#include "calvin_files/data/src/FamilialMultiDataData.h"
#include "calvin_files/data/src/FileHeader.h"
#include "calvin_files/data/src/GenericData.h"
#include "calvin_files/data/src/MarkerABSignals.h"
#include "calvin_files/data/src/ProbeSetMultiDataData.h"
#include "calvin_files/data/src/CytoGenotypeCallMultiDataData.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <map>
#include <string>
//

namespace affymetrix_calvin_io
{

/*! The identifier to identify a multi-data CHP file. */
#define CHP_MULTI_DATA_TYPE "affymetrix-multi-data-type-analysis"

/*! An enumerant to store the types of data stored in the file. */
typedef enum MultiDataType { 
    ExpressionMultiDataType, 
    ExpressionControlMultiDataType, 
    GenotypeMultiDataType, 
    GenotypeControlMultiDataType, 
    CopyNumberMultiDataType, 
    CytoMultiDataType, 
    CopyNumberVariationMultiDataType,
    DmetCopyNumberMultiDataType,
    DmetMultiAllelicMultiDataType,
    DmetBiAllelicMultiDataType,
    ChromosomeSummaryMultiDataType,
    SegmentCNMultiDataType,
    SegmentLOHMultiDataType,
    SegmentCNNeutralLOHMultiDataType,
    SegmentNormalDiploidMultiDataType,
    SegmentMosaicismMultiDataType,
    SegmentNoCallMultiDataType,
    FamilialSegmentOverlapsMultiDataType,
    FamilialSamplesMultiDataType,
    SegmentGenotypeConcordanceMultiDataType,
    SegmentGenotypeDiscordanceMultiDataType,
    SegmentCNLossLOHConcordanceMultiDataType,
    SegmentCNNeutralLOHConcordanceMultiDataType,
    SegmentHeteroUPDMultiDataType,
    SegmentIsoUPDMultiDataType,
    SegmentDenovoCopyNumberMultiDataType,
    SegmentHemizygousParentOfOriginMultiDataType,
	AllelePeaksMultiDataType,
	MarkerABSignalsMultiDataType,
	CytoGenotypeCallMultiDataType
} MultiDataType_t;

/*! The data set name. */
const static std::wstring MultiDataDataSetNames[] = 
{
	L"Expression",
	L"ExpressionControl",
	L"Genotype",
	L"GenotypeControl",
	L"CopyNumber",
	L"Cyto",
	L"CopyNumberVariation",
	L"DmetCopyNumber",
	L"DmetMultiAllelic",
	L"DmetBiAllelic",
    L"Summary",
    L"CN",
    L"LOH",
    L"CNNeutralLOH",
    L"NormalDiploid",
    L"Mosaicism",
    L"NoCall",
    L"SegmentOverlaps",
	L"Samples",
    L"GenotypeConcordance",
    L"GenotypeDiscordance",
    L"CNLossLOHConcordance",
    L"CNNeutralLOHConcordance",
    L"HeteroUPD",
    L"IsoUPD",
    L"DenovoCopyNumber",
    L"HemizygousParentOfOrigin",
	L"AllelePeaks",
	L"MarkerABSignal",
	L"Calls"
};

/*! The data types. */
const static MultiDataType MultiDataDataTypes[] = 
{
	ExpressionMultiDataType,
	ExpressionControlMultiDataType,
	GenotypeMultiDataType,
	GenotypeControlMultiDataType,
	CopyNumberMultiDataType,
	CytoMultiDataType,
	CopyNumberVariationMultiDataType,
	DmetCopyNumberMultiDataType,
	DmetMultiAllelicMultiDataType,
	DmetBiAllelicMultiDataType,
    ChromosomeSummaryMultiDataType,
    SegmentCNMultiDataType,
    SegmentLOHMultiDataType,
    SegmentCNNeutralLOHMultiDataType,
    SegmentNormalDiploidMultiDataType,
    SegmentMosaicismMultiDataType,
    SegmentNoCallMultiDataType,
    FamilialSegmentOverlapsMultiDataType,
	FamilialSamplesMultiDataType,
    SegmentGenotypeConcordanceMultiDataType,
    SegmentGenotypeDiscordanceMultiDataType,
    SegmentCNLossLOHConcordanceMultiDataType,
    SegmentCNNeutralLOHConcordanceMultiDataType,
    SegmentHeteroUPDMultiDataType,
    SegmentIsoUPDMultiDataType,
    SegmentDenovoCopyNumberMultiDataType,
    SegmentHemizygousParentOfOriginMultiDataType,
	AllelePeaksMultiDataType,
	MarkerABSignalsMultiDataType,
	CytoGenotypeCallMultiDataType
};

/*! Holds data set information. */
class DataSetInfo
{
public:
	/*! The data type. */
	MultiDataType dataType;

	/*! chp data sets */
	DataSet* entries;

	/*! The maximum length of the name column. */
	int maxName;

    /*! The maximum length of the familial seg type column. */
    int maxSegmentType;

    /*! The maximum length of the familial ref seg id column. */
    int maxReferenceSegmentID;

    /*! The maximum length of the familial seg id column. */
    int maxFamilialSegmentID;

	/*! The maximum length of the familial sample ARR id */
	int maxFamilialARRID;

	/*! The maximum length of the familial sample CHP id */
	int maxFamilialCHPID;

	/*! The maximum length of the familial sample CHP file name */
	int maxFamilialCHPFile;

	/*! The maximum length of the familial sample role */
	int maxFamilialRole;

	/*! The data set index. */
	int dataSetIndex;

	/*! An array of extra metric columns. */
	std::vector<ColumnInfo> metricColumns;

	/*! constructor */
	DataSetInfo();
};

/*! Holds data associated with genotype or expression CHP files. */
class CHPMultiDataData
{
public:
	/*! Constructor */
	CHPMultiDataData();

	/*! Constructor with file name.
	* @param filename The name of the CHP file.
    * @param groupNames A list of names for the data groups.
	*/
    CHPMultiDataData(const std::string& filename, const std::list<std::wstring> *groupNames = NULL);

	/*! Destructor */
	~CHPMultiDataData();

private:

	/*! The generic data item. */
	GenericData genericData;

	/*! chp data sets */
	std::map<MultiDataType, DataSetInfo> dataSetInfo;

	/*! data groups */
    std::map<MultiDataType, std::wstring> dataTypeGroupNames;

public:

    std::wstring GetGroupName(MultiDataType dataType);

	/*! The data set information */
    std::map<MultiDataType, DataSetInfo> &GetDataSetInfo();

	/*! The maximum length of a probe set name.
	* @param dataType The data type
	* @return The maximum probe set name length
	*/
	int GetMaxProbeSetName(MultiDataType dataType);

	/*! The maximum length of a segment id.
	* @param dataType The data type
	* @return The maximum length
	*/
    int GetMaxSegmentId(MultiDataType dataType);

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
    * @param groupName The group name
	*/
    void SetEntryCount(MultiDataType dataType, int32_t ln, int32_t maxln, const std::vector<ColumnInfo> &columns, const std::wstring &groupName = L"");

	/*! Sets the number of entries (probe sets).
	* @param dataType The data type
	* @param ln The number of probe sets.
	* @param maxln The maximum length of a probe set name.
    * @param groupName The group name
	*/
	void SetEntryCount(MultiDataType dataType, int32_t ln, int32_t maxln, const std::wstring &groupName = L"");

	/*! Sets the number of entries specifically for the familial file.
	* @param dataType The data type
	* @param ln The number of probe sets.
    * @param segmentTypeMax The maximum length of the familial seg type column
    * @param referenceSegmentIDMax The maximum length of the familial ref seg id column
    * @param familialSegmentIDMax The maximum length of the familial seg id column
    * @param groupName The group name
	*/
    void SetEntryCount(MultiDataType dataType, int32_t ln, int segmentTypeMax, int referenceSegmentIDMax, int familialSegmentIDMax, const std::wstring &groupName = L"");

    /*! Sets the number of entries specifically for the familial file.
	* @param dataType The data type
	* @param ln The number of probe sets.
    * @param familialSegmentIDMax The maximum length of the familial seg id column.
    * @param familialARRIDMax The maximum length of the familial sample ARR id.
    * @param familialCHPIDMax The maximum length of the familial sample CHP id.
    * @param familialCHPFile The maximum length of the familial sample CHP file name.
    * @param familialRole The maximum length of the familial sample role
    * @param groupName The group name
	*/
    void SetEntryCount(MultiDataType dataType, int32_t ln, int familialARRIDMax, int familialCHPIDMax, int familialCHPFileMax, int familialRoleMax, const std::wstring &groupName = L"");

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

	/*! Adds the application meta data information
	* @param params The application meta data
	*/
        void AddAppMetaInfo(const ParameterNameValueTypeList& params);

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
	FileHeader* GetFileHeader(); 

	/*! Gets the generic data object.
	* @return The data object.
	*/
	GenericData& GetGenericData();

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
	* @param entry The copy number results.
	*/
	void GetEntry(MultiDataType dataType, int index, affymetrix_calvin_data::DmetCopyNumberData &entry);

	/*! Gets the probe set data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The copy number results.
	*/
	void GetEntry(MultiDataType dataType, int index, affymetrix_calvin_data::DmetMultiAllelicData &entry);

	/*! Gets the probe set data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The copy number results.
	*/
	void GetEntry(MultiDataType dataType, int index, affymetrix_calvin_data::DmetBiAllelicData &entry);

	/*! Gets the allele peak data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The results.
	*/
	void GetEntry(MultiDataType dataType, int index, affymetrix_calvin_data::AllelePeaks &entry);

	/*! Gets the marker AB signal data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The results.
	*/
	void GetEntry(MultiDataType dataType, int index, affymetrix_calvin_data::MarkerABSignals &entry);

	/*! Gets the genotype data for cyto.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The results.
	*/
	void GetEntry(MultiDataType dataType, int index, affymetrix_calvin_data::CytoGenotypeCallData &entry);

	/*! Gets the probe set data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The copy number results.
	*/
	void GetCopyNumberEntryLog2Ratio(MultiDataType dataType, int index, float *val);

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

	/*! Gets the probe set data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The copy number variation results.
	*/
	void GetCopyNumberVariationEntry(MultiDataType dataType, int index, 
		affymetrix_calvin_data::ProbeSetMultiDataCopyNumberVariationRegionData &entry);

	/*! Gets the chromosome segment data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The results.
	*/
	void GetChromosomeSegmentEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ChromosomeSegmentData &entry);

	/*! Gets the chromosome segment data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The results.
	*/
	void GetChromosomeSegmentEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ChromosomeSegmentDataEx &entry);

	/*! Gets the chromosome summary data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The results.
	*/
	void GetChromosomeSummaryEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ChromosomeMultiDataSummaryData &entry);

	/*! Gets the familial file entry.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The results.
	*/
	void GetFamilialSegmentOverlapEntry(MultiDataType dataType, int index, affymetrix_calvin_data::FamilialSegmentOverlap &entry);
	
	/*! Gets the familial file entry.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The results.
	*/
	void GetFamilialSampleEntry(MultiDataType dataType, int index, affymetrix_calvin_data::FamilialSample &entry);

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

	/*! Returns the data group header.
	* @param name The name of the group.
	*/
    DataGroupHeader *GetDataGroupHeader(const std::wstring &name);

	/*! Returns the data group index.
	* @param dataType The data type.
	*/
    int GetDataGroupIndex(MultiDataType dataType);

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
	/*! Get the extra metric columns.
	* @param ds The data set info.
	* @param rowIndex The row index.
	* @param colIndex The column index
	* @param metrics The results.
	*/
	void GetExtraCopyNumberFloatTypeNoNameLog2Ratio(DataSetInfo *ds, int rowIndex, float *val);

	/*! Gets the probe set data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The genotype results.
	*/
	void GetGenericEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataGenotypeData &entry);

	/*! Gets the probe set data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The copy number results.
	*/
	void GetGenericEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCopyNumberData &entry);

	/*! Gets the probe set data (log2Ratio only).
	* @param dataType The data type
	* @param index The row index.
	* @param val The copy number result (log2Ratio).
	*/
	void GetGenericCopyNumberEntryLog2Ratio(MultiDataType dataType, int index, float *val);

	/*! Gets the probe set data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The cyto region results.
	*/
	void GetGenericEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCytoRegionData &entry);

	/*! Gets the probe set data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The expression results.
	*/
	void GetGenericEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataExpressionData &entry);

	/*! Gets the probe set data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The copy number variation region results.
	*/
	void GetGenericEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCopyNumberVariationRegionData &entry);

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
