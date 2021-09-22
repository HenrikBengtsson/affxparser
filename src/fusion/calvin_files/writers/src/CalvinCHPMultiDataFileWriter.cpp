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

#include "calvin_files/writers/src/CalvinCHPMultiDataFileWriter.h"
//
#include "calvin_files/data/src/CHPMultiDataData.h"
//

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
    int ng = writer->GetDataGroupCnt();
    for (int ig=0; ig<ng; ig++)
    {
	    DataGroupWriter &dataGroupWriter = writer->GetDataGroupWriter(ig);
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
    int ig = data->GetDataGroupIndex(dataType);
    DataGroupWriter &dataGroupWriter = writer->GetDataGroupWriter(ig);
    writer->SeekFromBeginPos(entryPos[dataType]);
    dataSetWriter = &dataGroupWriter.GetDataSetWriter(dataTypeToIndex[dataType]);
    maxName = data->GetMaxProbeSetName(dataType);
    if (dataType == FamilialSegmentOverlapsMultiDataType)
    {
        std::map<MultiDataType, DataSetInfo> &dinfo = data->GetDataSetInfo();
        maxSegmentType = dinfo[dataType].maxSegmentType;
        maxReferenceSegmentID = dinfo[dataType].maxReferenceSegmentID;
        maxFamilialSegmentID = dinfo[dataType].maxFamilialSegmentID;
    }
    else if (dataType == FamilialSamplesMultiDataType)
    {
        std::map<MultiDataType, DataSetInfo> &dinfo = data->GetDataSetInfo();
        maxFamilialARRID = dinfo[dataType].maxFamilialARRID;
        maxFamilialCHPID = dinfo[dataType].maxFamilialCHPID;
        maxFamilialCHPFile = dinfo[dataType].maxFamilialCHPFile;
        maxFamilialRole = dinfo[dataType].maxFamilialRole;
    }
    currentDataType = dataType;
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::ProbeSetMultiDataGenotypeData & p)
{
	writer->SeekFromBeginPos(entryPos[currentDataType]);
	dataSetWriter->Write(p.name, maxName);
	dataSetWriter->Write(p.call);
	dataSetWriter->Write(p.confidence);
	WriteMetrics(p.metrics);
	entryPos[currentDataType] = writer->GetFilePos();
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::ProbeSetMultiDataCopyNumberData & p)
{
	writer->SeekFromBeginPos(entryPos[currentDataType]);
	dataSetWriter->Write(p.name, maxName);
	dataSetWriter->Write(p.chr);
	dataSetWriter->Write(p.position);
	WriteMetrics(p.metrics);
	entryPos[currentDataType] = writer->GetFilePos();
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::AllelePeaks & p)
{
	writer->SeekFromBeginPos(entryPos[currentDataType]);
	dataSetWriter->Write(p.name, maxName);
	dataSetWriter->Write(p.chr);
	dataSetWriter->Write(p.position);
	WriteMetrics(p.peaks);
	entryPos[currentDataType] = writer->GetFilePos();
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::MarkerABSignals & p)
{
	writer->SeekFromBeginPos(entryPos[currentDataType]);
	dataSetWriter->Write(p.index);
	WriteMetrics(p.metrics);	
	entryPos[currentDataType] = writer->GetFilePos();
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::CytoGenotypeCallData & p)
{
	writer->SeekFromBeginPos(entryPos[currentDataType]);
	dataSetWriter->Write(p.index);
	dataSetWriter->Write(p.call);
	dataSetWriter->Write(p.confidence);
	dataSetWriter->Write(p.forcedCall);
	dataSetWriter->Write(p.aSignal);
	dataSetWriter->Write(p.bSignal);
	dataSetWriter->Write(p.signalStrength);
	dataSetWriter->Write(p.contrast);
	WriteMetrics(p.metrics);	
	entryPos[currentDataType] = writer->GetFilePos();
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::ProbeSetMultiDataCytoRegionData & p)
{
	writer->SeekFromBeginPos(entryPos[currentDataType]);
	dataSetWriter->Write(p.name, maxName);
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
	writer->SeekFromBeginPos(entryPos[currentDataType]);
	dataSetWriter->Write(p.name, maxName);
	dataSetWriter->Write(p.quantification);
	WriteMetrics(p.metrics);
	entryPos[currentDataType] = writer->GetFilePos();
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::ProbeSetMultiDataCopyNumberVariationRegionData & p)
{
	writer->SeekFromBeginPos(entryPos[currentDataType]);
	dataSetWriter->Write(p.name, maxName);
	dataSetWriter->Write(p.signal);
	dataSetWriter->Write(p.call);
	dataSetWriter->Write(p.confidenceScore);
	WriteMetrics(p.metrics);
	entryPos[currentDataType] = writer->GetFilePos();
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::DmetBiAllelicData &p)
{
	writer->SeekFromBeginPos(entryPos[currentDataType]);
	dataSetWriter->Write(p.name, maxName);
	dataSetWriter->Write(p.call);
	dataSetWriter->Write(p.confidence);
	dataSetWriter->Write(p.force);
	dataSetWriter->Write(p.signalA);
	dataSetWriter->Write(p.signalB);
	dataSetWriter->Write(p.contextA);
	dataSetWriter->Write(p.contextB);
	WriteMetrics(p.metrics);
	entryPos[currentDataType] = writer->GetFilePos();
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::DmetCopyNumberData &p)
{
	writer->SeekFromBeginPos(entryPos[currentDataType]);
	dataSetWriter->Write(p.name, maxName);
	dataSetWriter->Write(p.call);
	dataSetWriter->Write(p.confidence);
	dataSetWriter->Write(p.force);
	dataSetWriter->Write(p.estimate);
	dataSetWriter->Write(p.lower);
	dataSetWriter->Write(p.upper);
	WriteMetrics(p.metrics);
	entryPos[currentDataType] = writer->GetFilePos();
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::DmetMultiAllelicData &p)
{
	writer->SeekFromBeginPos(entryPos[currentDataType]);
	dataSetWriter->Write(p.name, maxName);
	dataSetWriter->Write(p.call);
	dataSetWriter->Write(p.confidence);
	dataSetWriter->Write(p.force);
	dataSetWriter->Write(p.alleleCount);
	dataSetWriter->Write(p.signalA);
	dataSetWriter->Write(p.signalB);
	dataSetWriter->Write(p.signalC);
	dataSetWriter->Write(p.signalD);
	dataSetWriter->Write(p.signalE);
	dataSetWriter->Write(p.signalF);
	dataSetWriter->Write(p.contextA);
	dataSetWriter->Write(p.contextB);
	dataSetWriter->Write(p.contextC);
	dataSetWriter->Write(p.contextD);
	dataSetWriter->Write(p.contextE);
	dataSetWriter->Write(p.contextF);
	WriteMetrics(p.metrics);
	entryPos[currentDataType] = writer->GetFilePos();
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::ChromosomeMultiDataSummaryData & p)
{
	writer->SeekFromBeginPos(entryPos[currentDataType]);
    dataSetWriter->Write(p.chr);
	dataSetWriter->Write(p.display, maxName);
	dataSetWriter->Write(p.startIndex);
	dataSetWriter->Write(p.markerCount);
    dataSetWriter->Write(p.minSignal);
    dataSetWriter->Write(p.maxSignal);
    dataSetWriter->Write(p.medianCnState);
    dataSetWriter->Write(p.homFrequency);
    dataSetWriter->Write(p.hetFrequency);
	WriteMetrics(p.metrics);
	entryPos[currentDataType] = writer->GetFilePos();
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::ChromosomeSegmentData & p)
{
	writer->SeekFromBeginPos(entryPos[currentDataType]);
    dataSetWriter->Write(p.segmentId);
	dataSetWriter->Write(p.chr);
	dataSetWriter->Write(p.startPosition);
	dataSetWriter->Write(p.stopPosition);
	dataSetWriter->Write(p.markerCount);
	dataSetWriter->Write(p.meanMarkerDistance);
	WriteMetrics(p.metrics);
	entryPos[currentDataType] = writer->GetFilePos();
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::ChromosomeSegmentDataEx & p)
{
	writer->SeekFromBeginPos(entryPos[currentDataType]);
    dataSetWriter->Write(p.segmentId);
	dataSetWriter->Write(p.referenceSampleKey);
	dataSetWriter->Write(p.familialSampleKey);
	dataSetWriter->Write(p.chr);
	dataSetWriter->Write(p.startPosition);
	dataSetWriter->Write(p.stopPosition);
	dataSetWriter->Write(p.call);
	dataSetWriter->Write(p.confidence);
    dataSetWriter->Write(p.markerCount);
	dataSetWriter->Write(p.homozygosity);
	dataSetWriter->Write(p.heterozygosity);
	WriteMetrics(p.metrics);
	entryPos[currentDataType] = writer->GetFilePos();
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::FamilialSample & p)
{
	writer->SeekFromBeginPos(entryPos[currentDataType]);
    dataSetWriter->Write(p.sampleKey);
	dataSetWriter->Write(p.arrID, maxFamilialARRID);
	dataSetWriter->Write(p.chpID, maxFamilialCHPID);
	dataSetWriter->Write(p.chpFilename, maxFamilialCHPFile);
	dataSetWriter->Write(p.role, maxFamilialRole);
    dataSetWriter->Write((u_int8_t)(p.roleValidity == true ? 1 : 0));
	dataSetWriter->Write(p.roleConfidence);
	entryPos[currentDataType] = writer->GetFilePos();
}

void CHPMultiDataFileWriter::WriteEntry(const affymetrix_calvin_data::FamilialSegmentOverlap & p)
{
	writer->SeekFromBeginPos(entryPos[currentDataType]);
    dataSetWriter->Write(p.segmentType, maxSegmentType);
	dataSetWriter->Write(p.referenceSampleKey);
	dataSetWriter->Write(p.referenceSegmentID, maxReferenceSegmentID);
	dataSetWriter->Write(p.familialSampleKey);
	dataSetWriter->Write(p.familialSegmentID, maxFamilialSegmentID);
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

