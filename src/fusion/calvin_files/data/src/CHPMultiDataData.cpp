////////////////////////////////////////////////////////////////
//
// Copyright (C) 2009 Affymetrix, Inc.
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

#include "calvin_files/data/src/CHPMultiDataData.h"
//
#include "calvin_files/data/src/DataSetHeader.h"
#include "calvin_files/utils/src/StringUtils.h"
//

using namespace std;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_utilities;

/*! The data set name. */
const static std::wstring MULTI_DATA_NAME = L"MultiData";

/*! The column name for the probe set name. */
const static std::wstring PROBE_SET_NAME = L"ProbeSetName";

/*! The column name for the cyto region. */
const static std::wstring CYTO_REGION_NAME = L"Region";

/*! The column name for the call. */
const static std::wstring GENOTYPE_CALL_NAME = L"Call";

/*! The column name for the confidence value. */
const static std::wstring GENOTYPE_CONFIDENCE_NAME = L"Confidence";

/*! The column name for the quantification value. */
const static std::wstring EXPRESSION_QUANTIFICATION_NAME = L"Quantification";

/*! The column name for the chromosome name. */
const static std::wstring COPY_NUMBER_CHR_NAME = L"Chromosome";

/*! The column name for the physical position of the SNP. */
const static std::wstring COPY_NUMBER_POSITION_NAME = L"Position";

/*! The column name for the call value. */
const static std::wstring CYTO_CALL_NAME = L"Call";

/*! The column name for the confidence value. */
const static std::wstring CYTO_CONFIDENCE_NAME = L"Confidence";

/*! The column name for the chromosome name. */
const static std::wstring CYTO_CHR_NAME = L"Chromosome";

/*! The column name for the physical position of the cyto region. */
const static std::wstring CYTO_START_POSITION_NAME = L"StartPosition";

/*! The column name for the physical position of the cyto region. */
const static std::wstring CYTO_STOP_POSITION_NAME = L"StopPosition";

/*! The column name for the copy number variation region. */
const static std::wstring COPY_NUMBER_VARIATION_REGION_NAME = L"Region";

/*! The column name for the copy number variation region. */
const static std::wstring COPY_NUMBER_VARIATION_SIGNAL_NAME = L"Signal";

/*! The column name for the copy number variation call. */
const static std::wstring COPY_NUMBER_VARIATION_CALL_NAME = L"Call";

/*! The column name for the copy number variation confidence value. */
const static std::wstring COPY_NUMBER_VARIATION_CONFIDENCE_NAME = L"Confidence";

/*! The column name for the region. */
const static std::wstring REGION = L"Region";

/*! The column name for the call. */
const static std::wstring CALL = L"Call";

/*! The column name for the call. */
const static std::wstring CN_CALL = L"CN Call";

/*! The column name for the confidence value. */
const static std::wstring CONFIDENCE = L"Confidence";

/*! The column name for the confidence value. */
const static std::wstring CN_CONFIDENCE = L"CN Confidence";

/*! The column name for the quantification value. */
const static std::wstring QUANTIFICATION = L"Quantification";

/*! The column name for the chromosome name. */
const static std::wstring CHR = L"Chromosome";

/*! The column name for the physical position of the SNP. */
const static std::wstring POSITION = L"Position";

/*! The column name for the physical position of the cyto region. */
const static std::wstring START_POSITION = L"StartPosition";

/*! The column name for the physical position of the cyto region. */
const static std::wstring STOP_POSITION = L"StopPosition";

/*! The column name for the copy number variation region. */
const static std::wstring SIGNAL = L"Signal";

/*! The column name for the force. */
const static std::wstring FORCE = L"Forced Call";

/*! The column name for the force. */
const static std::wstring CN_FORCE = L"CN_force";

/*! The column name for the estimate. */
const static std::wstring CN_ESTIMATE = L"CN_estim";

/*! The column name for the lower. */
const static std::wstring CN_LOWER = L"CN_lower";

/*! The column name for the upper. */
const static std::wstring CN_UPPER = L"CN_upper";

/*! The column name for the allele count. */
const static std::wstring ALLELE_COUNT = L"Allele Count";

/*! The column name for the mean marker distance. */
const static std::wstring MEAN_MARKER_DISTANCE = L"MeanMarkerDistance";

/*! The column name for signals. */
const static std::wstring SIGNAL_A = L"Signal A";
const static std::wstring SIGNAL_B = L"Signal B";
const static std::wstring SIGNAL_C = L"Signal C";
const static std::wstring SIGNAL_D = L"Signal D";
const static std::wstring SIGNAL_E = L"Signal E";
const static std::wstring SIGNAL_F = L"Signal F";
/*! The column name for contexts. */
const static std::wstring CONTEXT_A = L"Context A";
const static std::wstring CONTEXT_B = L"Context B";
const static std::wstring CONTEXT_C = L"Context C";
const static std::wstring CONTEXT_D = L"Context D";
const static std::wstring CONTEXT_E = L"Context E";
const static std::wstring CONTEXT_F = L"Context F";

/*! Columns for the chromosome display */
const static std::wstring CHR_DISPLAY = L"Display";

/*! Columns for chromosome summary - start index.  */
const static std::wstring START_INDEX = L"StartIndex";

/*! Columns for chromosome summary - min signal.  */
const static std::wstring MIN_SIGNAL = L"MinSignal";

/*! Columns for chromosome summary - max signal.  */
const static std::wstring MAX_SIGNAL = L"MaxSignal";

/*! Columns for chromosome summary - median cn state.  */
const static std::wstring MEDIAN_CN_STATE = L"MedianCnState";

/*! Columns for chromosome summary - hom frequency.  */
const static std::wstring HOM_FREQ = L"HomFrequency";

/*! Columns for chromosome summary - het frequency.  */
const static std::wstring HET_FREQ = L"HetFrequency";

/*! Columns for chromosome summary - mosaicism.  */
const static std::wstring MOSAICISM = L"Mosaicism";

/*! The segment id tag. */
const static std::wstring SEGMENT_ID = L"SegmentID";

/*! The marker count tag. */
const static std::wstring MARKER_COUNT = L"MarkerCount";

/*! The mixture tag. */
const static std::wstring MIXTURE = L"Mixture";

/*! The calibrated CN tag. */
const static std::wstring CALIBRATED_CN = L"CalibratedCN";

/*! Familial tags - segment type. */
const static std::wstring SEGMENT_TYPE = L"SegmentType";

/*! Familial tags - reference sample key. */
const static std::wstring REFERENCE_SAMPLE_KEY = L"ReferenceSampleKey";

/*! Familial tags - reference segment id. */
const static std::wstring REFERENCE_SEGMENT_ID = L"ReferenceSegmentID";

/*! Familial tags - sample key. */
const static std::wstring FAMILIAL_SAMPLE_KEY = L"FamilialSampleKey";

/*! Familial tags - sample id. */
const static std::wstring FAMILIAL_SEGMENT_ID = L"FamilialSegmentID";

const static std::wstring SAMPLE_KEY = L"SampleKey";
const static std::wstring ARRID = L"ARRID";
const static std::wstring CHPID = L"CHPID";
const static std::wstring CHP_FILENAME = L"CHPFilename";
const static std::wstring ROLE = L"Role";
const static std::wstring ROLE_VALIDITY = L"RoleValidity";
const static std::wstring ROLE_CONFIDENCE = L"RoleConfidence";
const static std::wstring CN = L"CN";
const static std::wstring LOH = L"LOH";
const static std::wstring CNNEUTRALLOH = L"CNNeutralLOH";
const static std::wstring NORMALDIPLOID = L"NormalDiploid";
const static std::wstring NOCALL = L"NoCall";
const static std::wstring HOMOZYGOSITY = L"Homozygosity";
const static std::wstring HETEROZYGOSITY = L"Heterozygosity";

const static std::wstring PROBE_SET_INDEX = L"Index";
const static std::wstring A_SIGNAL = L"ASignal";
const static std::wstring B_SIGNAL = L"BSignal";
const static std::wstring SCAR = L"SCAR";

const static std::wstring CONTRAST = L"Contrast";
const static std::wstring SIGNAL_STRENGTH = L"SignalStrength";

	/*! constructor */
DataSetInfo::DataSetInfo()    {
  entries = NULL;
  maxName = -1;
  maxSegmentType = -1;
  maxReferenceSegmentID = -1;
  maxFamilialSegmentID = -1;
  maxFamilialARRID = -1;
  maxFamilialCHPID = -1;
  maxFamilialCHPFile = -1;
  maxFamilialRole = -1;
  maxFamilialCHPFile = -1;
  dataSetIndex = -1;
}

std::wstring CHPMultiDataData::GetGroupName(MultiDataType dataType) {
  return dataTypeGroupNames[dataType];
}

/*! The data set information */
std::map<MultiDataType, DataSetInfo> &CHPMultiDataData::GetDataSetInfo() {
  return dataSetInfo;
}

/*! used for full column index for log2Ratio */
const static int cnlog2RatioIndexOffset = 4;

CHPMultiDataData::CHPMultiDataData()
{
	Clear();
}

CHPMultiDataData::CHPMultiDataData(const std::string& filename, const std::list<std::wstring> *groupNames) 
{ 
	Clear();
	SetFilename(filename);
    if (groupNames == NULL)
    {
	    DataGroupHeader dcHdr(MULTI_DATA_NAME);
	    genericData.Header().AddDataGroupHdr(dcHdr);
        genericData.Header().SetNumDataGroups(1);
    }
    else
    {
        std::list<std::wstring>::const_iterator it;
        for (it = groupNames->begin(); it != groupNames->end(); it++)
        {
            DataGroupHeader dcHdr(*it);
            genericData.Header().AddDataGroupHdr(dcHdr);
        }
        genericData.Header().SetNumDataGroups((int)groupNames->size());
    }
	genericData.Header().GetGenericDataHdr()->SetFileTypeId(CHP_MULTI_DATA_TYPE);
}

CHPMultiDataData::~CHPMultiDataData() 
{ 
	Clear(); 
}

DataSetHeader *CHPMultiDataData::GetDataSetHeader(MultiDataType dataType)
{
    int ng = genericData.Header().GetNumDataGroups();
    for (int ig=0; ig<ng; ig++)
    {
        DataGroupHeader &dh = genericData.Header().GetDataGroup(ig);
	    int n = dh.GetDataSetCnt();
	    for (int i=0; i<n; i++)
	    {
		    DataSetHeader &h = dh.GetDataSet(i);
		    if (h.GetName() == MultiDataDataSetNames[(int)dataType])
			    return &h;
	    }
    }
	return NULL;
}

void CHPMultiDataData::Clear()
{
	for (map<MultiDataType, DataSetInfo>::iterator it=dataSetInfo.begin(); it!=dataSetInfo.end(); ++it)
	{
		DataSetInfo &info = it->second;
		info.metricColumns.clear();
		if (info.entries){ info.entries->Delete();  info.entries = 0; }
	}
	dataSetInfo.clear();
    dataTypeGroupNames.clear();
	genericData.Header().Clear();
}

/*! Gets the file header.
 * @return The file header.
 */
FileHeader* CHPMultiDataData::GetFileHeader() {
  return &genericData.Header();
}

/*! Gets the generic data object.
 * @return The data object.
 */
GenericData& CHPMultiDataData::GetGenericData() {
  return genericData;
}

void CHPMultiDataData::SetFilename(const std::string &p)
{
	genericData.Header().SetFilename(p);
}

std::string CHPMultiDataData::GetFilename() const
{
	return ((GenericData&)genericData).Header().GetFilename();
}

int32_t CHPMultiDataData::GetMetricColumnLength(MultiDataType dataType, int col)
{
	OpenMultiDataDataSet(dataType);
	return dataSetInfo[dataType].metricColumns[col].GetLength();
}

int32_t CHPMultiDataData::GetNumMetricColumns(MultiDataType dataType)
{
	OpenMultiDataDataSet(dataType);
	return (int32_t)dataSetInfo[dataType].metricColumns.size();
}

wstring CHPMultiDataData::GetMetricColumnName(MultiDataType dataType, int colIndex)
{
	OpenMultiDataDataSet(dataType);
	return dataSetInfo[dataType].metricColumns[colIndex].GetName();
}

int32_t CHPMultiDataData::GetEntryCount(MultiDataType dataType)
{
	DataSetHeader *h = GetDataSetHeader(dataType);
	return (h == NULL ? 0 : h->GetRowCnt());
}

int CHPMultiDataData::GetMaxProbeSetName(MultiDataType dataType)
{
	OpenMultiDataDataSet(dataType);
	map<MultiDataType, DataSetInfo>::iterator pos = dataSetInfo.find(dataType);
	if (pos != dataSetInfo.end())
	{
		return dataSetInfo[dataType].maxName;
	}
	return 0;
}

int CHPMultiDataData::GetMaxSegmentId(MultiDataType dataType)
{
	OpenMultiDataDataSet(dataType);
	map<MultiDataType, DataSetInfo>::iterator pos = dataSetInfo.find(dataType);
	if (pos != dataSetInfo.end())
	{
        return dataSetInfo[dataType].maxName;
	}
	return 0;
}

int CHPMultiDataData::GetDataGroupIndex(MultiDataType dataType)
{
    const std::wstring &name = dataTypeGroupNames[dataType];
    int ng = genericData.Header().GetNumDataGroups();
    for (int ig=0; ig<ng; ig++)
    {
        DataGroupHeader &dh = genericData.Header().GetDataGroup(ig);
	    if (dh.GetName() == name)
            return ig;
    }
    return -1;
}

DataGroupHeader *CHPMultiDataData::GetDataGroupHeader(const std::wstring &name)
{
    int ng = genericData.Header().GetNumDataGroups();
    for (int ig=0; ig<ng; ig++)
    {
        DataGroupHeader &dh = genericData.Header().GetDataGroup(ig);
	    if (dh.GetName() == name)
            return &genericData.Header().GetDataGroup(ig);
    }
    return NULL;
}

void CHPMultiDataData::SetEntryCount(MultiDataType dataType, int32_t ln, int32_t maxln, const std::vector<ColumnInfo> &columns, const std::wstring &groupName)
{
	DataSetInfo info;
	info.maxName = maxln;
	info.metricColumns = columns;
	info.entries = NULL;
	info.dataType = dataType;
	info.dataSetIndex = (int)dataSetInfo.size();
	dataSetInfo[dataType] = info;

	DataSetHeader dsHdr;
	dsHdr.SetRowCnt(ln);
	dsHdr.SetName(MultiDataDataSetNames[(int)dataType]);
	AddColumns(info, dsHdr);

    if (groupName.empty() == true)
        dataTypeGroupNames[dataType] = MULTI_DATA_NAME;
    else
        dataTypeGroupNames[dataType] = groupName;
	DataGroupHeader* dgHdr = GetDataGroupHeader(dataTypeGroupNames[dataType]);
	dgHdr->AddDataSetHdr(dsHdr);
}

void CHPMultiDataData::SetEntryCount(MultiDataType dataType, int32_t ln, int32_t maxln, const std::wstring &groupName)
{
	std::vector<ColumnInfo> empty;
	SetEntryCount(dataType, ln, maxln, empty, groupName);
}

void CHPMultiDataData::SetEntryCount(MultiDataType dataType, int32_t ln, int segmentTypeMax, int referenceSegmentIDMax, int familialSegmentIDMax, const std::wstring &groupName)
{
	DataSetInfo info;
    info.maxSegmentType = segmentTypeMax;
    info.maxReferenceSegmentID = referenceSegmentIDMax;
    info.maxFamilialSegmentID = familialSegmentIDMax;
	info.entries = NULL;
	info.dataType = dataType;
	info.dataSetIndex = (int)dataSetInfo.size();
	dataSetInfo[dataType] = info;

	DataSetHeader dsHdr;
	dsHdr.SetRowCnt(ln);
	dsHdr.SetName(MultiDataDataSetNames[(int)dataType]);
	AddColumns(info, dsHdr);

    if (groupName.empty() == true)
        dataTypeGroupNames[dataType] = MULTI_DATA_NAME;
    else
        dataTypeGroupNames[dataType] = groupName;
	DataGroupHeader* dgHdr = GetDataGroupHeader(dataTypeGroupNames[dataType]);
	dgHdr->AddDataSetHdr(dsHdr);
}

void CHPMultiDataData::SetEntryCount(MultiDataType dataType, int32_t ln, int familialARRIDMax, int familialCHPIDMax, int familialCHPFileMax, int familialRoleMax, const std::wstring &groupName)
{
	DataSetInfo info;
    info.maxFamilialARRID = familialARRIDMax;
    info.maxFamilialCHPID = familialCHPIDMax;
    info.maxFamilialCHPFile = familialCHPFileMax;
    info.maxFamilialRole = familialRoleMax;
	info.entries = NULL;
	info.dataType = dataType;
	info.dataSetIndex = (int)dataSetInfo.size();
	dataSetInfo[dataType] = info;

	DataSetHeader dsHdr;
	dsHdr.SetRowCnt(ln);
	dsHdr.SetName(MultiDataDataSetNames[(int)dataType]);
	AddColumns(info, dsHdr);

    if (groupName.empty() == true)
        dataTypeGroupNames[dataType] = MULTI_DATA_NAME;
    else
        dataTypeGroupNames[dataType] = groupName;
	DataGroupHeader* dgHdr = GetDataGroupHeader(dataTypeGroupNames[dataType]);
	dgHdr->AddDataSetHdr(dsHdr);
}

void CHPMultiDataData::GetGenotypeEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataGenotypeData &entry)
{
	GetGenericEntry(dataType, index, entry);
}

void CHPMultiDataData::GetCopyNumberEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCopyNumberData &entry)
{
	GetGenericEntry(dataType, index, entry);
}

void CHPMultiDataData::GetEntry(MultiDataType dataType, int index, affymetrix_calvin_data::DmetCopyNumberData &entry)
{
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
	{
		int colIndex = 0;
		entry.name.clear();
		ds->entries->GetData(index, colIndex++, entry.name);
		ds->entries->GetData(index, colIndex++, entry.call);
		ds->entries->GetData(index, colIndex++, entry.confidence);
		ds->entries->GetData(index, colIndex++, entry.force);
		ds->entries->GetData(index, colIndex++, entry.estimate);
		ds->entries->GetData(index, colIndex++, entry.lower);
		ds->entries->GetData(index, colIndex++, entry.upper);
		GetExtraMetricEntries(ds, index, colIndex, entry.metrics);
	}
}

void CHPMultiDataData::GetEntry(MultiDataType dataType, int index, affymetrix_calvin_data::DmetBiAllelicData &entry)
{
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
	{
		int colIndex = 0;
		entry.name.clear();
		ds->entries->GetData(index, colIndex++, entry.name);
		ds->entries->GetData(index, colIndex++, entry.call);
		ds->entries->GetData(index, colIndex++, entry.confidence);
		ds->entries->GetData(index, colIndex++, entry.force);
		ds->entries->GetData(index, colIndex++, entry.signalA);
		ds->entries->GetData(index, colIndex++, entry.signalB);
		ds->entries->GetData(index, colIndex++, entry.contextA);
		ds->entries->GetData(index, colIndex++, entry.contextB);
		GetExtraMetricEntries(ds, index, colIndex, entry.metrics);
	}
}

void CHPMultiDataData::GetEntry(MultiDataType dataType, int index, affymetrix_calvin_data::DmetMultiAllelicData &entry)
{
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
	{
		int colIndex = 0;
		entry.name.clear();
		ds->entries->GetData(index, colIndex++, entry.name);
		ds->entries->GetData(index, colIndex++, entry.call);
		ds->entries->GetData(index, colIndex++, entry.confidence);
		ds->entries->GetData(index, colIndex++, entry.force);
		ds->entries->GetData(index, colIndex++, entry.alleleCount);
		ds->entries->GetData(index, colIndex++, entry.signalA);
		ds->entries->GetData(index, colIndex++, entry.signalB);
		ds->entries->GetData(index, colIndex++, entry.signalC);
		ds->entries->GetData(index, colIndex++, entry.signalD);
		ds->entries->GetData(index, colIndex++, entry.signalE);
		ds->entries->GetData(index, colIndex++, entry.signalF);
		ds->entries->GetData(index, colIndex++, entry.contextA);
		ds->entries->GetData(index, colIndex++, entry.contextB);
		ds->entries->GetData(index, colIndex++, entry.contextC);
		ds->entries->GetData(index, colIndex++, entry.contextD);
		ds->entries->GetData(index, colIndex++, entry.contextE);
		ds->entries->GetData(index, colIndex++, entry.contextF);
		GetExtraMetricEntries(ds, index, colIndex, entry.metrics);
	}
}

void CHPMultiDataData::GetEntry(MultiDataType dataType, int index, affymetrix_calvin_data::AllelePeaks &entry)
{
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
	{
		int colIndex = 0;
		entry.name.clear();
		ds->entries->GetData(index, colIndex++, entry.name);
		ds->entries->GetData(index, colIndex++, entry.chr);
		ds->entries->GetData(index, colIndex++, entry.position);
		GetExtraMetricEntries(ds, index, colIndex, entry.peaks);
	}
}



void CHPMultiDataData::GetEntry(MultiDataType dataType, int index, affymetrix_calvin_data::MarkerABSignals &entry)
{
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);	

	if (ds && ds->entries && ds->entries->IsOpen())
	{
		int colIndex = 0;
		ds->entries->GetData(index, colIndex++, entry.index);
		GetExtraMetricEntries(ds, index, colIndex, entry.metrics);
	}
}


void CHPMultiDataData::GetEntry(MultiDataType dataType, int index, affymetrix_calvin_data::CytoGenotypeCallData &entry)
{
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
	{
		int colIndex = 0;
		ds->entries->GetData(index, colIndex++, entry.index);
		ds->entries->GetData(index, colIndex++, entry.call);
		ds->entries->GetData(index, colIndex++, entry.confidence);
		ds->entries->GetData(index, colIndex++, entry.forcedCall);
		ds->entries->GetData(index, colIndex++, entry.aSignal);
		ds->entries->GetData(index, colIndex++, entry.bSignal);
		ds->entries->GetData(index, colIndex++, entry.signalStrength);
		ds->entries->GetData(index, colIndex++, entry.contrast);
		GetExtraMetricEntries(ds, index, colIndex, entry.metrics);
	}
}

void CHPMultiDataData::GetCopyNumberEntryLog2Ratio(MultiDataType dataType, int index, float* val)
{
	GetGenericCopyNumberEntryLog2Ratio(dataType, index, val);
}

void CHPMultiDataData::GetCytoEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCytoRegionData &entry)
{
	GetGenericEntry(dataType, index, entry);
}

void CHPMultiDataData::GetExpressionEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataExpressionData &entry)
{
	GetGenericEntry(dataType, index, entry);
}

void CHPMultiDataData::GetCopyNumberVariationEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCopyNumberVariationRegionData &entry)
{
	GetGenericEntry(dataType, index, entry);
}

void CHPMultiDataData::GetChromosomeSegmentEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ChromosomeSegmentData &entry)
{
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
	{
		int colIndex = 0;
		ds->entries->GetData(index, colIndex++, entry.segmentId);
		ds->entries->GetData(index, colIndex++, entry.chr);
		ds->entries->GetData(index, colIndex++, entry.startPosition);
		ds->entries->GetData(index, colIndex++, entry.stopPosition);
		ds->entries->GetData(index, colIndex++, entry.markerCount);
		ds->entries->GetData(index, colIndex++, entry.meanMarkerDistance);
		GetExtraMetricEntries(ds, index, colIndex, entry.metrics);
	}
}

void CHPMultiDataData::GetChromosomeSegmentEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ChromosomeSegmentDataEx &entry)
{
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
	{
		int colIndex = 0;
		ds->entries->GetData(index, colIndex++, entry.segmentId);
		ds->entries->GetData(index, colIndex++, entry.referenceSampleKey);
		ds->entries->GetData(index, colIndex++, entry.familialSampleKey);
		ds->entries->GetData(index, colIndex++, entry.chr);
		ds->entries->GetData(index, colIndex++, entry.startPosition);
		ds->entries->GetData(index, colIndex++, entry.stopPosition);
		ds->entries->GetData(index, colIndex++, entry.call);
		ds->entries->GetData(index, colIndex++, entry.confidence);
		ds->entries->GetData(index, colIndex++, entry.markerCount);
		ds->entries->GetData(index, colIndex++, entry.homozygosity);
		ds->entries->GetData(index, colIndex++, entry.heterozygosity);
		GetExtraMetricEntries(ds, index, colIndex, entry.metrics);
	}
}

void CHPMultiDataData::GetChromosomeSummaryEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ChromosomeMultiDataSummaryData &entry)
{
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
	{
		int colIndex = 0;
		ds->entries->GetData(index, colIndex++, entry.chr);
		ds->entries->GetData(index, colIndex++, entry.display);
		ds->entries->GetData(index, colIndex++, entry.startIndex);
		ds->entries->GetData(index, colIndex++, entry.markerCount);
		ds->entries->GetData(index, colIndex++, entry.minSignal);
		ds->entries->GetData(index, colIndex++, entry.maxSignal);
		ds->entries->GetData(index, colIndex++, entry.medianCnState);
		ds->entries->GetData(index, colIndex++, entry.homFrequency);
		ds->entries->GetData(index, colIndex++, entry.hetFrequency);
		GetExtraMetricEntries(ds, index, colIndex++, entry.metrics);
    }
}

void CHPMultiDataData::GetFamilialSampleEntry(MultiDataType dataType, int index, affymetrix_calvin_data::FamilialSample &entry)
{
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
	{
		int colIndex = 0;
		ds->entries->GetData(index, colIndex++, entry.sampleKey);
		ds->entries->GetData(index, colIndex++, entry.arrID);
		ds->entries->GetData(index, colIndex++, entry.chpID);
		ds->entries->GetData(index, colIndex++, entry.chpFilename);
		ds->entries->GetData(index, colIndex++, entry.role);
		u_int8_t rv;
		ds->entries->GetData(index, colIndex++, rv);
		entry.roleValidity = (rv == 1 ? true : false);
		ds->entries->GetData(index, colIndex++, entry.roleConfidence);
	}
}

void CHPMultiDataData::GetFamilialSegmentOverlapEntry(MultiDataType dataType, int index, affymetrix_calvin_data::FamilialSegmentOverlap &entry)
{
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
	{
		int colIndex = 0;
		ds->entries->GetData(index, colIndex++, entry.segmentType);
		ds->entries->GetData(index, colIndex++, entry.referenceSampleKey);
		ds->entries->GetData(index, colIndex++, entry.referenceSegmentID);
		ds->entries->GetData(index, colIndex++, entry.familialSampleKey);
		ds->entries->GetData(index, colIndex++, entry.familialSegmentID);
    }
}

void CHPMultiDataData::GetGenericEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataGenotypeData &entry)
{
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
	{
		int colIndex = 0;
		entry.name.clear();
		ds->entries->GetData(index, colIndex++, entry.name);
		ds->entries->GetData(index, colIndex++, entry.call);
		ds->entries->GetData(index, colIndex++, entry.confidence);
		GetExtraMetricEntries(ds, index, colIndex, entry.metrics);
	}
}

void CHPMultiDataData::GetGenericEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCopyNumberData &entry)
{
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
	{
		int colIndex = 0;
		entry.name.clear();
		ds->entries->GetData(index, colIndex++, entry.name);
		ds->entries->GetData(index, colIndex++, entry.chr);
		ds->entries->GetData(index, colIndex++, entry.position);
		GetExtraMetricEntries(ds, index, colIndex, entry.metrics);
	}
}

void CHPMultiDataData::GetGenericCopyNumberEntryLog2Ratio(MultiDataType dataType, int index, float* val)
{
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
	{
		GetExtraCopyNumberFloatTypeNoNameLog2Ratio(ds, index, val);
	}
}

void CHPMultiDataData::GetGenericEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCytoRegionData &entry)
{
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
	{
		int colIndex = 0;
		entry.name.clear();
		ds->entries->GetData(index, colIndex++, entry.name);
		ds->entries->GetData(index, colIndex++, entry.chr);
		ds->entries->GetData(index, colIndex++, entry.startPosition);
		ds->entries->GetData(index, colIndex++, entry.stopPosition);
		ds->entries->GetData(index, colIndex++, entry.call);
		ds->entries->GetData(index, colIndex++, entry.confidenceScore);
		GetExtraMetricEntries(ds, index, colIndex, entry.metrics);
	}
}

void CHPMultiDataData::GetGenericEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataExpressionData &entry)
{
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
	{
		int colIndex = 0;
		entry.name.clear();
		ds->entries->GetData(index, colIndex++, entry.name);
		ds->entries->GetData(index, colIndex++, entry.quantification);
		GetExtraMetricEntries(ds, index, colIndex, entry.metrics);
	}
}

void CHPMultiDataData::GetGenericEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCopyNumberVariationRegionData &entry)
{
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
	{
		int colIndex = 0;
		entry.name.clear();
		ds->entries->GetData(index, colIndex++, entry.name);	
		ds->entries->GetData(index, colIndex++, entry.signal);
		ds->entries->GetData(index, colIndex++, entry.call);
		ds->entries->GetData(index, colIndex++, entry.confidenceScore);
		GetExtraMetricEntries(ds, index, colIndex, entry.metrics);
	}
}

void CHPMultiDataData::GetExtraMetricEntries(DataSetInfo *ds, int rowIndex, int colIndex, std::vector<affymetrix_calvin_parameter::ParameterNameValueType> &metrics)
{
	int32_t ncols = (int32_t) ds->metricColumns.size();
	metrics.resize(ncols);
	for (int32_t icol=0; icol<ncols; icol++)
	{
		metrics[icol].SetName(ds->metricColumns[icol].GetName());
		switch (ds->metricColumns[icol].GetColumnType())
		{
		case ByteColType:
			{
				int8_t val;
				ds->entries->GetData(rowIndex, colIndex++, val);
				metrics[icol].SetValueInt8(val);
			}
			break;

		case UByteColType:
			{
				u_int8_t val;
				ds->entries->GetData(rowIndex, colIndex++, val);
				metrics[icol].SetValueUInt8(val);
			}
			break;

		case ShortColType:
			{
				int16_t val;
				ds->entries->GetData(rowIndex, colIndex++, val);
				metrics[icol].SetValueInt16(val);
			}
			break;

		case UShortColType:
			{
				u_int16_t val;
				ds->entries->GetData(rowIndex, colIndex++, val);
				metrics[icol].SetValueUInt16(val);
			}
			break;

		case IntColType:
			{
				int32_t val;
				ds->entries->GetData(rowIndex, colIndex++, val);
				metrics[icol].SetValueInt32(val);
			}
			break;

		case UIntColType:
			{
				u_int32_t val;
				ds->entries->GetData(rowIndex, colIndex++, val);
				metrics[icol].SetValueUInt32(val);
			}
			break;

		case FloatColType:
			{
				float val;
				ds->entries->GetData(rowIndex, colIndex++, val);
				metrics[icol].SetValueFloat(val);
			}
			break;

		case ASCIICharColType:
			{
				string val;
				ds->entries->GetData(rowIndex, colIndex++, val);
				metrics[icol].SetValueAscii(val);
			}
			break;

		case UnicodeCharColType:
			{
				wstring val;
				ds->entries->GetData(rowIndex, colIndex++, val);
				metrics[icol].SetValueText(val);
			}
			break;
		}
	}
}

void CHPMultiDataData::GetExtraCopyNumberFloatTypeNoNameLog2Ratio(DataSetInfo *ds, int rowIndex, float *val)
{
	float valFloat = 0.0f;

	ds->entries->GetData(rowIndex, cnlog2RatioIndexOffset, (float&)valFloat);
	*val = valFloat;
}

u_int8_t CHPMultiDataData::GetGenoCall(MultiDataType dataType, int index)
{
	u_int8_t call = 0;
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
		ds->entries->GetData(index, 1, call);
	return call;
}

float CHPMultiDataData::GetGenoConfidence(MultiDataType dataType, int index)
{
	float conf = 0.0f;
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
		ds->entries->GetData(index, 2, conf);
	return conf;
}

float CHPMultiDataData::GetExpressionQuantification(MultiDataType dataType, int index)
{
	float quant = 0.0f;
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
	{
		ds->entries->GetData(index, 1, quant);
	}
	return quant;
}

std::string CHPMultiDataData::GetProbeSetName(MultiDataType dataType, int index)
{
	std::string name;
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
	{
		ds->entries->GetData(index, 0, name);
	}
	return name;
}

void CHPMultiDataData::AddColumns(DataSetInfo &info, DataSetHeader& hdr)
{
	switch (info.dataType)
	{
	case ExpressionMultiDataType:
		hdr.AddAsciiColumn(PROBE_SET_NAME, info.maxName);
		hdr.AddFloatColumn(QUANTIFICATION);
		break;

	case GenotypeMultiDataType:
		hdr.AddAsciiColumn(PROBE_SET_NAME, info.maxName);
		hdr.AddUByteColumn(CALL);
		hdr.AddFloatColumn(CONFIDENCE);
		break;

	case CopyNumberMultiDataType:
		hdr.AddAsciiColumn(PROBE_SET_NAME, info.maxName);
		hdr.AddUByteColumn(CHR);
		hdr.AddUIntColumn(POSITION);
		break;

	case CytoMultiDataType:
		hdr.AddAsciiColumn(REGION, info.maxName);
		hdr.AddUByteColumn(CHR);
		hdr.AddUIntColumn(START_POSITION);
		hdr.AddUIntColumn(STOP_POSITION);
		hdr.AddUByteColumn(CALL);
		hdr.AddFloatColumn(CONFIDENCE);
		break;

	case CopyNumberVariationMultiDataType:
		hdr.AddAsciiColumn(REGION, info.maxName);  
		hdr.AddFloatColumn(SIGNAL);
		hdr.AddUByteColumn(CALL);
		hdr.AddFloatColumn(CONFIDENCE);
		break;

	case DmetCopyNumberMultiDataType:
		hdr.AddAsciiColumn(PROBE_SET_NAME, info.maxName);
		hdr.AddShortColumn(CN_CALL);
		hdr.AddFloatColumn(CN_CONFIDENCE);
		hdr.AddShortColumn(CN_FORCE);
		hdr.AddFloatColumn(CN_ESTIMATE);
		hdr.AddFloatColumn(CN_LOWER);
		hdr.AddFloatColumn(CN_UPPER);
		break;

	case DmetMultiAllelicMultiDataType:
		hdr.AddAsciiColumn(PROBE_SET_NAME, info.maxName);
		hdr.AddUByteColumn(CALL);
		hdr.AddFloatColumn(CONFIDENCE);
		hdr.AddUByteColumn(FORCE);
		hdr.AddUByteColumn(ALLELE_COUNT);
		hdr.AddFloatColumn(SIGNAL_A);
		hdr.AddFloatColumn(SIGNAL_B);
		hdr.AddFloatColumn(SIGNAL_C);
		hdr.AddFloatColumn(SIGNAL_D);
		hdr.AddFloatColumn(SIGNAL_E);
		hdr.AddFloatColumn(SIGNAL_F);
		hdr.AddUByteColumn(CONTEXT_A);
		hdr.AddUByteColumn(CONTEXT_B);
		hdr.AddUByteColumn(CONTEXT_C);
		hdr.AddUByteColumn(CONTEXT_D);
		hdr.AddUByteColumn(CONTEXT_E);
		hdr.AddUByteColumn(CONTEXT_F);
		break;

	case DmetBiAllelicMultiDataType:
		hdr.AddAsciiColumn(PROBE_SET_NAME, info.maxName);
		hdr.AddUByteColumn(CALL);
		hdr.AddFloatColumn(CONFIDENCE);
		hdr.AddUByteColumn(FORCE);
		hdr.AddFloatColumn(SIGNAL_A);
		hdr.AddFloatColumn(SIGNAL_B);
		hdr.AddUByteColumn(CONTEXT_A);
		hdr.AddUByteColumn(CONTEXT_B);
		break;

    case ChromosomeSummaryMultiDataType:
		hdr.AddUByteColumn(CHR);
		hdr.AddAsciiColumn(CHR_DISPLAY, info.maxName);
		hdr.AddUIntColumn(START_INDEX);
		hdr.AddUIntColumn(MARKER_COUNT);
        hdr.AddFloatColumn(MIN_SIGNAL);
        hdr.AddFloatColumn(MAX_SIGNAL);
        hdr.AddFloatColumn(MEDIAN_CN_STATE);
        hdr.AddFloatColumn(HOM_FREQ);
        hdr.AddFloatColumn(HET_FREQ);
        break;

    case SegmentCNMultiDataType:
		hdr.AddUIntColumn(SEGMENT_ID);
		hdr.AddUByteColumn(CHR);
		hdr.AddUIntColumn(START_POSITION);
		hdr.AddUIntColumn(STOP_POSITION);
		hdr.AddIntColumn(MARKER_COUNT);
		hdr.AddUIntColumn(MEAN_MARKER_DISTANCE);
        break;

    case SegmentLOHMultiDataType:
		hdr.AddUIntColumn(SEGMENT_ID);
		hdr.AddUByteColumn(CHR);
		hdr.AddUIntColumn(START_POSITION);
		hdr.AddUIntColumn(STOP_POSITION);
		hdr.AddIntColumn(MARKER_COUNT);
		hdr.AddUIntColumn(MEAN_MARKER_DISTANCE);
        break;

	case SegmentCNNeutralLOHMultiDataType:
		hdr.AddUIntColumn(SEGMENT_ID);
		hdr.AddUByteColumn(CHR);
		hdr.AddUIntColumn(START_POSITION);
		hdr.AddUIntColumn(STOP_POSITION);
		hdr.AddIntColumn(MARKER_COUNT);
		hdr.AddUIntColumn(MEAN_MARKER_DISTANCE);
        break;

    case SegmentNormalDiploidMultiDataType:
		hdr.AddUIntColumn(SEGMENT_ID);
		hdr.AddUByteColumn(CHR);
		hdr.AddUIntColumn(START_POSITION);
		hdr.AddUIntColumn(STOP_POSITION);
		hdr.AddIntColumn(MARKER_COUNT);
		hdr.AddUIntColumn(MEAN_MARKER_DISTANCE);
        break;

    case SegmentNoCallMultiDataType:
		hdr.AddUIntColumn(SEGMENT_ID);
		hdr.AddUByteColumn(CHR);
		hdr.AddUIntColumn(START_POSITION);
		hdr.AddUIntColumn(STOP_POSITION);
		hdr.AddIntColumn(MARKER_COUNT);
		hdr.AddUIntColumn(MEAN_MARKER_DISTANCE);
        break;

    case SegmentMosaicismMultiDataType:
		hdr.AddUIntColumn(SEGMENT_ID);
		hdr.AddUByteColumn(CHR);
		hdr.AddUIntColumn(START_POSITION);
		hdr.AddUIntColumn(STOP_POSITION);
		hdr.AddIntColumn(MARKER_COUNT);
		hdr.AddUIntColumn(MEAN_MARKER_DISTANCE);
        break;

    case SegmentGenotypeConcordanceMultiDataType:
    case SegmentGenotypeDiscordanceMultiDataType:
    case SegmentCNLossLOHConcordanceMultiDataType:
    case SegmentCNNeutralLOHConcordanceMultiDataType:
    case SegmentHeteroUPDMultiDataType:
    case SegmentIsoUPDMultiDataType:
    case SegmentDenovoCopyNumberMultiDataType:
    case SegmentHemizygousParentOfOriginMultiDataType:
		hdr.AddUIntColumn(SEGMENT_ID);
		hdr.AddUIntColumn(REFERENCE_SAMPLE_KEY);
		hdr.AddUIntColumn(FAMILIAL_SAMPLE_KEY);
		hdr.AddUByteColumn(CHR);
		hdr.AddUIntColumn(START_POSITION);
		hdr.AddUIntColumn(STOP_POSITION);
		hdr.AddUByteColumn(CALL);
		hdr.AddFloatColumn(CONFIDENCE);
        hdr.AddIntColumn(MARKER_COUNT);
		hdr.AddFloatColumn(HOMOZYGOSITY);
		hdr.AddFloatColumn(HETEROZYGOSITY);
        break;

	case FamilialSegmentOverlapsMultiDataType:
        hdr.AddAsciiColumn(SEGMENT_TYPE, info.maxSegmentType);
        hdr.AddUIntColumn(REFERENCE_SAMPLE_KEY);
        hdr.AddAsciiColumn(REFERENCE_SEGMENT_ID, info.maxReferenceSegmentID);
        hdr.AddUIntColumn(FAMILIAL_SAMPLE_KEY);
        hdr.AddAsciiColumn(FAMILIAL_SEGMENT_ID, info.maxFamilialSegmentID);
        break;

	case FamilialSamplesMultiDataType:
		hdr.AddUIntColumn(SAMPLE_KEY);
		hdr.AddAsciiColumn(ARRID, info.maxFamilialARRID);
		hdr.AddAsciiColumn(CHPID, info.maxFamilialCHPID);
		hdr.AddUnicodeColumn(CHP_FILENAME, info.maxFamilialCHPFile);
		hdr.AddAsciiColumn(ROLE, info.maxFamilialRole);
		hdr.AddUByteColumn(ROLE_VALIDITY);
		hdr.AddFloatColumn(ROLE_CONFIDENCE);
		break;

	case AllelePeaksMultiDataType:
		hdr.AddAsciiColumn(PROBE_SET_NAME, info.maxName);
		hdr.AddUByteColumn(CHR);
		hdr.AddUIntColumn(POSITION);
		break;

	case MarkerABSignalsMultiDataType:
		hdr.AddUIntColumn(PROBE_SET_INDEX);
		/*hdr.AddFloatColumn(A_SIGNAL);
		hdr.AddFloatColumn(B_SIGNAL);
		hdr.AddFloatColumn(SCAR);*/
		break;

	case CytoGenotypeCallMultiDataType:
		hdr.AddUIntColumn(PROBE_SET_INDEX);
		hdr.AddByteColumn(CALL);
		hdr.AddFloatColumn(CONFIDENCE);
		hdr.AddByteColumn(FORCE);
		hdr.AddFloatColumn(A_SIGNAL);
		hdr.AddFloatColumn(B_SIGNAL);
		hdr.AddFloatColumn(SIGNAL_STRENGTH);
		hdr.AddFloatColumn(CONTRAST);
		break;

	default:
		break;
	}

	for (vector<ColumnInfo>::iterator it=info.metricColumns.begin(); it!=info.metricColumns.end(); ++it)
	{
		hdr.AddColumn(*it);
	}
}

DataSetInfo *CHPMultiDataData::OpenMultiDataDataSet(MultiDataType dataType)
{
	map<MultiDataType, DataSetInfo>::iterator pos = dataSetInfo.find(dataType);
	if (pos != dataSetInfo.end())
	{
		return &dataSetInfo[dataType];
	}
	//read metrics column data
	DataSetInfo info;
    if (dataTypeGroupNames.empty() == true)
    {
        std::map<std::wstring, MultiDataType> nameTypeMap;
        int nnames = sizeof(MultiDataDataSetNames) / sizeof(std::wstring);
        for (int iname=0; iname<nnames; iname++)
            nameTypeMap[MultiDataDataSetNames[iname]] = MultiDataDataTypes[iname];
        int ng = genericData.Header().GetDataGroupCnt();
        for (int ig=0; ig<ng; ig++)
        {
            DataGroupHeader &dh = genericData.Header().GetDataGroup(ig);
            std::wstring name = dh.GetName();
            int ns = dh.GetDataSetCnt();
            for (int is=0; is<ns; is++)
            {
                DataSetHeader &h = dh.GetDataSet(is);
                dataTypeGroupNames[nameTypeMap[h.GetName()]] = dh.GetName();
            }
        }
    }
	info.entries = genericData.DataSet(dataTypeGroupNames[dataType], MultiDataDataSetNames[dataType]);
	if (info.entries)
	{
		info.entries->Open();
		int32_t ncols = info.entries->Header().GetColumnCnt();
		info.metricColumns.clear();
		int startCol = 0;
		if (dataType == ExpressionMultiDataType || dataType == ExpressionControlMultiDataType)
		{
			startCol = 2;
		}
		else if (dataType == GenotypeMultiDataType || dataType == GenotypeControlMultiDataType)
		{
			startCol = 3;
		}
		else if (dataType == CopyNumberMultiDataType)
		{
			startCol = 3;
		}
		else if (dataType == CytoMultiDataType)
		{
			startCol = 6;
		}
		else if (dataType == CopyNumberVariationMultiDataType)
		{
			startCol = 4;
		}
		else if (dataType == DmetCopyNumberMultiDataType)
		{
			startCol = 7;
		}
		else if (dataType == DmetMultiAllelicMultiDataType)
		{
			startCol = 17;
		}
		else if (dataType == DmetBiAllelicMultiDataType)
		{
			startCol = 8;
		}
        else if (dataType == ChromosomeSummaryMultiDataType)
        {
            startCol = 9;
        }
        else if (dataType == SegmentCNMultiDataType ||
            dataType == SegmentLOHMultiDataType ||
            dataType == SegmentCNNeutralLOHMultiDataType ||
            dataType == SegmentNormalDiploidMultiDataType ||
            dataType == SegmentNoCallMultiDataType ||
            dataType == SegmentMosaicismMultiDataType)
		{
			startCol = 6;
		}
		else if (dataType == SegmentGenotypeConcordanceMultiDataType ||
            dataType == SegmentGenotypeDiscordanceMultiDataType ||
            dataType == SegmentCNLossLOHConcordanceMultiDataType ||
            dataType == SegmentCNNeutralLOHConcordanceMultiDataType ||
            dataType == SegmentHeteroUPDMultiDataType ||
            dataType == SegmentIsoUPDMultiDataType ||
            dataType == SegmentDenovoCopyNumberMultiDataType ||
            dataType == SegmentHemizygousParentOfOriginMultiDataType)
        {
            startCol = 11;
        }
        else if (dataType == FamilialSegmentOverlapsMultiDataType)
        {
            startCol = 5;
        }
		else if (dataType == FamilialSamplesMultiDataType)
		{
			startCol = 7;
		}
		else if (dataType == AllelePeaksMultiDataType)
		{
			startCol = 3;
		}
		else if (dataType == MarkerABSignalsMultiDataType)
		{
			startCol = 1;
		}
		else if (dataType == CytoGenotypeCallMultiDataType)
		{
			startCol = 8;
		}
		for (int32_t icol=startCol; icol<ncols; icol++)
		{
			info.metricColumns.push_back(info.entries->Header().GetColumnInfo(icol));
		}
		dataSetInfo[dataType] = info;
		return &dataSetInfo[dataType];
	}
	return NULL;
}

std::wstring CHPMultiDataData::GetArrayType()
{
	return GetWStringFromGenericHdr(ARRAY_TYPE_PARAM_NAME);
}

void CHPMultiDataData::SetArrayType(const std::wstring& value)
{
	SetWStringToGenericHdr(ARRAY_TYPE_PARAM_NAME, value, ARRAY_TYPE_MAX_LEN);
}

std::wstring CHPMultiDataData::GetAlgName()
{
	return GetWStringFromGenericHdr(ALGORITHM_NAME_PARAM_NAME);
}

void CHPMultiDataData::SetAlgName(const std::wstring& value)
{
	SetWStringToGenericHdr(ALGORITHM_NAME_PARAM_NAME, value);
}

std::wstring CHPMultiDataData::GetAlgVersion()
{
	return GetWStringFromGenericHdr(ALG_VERSION_PARAM_NAME);
}

void CHPMultiDataData::SetAlgVersion(const std::wstring& value)
{
	SetWStringToGenericHdr(ALG_VERSION_PARAM_NAME, value);
}

ParameterNameValueTypeList CHPMultiDataData::GetAlgParams()
{
	ParameterNameValueTypeList nvt;
	ParameterNameValueTypeIt begin, end;
	ParameterNameValueType param;

	genericData.Header().GetGenericDataHdr()->GetNameValIterators(begin, end);
	while(begin != end)
	{
		std::wstring key = begin->GetName();
		if(key.compare(0, ALGORITHM_PARAM_NAME_PREFIX_S.size(),ALGORITHM_PARAM_NAME_PREFIX_S) == 0)
		{
			param = *begin;
			key.erase(0, ALGORITHM_PARAM_NAME_PREFIX_S.size());
			param.SetName(key);
			nvt.push_back(param);
		}
		begin++;
	}
	return nvt;
}

void CHPMultiDataData::AddAlgParams(const ParameterNameValueTypeList& params)
{
	ParameterNameValueType param;
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	for (ParameterNameValueTypeList::const_iterator it=params.begin(); it != params.end(); it++)
	{
		param = *it;
		param.SetName(ALGORITHM_PARAM_NAME_PREFIX_S + param.GetName());
		hdr->AddNameValParam(param);
	}
}

void CHPMultiDataData::AddAppMetaInfo(const ParameterNameValueTypeList& params)
{
	ParameterNameValueType param;
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	for (ParameterNameValueTypeList::const_iterator it=params.begin(); it != params.end(); ++it)
	{
		param = *it;
		param.SetName(APPLICATION_META_INFO_PREFIX_S + param.GetName());
		hdr->AddNameValParam(param);
	}
}


ParameterNameValueTypeList CHPMultiDataData::GetSummaryParams()
{
	ParameterNameValueTypeList nvt;
	ParameterNameValueTypeIt begin, end;
	ParameterNameValueType param;

	genericData.Header().GetGenericDataHdr()->GetNameValIterators(begin, end);
	while(begin != end)
	{
		std::wstring key = begin->GetName();
		if(key.compare(0, CHIP_SUMMARY_PARAMETER_NAME_PREFIX_S.size(),CHIP_SUMMARY_PARAMETER_NAME_PREFIX) == 0)
		{
			param = *begin;
			key.erase(0, CHIP_SUMMARY_PARAMETER_NAME_PREFIX_S.size());
			param.SetName(key);
			nvt.push_back(param);
		}
		begin++;
	}
	return nvt;
}

void CHPMultiDataData::AddSummaryParams(const ParameterNameValueTypeList& params)
{
	ParameterNameValueType param;
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	for (ParameterNameValueTypeList::const_iterator it=params.begin(); it != params.end(); it++)
	{
		param = *it;
		param.SetName(CHIP_SUMMARY_PARAMETER_NAME_PREFIX + param.GetName());
		hdr->AddNameValParam(param);
	}
}

std::wstring CHPMultiDataData::GetWStringFromGenericHdr(const std::wstring& name)
{
	std::wstring result;
	ParameterNameValueType paramType;
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	if (hdr->FindNameValParam(name, paramType))
	{
		result = paramType.GetValueText();
	}
	return result;
}

void CHPMultiDataData::SetWStringToGenericHdr(const std::wstring& name, const std::wstring value, int32_t reserve)
{
	ParameterNameValueType paramType;
	paramType.SetName(name);
	paramType.SetValueText(value, reserve);
	GenericDataHeader* hdr = genericData.Header().GetGenericDataHdr();
	hdr->AddNameValParam(paramType);
}

