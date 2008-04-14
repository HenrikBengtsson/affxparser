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


#include "CalvinCHPMultiDataFileUpdater.h"

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
            dataSetIndexMap[MultiDataDataTypes[idt]] = ids;
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
void CalvinCHPMultiDataFileUpdater::UpdateMultiData(MultiDataType dataType, int row, const ProbeSetMultiDataGenotypeData &entry, const vector<ColumnInfo> &metricColumns)
{
    int dsIndex = dataSetIndexMap[dataType];
    UpdateMultiData(dataType, row, entry);
    UpdateMetrics(dsIndex, row, 3, entry.metrics, metricColumns);
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
