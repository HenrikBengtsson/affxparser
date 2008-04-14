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

#include "CalvinCHPMultiDataFileWriter.h"
#include "CHPMultiDataData.h"

using namespace std;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;

CHPMultiDataFileWriter::CHPMultiDataFileWriter(CHPMultiDataData &p)
{
    data = &p;
    map<MultiDataType, DataSetInfo> &info = p.GetDataSetInfo();
    map<MultiDataType, DataSetInfo>::iterator it;
    for (it=info.begin(); it!=info.end(); it++)
    {
        dataTypeToIndex[it->first] = it->second.dataSetIndex;
        indexToDataType[it->second.dataSetIndex] = it->first;
    }
	writer = new GenericFileWriter(p.GetFileHeader());
	WriteHeaders();
}

CHPMultiDataFileWriter::~CHPMultiDataFileWriter()
{
    data = NULL;
	delete writer;
}

void CHPMultiDataFileWriter::WriteHeaders()
{
	writer->WriteHeader();
	DataGroupWriter &dataGroupWriter = writer->GetDataGroupWriter(0);
	dataGroupWriter.WriteHeader();

    int32_t n = dataGroupWriter.GetDataSetWriterCnt();
    for (int32_t i=0; i<n; i++)
    {
        dataSetWriter = &dataGroupWriter.GetDataSetWriter(i);
		dataSetWriter->WriteHeader();
		entryPos[indexToDataType[i]] = SetFilePositions();
		dataGroupWriter.UpdateNextDataGroupPos();
	}
}

int32_t CHPMultiDataFileWriter::SetFilePositions()
{
	int32_t dataSetSz = dataSetWriter->GetDataSetSize();
	int32_t offset = writer->GetFilePos();
	writer->SeekFromCurrentPos(dataSetSz + 1);
	dataSetWriter->UpdateNextDataSetOffset();
	return offset;
}

void CHPMultiDataFileWriter::SeekToDataSet(MultiDataType dataType)
{
	DataGroupWriter &dataGroupWriter = writer->GetDataGroupWriter(0);
	writer->SeekFromBeginPos(entryPos[dataType]);
	dataSetWriter = &dataGroupWriter.GetDataSetWriter(dataTypeToIndex[dataType]);
    maxProbeSetName = data->GetMaxProbeSetName(dataType);
    currentDataType = dataType;
    seekPos = true;
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::ProbeSetMultiDataGenotypeData & p)
{
    //if (seekPos == true)
    {
        writer->SeekFromBeginPos(entryPos[currentDataType]);
        seekPos = false;
    }
    dataSetWriter->Write(p.name, maxProbeSetName);
	dataSetWriter->Write(p.call);
	dataSetWriter->Write(p.confidence);
    WriteMetrics(p.metrics);
    entryPos[currentDataType] = writer->GetFilePos();
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::ProbeSetMultiDataCopyNumberData & p)
{
    //if (seekPos == true)
    {
        writer->SeekFromBeginPos(entryPos[currentDataType]);
        seekPos = false;
    }
    dataSetWriter->Write(p.name, maxProbeSetName);
	dataSetWriter->Write(p.chr);
	dataSetWriter->Write(p.position);
    WriteMetrics(p.metrics);
    entryPos[currentDataType] = writer->GetFilePos();
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::ProbeSetMultiDataCytoRegionData & p)
{
    //if (seekPos == true)
    {
        writer->SeekFromBeginPos(entryPos[currentDataType]);
        seekPos = false;
    }
    dataSetWriter->Write(p.name, maxProbeSetName);
    dataSetWriter->Write(p.chr);
    dataSetWriter->Write(p.startPosition);
    dataSetWriter->Write(p.stopPosition);
    dataSetWriter->Write(p.call);
    dataSetWriter->Write(p.confidenceScore);
    WriteMetrics(p.metrics);
    entryPos[currentDataType] = writer->GetFilePos();
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::ProbeSetMultiDataExpressionData & p)
{
    //if (seekPos == true)
    {
        writer->SeekFromBeginPos(entryPos[currentDataType]);
        seekPos = false;
    }
    dataSetWriter->Write(p.name, maxProbeSetName);
	dataSetWriter->Write(p.quantification);
    WriteMetrics(p.metrics);
    entryPos[currentDataType] = writer->GetFilePos();
}

void CHPMultiDataFileWriter::WriteMetrics(const std::vector<affymetrix_calvin_parameter::ParameterNameValueType> &metrics)
{
	int ncols = (int) metrics.size();
	for (int icol=0; icol<ncols; icol++)
	{
		const ParameterNameValueType &nv = metrics[icol];
		switch (nv.GetParameterType())
		{
			case ParameterNameValueType::Int8Type:
				dataSetWriter->Write(nv.GetValueInt8());
				break;

			case ParameterNameValueType::UInt8Type:
				dataSetWriter->Write(nv.GetValueUInt8());
				break;

			case ParameterNameValueType::Int16Type:
				dataSetWriter->Write(nv.GetValueInt16());
				break;

			case ParameterNameValueType::UInt16Type:
				dataSetWriter->Write(nv.GetValueUInt16());
				break;

			case ParameterNameValueType::Int32Type:
				dataSetWriter->Write(nv.GetValueInt32());
				break;

			case ParameterNameValueType::UInt32Type:
				dataSetWriter->Write(nv.GetValueUInt32());
				break;

			case ParameterNameValueType::FloatType:
				dataSetWriter->Write(nv.GetValueFloat());
				break;

			case ParameterNameValueType::AsciiType:
				dataSetWriter->Write(nv.GetValueAscii(), data->GetMetricColumnLength(currentDataType, icol));
				break;

			case ParameterNameValueType::TextType:
				dataSetWriter->Write(nv.GetValueText(), data->GetMetricColumnLength(currentDataType, icol));
				break;
        // @todo check this is ok.
    case ParameterNameValueType::UnknownType:
      break;
		}
	}
}

