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

#include "CHPMultiDataData.h"
#include "DataSetHeader.h"
#include "StringUtils.h"

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

CHPMultiDataData::CHPMultiDataData()
{
	Clear();
}

CHPMultiDataData::CHPMultiDataData(const std::string& filename) 
{ 
	Clear();
	SetFilename(filename);
	DataGroupHeader dcHdr(MULTI_DATA_NAME);
	genericData.Header().AddDataGroupHdr(dcHdr);
	genericData.Header().GetGenericDataHdr()->SetFileTypeId(CHP_MULTI_DATA_TYPE);
}

CHPMultiDataData::~CHPMultiDataData() 
{ 
	Clear(); 
}

DataSetHeader *CHPMultiDataData::GetDataSetHeader(MultiDataType dataType)
{
    int n = genericData.Header().GetDataGroup(0).GetDataSetCnt();
    for (int i=0; i<n; i++)
    {
        DataSetHeader &h = genericData.Header().GetDataGroup(0).GetDataSet(i);
        if (h.GetName() == MultiDataDataSetNames[(int)dataType])
            return &h;
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
	genericData.Header().Clear();
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
		return dataSetInfo[dataType].maxProbeSetName;
    return 0;
}

void CHPMultiDataData::SetEntryCount(MultiDataType dataType, int32_t ln, int32_t maxln, const std::vector<ColumnInfo> &columns)
{
    DataSetInfo info;
    info.maxProbeSetName = maxln;
    info.metricColumns = columns;
    info.entries = NULL;
    info.dataType = dataType;
    info.dataSetIndex = (int)dataSetInfo.size();
    dataSetInfo[dataType] = info;

	DataSetHeader dsHdr;
	dsHdr.SetRowCnt(ln);
	dsHdr.SetName(MultiDataDataSetNames[(int)dataType]);
	AddColumns(info, dsHdr);

	DataGroupHeader* dgHdr = &genericData.Header().GetDataGroup(0);
	dgHdr->AddDataSetHdr(dsHdr);
}

void CHPMultiDataData::SetEntryCount(MultiDataType dataType, int32_t ln, int32_t maxln)
{
    std::vector<ColumnInfo> empty;
    SetEntryCount(dataType, ln, maxln, empty);
}

void CHPMultiDataData::GetGenotypeEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataGenotypeData &entry)
{
    GetGenericGenotypeEntry(dataType, index, entry);
}

void CHPMultiDataData::GetCopyNumberEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCopyNumberData &entry)
{
    GetGenericCopyNumberEntry(dataType, index, entry);
}

void CHPMultiDataData::GetCytoEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCytoRegionData &entry)
{
    GetGenericCytoRegionEntry(dataType, index, entry);
}

void CHPMultiDataData::GetExpressionEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataExpressionData &entry)
{
    GetGenericExpressionEntry(dataType, index, entry);
}

void CHPMultiDataData::GetGenericGenotypeEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataGenotypeData &entry)
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

void CHPMultiDataData::GetGenericCopyNumberEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCopyNumberData &entry)
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

void CHPMultiDataData::GetGenericCytoRegionEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataCytoRegionData &entry)
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

void CHPMultiDataData::GetGenericExpressionEntry(MultiDataType dataType, int index, affymetrix_calvin_data::ProbeSetMultiDataExpressionData &entry)
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
		ds->entries->GetData(index, 1, quant);
	return quant;
}

std::string CHPMultiDataData::GetProbeSetName(MultiDataType dataType, int index)
{
	std::string name;
	DataSetInfo *ds = OpenMultiDataDataSet(dataType);
	if (ds && ds->entries && ds->entries->IsOpen())
		ds->entries->GetData(index, 0, name);
	return name;
}

void CHPMultiDataData::AddColumns(DataSetInfo &info, DataSetHeader& hdr)
{
    switch (info.dataType)
    {
    case ExpressionMultiDataType:
        hdr.AddAsciiColumn(PROBE_SET_NAME, info.maxProbeSetName);
        hdr.AddFloatColumn(EXPRESSION_QUANTIFICATION_NAME);
        break;
        
    case GenotypeMultiDataType:
        hdr.AddAsciiColumn(PROBE_SET_NAME, info.maxProbeSetName);
        hdr.AddUByteColumn(GENOTYPE_CALL_NAME);
        hdr.AddFloatColumn(GENOTYPE_CONFIDENCE_NAME);
        break;

    case CopyNumberMultiDataType:
        hdr.AddAsciiColumn(PROBE_SET_NAME, info.maxProbeSetName);
        hdr.AddUByteColumn(COPY_NUMBER_CHR_NAME);
        hdr.AddUIntColumn(COPY_NUMBER_POSITION_NAME);
        break;

    case CytoMultiDataType:
        hdr.AddAsciiColumn(CYTO_REGION_NAME, info.maxProbeSetName);
        hdr.AddUByteColumn(CYTO_CHR_NAME);
        hdr.AddUIntColumn(CYTO_START_POSITION_NAME);
        hdr.AddUIntColumn(CYTO_STOP_POSITION_NAME);
        hdr.AddUByteColumn(CYTO_CALL_NAME);
        hdr.AddFloatColumn(CYTO_CONFIDENCE_NAME);
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
		return &dataSetInfo[dataType];

    DataSetInfo info;
    info.entries = genericData.DataSet(MULTI_DATA_NAME, MultiDataDataSetNames[dataType]);
	if (info.entries)
	{
		info.entries->Open();
		int32_t ncols = info.entries->Header().GetColumnCnt();
		info.metricColumns.clear();
        int startCol = 0;
        if (dataType == ExpressionMultiDataType)
            startCol = 2;
        else if (dataType == GenotypeMultiDataType)
            startCol = 3;
        else if (dataType == CopyNumberMultiDataType)
            startCol = 3;
        else if (dataType == CytoMultiDataType)
            startCol = 6;

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

