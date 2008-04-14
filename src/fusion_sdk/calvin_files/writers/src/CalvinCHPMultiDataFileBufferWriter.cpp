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

#include "CalvinCHPFileUpdater.h"
#include "CalvinCHPMultiDataFileBufferWriter.h"
#include "FileOutput.h"
#include "FileIO.h"

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

void CHPMultiDataFileBufferWriter::Cleanup()
{
	for (int target=0; target<(int)m_TargetMultiDataGenotypeBuffers.size(); target++) 
	{ 
        int nbufs = (int) m_TargetMultiDataGenotypeBuffers[target].size();
        for (int ibuf=0; ibuf<nbufs; ibuf++)
            delete[] m_TargetMultiDataGenotypeBuffers[target][ibuf];
		m_TargetMultiDataGenotypeBuffers[target].clear();
	}
	m_TargetMultiDataGenotypeBuffers.clear();
	m_TargetMultiDataGenotypeRowIndexes.clear();

	for (int target=0; target<(int)m_TargetMultiDataCopyNumberBuffers.size(); target++) 
	{ 
        int nbufs = (int) m_TargetMultiDataCopyNumberBuffers[target].size();
        for (int ibuf=0; ibuf<nbufs; ibuf++)
            delete[] m_TargetMultiDataCopyNumberBuffers[target][ibuf];
		m_TargetMultiDataCopyNumberBuffers[target].clear();
	}
	m_TargetMultiDataCopyNumberBuffers.clear();
	m_TargetMultiDataCopyNumberRowIndexes.clear();
    
	for (int target=0; target<(int)m_TargetMultiDataCytoRegionBuffers.size(); target++) 
	{ 
        int nbufs = (int) m_TargetMultiDataCytoRegionBuffers[target].size();
        for (int ibuf=0; ibuf<nbufs; ibuf++)
            delete[] m_TargetMultiDataCytoRegionBuffers[target][ibuf];
		m_TargetMultiDataCytoRegionBuffers[target].clear();
	}
	m_TargetMultiDataCytoRegionBuffers.clear();
	m_TargetMultiDataCytoRegionRowIndexes.clear();

	for (int target=0; target<(int)m_TargetMultiDataExpressionBuffers.size(); target++) 
	{ 
        int nbufs = (int) m_TargetMultiDataExpressionBuffers[target].size();
        for (int ibuf=0; ibuf<nbufs; ibuf++)
            delete[] m_TargetMultiDataExpressionBuffers[target][ibuf];
		m_TargetMultiDataExpressionBuffers[target].clear();
	}
	m_TargetMultiDataExpressionBuffers.clear();
	m_TargetMultiDataExpressionRowIndexes.clear();

    dataSetIndexMap.clear();
}

void CHPMultiDataFileBufferWriter::Initialize(vector<string> *CHPFileNames, std::vector<MultiDataType> &dataTypes, std::map<MultiDataType, int> &maxProbeSetNameLength)
{
	m_CHPFileNames = CHPFileNames;
    m_maxProbeSetNameLength = maxProbeSetNameLength;
    exprBufferSize = 0;
    genoBufferSize = 0;
    cnBufferSize = 0;
    cyBufferSize = 0;
	Cleanup();
	for (int i=0; i<(int)m_CHPFileNames->size(); i++)
	{
        for (int itype=0; itype<(int)dataTypes.size(); itype++)
        {
            switch (dataTypes[itype])
            {
                case CopyNumberMultiDataType:
                    {
		                vector<char *> buffer;
		                m_TargetMultiDataCopyNumberBuffers.push_back(buffer);
                    }
                    m_TargetMultiDataCopyNumberRowIndexes.push_back(0);
                    break;

                case CytoMultiDataType:
                    {
		                vector<char *> buffer;
		                m_TargetMultiDataCytoRegionBuffers.push_back(buffer);
                    }
                    m_TargetMultiDataCytoRegionRowIndexes.push_back(0);
                    break;

                case GenotypeMultiDataType:
                    {
		                vector<char *> buffer;
		                m_TargetMultiDataGenotypeBuffers.push_back(buffer);
                    }
                    m_TargetMultiDataGenotypeRowIndexes.push_back(0);
                    break;

                case ExpressionMultiDataType:
                    {
		                vector<char *> buffer;
		                m_TargetMultiDataExpressionBuffers.push_back(buffer);
                    }
                    m_TargetMultiDataExpressionRowIndexes.push_back(0);
                    break;

                default:
                    // we currently do not handle any other types
                    break;

            }
        }
	}
    m_BufferSize = 0;
    for (int i=0; i<(int)dataTypes.size(); i++)    
    {
        dataSetIndexMap[dataTypes[i]] = i;
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
    if (genoBufferSize == 0)
        genoBufferSize = m_maxProbeSetNameLength[dataType] + sizeof(int) + sizeof(entry.call) + sizeof(entry.confidence) + GetMetricBufferSize(entry.metrics);
    char *buffer = new char[genoBufferSize];
    memset(buffer, 0, genoBufferSize);
    char *pbuffer = buffer;
    MmSetUInt32_N((uint32_t *)pbuffer, m_maxProbeSetNameLength[dataType]);
    pbuffer += sizeof(int);
    memcpy(pbuffer, entry.name.c_str(), entry.name.length());
    pbuffer += m_maxProbeSetNameLength[dataType];
    *pbuffer = entry.call;
    pbuffer += sizeof(char);
    MmSetFloat_N((float *)pbuffer, entry.confidence);
    pbuffer += sizeof(float);
    CopyMetricToBuffer(entry.metrics, pbuffer);
    m_TargetMultiDataGenotypeBuffers[target].push_back(buffer);
    m_BufferSize += genoBufferSize;

	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::WriteMultiDataExpressionEntry(MultiDataType dataType, int target, const ProbeSetMultiDataExpressionData &entry)
{
    if (exprBufferSize == 0)
        exprBufferSize = m_maxProbeSetNameLength[dataType] + sizeof(int) + sizeof(entry.quantification) + GetMetricBufferSize(entry.metrics);
    char *buffer = new char[exprBufferSize];
    memset(buffer, 0, exprBufferSize);
    char *pbuffer = buffer;
    MmSetUInt32_N((uint32_t *)pbuffer, m_maxProbeSetNameLength[dataType]);
    pbuffer += sizeof(int);
    memcpy(pbuffer, entry.name.c_str(), entry.name.length());
    pbuffer += m_maxProbeSetNameLength[dataType];
    MmSetFloat_N((float *)pbuffer, entry.quantification);
    pbuffer += sizeof(float);
    CopyMetricToBuffer(entry.metrics, pbuffer);
    m_TargetMultiDataExpressionBuffers[target].push_back(buffer);
    m_BufferSize += exprBufferSize;
    
	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
    }
}


void CHPMultiDataFileBufferWriter::WriteMultiDataCopyNumberEntry(MultiDataType dataType, int target, const ProbeSetMultiDataCopyNumberData &entry)
{
    if (cnBufferSize == 0)
        cnBufferSize = m_maxProbeSetNameLength[dataType] + sizeof(int) + sizeof(entry.chr) + sizeof(entry.position) + GetMetricBufferSize(entry.metrics);
    char *buffer = new char[cnBufferSize];
    memset(buffer, 0, cnBufferSize);
    char *pbuffer = buffer;
    MmSetUInt32_N((uint32_t *)pbuffer, m_maxProbeSetNameLength[dataType]);
    pbuffer += sizeof(int);
    memcpy(pbuffer, entry.name.c_str(), entry.name.length());
    pbuffer += m_maxProbeSetNameLength[dataType];
    MmSetUInt8((uint8_t *)pbuffer, entry.chr);
    pbuffer += sizeof(entry.chr);
    MmSetUInt32_N((uint32_t *)pbuffer, entry.position);
    pbuffer += sizeof(entry.position);
    CopyMetricToBuffer(entry.metrics, pbuffer);
    m_TargetMultiDataCopyNumberBuffers[target].push_back(buffer);
    m_BufferSize += cnBufferSize;

    if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::WriteMultiDataCytoRegionEntry(MultiDataType dataType, int target, const ProbeSetMultiDataCytoRegionData &entry)
{
    if (cyBufferSize == 0)
        cyBufferSize = m_maxProbeSetNameLength[dataType] + sizeof(int) + sizeof(entry.chr) +
                sizeof(entry.startPosition) + sizeof(entry.stopPosition) + sizeof(entry.call) +
                sizeof(entry.confidenceScore) + GetMetricBufferSize(entry.metrics);
    char *buffer = new char[cyBufferSize];
    memset(buffer, 0, cyBufferSize);
    char *pbuffer = buffer;
    MmSetUInt32_N((uint32_t *)pbuffer, m_maxProbeSetNameLength[dataType]);
    pbuffer += sizeof(int);
    memcpy(pbuffer, entry.name.c_str(), entry.name.length());
    pbuffer += m_maxProbeSetNameLength[dataType];
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
    m_TargetMultiDataCytoRegionBuffers[target].push_back(buffer);
    m_BufferSize += cyBufferSize;

    if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::FlushBuffer()
{
	if(m_BufferSize > 0)
	{
		for (int target=0; target<(int)m_CHPFileNames->size(); target++)
		{
			CalvinCHPFileUpdater updater;
			updater.OpenCHPFile((*m_CHPFileNames)[target].c_str());

            if (m_TargetMultiDataGenotypeRowIndexes.size() > 0)
            {
                updater.UpdateMultiDataGenotypeEntryBuffer(GenotypeMultiDataType, m_TargetMultiDataGenotypeRowIndexes[target], genoBufferSize, m_TargetMultiDataGenotypeBuffers[target]);
			    m_TargetMultiDataGenotypeRowIndexes[target] += (int) m_TargetMultiDataGenotypeBuffers[target].size();
                int nbufs = (int) m_TargetMultiDataGenotypeBuffers[target].size();
                for (int ibuf=0; ibuf<nbufs; ibuf++)
                    delete[] m_TargetMultiDataGenotypeBuffers[target][ibuf];
		        m_TargetMultiDataGenotypeBuffers[target].clear();
            }

            if (m_TargetMultiDataCopyNumberRowIndexes.size() > 0)
            {
                updater.UpdateMultiDataCopyNumberEntryBuffer(CopyNumberMultiDataType, m_TargetMultiDataCopyNumberRowIndexes[target], cnBufferSize, m_TargetMultiDataCopyNumberBuffers[target]);
			    m_TargetMultiDataCopyNumberRowIndexes[target] += (int) m_TargetMultiDataCopyNumberBuffers[target].size();
                int nbufs = (int) m_TargetMultiDataCopyNumberBuffers[target].size();
                for (int ibuf=0; ibuf<nbufs; ibuf++)
                    delete[] m_TargetMultiDataCopyNumberBuffers[target][ibuf];
		        m_TargetMultiDataCopyNumberBuffers[target].clear();
            }

            if (m_TargetMultiDataCytoRegionRowIndexes.size() > 0)
            {
                updater.UpdateMultiDataCytoRegionEntryBuffer(CytoMultiDataType, m_TargetMultiDataCytoRegionRowIndexes[target], cyBufferSize, m_TargetMultiDataCytoRegionBuffers[target]);
			    m_TargetMultiDataCytoRegionRowIndexes[target] += (int) m_TargetMultiDataCytoRegionBuffers[target].size();
                int nbufs = (int) m_TargetMultiDataCytoRegionBuffers[target].size();
                for (int ibuf=0; ibuf<nbufs; ibuf++)
                    delete[] m_TargetMultiDataCytoRegionBuffers[target][ibuf];
		        m_TargetMultiDataCytoRegionBuffers[target].clear();
            }

            if (m_TargetMultiDataExpressionRowIndexes.size() > 0)
            {
                updater.UpdateMultiDataExpressionEntryBuffer(ExpressionMultiDataType, m_TargetMultiDataExpressionRowIndexes[target], exprBufferSize, m_TargetMultiDataExpressionBuffers[target]);
			    m_TargetMultiDataExpressionRowIndexes[target] += (int) m_TargetMultiDataExpressionBuffers[target].size();
                int nbufs = (int) m_TargetMultiDataExpressionBuffers[target].size();
                for (int ibuf=0; ibuf<nbufs; ibuf++)
                    delete[] m_TargetMultiDataExpressionBuffers[target][ibuf];
		        m_TargetMultiDataExpressionBuffers[target].clear();
            }

			updater.CloseCHPFile();
		}
	}
	m_BufferSize = 0;
}

