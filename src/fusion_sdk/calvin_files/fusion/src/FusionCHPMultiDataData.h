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


#ifndef _FusionCHPMultiDataData_HEADER_
#define _FusionCHPMultiDataData_HEADER_

/*! \file FusionCHPMultiDataData.h This file provides CHP file reading capabilities for multi-data CHP files.*/

#include "calvin_files/data/src/CHPMultiDataData.h"
#include "calvin_files/fusion/src/FusionCHPData.h"
//

namespace affymetrix_fusion_io
{

/*! This class provides storage and reading capabilities for multi-data CHP files */
class FusionCHPMultiDataData : public FusionCHPData
{
private:

	/*! Constructor */
	FusionCHPMultiDataData();

	/*! The CHP object. */
	affymetrix_calvin_io::CHPMultiDataData chpData;

public:

	/*! Destructor */
	~FusionCHPMultiDataData();

	/*! Get the id of the file (only valid for Command Console "calvin" files)
	 * @return The unique file id.
	 */
    affymetrix_calvin_utilities::AffymetrixGuidType FileId() { return chpData.GetGenericData().FileIdentifier(); }

	/*! Returns the GenericData object associated with a Calvin file, NULL for GCOS files. */
    affymetrix_calvin_io::GenericData *GetGenericData() { return &chpData.GetGenericData(); }

    /*! Returns the data set header.
     * @param dataType The data type.
     */
    affymetrix_calvin_io::DataSetHeader *GetDataSetHeader(affymetrix_calvin_io::MultiDataType dataType) { return chpData.GetDataSetHeader(dataType); }

    /*! The data set information */
    std::map<affymetrix_calvin_io::MultiDataType, affymetrix_calvin_io::DataSetInfo> &GetDataSetInfo() { return chpData.GetDataSetInfo(); }

	/*! The maximum length of a probe set name.
     * @param dataType The data type
     * @return The maximum probe set name length
     */
    int GetMaxProbeSetName(affymetrix_calvin_io::MultiDataType dataType) { return chpData.GetMaxProbeSetName(dataType); }

	/*! Clears the members. */
    void Clear() { chpData.Clear(); }

	/*! Gets the class name. */
	affymetrix_calvin_utilities::AffymetrixGuidType GetObjectName();

	/*! Gets the name of the algorithm.
	 * @return The algorithm name.
	 */
	std::wstring GetAlgName() { return chpData.GetAlgName(); }

	/*! Gets the algorithm version.
	 * @return The version.
	 */
	std::wstring GetAlgVersion() { return chpData.GetAlgVersion(); }

	/*! Sets the array type */
	std::wstring GetArrayType() { return chpData.GetArrayType(); }

	/*! Gets the algorithm parameters
	 * @return The algoirhtm parameters.
	 */
	ParameterNameValueTypeList GetAlgParams() { return chpData.GetAlgParams(); }

	/*! Gets the summary parameters
	 * @return The summary parameters.
	 */
	ParameterNameValueTypeList GetSummaryParams() { return chpData.GetSummaryParams(); }

	/*! Gets the number of entries (probe sets)
     * @param dataType The data type
     */
	int32_t GetEntryCount(affymetrix_calvin_io::MultiDataType dataType) { return chpData.GetEntryCount(dataType); }

	/*! Gets the probe set data.
     * @param dataType The data type
	 * @param index The row index.
	 * @param entry The genotype results.
	 */
    void GetGenotypeEntry(affymetrix_calvin_io::MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataGenotypeData &entry)
    { chpData.GetGenotypeEntry(dataType, index, entry); } 

	/*! Gets the probe set data.
     * @param dataType The data type
	 * @param index The row index.
	 * @param entry The copy number results.
	 */
    void GetCopyNumberEntry(affymetrix_calvin_io::MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCopyNumberData &entry)
    { chpData.GetCopyNumberEntry(dataType, index, entry); } 

	/*! Gets the probe set data.
     * @param dataType The data type
	 * @param index The row index.
	 * @param entry The copy number results.
	 */
    void GetCopyNumberEntryLog2Ratio(affymetrix_calvin_io::MultiDataType dataType, int index, int log2RatioIndex, float *val)
    { chpData.GetCopyNumberEntryLog2Ratio(dataType, index, val); } 

	/*! Gets the cyto region data.
     * @param dataType The data type
	 * @param index The row index.
	 * @param entry The cyto region results.
	 */
    void GetCytoRegionEntry(affymetrix_calvin_io::MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCytoRegionData &entry)
    { chpData.GetCytoEntry(dataType, index, entry); } 

    /*! Gets the copy number variation region data.
     * @param dataType The data type
	 * @param index The row index.
	 * @param entry The cyto region results.
	 */
    void GetCopyNumberVariationRegionEntry(affymetrix_calvin_io::MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCopyNumberVariationRegionData &entry)
    { chpData.GetCopyNumberVariationEntry(dataType, index, entry); } 

	/*! Gets the probe set data.
     * @param dataType The data type
	 * @param index The row index.
	 * @param entry The expression results.
	 */
	void GetExpressionEntry(affymetrix_calvin_io::MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataExpressionData &entry)
    { chpData.GetExpressionEntry(dataType, index, entry); }

	/*! Gets the call of the probe set.
     * @param dataType The data type
	 * @param index The row index.
	 * @return The call.
	 */
    u_int8_t GetGenoCall(affymetrix_calvin_io::MultiDataType dataType, int index) { return chpData.GetGenoCall(dataType, index); }

	/*! Gets the confidence in the call of the probe set.
     * @param dataType The data type
	 * @param index The row index.
	 * @return The confidence.
	 */
    float GetGenoConfidence(affymetrix_calvin_io::MultiDataType dataType, int index) { return chpData.GetGenoConfidence(dataType, index); }

    /*! Gets the quantification of the probe set.
     * @param dataType The data type
	 * @param index The row index.
	 * @return The quantification.
	 */
    float GetExpressionQuantification(affymetrix_calvin_io::MultiDataType dataType, int index) { return chpData.GetExpressionQuantification(dataType, index); }

	/*! Gets the chromosome segment data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The results.
	*/
	void GetChromosomeSegmentEntry(affymetrix_calvin_io::MultiDataType dataType, int index, affymetrix_calvin_data::ChromosomeSegmentData &entry)
    { chpData.GetChromosomeSegmentEntry(dataType, index, entry); }

	/*! Gets the chromosome segment data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The results.
	*/
	void GetChromosomeSegmentEntry(affymetrix_calvin_io::MultiDataType dataType, int index, affymetrix_calvin_data::ChromosomeSegmentDataEx &entry)
    { chpData.GetChromosomeSegmentEntry(dataType, index, entry); }

	/*! Gets the chromosome summary data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The results.
	*/
	void GetChromosomeSummaryEntry(affymetrix_calvin_io::MultiDataType dataType, int index, affymetrix_calvin_data::ChromosomeMultiDataSummaryData &entry)
    { chpData.GetChromosomeSummaryEntry(dataType, index, entry); }
    
	/*! Gets the familial file entry.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The results.
	*/
	void GetFamilialSegmentOverlapEntry(affymetrix_calvin_io::MultiDataType dataType, int index, affymetrix_calvin_data::FamilialSegmentOverlap &entry)
    { chpData.GetFamilialSegmentOverlapEntry(dataType, index, entry); }

	/*! Gets the familial file entry.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The results.
	*/
	void GetFamilialSampleEntry(affymetrix_calvin_io::MultiDataType dataType, int index, affymetrix_calvin_data::FamilialSample &entry)
	{ chpData.GetFamilialSampleEntry(dataType, index, entry); }

	/*! Gets the DMET probe set data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The copy number results.
	*/
	void GetDmetCopyNumberEntry(affymetrix_calvin_io::MultiDataType dataType, int index, affymetrix_calvin_data::DmetCopyNumberData &entry)
	{ chpData.GetEntry(dataType, index, entry); }

	/*! Gets the DMET probe set data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The copy number results.
	*/
	void GetDmetMultiAllelicEntry(affymetrix_calvin_io::MultiDataType dataType, int index, affymetrix_calvin_data::DmetMultiAllelicData &entry)
	{ chpData.GetEntry(dataType, index, entry); }

	/*! Gets the DMET probe set data.
	* @param dataType The data type
	* @param index The row index.
	* @param entry The copy number results.
	*/
	void GetDmetBiAllelicEntry(affymetrix_calvin_io::MultiDataType dataType, int index, affymetrix_calvin_data::DmetBiAllelicData &entry)
	{ chpData.GetEntry(dataType, index, entry); }

	/*! Gets the probe set data.
     * @param dataType The data type
	 * @param index The row index.
	 * @param entry The copy number results.
	 */
	void GetAllelePeakEntry(affymetrix_calvin_io::MultiDataType dataType, int index, affymetrix_calvin_data::AllelePeaks &entry)
    { chpData.GetEntry(dataType, index, entry); } 

	/*! Gets the probe set data.
     * @param dataType The data type
	 * @param index The row index.
	 * @param entry The AB signals.
	 */
	void GetMarkerABSignalsEntry(affymetrix_calvin_io::MultiDataType dataType, int index, affymetrix_calvin_data::MarkerABSignals &entry)
    { chpData.GetEntry(dataType, index, entry); } 

	/*! Gets the probe set data.
     * @param dataType The data type
	 * @param index The row index.
	 * @param entry The copy number results.
	 */
	void GetCytoGenotypeEntry(affymetrix_calvin_io::MultiDataType dataType, int index, affymetrix_calvin_data::CytoGenotypeCallData &entry)
    { chpData.GetEntry(dataType, index, entry); } 

	/*! Get the probe set name.
     * @param dataType The data type
	 * @param index The row index.
	 * @return The probe set name.
	 */
    std::string GetProbeSetName(affymetrix_calvin_io::MultiDataType dataType, int index) { return chpData.GetProbeSetName(dataType, index); }

    /*! Get the length of the metric columns.
     * @param dataType The data type
     * @param col The column index (of the metric columns)
     * @return The length.
     */
    int32_t GetMetricColumnLength(affymetrix_calvin_io::MultiDataType dataType, int col) { return chpData.GetMetricColumnLength(dataType, col); }

    /*! Get the length of the metric columns.
     * @param dataType The data type
     * @return The number of columns.
     */
    int32_t GetNumMetricColumns(affymetrix_calvin_io::MultiDataType dataType) { return chpData.GetNumMetricColumns(dataType); }

    /*! Get the metric column name.
     * @param dataType The data type
     * @param colIndex the metric column index
     * @return The column name
     */
    std::wstring GetMetricColumnName(affymetrix_calvin_io::MultiDataType dataType, int colIndex) { return chpData.GetMetricColumnName(dataType, colIndex); }

private:

	/*! Reads the CHP file.
	 * @return True if successful.
	 */
	bool Read();

	/*! Reads the header of the CHP file
	 * @return True if successful
	 */
	bool ReadHeader();

public:
	/*! A class to register the MultiData CHP reader. */
	class Reg : public FusionCHPDataReg
	{
	public:
		/*! Constructor - register the MultiData file type. */
		Reg() : FusionCHPDataReg()
		{
			std::list<std::string> ids;
			ids.push_back(CHP_MULTI_DATA_TYPE);
			SetFileTypeIds(ids);
		}

		/*! Creates a MultiData CHP object.
		 * @return The MultiData CHP object.
		 */
		FusionCHPData *MakeObject() { return new FusionCHPMultiDataData; }
	};

public:
	/*! The one and only registration object. This registers the class as a CHP reader. */
	static Reg reg;

	/*! Converts the type to the MultiData CHP type.
	 * @param chip The pointer to the CHP data object.
	 * @return The MultiData CHP data type or NULL if not compatible.
	 */
	static FusionCHPMultiDataData * FromBase(FusionCHPData *chip);
};

}

#endif
