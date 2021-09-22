////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
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
 * @file   CHPFileBufferWriter.cpp
 * @author David Le
 * @date   Mon May 15 12:09:42 2006
 * 
 * @brief  Class for writing signals to a buffer before writing to CHP files.
 */

#include "file/CHPFileBufferWriter.h"
//
#include "file/CHPFileUpdater.h"
//

using namespace affxchpwriter;

CCHPFileBufferWriter::CCHPFileBufferWriter()
{
	m_BufferSize = 0;
	m_MaxBufferSize = MAX_BUFFER_SIZE;
	m_IsGenotype = false;	// default is expression
}

CCHPFileBufferWriter::~CCHPFileBufferWriter()
{
	FlushBuffer();
	Cleanup();
}

void CCHPFileBufferWriter::Cleanup()
{
	if (m_IsGenotype)
	{
		for (int target=0; target<m_TargetGenotypeEntryBuffers.size(); target++) 
		{ 
			m_TargetGenotypeEntryBuffers[target].erase(m_TargetGenotypeEntryBuffers[target].begin(), m_TargetGenotypeEntryBuffers[target].end());
		}
		m_TargetGenotypeEntryBuffers.erase(m_TargetGenotypeEntryBuffers.begin(), m_TargetGenotypeEntryBuffers.end());
	}
	else
	{
		for (int target=0; target<m_TargetExpressionEntryBuffers.size(); target++) 
		{ 
			m_TargetExpressionEntryBuffers[target].erase(m_TargetExpressionEntryBuffers[target].begin(), m_TargetExpressionEntryBuffers[target].end());
		}
		m_TargetExpressionEntryBuffers.erase(m_TargetExpressionEntryBuffers.begin(), m_TargetExpressionEntryBuffers.end());
	}
}

void CCHPFileBufferWriter::Initialize(std::vector<std::string> *CHPFileNames, bool IsGenotype)
{
	m_CHPFileNames = CHPFileNames;
	Cleanup();
	for (int i=0; i<m_CHPFileNames->size(); i++)
	{
		if (IsGenotype)
		{
			std::vector<GenotypeBufferEntry> entryBuffer;
			m_TargetGenotypeEntryBuffers.push_back(entryBuffer);
		}
		else
		{
			std::vector<ExpressionBufferEntry> entryBuffer;
			m_TargetExpressionEntryBuffers.push_back(entryBuffer);
		}
		m_TargetEntryRowIndexes.push_back(0);
	}	
	m_BufferSize = 0;
	m_IsGenotype = IsGenotype;
}

void CCHPFileBufferWriter::WriteGenotypeEntry(int target, affxchp::CGenotypeProbeSetResults &entry)
{
	GenotypeBufferEntry bufferEntry;
	bufferEntry.call = entry.AlleleCall;
	bufferEntry.confidence = entry.Confidence;
	bufferEntry.RAS1 = entry.RAS1;
	bufferEntry.RAS2 = entry.RAS2;
	bufferEntry.aaCall = entry.pvalue_AA;
	bufferEntry.abCall = entry.pvalue_AB;
	bufferEntry.bbCall = entry.pvalue_BB;
	bufferEntry.noCall = entry.pvalue_NoCall;
	m_TargetGenotypeEntryBuffers[target].push_back(bufferEntry);
	m_BufferSize += sizeof(GenotypeBufferEntry);
	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CCHPFileBufferWriter::WriteExpressionEntry(int target, affxchp::CExpressionProbeSetResults &entry)
{
	ExpressionBufferEntry bufferEntry;
	bufferEntry.Detection = entry.Detection;
	bufferEntry.DetectionPValue = entry.DetectionPValue;
	bufferEntry.Signal = entry.Signal;
	bufferEntry.NumPairs = entry.NumPairs;
	bufferEntry.NumUsedPairs = entry.NumUsedPairs;
	m_TargetExpressionEntryBuffers[target].push_back(bufferEntry);
	m_BufferSize += sizeof(ExpressionBufferEntry);
	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CCHPFileBufferWriter::FlushBuffer()
{
	if(m_BufferSize > 0) {
		if (m_IsGenotype)
		{
			for (int target=0; target<m_CHPFileNames->size(); target++)
			{
				CCHPFileUpdater updater;
				updater.OpenCHPFile((*m_CHPFileNames)[target].c_str());
				for (int i=0; i<m_TargetGenotypeEntryBuffers[target].size(); i++)
				{
					GenotypeBufferEntry bufferEntry = m_TargetGenotypeEntryBuffers[target][i];
					updater.UpdateGenotypeEntry(
						m_TargetEntryRowIndexes[target]+i, 
						bufferEntry.call,
						bufferEntry.confidence,
						bufferEntry.RAS1,
						bufferEntry.RAS2,
						bufferEntry.aaCall,
						bufferEntry.abCall,
						bufferEntry.bbCall,
						bufferEntry.noCall);
				}
				updater.CloseCHPFile();
				m_TargetEntryRowIndexes[target] += m_TargetGenotypeEntryBuffers[target].size();
				m_TargetGenotypeEntryBuffers[target].erase(m_TargetGenotypeEntryBuffers[target].begin(), m_TargetGenotypeEntryBuffers[target].end());
			}
		}
		else
		{
			for (int target=0; target<m_CHPFileNames->size(); target++)
			{
				CCHPFileUpdater updater;
				updater.OpenCHPFile((*m_CHPFileNames)[target].c_str());
				for (int i=0; i<m_TargetExpressionEntryBuffers[target].size(); i++)
				{
					ExpressionBufferEntry bufferEntry = m_TargetExpressionEntryBuffers[target][i];
					updater.UpdateExpressionEntry(
						m_TargetEntryRowIndexes[target]+i, 
						bufferEntry.Detection,
						bufferEntry.DetectionPValue,
						bufferEntry.Signal,
						bufferEntry.NumPairs,
						bufferEntry.NumUsedPairs);
				}
				updater.CloseCHPFile();
				m_TargetEntryRowIndexes[target] += m_TargetExpressionEntryBuffers[target].size();
				m_TargetExpressionEntryBuffers[target].erase(m_TargetExpressionEntryBuffers[target].begin(), m_TargetExpressionEntryBuffers[target].end());
			}
		}
	}
	m_BufferSize = 0;
}
