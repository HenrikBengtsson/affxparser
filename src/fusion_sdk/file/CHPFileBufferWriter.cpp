/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License,
// or (at your option) any later version.
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
/////////////////////////////////////////////////////////////////

/**
 * @file   CHPFileBufferWriter.cpp
 * @author David Le
 * @date   Mon May 15 12:09:42 2006
 * 
 * @brief  Class for writing signals to a buffer before writing to CHP files.
 */

#include "CHPFileUpdater.h"
#include "CHPFileBufferWriter.h"

using namespace affxchpwriter;

CCHPFileBufferWriter::CCHPFileBufferWriter()
{
	m_BufferSize = 0;
	m_MaxBufferSize = MAX_BUFFER_SIZE;
}

CCHPFileBufferWriter::~CCHPFileBufferWriter()
{
	FlushBuffer();
	Cleanup();
}

void CCHPFileBufferWriter::Cleanup()
{
	for (int target=0; target<m_TargetEntryBuffers.size(); target++) 
	{ 
		m_TargetEntryBuffers[target].erase(m_TargetEntryBuffers[target].begin(), m_TargetEntryBuffers[target].end());
	}
	m_TargetEntryBuffers.erase(m_TargetEntryBuffers.begin(), m_TargetEntryBuffers.end());
}

void CCHPFileBufferWriter::Initialize(std::vector<std::string> *CHPFileNames)
{
	m_CHPFileNames = CHPFileNames;
	Cleanup();
	for (int i=0; i<m_CHPFileNames->size(); i++)
	{
		std::vector<ExpressionBufferEntry> entryBuffer;
		m_TargetEntryBuffers.push_back(entryBuffer);
		m_TargetEntryRowIndexes.push_back(0);
	}	
	m_BufferSize = 0;
}

void CCHPFileBufferWriter::WriteExpressionEntry(int target, affxchp::CExpressionProbeSetResults &entry)
{
	ExpressionBufferEntry bufferEntry;
	bufferEntry.Detection = entry.Detection;
	bufferEntry.DetectionPValue = entry.DetectionPValue;
	bufferEntry.Signal = entry.Signal;
	bufferEntry.NumPairs = entry.NumPairs;
	bufferEntry.NumUsedPairs = entry.NumUsedPairs;
	m_TargetEntryBuffers[target].push_back(bufferEntry);
	m_BufferSize += sizeof(ExpressionBufferEntry);
	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CCHPFileBufferWriter::FlushBuffer()
{
	if(m_BufferSize > 0) {
		for (int target=0; target<m_CHPFileNames->size(); target++)
		{
			CCHPFileUpdater updater;
			updater.OpenCHPFile((*m_CHPFileNames)[target].c_str());
			for (int i=0; i<m_TargetEntryBuffers[target].size(); i++)
			{
				ExpressionBufferEntry bufferEntry = m_TargetEntryBuffers[target][i];
				updater.UpdateExpressionEntry(
					m_TargetEntryRowIndexes[target]+i, 
					bufferEntry.Detection,
					bufferEntry.DetectionPValue,
					bufferEntry.Signal,
					bufferEntry.NumPairs,
					bufferEntry.NumUsedPairs);
			}
			updater.CloseCHPFile();
			m_TargetEntryRowIndexes[target] += m_TargetEntryBuffers[target].size();
			m_TargetEntryBuffers[target].erase(m_TargetEntryBuffers[target].begin(), m_TargetEntryBuffers[target].end());
		}
	}
	m_BufferSize = 0;
}
