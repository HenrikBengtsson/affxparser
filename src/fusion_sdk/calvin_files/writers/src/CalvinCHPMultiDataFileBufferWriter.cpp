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


/**
 * @file   CalvinCHPMultiDataFileBufferWriter.cpp
 * @brief  Class for writing MultiDatas to a buffer before writing to CHP files.
 */

#ifdef _MSC_VER
#pragma warning(disable: 4996) // don't show deprecated warnings.
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#endif

//
#include "calvin_files/writers/src/CalvinCHPMultiDataFileBufferWriter.h"
//
#include "calvin_files/writers/src/CalvinCHPFileUpdater.h"
#include "calvin_files/writers/src/FileOutput.h"
//
#include "file/FileIO.h"
//
#include <cstring>
#include <string.h>
#include <string>
//




using namespace std;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_data;
using namespace affymetrix_calvin_parameter;

CHPMultiDataFileBufferWriter::CHPMultiDataFileBufferWriter()
{
	m_BufferSize = 0; 
	m_MaxBufferSize = MAX_BUFFER_SIZE;
}

CHPMultiDataFileBufferWriter::~CHPMultiDataFileBufferWriter()
{
	FlushBuffer();
	Cleanup();
}

static void ClearBuffer(std::vector< std::vector<char *> > &buffers, std::vector<int> &rowIndexes)
{
	for (int target=0; target<(int)buffers.size(); target++) 
	{ 
		int nbufs = (int) buffers[target].size();
		for (int ibuf=0; ibuf < nbufs; ibuf++)
		{
			delete[] buffers[target][ibuf];
            buffers[target][ibuf] = NULL;
		}
		buffers[target].clear();
	}
	buffers.clear();
	rowIndexes.clear();
}

void CHPMultiDataFileBufferWriter::Cleanup()
{
    std::map<MultiDataType, std::vector< std::vector<char *> > >::iterator it;
    std::map<MultiDataType, std::vector<int> >::iterator rit;
    for (rit = rowIndexes.begin(), it = dataBuffers.begin(); rit != rowIndexes.end() && it != dataBuffers.end(); rit++, it++)
    {
        ClearBuffer(it->second, rit->second);
    }
    rowIndexes.clear();
    dataBuffers.clear();
    dataBufferSz.clear();
}

void CHPMultiDataFileBufferWriter::Initialize(vector<string> *CHPFileNames, std::vector<MultiDataType> &dataTypes, std::map<MultiDataType, int> &maxProbeSetNmLn)
{
    std::map<MultiDataType, int> empty;
    Initialize(CHPFileNames, dataTypes, maxProbeSetNmLn, empty, empty, empty, empty, empty, empty, empty);
}

void CHPMultiDataFileBufferWriter::Initialize(vector<string> *CHPFileNames,
											  vector<MultiDataType> &dataTypes,
                                              map<MultiDataType, int> &maxSegmentTypeLn,
                                              map<MultiDataType, int> &maxReferenceSegmentIDLn,
                                              map<MultiDataType, int> &maxFamilialSegmentIDLn,
										  	  map<MultiDataType, int> &maxFamilialARRIDLn,
											  map<MultiDataType, int> &maxFamilialCHPIDLn,
											  map<MultiDataType, int> &maxFamilialCHPFilenameLn,
											  map<MultiDataType, int> &maxFamilialRoleLn
)
{
    std::map<MultiDataType, int> empty;
    Initialize(CHPFileNames,
        dataTypes,
        empty,
        maxSegmentTypeLn,
        maxReferenceSegmentIDLn,
        maxFamilialSegmentIDLn,
        maxFamilialARRIDLn,
        maxFamilialCHPIDLn,
        maxFamilialCHPFilenameLn,
        maxFamilialRoleLn);
}

void CHPMultiDataFileBufferWriter::Initialize
(
    std::vector<std::string> *CHPFileNames,
    std::vector<MultiDataType> &dataTypes,
    std::map<MultiDataType, int> &maxProbeSetNmLn,
    std::map<MultiDataType, int> &maxSegmentTypeLn,
    std::map<MultiDataType, int> &maxReferenceSegmentIDLn,
    std::map<MultiDataType, int> &maxFamilialSegmentIDLn,	  
    std::map<MultiDataType, int> &maxFamilialARRIDLn,
    std::map<MultiDataType, int> &maxFamilialCHPIDLn,
    std::map<MultiDataType, int> &maxFamilialCHPFilenameLn,
    std::map<MultiDataType, int> &maxFamilialRoleLn
)
{
	chpFileNames = CHPFileNames;
    maxSegmentType = maxSegmentTypeLn;
    maxReferenceSegmentID = maxReferenceSegmentIDLn;
    maxFamilialSegmentID = maxFamilialSegmentIDLn;
	maxFamilialARRID = maxFamilialARRIDLn;
	maxFamilialCHPID = maxFamilialCHPIDLn;
	maxFamilialCHPFilename = maxFamilialCHPFilenameLn;
	maxFamilialRole = maxFamilialRoleLn;
    maxProbeSetNameLength = maxProbeSetNmLn;
	Cleanup();
	for (int i=0; i<(int)chpFileNames->size(); i++)
	{
		for (int itype=0; itype<(int)dataTypes.size(); itype++)
		{
			vector<char*> buffer;
            dataBuffers[dataTypes[itype]].push_back(buffer);
            rowIndexes[dataTypes[itype]].push_back(0);
		}
	}
	m_BufferSize = 0;
	for (int i=0; i<(int)dataTypes.size(); i++)    
	{
        dataBufferSz[dataTypes[i]] = 0;
	}
}

// static to keep function local to this file - perhaps this should be a static private member function? -AW
static int GetMetricBufferSize(const std::vector<affymetrix_calvin_parameter::ParameterNameValueType> &metrics)
{
	int bufferSize = 0;
	int ncols = (int) metrics.size();
	for (int icol=0; icol<ncols; icol++)
	{
		const ParameterNameValueType &nv = metrics[icol];
		switch (nv.GetParameterType())
		{
		case ParameterNameValueType::Int8Type:
			bufferSize += sizeof(nv.GetValueInt8());
			break;

		case ParameterNameValueType::UInt8Type:
			bufferSize += sizeof(nv.GetValueUInt8());
			break;

		case ParameterNameValueType::Int16Type:
			bufferSize += sizeof(nv.GetValueInt16());
			break;

		case ParameterNameValueType::UInt16Type:
			bufferSize += sizeof(nv.GetValueUInt16());
			break;

		case ParameterNameValueType::Int32Type:
			bufferSize += sizeof(nv.GetValueInt32());
			break;

		case ParameterNameValueType::UInt32Type:
			bufferSize += sizeof(nv.GetValueUInt32());
			break;

		case ParameterNameValueType::FloatType:
			bufferSize += sizeof(nv.GetValueFloat());
			break;

		case ParameterNameValueType::AsciiType:
			bufferSize += sizeof(int);
			bufferSize += (int)(nv.GetValueAscii().length() * sizeof(char));
			break;

		case ParameterNameValueType::TextType:
			bufferSize += sizeof(int);
			bufferSize += (int)(nv.GetValueText().length() * sizeof(wchar_t));
			break;

		case ParameterNameValueType::UnknownType:
			break;
		}
	}
	return bufferSize;
}

// static to keep function local to this file - perhaps this should be a static private member function? -AW
static void CopyMetricToBuffer(const std::vector<affymetrix_calvin_parameter::ParameterNameValueType> &metrics, char * &pbuffer)
{
	int ncols = (int) metrics.size();
	for (int icol=0; icol<ncols; icol++)
	{
		const ParameterNameValueType &nv = metrics[icol];
		switch (nv.GetParameterType())
		{
		case ParameterNameValueType::Int8Type:
			*pbuffer = nv.GetValueInt8();
			pbuffer += sizeof(int8_t);
			break;

		case ParameterNameValueType::UInt8Type:
			*pbuffer = nv.GetValueUInt8();
			pbuffer += sizeof(u_int8_t);
			break;

		case ParameterNameValueType::Int16Type:
			MmSetUInt16_N((uint16_t *)pbuffer, nv.GetValueInt16());
			pbuffer += sizeof(int16_t);
			break;

		case ParameterNameValueType::UInt16Type:
			MmSetUInt16_N((uint16_t *)pbuffer, nv.GetValueUInt16());
			pbuffer += sizeof(u_int16_t);
			break;

		case ParameterNameValueType::Int32Type:
			MmSetUInt32_N((uint32_t *)pbuffer, nv.GetValueInt32());
			pbuffer += sizeof(int32_t);
			break;

		case ParameterNameValueType::UInt32Type:
			MmSetUInt32_N((uint32_t *)pbuffer, nv.GetValueUInt32());
			pbuffer += sizeof(u_int32_t);
			break;

		case ParameterNameValueType::FloatType:
			MmSetFloat_N((float *)pbuffer, nv.GetValueFloat());
			pbuffer += sizeof(float);
			break;

		case ParameterNameValueType::AsciiType:
			{
				string s = nv.GetValueAscii();
				int l = (int) s.length();
				MmSetUInt32_N((uint32_t *)pbuffer, l);
				pbuffer += sizeof(int32_t);
				strcpy(pbuffer, s.c_str());
				pbuffer += l;
			}
			break;

		case ParameterNameValueType::TextType:
			// @todo write out wide strings
			assert(0==1);
			break;

		case ParameterNameValueType::UnknownType:
			break;
		}
	}
}

void CHPMultiDataFileBufferWriter::WriteMultiDataGenotypeEntry(MultiDataType dataType, int target, const ProbeSetMultiDataGenotypeData &entry)
{
	if (dataBufferSz[dataType] == 0)
	{
		dataBufferSz[dataType] = maxProbeSetNameLength[dataType] + sizeof(int) + sizeof(entry.call) + sizeof(entry.confidence) + GetMetricBufferSize(entry.metrics);
	}
	char *buffer = new char[dataBufferSz[dataType]];
	memset(buffer, 0, dataBufferSz[dataType]);
	char *pbuffer = buffer;
	MmSetUInt32_N((uint32_t *)pbuffer, entry.name.length());
	pbuffer += sizeof(int);
	memcpy(pbuffer, entry.name.c_str(), entry.name.length());
	pbuffer += maxProbeSetNameLength[dataType];
	*pbuffer = entry.call;
	pbuffer += sizeof(char);
	MmSetFloat_N((float *)pbuffer, entry.confidence);
	pbuffer += sizeof(float);
	CopyMetricToBuffer(entry.metrics, pbuffer);
	dataBuffers[dataType][target].push_back(buffer);
	m_BufferSize += dataBufferSz[dataType];

	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::WriteMultiDataExpressionEntry(MultiDataType dataType, int target, const ProbeSetMultiDataExpressionData &entry)
{
	if (dataBufferSz[dataType] == 0)
		dataBufferSz[dataType] = maxProbeSetNameLength[dataType] + sizeof(int) + sizeof(entry.quantification) + GetMetricBufferSize(entry.metrics);
	char *buffer = new char[dataBufferSz[dataType]];
	memset(buffer, 0, dataBufferSz[dataType]);
	char *pbuffer = buffer;
	MmSetUInt32_N((uint32_t *)pbuffer, entry.name.length());
	pbuffer += sizeof(int);
	memcpy(pbuffer, entry.name.c_str(), entry.name.length());
	pbuffer += maxProbeSetNameLength[dataType];
	MmSetFloat_N((float *)pbuffer, entry.quantification);
	pbuffer += sizeof(float);
	CopyMetricToBuffer(entry.metrics, pbuffer);
	dataBuffers[dataType][target].push_back(buffer);
	m_BufferSize += dataBufferSz[dataType];

	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::WriteMultiDataCopyNumberEntry(MultiDataType dataType, int target, const ProbeSetMultiDataCopyNumberData &entry)
{
	if (dataBufferSz[dataType] == 0)
		dataBufferSz[dataType] = maxProbeSetNameLength[dataType] + sizeof(int) + sizeof(entry.chr) + sizeof(entry.position) + GetMetricBufferSize(entry.metrics);
	char *buffer = new char[dataBufferSz[dataType]];
	memset(buffer, 0, dataBufferSz[dataType]);
	char *pbuffer = buffer;
	MmSetUInt32_N((uint32_t *)pbuffer, entry.name.length());
	pbuffer += sizeof(int);
	memcpy(pbuffer, entry.name.c_str(), entry.name.length());
	pbuffer += maxProbeSetNameLength[dataType];
	MmSetUInt8((uint8_t *)pbuffer, entry.chr);
	pbuffer += sizeof(entry.chr);
	MmSetUInt32_N((uint32_t *)pbuffer, entry.position);
	pbuffer += sizeof(entry.position);
	CopyMetricToBuffer(entry.metrics, pbuffer);
	dataBuffers[dataType][target].push_back(buffer);
	m_BufferSize += dataBufferSz[dataType];

	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::WriteEntry(MultiDataType dataType, int target, const AllelePeaks &entry)
{
	if (dataBufferSz[dataType] == 0)
		dataBufferSz[dataType] = maxProbeSetNameLength[dataType] + sizeof(int) + sizeof(entry.chr) + sizeof(entry.position) + GetMetricBufferSize(entry.peaks);
	char *buffer = new char[dataBufferSz[dataType]];
	memset(buffer, 0, dataBufferSz[dataType]);
	char *pbuffer = buffer;
	MmSetUInt32_N((uint32_t *)pbuffer, entry.name.length());
	pbuffer += sizeof(int);
	memcpy(pbuffer, entry.name.c_str(), entry.name.length());
	pbuffer += maxProbeSetNameLength[dataType];
	MmSetUInt8((uint8_t *)pbuffer, entry.chr);
	pbuffer += sizeof(entry.chr);
	MmSetUInt32_N((uint32_t *)pbuffer, entry.position);
	pbuffer += sizeof(entry.position);
	CopyMetricToBuffer(entry.peaks, pbuffer);
	dataBuffers[dataType][target].push_back(buffer);
	m_BufferSize += dataBufferSz[dataType];

	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::WriteEntry(MultiDataType dataType, int target, const MarkerABSignals &entry)
{
	if (dataBufferSz[dataType] == 0)
		dataBufferSz[dataType] = sizeof(entry.index) + GetMetricBufferSize(entry.metrics);
	char *buffer = new char[dataBufferSz[dataType]];
	memset(buffer, 0, dataBufferSz[dataType]);
	char *pbuffer = buffer;

	MmSetUInt32_N((uint32_t *)pbuffer, entry.index);
	pbuffer += sizeof(int);
	CopyMetricToBuffer(entry.metrics, pbuffer);

	dataBuffers[dataType][target].push_back(buffer);
	m_BufferSize += dataBufferSz[dataType];

	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::WriteEntry(MultiDataType dataType, int target, const CytoGenotypeCallData &entry)
{
	if (dataBufferSz[dataType] == 0)
		dataBufferSz[dataType] = sizeof(entry.index) + sizeof(entry.call) + sizeof(entry.confidence) + sizeof(entry.forcedCall)
		+ sizeof(entry.aSignal) + sizeof(entry.bSignal) + sizeof (entry.signalStrength) + sizeof(entry.contrast) +		
		GetMetricBufferSize(entry.metrics);

	char *buffer = new char[dataBufferSz[dataType]];
	memset(buffer, 0, dataBufferSz[dataType]);
	char *pbuffer = buffer;

	MmSetUInt32_N((uint32_t *)pbuffer, entry.index);
	pbuffer += sizeof(entry.index);   
	MmSetUInt8((uint8_t *)pbuffer, entry.call);
	pbuffer += sizeof(entry.call);
	MmSetFloat_N((float *)pbuffer, entry.confidence);
	pbuffer += sizeof(entry.confidence);
	MmSetUInt8((uint8_t *)pbuffer, entry.forcedCall);
	pbuffer += sizeof(entry.forcedCall);
	MmSetFloat_N((float *)pbuffer, entry.aSignal);
	pbuffer += sizeof(entry.confidence);
	MmSetFloat_N((float *)pbuffer, entry.bSignal);
	pbuffer += sizeof(entry.confidence);
	MmSetFloat_N((float *)pbuffer, entry.signalStrength);
	pbuffer += sizeof(entry.confidence);
	MmSetFloat_N((float *)pbuffer, entry.contrast);
	pbuffer += sizeof(entry.confidence);
	CopyMetricToBuffer(entry.metrics, pbuffer);

	dataBuffers[dataType][target].push_back(buffer);
	m_BufferSize += dataBufferSz[dataType];

	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::WriteMultiDataCytoRegionEntry(MultiDataType dataType, int target, const ProbeSetMultiDataCytoRegionData &entry)
{
	if (dataBufferSz[dataType] == 0)
		dataBufferSz[dataType] = maxProbeSetNameLength[dataType] + sizeof(int) + sizeof(entry.chr) +
		sizeof(entry.startPosition) + sizeof(entry.stopPosition) + sizeof(entry.call) +
		sizeof(entry.confidenceScore) + GetMetricBufferSize(entry.metrics);
	char *buffer = new char[dataBufferSz[dataType]];
	memset(buffer, 0, dataBufferSz[dataType]);
	char *pbuffer = buffer;
	MmSetUInt32_N((uint32_t *)pbuffer, entry.name.length());
	pbuffer += sizeof(int);
	memcpy(pbuffer, entry.name.c_str(), entry.name.length());
	pbuffer += maxProbeSetNameLength[dataType];
	MmSetUInt8((uint8_t *)pbuffer, entry.chr);
	pbuffer += sizeof(entry.chr);
	MmSetUInt32_N((uint32_t *)pbuffer, entry.startPosition);
	pbuffer += sizeof(entry.startPosition);
	MmSetUInt32_N((uint32_t *)pbuffer, entry.stopPosition);
	pbuffer += sizeof(entry.stopPosition);
	MmSetUInt8((uint8_t *)pbuffer, entry.call);
	pbuffer += sizeof(entry.call);
	MmSetFloat_N((float *)pbuffer, entry.confidenceScore);
	pbuffer += sizeof(entry.confidenceScore);
	CopyMetricToBuffer(entry.metrics, pbuffer);
	dataBuffers[dataType][target].push_back(buffer);
	m_BufferSize += dataBufferSz[dataType];

	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::WriteMultiDataCopyNumberVariationRegionEntry(MultiDataType dataType, int target,
                                                                                const affymetrix_calvin_data::ProbeSetMultiDataCopyNumberVariationRegionData  &entry)
{
	if (dataBufferSz[dataType] == 0)
		dataBufferSz[dataType] = maxProbeSetNameLength[dataType] + sizeof(int) + sizeof(entry.signal) + sizeof(entry.call) +
		sizeof(entry.confidenceScore) + GetMetricBufferSize(entry.metrics);
	char *buffer = new char[dataBufferSz[dataType]];
	memset(buffer, 0, dataBufferSz[dataType]);
	char *pbuffer = buffer;
	MmSetUInt32_N((uint32_t *)pbuffer, entry.name.length());
	pbuffer += sizeof(int);
	memcpy(pbuffer, entry.name.c_str(), entry.name.length());
	pbuffer += maxProbeSetNameLength[dataType]; 
	MmSetFloat_N((float *)pbuffer, entry.signal);
	pbuffer += sizeof(entry.signal);
	MmSetUInt8((uint8_t *)pbuffer, entry.call);
	pbuffer += sizeof(entry.call);
	MmSetFloat_N((float *)pbuffer, entry.confidenceScore);
	pbuffer += sizeof(entry.confidenceScore);
	CopyMetricToBuffer(entry.metrics, pbuffer);
	dataBuffers[dataType][target].push_back(buffer);
	m_BufferSize += dataBufferSz[dataType];

	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::WriteEntry(MultiDataType dataType, int target, const DmetBiAllelicData &entry)
{
	if (dataBufferSz[dataType] == 0)
	{
		dataBufferSz[dataType] = maxProbeSetNameLength[dataType] + 
												sizeof(int) + 
												sizeof(entry.call) + 
												sizeof(entry.confidence) + 
												sizeof(entry.force) + 
												sizeof(entry.signalA) + 
												sizeof(entry.signalB) + 
												sizeof(entry.contextA) + 
												sizeof(entry.contextB) + 
												GetMetricBufferSize(entry.metrics);
	}
	char *buffer = new char[dataBufferSz[dataType]];
	memset(buffer, 0, dataBufferSz[dataType]);
	char *pbuffer = buffer;
	MmSetUInt32_N((uint32_t*)pbuffer, entry.name.length());
	pbuffer += sizeof(int);
	memcpy(pbuffer, entry.name.c_str(), entry.name.length());
	pbuffer += maxProbeSetNameLength[dataType];
	MmSetUInt8((uint8_t*)pbuffer, entry.call);
	pbuffer += sizeof(entry.call);
	MmSetFloat_N((float*)pbuffer, entry.confidence);
	pbuffer += sizeof(entry.confidence);
	MmSetUInt8((uint8_t*)pbuffer, entry.force);
	pbuffer += sizeof(entry.force);
	MmSetFloat_N((float*)pbuffer, entry.signalA);
	pbuffer += sizeof(entry.signalA);
	MmSetFloat_N((float*)pbuffer, entry.signalB);
	pbuffer += sizeof(entry.signalB);
	MmSetUInt8((uint8_t*)pbuffer, entry.contextA);
	pbuffer += sizeof(entry.contextA);
	MmSetUInt8((uint8_t*)pbuffer, entry.contextB);
	pbuffer += sizeof(entry.contextB);
	CopyMetricToBuffer(entry.metrics, pbuffer);
	dataBuffers[dataType][target].push_back(buffer);
	m_BufferSize += dataBufferSz[dataType];
	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::WriteEntry(MultiDataType dataType, int target, const DmetMultiAllelicData &entry)
{
	if (dataBufferSz[dataType] == 0)
	{
		dataBufferSz[dataType] = maxProbeSetNameLength[dataType] + 
												sizeof(int) + 
												sizeof(entry.call) + 
												sizeof(entry.confidence) + 
												sizeof(entry.force) + 
												sizeof(entry.alleleCount) + 
												sizeof(entry.signalA) + 
												sizeof(entry.signalB) + 
												sizeof(entry.signalC) + 
												sizeof(entry.signalD) + 
												sizeof(entry.signalE) + 
												sizeof(entry.signalF) + 
												sizeof(entry.contextA) + 
												sizeof(entry.contextB) + 
												sizeof(entry.contextC) + 
												sizeof(entry.contextD) + 
												sizeof(entry.contextE) + 
												sizeof(entry.contextF) + 
												GetMetricBufferSize(entry.metrics);
	}
	char *buffer = new char[dataBufferSz[dataType]];
	memset(buffer, 0, dataBufferSz[dataType]);
	char *pbuffer = buffer;
	MmSetUInt32_N((uint32_t *)pbuffer, entry.name.length());
	pbuffer += sizeof(int);
	memcpy(pbuffer, entry.name.c_str(), entry.name.length());
	pbuffer += maxProbeSetNameLength[dataType];
	MmSetUInt8((uint8_t*)pbuffer, entry.call);
	pbuffer += sizeof(entry.call);
	MmSetFloat_N((float*)pbuffer, entry.confidence);
	pbuffer += sizeof(entry.confidence);
	MmSetUInt8((uint8_t*)pbuffer, entry.force);
	pbuffer += sizeof(entry.force);
	MmSetUInt8((uint8_t*)pbuffer, entry.alleleCount);
	pbuffer += sizeof(entry.alleleCount);
	MmSetFloat_N((float*)pbuffer, entry.signalA);
	pbuffer += sizeof(entry.signalA);
	MmSetFloat_N((float*)pbuffer, entry.signalB);
	pbuffer += sizeof(entry.signalB);
	MmSetFloat_N((float*)pbuffer, entry.signalC);
	pbuffer += sizeof(entry.signalC);
	MmSetFloat_N((float*)pbuffer, entry.signalD);
	pbuffer += sizeof(entry.signalD);
	MmSetFloat_N((float*)pbuffer, entry.signalE);
	pbuffer += sizeof(entry.signalE);
	MmSetFloat_N((float*)pbuffer, entry.signalF);
	pbuffer += sizeof(entry.signalF);
	MmSetUInt8((uint8_t*)pbuffer, entry.contextA);
	pbuffer += sizeof(entry.contextA);
	MmSetUInt8((uint8_t*)pbuffer, entry.contextB);
	pbuffer += sizeof(entry.contextB);
	MmSetUInt8((uint8_t*)pbuffer, entry.contextC);
	pbuffer += sizeof(entry.contextC);
	MmSetUInt8((uint8_t*)pbuffer, entry.contextD);
	pbuffer += sizeof(entry.contextD);
	MmSetUInt8((uint8_t*)pbuffer, entry.contextE);
	pbuffer += sizeof(entry.contextE);
	MmSetUInt8((uint8_t*)pbuffer, entry.contextF);
	pbuffer += sizeof(entry.contextF);
	CopyMetricToBuffer(entry.metrics, pbuffer);
	dataBuffers[dataType][target].push_back(buffer);
	m_BufferSize += dataBufferSz[dataType];
	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::WriteEntry(MultiDataType dataType, int target, const DmetCopyNumberData &entry)
{
	if (dataBufferSz[dataType] == 0)
	{
		dataBufferSz[dataType] = maxProbeSetNameLength[dataType] + 
												sizeof(int) + 
												sizeof(entry.call) + 
												sizeof(entry.confidence) + 
												sizeof(entry.force) + 
												sizeof(entry.estimate) + 
												sizeof(entry.lower) + 
												sizeof(entry.upper) + 
												GetMetricBufferSize(entry.metrics);
	}
	char *buffer = new char[dataBufferSz[dataType]];
	memset(buffer, 0, dataBufferSz[dataType]);
	char *pbuffer = buffer;
	MmSetUInt32_N((uint32_t*)pbuffer, entry.name.length());
	pbuffer += sizeof(int);
	memcpy(pbuffer, entry.name.c_str(), entry.name.length());
	pbuffer += maxProbeSetNameLength[dataType];
	MmSetUInt16_N((uint16_t*)pbuffer, entry.call);
	pbuffer += sizeof(entry.call);
	MmSetFloat_N((float*)pbuffer, entry.confidence);
	pbuffer += sizeof(entry.confidence);
	MmSetUInt16_N((uint16_t*)pbuffer, entry.force);
	pbuffer += sizeof(entry.force);
	MmSetFloat_N((float*)pbuffer, entry.estimate);
	pbuffer += sizeof(entry.estimate);
	MmSetFloat_N((float*)pbuffer, entry.lower);
	pbuffer += sizeof(entry.lower);
	MmSetFloat_N((float*)pbuffer, entry.upper);
	pbuffer += sizeof(entry.upper);
	CopyMetricToBuffer(entry.metrics, pbuffer);
	dataBuffers[dataType][target].push_back(buffer);
	m_BufferSize += dataBufferSz[dataType];
	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::WriteEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::ChromosomeMultiDataSummaryData & entry)
{
	if (dataBufferSz[dataType] == 0)
	{
		dataBufferSz[dataType] = 
            sizeof(entry.chr) +
			sizeof(int) + 
			maxProbeSetNameLength[dataType] +
			sizeof(entry.startIndex) +
			sizeof(entry.markerCount) +
            sizeof(entry.minSignal) +
            sizeof(entry.maxSignal) +
            sizeof(entry.medianCnState) +
            sizeof(entry.homFrequency) +
            sizeof(entry.hetFrequency) +
			GetMetricBufferSize(entry.metrics);
	}
	char *buffer = new char[dataBufferSz[dataType]];
	memset(buffer, 0, dataBufferSz[dataType]);
	char *pbuffer = buffer;

    MmSetUInt8((uint8_t*) pbuffer, entry.chr);
	pbuffer += sizeof(u_int8_t);

	MmSetUInt32_N((u_int32_t *)pbuffer, entry.display.length());
	pbuffer += sizeof(int);

	memcpy(pbuffer, entry.display.c_str(), entry.display.length());
	pbuffer += maxProbeSetNameLength[dataType];

	MmSetUInt32_N((u_int32_t *)pbuffer, entry.startIndex);
	pbuffer += sizeof(u_int32_t);

	MmSetUInt32_N((u_int32_t *)pbuffer, entry.markerCount);
	pbuffer += sizeof(u_int32_t);

	MmSetFloat_N((float *)pbuffer, entry.minSignal);
	pbuffer += sizeof(float);

	MmSetFloat_N((float *)pbuffer, entry.maxSignal);
	pbuffer += sizeof(float);

	MmSetFloat_N((float *)pbuffer, entry.medianCnState);
	pbuffer += sizeof(float);

	MmSetFloat_N((float *)pbuffer, entry.homFrequency);
	pbuffer += sizeof(float);

	MmSetFloat_N((float *)pbuffer, entry.hetFrequency);
	pbuffer += sizeof(float);

	CopyMetricToBuffer(entry.metrics, pbuffer);

	dataBuffers[dataType][target].push_back(buffer);
	m_BufferSize += dataBufferSz[dataType];

	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::WriteEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::ChromosomeSegmentData & entry)
{
	if (dataBufferSz[dataType] == 0)
	{
		dataBufferSz[dataType] = 
			sizeof(entry.segmentId) +
            sizeof(entry.chr) +
            sizeof(entry.startPosition) +
            sizeof(entry.stopPosition) +
			sizeof(entry.markerCount) +
            sizeof(entry.meanMarkerDistance) +
            GetMetricBufferSize(entry.metrics);
	}

	char *buffer = new char[dataBufferSz[dataType]];
	memset(buffer, 0, dataBufferSz[dataType]);
	char *pbuffer = buffer;

	MmSetUInt32_N((uint32_t *)pbuffer, entry.segmentId);
	pbuffer += sizeof(entry.segmentId);

	MmSetUInt8((uint8_t *)pbuffer, entry.chr);
	pbuffer += sizeof(entry.chr);

	MmSetUInt32_N((uint32_t *)pbuffer, entry.startPosition);
	pbuffer += sizeof(entry.startPosition);

	MmSetUInt32_N((uint32_t *)pbuffer, entry.stopPosition);
	pbuffer += sizeof(entry.stopPosition);

	MmSetUInt32_N((uint32_t *)pbuffer, entry.markerCount);
	pbuffer += sizeof(entry.markerCount);

	MmSetUInt32_N((uint32_t *)pbuffer, entry.meanMarkerDistance);
	pbuffer += sizeof(entry.meanMarkerDistance);

	CopyMetricToBuffer(entry.metrics, pbuffer);

	dataBuffers[dataType][target].push_back(buffer);
	m_BufferSize += dataBufferSz[dataType];

	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::WriteEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::ChromosomeSegmentDataEx & entry)
{
	if (dataBufferSz[dataType] == 0)
	{
		dataBufferSz[dataType] = 
            sizeof(entry.segmentId) + 
            sizeof(entry.chr) +
            sizeof(entry.startPosition) +
            sizeof(entry.stopPosition) +
            sizeof(entry.call) +
            sizeof(entry.confidence) +
            sizeof(entry.markerCount) +
            GetMetricBufferSize(entry.metrics) +
			sizeof(entry.referenceSampleKey) +
			sizeof(entry.familialSampleKey) +
			sizeof(entry.homozygosity) +
			sizeof(entry.heterozygosity);
	}

	char *buffer = new char[dataBufferSz[dataType]];
	memset(buffer, 0, dataBufferSz[dataType]);
	char *pbuffer = buffer;

	MmSetUInt32_N((uint32_t *)pbuffer, entry.segmentId);
	pbuffer += sizeof(entry.segmentId);

	MmSetUInt32_N((uint32_t *)pbuffer, entry.referenceSampleKey);
	pbuffer += sizeof(entry.referenceSampleKey);

	MmSetUInt32_N((uint32_t *)pbuffer, entry.familialSampleKey);
	pbuffer += sizeof(entry.familialSampleKey);

	MmSetUInt8((uint8_t *)pbuffer, entry.chr);
	pbuffer += sizeof(entry.chr);

	MmSetUInt32_N((uint32_t *)pbuffer, entry.startPosition);
	pbuffer += sizeof(entry.startPosition);

	MmSetUInt32_N((uint32_t *)pbuffer, entry.stopPosition);
	pbuffer += sizeof(entry.stopPosition);

	MmSetUInt8((uint8_t *)pbuffer, entry.call);
	pbuffer += sizeof(entry.call);

	MmSetFloat_N((float *)pbuffer, entry.confidence);
	pbuffer += sizeof(entry.confidence);

    MmSetUInt32_N((uint32_t *)pbuffer, entry.markerCount);
	pbuffer += sizeof(entry.markerCount);

	MmSetFloat_N((float *)pbuffer, entry.homozygosity);
	pbuffer += sizeof(entry.homozygosity);

	MmSetFloat_N((float *)pbuffer, entry.heterozygosity);
	pbuffer += sizeof(entry.heterozygosity);

	CopyMetricToBuffer(entry.metrics, pbuffer);

	dataBuffers[dataType][target].push_back(buffer);
	m_BufferSize += dataBufferSz[dataType];

	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::WriteEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::FamilialSample& entry)
{
	if (dataBufferSz[dataType] == 0)
	{
		dataBufferSz[dataType] = 
			sizeof(entry.sampleKey) +
            sizeof(int) +
            maxFamilialARRID[dataType] + 
            sizeof(int) +
            maxFamilialCHPID[dataType] + 
            sizeof(int) +
            (2*maxFamilialCHPFilename[dataType]) + 
            sizeof(int) +
            maxFamilialRole[dataType] + 
            sizeof(entry.roleConfidence) +
            sizeof(u_int8_t);
	}
	char *buffer = new char[dataBufferSz[dataType]];
	memset(buffer, 0, dataBufferSz[dataType]);
	char *pbuffer = buffer;


    MmSetUInt32_N((uint32_t*) pbuffer, entry.sampleKey);
	pbuffer += sizeof(u_int32_t);

	MmSetUInt32_N((uint32_t *)pbuffer, maxFamilialARRID[dataType]);
	pbuffer += sizeof(int);

	memcpy(pbuffer, entry.arrID.c_str(), entry.arrID.length());
	pbuffer += maxFamilialARRID[dataType];

	MmSetUInt32_N((uint32_t *)pbuffer, maxFamilialCHPID[dataType]);
	pbuffer += sizeof(int);

	memcpy(pbuffer, entry.chpID.c_str(), entry.chpID.length());
	pbuffer += maxFamilialCHPID[dataType];

	MmSetUInt32_N((uint32_t *)pbuffer, maxFamilialCHPFilename[dataType]);
	pbuffer += sizeof(int);

    for (int ilen=0; ilen<(int)entry.chpFilename.length(); ilen++)
    {
        u_int16_t cvalue = (u_int16_t) entry.chpFilename[ilen];
        cvalue = htons(cvalue);
        memcpy((u_int16_t *)(pbuffer+ilen*sizeof(u_int16_t)), &cvalue, sizeof(u_int16_t));
    }
	//memcpy(pbuffer, entry.chpFilename.c_str(), 2*entry.chpFilename.length());
	pbuffer += (2*maxFamilialCHPFilename[dataType]);


	MmSetUInt32_N((uint32_t *)pbuffer, maxFamilialRole[dataType]);
	pbuffer += sizeof(int);

	memcpy(pbuffer, entry.role.c_str(), entry.role.length());
	pbuffer += maxFamilialRole[dataType];

	MmSetUInt8((uint8_t*) pbuffer, (entry.roleValidity == true ? 1 : 0));
	pbuffer += sizeof(u_int8_t);

    MmSetFloat_N((float*) pbuffer, entry.roleConfidence);
	pbuffer += sizeof(float);

	dataBuffers[dataType][target].push_back(buffer);
	m_BufferSize += dataBufferSz[dataType];

	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::WriteEntry(MultiDataType dataType, int target, const affymetrix_calvin_data::FamilialSegmentOverlap & entry)
{
	if (dataBufferSz[dataType] == 0)
	{
		dataBufferSz[dataType] = 
            sizeof(int) +
            maxSegmentType[dataType] + 
            sizeof(int) +
            maxReferenceSegmentID[dataType] + 
            sizeof(int) +
            maxFamilialSegmentID[dataType] + 
            sizeof(entry.referenceSampleKey) +
            sizeof(entry.familialSampleKey);
	}
	char *buffer = new char[dataBufferSz[dataType]];
	memset(buffer, 0, dataBufferSz[dataType]);
	char *pbuffer = buffer;

    
	MmSetUInt32_N((uint32_t *)pbuffer, maxSegmentType[dataType]);
	pbuffer += sizeof(int);

	memcpy(pbuffer, entry.segmentType.c_str(), entry.segmentType.length());
	pbuffer += maxSegmentType[dataType];

    MmSetUInt32_N((uint32_t*) pbuffer, entry.referenceSampleKey);
	pbuffer += sizeof(u_int32_t);

	MmSetUInt32_N((uint32_t *)pbuffer, maxReferenceSegmentID[dataType]);
	pbuffer += sizeof(int);

	memcpy(pbuffer, entry.referenceSegmentID.c_str(), entry.referenceSegmentID.length());
	pbuffer += maxReferenceSegmentID[dataType];

    MmSetUInt32_N((uint32_t*) pbuffer, entry.familialSampleKey);
	pbuffer += sizeof(u_int32_t);

	MmSetUInt32_N((uint32_t *)pbuffer, maxFamilialSegmentID[dataType]);
	pbuffer += sizeof(int);

	memcpy(pbuffer, entry.familialSegmentID.c_str(), entry.familialSegmentID.length());
	pbuffer += maxFamilialSegmentID[dataType];

	dataBuffers[dataType][target].push_back(buffer);
	m_BufferSize += dataBufferSz[dataType];

	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

static void ClearBuffer(std::vector<char *> &buffer)
{
    int nbufs = (int) buffer.size();
    for (int ibuf=0; ibuf<nbufs; ibuf++)
    {
        delete[] buffer[ibuf];
        buffer[ibuf] = NULL;
    }
    buffer.clear();
}

void CHPMultiDataFileBufferWriter::FlushBuffer()
{
	if(m_BufferSize > 0)
	{
		MultiDataType segmentTypes[] = {
			SegmentCNMultiDataType,
			SegmentLOHMultiDataType,
			SegmentCNNeutralLOHMultiDataType,
			SegmentNormalDiploidMultiDataType,
			SegmentMosaicismMultiDataType,
			SegmentNoCallMultiDataType,
			SegmentGenotypeConcordanceMultiDataType,
			SegmentGenotypeDiscordanceMultiDataType,
			SegmentCNLossLOHConcordanceMultiDataType,
			SegmentCNNeutralLOHConcordanceMultiDataType,
			SegmentHeteroUPDMultiDataType,
			SegmentIsoUPDMultiDataType,
			SegmentDenovoCopyNumberMultiDataType,
			SegmentHemizygousParentOfOriginMultiDataType
		};
		int nSegmentTypes = sizeof(segmentTypes) / sizeof(MultiDataType);

		for (int target = 0; target < (int)chpFileNames->size(); target++)
		{
			CalvinCHPFileUpdater updater;
			updater.OpenCHPFile((*chpFileNames)[target].c_str());
            MultiDataType dataType;

            dataType = GenotypeMultiDataType;
			if (dataBuffers.find(dataType) != dataBuffers.end() && dataBuffers[dataType][target].size() > 0)
			{
				updater.UpdateMultiDataGenotypeEntryBuffer(dataType,
                    rowIndexes[dataType][target],
                    dataBufferSz[dataType],
                    dataBuffers[dataType][target]);
				rowIndexes[dataType][target] += (int) dataBuffers[dataType][target].size();
                ClearBuffer(dataBuffers[dataType][target]);
			}

            dataType = CopyNumberMultiDataType;
			if (dataBuffers.find(dataType) != dataBuffers.end() && dataBuffers[dataType][target].size() > 0)
			{
				updater.UpdateMultiDataCopyNumberEntryBuffer(dataType,
                    rowIndexes[dataType][target],
                    dataBufferSz[dataType],
                    dataBuffers[dataType][target]);
				rowIndexes[dataType][target] += (int) dataBuffers[dataType][target].size();
                ClearBuffer(dataBuffers[dataType][target]);
			}

            dataType = CytoMultiDataType;
			if (dataBuffers.find(dataType) != dataBuffers.end() && dataBuffers[dataType][target].size() > 0)
			{
				updater.UpdateMultiDataCytoRegionEntryBuffer(dataType,
                    rowIndexes[dataType][target],
                    dataBufferSz[dataType],
                    dataBuffers[dataType][target]);
				rowIndexes[dataType][target] += (int) dataBuffers[dataType][target].size();
                ClearBuffer(dataBuffers[dataType][target]);
			}

            dataType = ExpressionMultiDataType;
			if (dataBuffers.find(dataType) != dataBuffers.end() && dataBuffers[dataType][target].size() > 0)
			{
				updater.UpdateMultiDataExpressionEntryBuffer(dataType,
                    rowIndexes[dataType][target],
                    dataBufferSz[dataType],
                    dataBuffers[dataType][target]);
				rowIndexes[dataType][target] += (int) dataBuffers[dataType][target].size();
                ClearBuffer(dataBuffers[dataType][target]);
			}

            dataType = CopyNumberVariationMultiDataType;
			if (dataBuffers.find(dataType) != dataBuffers.end() && dataBuffers[dataType][target].size() > 0)
			{
				updater.UpdateMultiDataCopyNumberVariationRegionEntryBuffer(dataType,
                    rowIndexes[dataType][target],
                    dataBufferSz[dataType],
                    dataBuffers[dataType][target]);
				rowIndexes[dataType][target] += (int) dataBuffers[dataType][target].size();
                ClearBuffer(dataBuffers[dataType][target]);
			}

            dataType = DmetBiAllelicMultiDataType;
			if (dataBuffers.find(dataType) != dataBuffers.end() && dataBuffers[dataType][target].size() > 0)
			{
				updater.UpdateDmetBiAllelicEntryBuffer(dataType,
                    rowIndexes[dataType][target],
                    dataBufferSz[dataType],
                    dataBuffers[dataType][target]);
				rowIndexes[dataType][target] += (int) dataBuffers[dataType][target].size();
                ClearBuffer(dataBuffers[dataType][target]);
			}

            dataType = DmetMultiAllelicMultiDataType;
			if (dataBuffers.find(dataType) != dataBuffers.end() && dataBuffers[dataType][target].size() > 0)
			{
				updater.UpdateDmetMultiAllelicEntryBuffer(dataType,
                    rowIndexes[dataType][target],
                    dataBufferSz[dataType],
                    dataBuffers[dataType][target]);
				rowIndexes[dataType][target] += (int) dataBuffers[dataType][target].size();
                ClearBuffer(dataBuffers[dataType][target]);
			}

            dataType = DmetCopyNumberMultiDataType;
			if (dataBuffers.find(dataType) != dataBuffers.end() && dataBuffers[dataType][target].size() > 0)
			{
				updater.UpdateDmetCopyNumberEntryBuffer(dataType,
                    rowIndexes[dataType][target],
                    dataBufferSz[dataType],
                    dataBuffers[dataType][target]);
				rowIndexes[dataType][target] += (int) dataBuffers[dataType][target].size();
                ClearBuffer(dataBuffers[dataType][target]);
			}

            dataType = ChromosomeSummaryMultiDataType;
			if (dataBuffers.find(dataType) != dataBuffers.end() && dataBuffers[dataType][target].size() > 0)
			{
				updater.UpdateChromosomeSummaryEntryBuffer(dataType,
                    rowIndexes[dataType][target],
                    dataBufferSz[dataType],
                    dataBuffers[dataType][target]);
				rowIndexes[dataType][target] += (int) dataBuffers[dataType][target].size();
                ClearBuffer(dataBuffers[dataType][target]);
			}

			dataType = CytoGenotypeCallMultiDataType;
			if (dataBuffers.find(dataType) != dataBuffers.end() && dataBuffers[dataType][target].size() > 0)
			{
				updater.UpdateMultiDataCytoGenotypeEntryBuffer(dataType,
                    rowIndexes[dataType][target],
                    dataBufferSz[dataType],
                    dataBuffers[dataType][target]);
				rowIndexes[dataType][target] += (int) dataBuffers[dataType][target].size();
                ClearBuffer(dataBuffers[dataType][target]);
			}

			for (int iseg=0; iseg<nSegmentTypes; iseg++)
			{
				dataType = segmentTypes[iseg];
				if (dataBuffers.find(dataType) != dataBuffers.end() && dataBuffers[dataType][target].size() > 0)
				{
					updater.UpdateChromosomeSegmentEntryBuffer(dataType,
						rowIndexes[dataType][target],
						dataBufferSz[dataType],
						dataBuffers[dataType][target]);
					rowIndexes[dataType][target] += (int) dataBuffers[dataType][target].size();
					ClearBuffer(dataBuffers[dataType][target]);
				}
			}

            dataType = FamilialSegmentOverlapsMultiDataType;
			if (dataBuffers.find(dataType) != dataBuffers.end() && dataBuffers[dataType][target].size() > 0)
			{
                updater.UpdateFamilialSegmentOverlapEntryBuffer(dataType,
                    rowIndexes[dataType][target],
                    dataBufferSz[dataType],
                    dataBuffers[dataType][target]);
				rowIndexes[dataType][target] += (int) dataBuffers[dataType][target].size();
                ClearBuffer(dataBuffers[dataType][target]);
			}

            dataType = FamilialSamplesMultiDataType;
			if (dataBuffers.find(dataType) != dataBuffers.end() && dataBuffers[dataType][target].size() > 0)
			{
                updater.UpdateFamilialSampleEntryBuffer(dataType,
                    rowIndexes[dataType][target],
                    dataBufferSz[dataType],
                    dataBuffers[dataType][target]);
				rowIndexes[dataType][target] += (int) dataBuffers[dataType][target].size();
                ClearBuffer(dataBuffers[dataType][target]);
			}

            dataType = AllelePeaksMultiDataType;
			if (dataBuffers.find(dataType) != dataBuffers.end() && dataBuffers[dataType][target].size() > 0)
			{
				updater.UpdateMultiDataAllelePeaksEntryBuffer(dataType,
                    rowIndexes[dataType][target],
                    dataBufferSz[dataType],
                    dataBuffers[dataType][target]);
				rowIndexes[dataType][target] += (int) dataBuffers[dataType][target].size();
                ClearBuffer(dataBuffers[dataType][target]);
			}

            dataType = MarkerABSignalsMultiDataType;
				if (dataBuffers.find(dataType) != dataBuffers.end() && dataBuffers[dataType][target].size() > 0)
			{
				updater.UpdateMultiDataMarkerABSignalsEntryBuffer(dataType,
                    rowIndexes[dataType][target],
                    dataBufferSz[dataType],
                    dataBuffers[dataType][target]);
				rowIndexes[dataType][target] += (int) dataBuffers[dataType][target].size();
                ClearBuffer(dataBuffers[dataType][target]);
			}

			updater.CloseCHPFile();
		}
	}
	m_BufferSize = 0;
}

