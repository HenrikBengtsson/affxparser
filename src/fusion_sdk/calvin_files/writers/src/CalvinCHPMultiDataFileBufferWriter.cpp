////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006 Affymetrix, Inc.
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
		m_TargetMultiDataGenotypeBuffers[target].erase(m_TargetMultiDataGenotypeBuffers[target].begin(), m_TargetMultiDataGenotypeBuffers[target].end());
	}
	m_TargetMultiDataGenotypeBuffers.erase(m_TargetMultiDataGenotypeBuffers.begin(), m_TargetMultiDataGenotypeBuffers.end());
	m_TargetMultiDataGenotypeRowIndexes.clear();

    for (int target=0; target<(int)m_TargetMultiDataGenotypeControlBuffers.size(); target++) 
	{ 
		m_TargetMultiDataGenotypeControlBuffers[target].erase(m_TargetMultiDataGenotypeControlBuffers[target].begin(), m_TargetMultiDataGenotypeControlBuffers[target].end());
	}
	m_TargetMultiDataGenotypeControlBuffers.erase(m_TargetMultiDataGenotypeControlBuffers.begin(), m_TargetMultiDataGenotypeControlBuffers.end());
	m_TargetMultiDataGenotypeControlRowIndexes.clear();

	for (int target=0; target<(int)m_TargetMultiDataExpressionBuffers.size(); target++) 
	{ 
		m_TargetMultiDataExpressionBuffers[target].erase(m_TargetMultiDataExpressionBuffers[target].begin(), m_TargetMultiDataExpressionBuffers[target].end());
	}
	m_TargetMultiDataExpressionBuffers.erase(m_TargetMultiDataExpressionBuffers.begin(), m_TargetMultiDataExpressionBuffers.end());
	m_TargetMultiDataExpressionRowIndexes.clear();

    for (int target=0; target<(int)m_TargetMultiDataExpressionControlBuffers.size(); target++) 
	{ 
		m_TargetMultiDataExpressionControlBuffers[target].erase(m_TargetMultiDataExpressionControlBuffers[target].begin(), m_TargetMultiDataExpressionControlBuffers[target].end());
	}
	m_TargetMultiDataExpressionControlBuffers.erase(m_TargetMultiDataExpressionControlBuffers.begin(), m_TargetMultiDataExpressionControlBuffers.end());
	m_TargetMultiDataExpressionControlRowIndexes.clear();

    dataSetIndexMap.clear();
}

void CHPMultiDataFileBufferWriter::Initialize(vector<string> *CHPFileNames, std::vector<MultiDataType> &dataTypes)
{
	m_CHPFileNames = CHPFileNames;
	Cleanup();
	for (int i=0; i<(int)m_CHPFileNames->size(); i++)
	{
        for (int itype=0; itype<(int)dataTypes.size(); itype++)
        {
            switch (dataTypes[itype])
            {
                case GenotypeMultiDataType:
                    {
		                vector<ProbeSetMultiDataGenotypeData> buffer;
		                m_TargetMultiDataGenotypeBuffers.push_back(buffer);
                    }
                    m_TargetMultiDataGenotypeRowIndexes.push_back(0);
                    break;

                case GenotypeControlMultiDataType:
                    {
		                vector<ProbeSetMultiDataGenotypeData> buffer;
		                m_TargetMultiDataGenotypeControlBuffers.push_back(buffer);
                    }
                    m_TargetMultiDataGenotypeControlRowIndexes.push_back(0);
                    break;

                case ExpressionMultiDataType:
                    {
	                    vector<ProbeSetMultiDataExpressionData> buffer;
	                    m_TargetMultiDataExpressionBuffers.push_back(buffer);
                    }
                    m_TargetMultiDataExpressionRowIndexes.push_back(0);
                    break;

                case ExpressionControlMultiDataType:
                    {
		                vector<ProbeSetMultiDataExpressionData> buffer;
		                m_TargetMultiDataExpressionControlBuffers.push_back(buffer);
                    }
                    m_TargetMultiDataExpressionControlRowIndexes.push_back(0);
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

void CHPMultiDataFileBufferWriter::WriteMultiDataGenotypeEntry(MultiDataType dataType, int target, const ProbeSetMultiDataGenotypeData &entry)
{
    if (dataType == GenotypeMultiDataType)
        m_TargetMultiDataGenotypeBuffers[target].push_back(entry);
    else
        m_TargetMultiDataGenotypeControlBuffers[target].push_back(entry);
	m_BufferSize += sizeof(entry.call) + sizeof(entry.confidence);
    m_BufferSize += GetMetricBufferSize(entry.metrics);
	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPMultiDataFileBufferWriter::WriteMultiDataExpressionEntry(MultiDataType dataType, int target, const ProbeSetMultiDataExpressionData &entry)
{
    if (dataType == ExpressionMultiDataType)
        m_TargetMultiDataExpressionBuffers[target].push_back(entry);
    else
        m_TargetMultiDataExpressionControlBuffers[target].push_back(entry);
    m_BufferSize += sizeof(entry.quantification);
    m_BufferSize += GetMetricBufferSize(entry.metrics);
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
                updater.UpdateMultiDataGenotypeEntryBuffer(GenotypeMultiDataType, m_TargetMultiDataGenotypeRowIndexes[target], m_TargetMultiDataGenotypeBuffers[target]);
			    m_TargetMultiDataGenotypeRowIndexes[target] += (int) m_TargetMultiDataGenotypeBuffers[target].size();
    			m_TargetMultiDataGenotypeBuffers[target].clear();
            }

            if (m_TargetMultiDataGenotypeControlRowIndexes.size() > 0)
            {
                updater.UpdateMultiDataGenotypeEntryBuffer(GenotypeControlMultiDataType, m_TargetMultiDataGenotypeControlRowIndexes[target], m_TargetMultiDataGenotypeControlBuffers[target]);
			    m_TargetMultiDataGenotypeControlRowIndexes[target] += (int) m_TargetMultiDataGenotypeControlBuffers[target].size();
    			m_TargetMultiDataGenotypeControlBuffers[target].clear();
            }

            if (m_TargetMultiDataExpressionRowIndexes.size() > 0)
            {
                updater.UpdateMultiDataExpressionEntryBuffer(ExpressionMultiDataType, m_TargetMultiDataExpressionRowIndexes[target], m_TargetMultiDataExpressionBuffers[target]);
			    m_TargetMultiDataExpressionRowIndexes[target] += (int) m_TargetMultiDataExpressionBuffers[target].size();
    			m_TargetMultiDataExpressionBuffers[target].clear();
            }

            if (m_TargetMultiDataExpressionControlRowIndexes.size() > 0)
            {
                updater.UpdateMultiDataExpressionEntryBuffer(ExpressionControlMultiDataType, m_TargetMultiDataExpressionControlRowIndexes[target], m_TargetMultiDataExpressionControlBuffers[target]);
			    m_TargetMultiDataExpressionControlRowIndexes[target] += (int) m_TargetMultiDataExpressionControlBuffers[target].size();
    			m_TargetMultiDataExpressionControlBuffers[target].clear();
            }

			updater.CloseCHPFile();
		}
	}
	m_BufferSize = 0;
}

int CHPMultiDataFileBufferWriter::GetMetricBufferSize(const std::vector<affymetrix_calvin_parameter::ParameterNameValueType> &metrics)
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
				bufferSize += (int)(nv.GetValueAscii().length() * sizeof(char));
				break;

			case ParameterNameValueType::TextType:
				bufferSize += (int)(nv.GetValueText().length() * sizeof(wchar_t));
				break;
        // @todo check this is ok.
    case ParameterNameValueType::UnknownType:
      break;
		}
	}
    return bufferSize;
}
