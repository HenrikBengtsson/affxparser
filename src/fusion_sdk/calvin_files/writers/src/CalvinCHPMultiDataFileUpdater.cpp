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


#include "calvin_files/writers/src/CalvinCHPMultiDataFileUpdater.h"
//

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_calvin_data;
using namespace std;

/*! The data group index. */
#define MULTI_DATA_DATA_GROUP 0

/*
 * Initialize any needed members.
 */
CalvinCHPMultiDataFileUpdater::CalvinCHPMultiDataFileUpdater() : DataSetUpdater()
{
}

/*
 * Clean up.
 */
CalvinCHPMultiDataFileUpdater::~CalvinCHPMultiDataFileUpdater()
{
}

void CalvinCHPMultiDataFileUpdater::Initialize(const char *file)
{
	DataSetUpdater::Initialize(file);
	dataSetIndexMap.clear();
	int nds=(int)dataSetNames[0].size();
	int ndt=sizeof(MultiDataDataTypes) / sizeof(MultiDataType);
	for (int ids=0; ids<nds; ids++)
	{
		for (int idt=0; idt<ndt; idt++)
		{
			if (dataSetNames[0][ids] == MultiDataDataSetNames[idt])
			{
				dataSetIndexMap[MultiDataDataTypes[idt]] = ids;
			}
		}
	}
}

/*
* Update the value for the given row.
*/
void CalvinCHPMultiDataFileUpdater::UpdateMultiData(MultiDataType dataType, int row, const ProbeSetMultiDataGenotypeData &entry)
{
	int dsIndex = dataSetIndexMap[dataType];
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 1, entry.call);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 2, entry.confidence);
}

/*
* Update the value for the given row.
*/
void CalvinCHPMultiDataFileUpdater::UpdateMultiData(MultiDataType dataType, int row, const ProbeSetMultiDataGenotypeData &entry, const vector<ColumnInfo> &metricColumns)
{
	int dsIndex = dataSetIndexMap[dataType];
	UpdateMultiData(dataType, row, entry);
	UpdateMetrics(dsIndex, row, 3, entry.metrics, metricColumns);
}

/*
* Update the value for the given row.
*/
void CalvinCHPMultiDataFileUpdater::UpdateMultiData(MultiDataType dataType, int row, const ProbeSetMultiDataCopyNumberData &entry, const vector<ColumnInfo> &metricColumns)
{
	int dsIndex = dataSetIndexMap[dataType];
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 1, entry.chr);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 2, entry.position);
	UpdateMetrics(dsIndex, row, 3, entry.metrics, metricColumns);
}

/*
* Update the value for the given row.
*/
void CalvinCHPMultiDataFileUpdater::UpdateMultiData(MultiDataType dataType, int row, const ProbeSetMultiDataCytoRegionData &entry, const vector<ColumnInfo> &metricColumns)
{
	int dsIndex = dataSetIndexMap[dataType];
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 1, entry.chr);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 2, entry.startPosition);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 3, entry.stopPosition);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 4, entry.call);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 5, entry.confidenceScore);
	UpdateMetrics(dsIndex, row, 6, entry.metrics, metricColumns);
}

/*
* Update the value for the given row.
*/
void CalvinCHPMultiDataFileUpdater::UpdateMultiData(MultiDataType dataType, int row, const ProbeSetMultiDataExpressionData &entry)
{
	int dsIndex = dataSetIndexMap[dataType];
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 1, entry.quantification);
}

/*
* Update the value for the given row.
*/
void CalvinCHPMultiDataFileUpdater::UpdateMultiData(MultiDataType dataType, int row, const ProbeSetMultiDataExpressionData &entry, const vector<ColumnInfo> &metricColumns)
{
	int dsIndex = dataSetIndexMap[dataType];
	UpdateMultiData(dataType, row, entry);
	UpdateMetrics(dsIndex, row, 2, entry.metrics, metricColumns);
}

void CalvinCHPMultiDataFileUpdater::UpdateMultiData(MultiDataType dataType, int row, const affymetrix_calvin_data::ProbeSetMultiDataCopyNumberVariationRegionData
																										&entry, const std::vector<ColumnInfo> &metricColumns)
{
	int dsIndex = dataSetIndexMap[dataType];  
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 1, entry.signal);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 2, entry.call);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 3, entry.confidenceScore);
	UpdateMetrics(dsIndex, row, 4, entry.metrics, metricColumns);
}

/*
* Update the value for the given row.
*/
void CalvinCHPMultiDataFileUpdater::UpdateMultiData(MultiDataType dataType, int row, const DmetBiAllelicData &entry)
{
	vector<ColumnInfo> metricColumns;
	UpdateMultiData(dataType, row, entry, metricColumns);
}

/*
* Update the value for the given row.
*/
void CalvinCHPMultiDataFileUpdater::UpdateMultiData(MultiDataType dataType, int row, const DmetBiAllelicData &entry, const vector<ColumnInfo> &metricColumns)
{
	int dsIndex = dataSetIndexMap[dataType];
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 1, entry.call);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 2, entry.confidence);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 3, entry.force);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 4, entry.signalA);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 5, entry.signalB);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 6, entry.contextA);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 7, entry.contextB);
	if(metricColumns.size() > 0)
	{
		UpdateMetrics(dsIndex, row, 8, entry.metrics, metricColumns);
	}
}

/*
* Update the value for the given row.
*/
void CalvinCHPMultiDataFileUpdater::UpdateMultiData(MultiDataType dataType, int row, const DmetCopyNumberData &entry)
{
	vector<ColumnInfo> metricColumns;
	UpdateMultiData(dataType, row, entry, metricColumns);
}

/*
* Update the value for the given row.
*/
void CalvinCHPMultiDataFileUpdater::UpdateMultiData(MultiDataType dataType, int row, const DmetCopyNumberData &entry, const vector<ColumnInfo> &metricColumns)
{
	int dsIndex = dataSetIndexMap[dataType];
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 1, entry.call);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 2, entry.confidence);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 3, entry.force);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 4, entry.estimate);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 5, entry.lower);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 6, entry.upper);
	if(metricColumns.size() > 0)
	{
		UpdateMetrics(dsIndex, row, 7, entry.metrics, metricColumns);
	}
}

/*
* Update the value for the given row.
*/
void CalvinCHPMultiDataFileUpdater::UpdateMultiData(MultiDataType dataType, int row, const DmetMultiAllelicData &entry)
{
	vector<ColumnInfo> metricColumns;
	UpdateMultiData(dataType, row, entry, metricColumns);
}

/*
* Update the value for the given row.
*/
void CalvinCHPMultiDataFileUpdater::UpdateMultiData(MultiDataType dataType, int row, const DmetMultiAllelicData &entry, const vector<ColumnInfo> &metricColumns)
{
	int dsIndex = dataSetIndexMap[dataType];
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 1, entry.call);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 2, entry.confidence);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 3, entry.force);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 4, entry.alleleCount);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 5, entry.signalA);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 6, entry.signalB);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 7, entry.signalC);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 8, entry.signalD);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 9, entry.signalE);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 10, entry.signalF);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 11, entry.contextA);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 12, entry.contextB);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 13, entry.contextC);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 14, entry.contextD);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 15, entry.contextE);
	Update(MULTI_DATA_DATA_GROUP, dsIndex, row, 16, entry.contextF);
	if(metricColumns.size() > 0)
	{
		UpdateMetrics(dsIndex, row, 17, entry.metrics, metricColumns);
	}
}

void CalvinCHPMultiDataFileUpdater::UpdateMetrics(int dataSetIndex, int row, int startColIndex, const std::vector<affymetrix_calvin_parameter::ParameterNameValueType> &metrics, const std::vector<ColumnInfo> &metricColumns)
{
	int colIndex=0;
	for (vector<ParameterNameValueType>::const_iterator it=metrics.begin(); it!=metrics.end(); it++)
	{
		switch (metricColumns[colIndex].GetColumnType())
		{
		case ByteColType:
			Update(MULTI_DATA_DATA_GROUP, dataSetIndex, row, colIndex+startColIndex, it->GetValueInt8());
			break;

		case UByteColType:
			Update(MULTI_DATA_DATA_GROUP, dataSetIndex, row, colIndex+startColIndex, it->GetValueUInt8());
			break;

		case ShortColType:
			Update(MULTI_DATA_DATA_GROUP, dataSetIndex, row, colIndex+startColIndex, it->GetValueInt16());
			break;

		case UShortColType:
			Update(MULTI_DATA_DATA_GROUP, dataSetIndex, row, colIndex+startColIndex, it->GetValueUInt16());
			break;

		case IntColType:
			Update(MULTI_DATA_DATA_GROUP, dataSetIndex, row, colIndex+startColIndex, it->GetValueInt32());
			break;

		case UIntColType:
			Update(MULTI_DATA_DATA_GROUP, dataSetIndex, row, colIndex+startColIndex, it->GetValueUInt32());
			break;

		case FloatColType:
			Update(MULTI_DATA_DATA_GROUP, dataSetIndex, row, colIndex+startColIndex, it->GetValueFloat());
			break;

		case ASCIICharColType:
			Update(MULTI_DATA_DATA_GROUP, dataSetIndex, row, colIndex+startColIndex, it->GetValueAscii());
			break;

		case UnicodeCharColType:
			Update(MULTI_DATA_DATA_GROUP, dataSetIndex, row, colIndex+startColIndex, it->GetValueText());
			break;
		}
		++colIndex;
	}
}
