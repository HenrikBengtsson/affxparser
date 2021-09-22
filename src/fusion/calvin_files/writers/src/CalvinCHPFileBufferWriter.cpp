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
 * @file   CalvinCHPFileBufferWriter.cpp
 * @author David Le
 * @date   Mon May 15 12:09:42 2006
 * 
 * @brief  Class for writing signals to a buffer before writing to CHP files.
 */

#include "calvin_files/writers/src/CalvinCHPFileBufferWriter.h"
//
#include "calvin_files/writers/src/CalvinCHPFileUpdater.h"
//

using namespace affymetrix_calvin_io;

CHPFileBufferWriter::CHPFileBufferWriter()
{
	m_BufferSize = 0;
	m_MaxBufferSize = MAX_BUFFER_SIZE;
}

CHPFileBufferWriter::~CHPFileBufferWriter()
{
	FlushBuffer();
	Cleanup();
}

void CHPFileBufferWriter::Cleanup()
{
	for (int target=0; target<m_TargetEntryBuffers.size(); target++) 
	{ 
		m_TargetEntryBuffers[target].erase(m_TargetEntryBuffers[target].begin(), m_TargetEntryBuffers[target].end());
	}
	m_TargetEntryBuffers.erase(m_TargetEntryBuffers.begin(), m_TargetEntryBuffers.end());
}

void CHPFileBufferWriter::Initialize(std::vector<std::string> *CHPFileNames)
{
	m_CHPFileNames = CHPFileNames;
	Cleanup();
	for (int i=0; i<m_CHPFileNames->size(); i++)
	{
		std::vector<GenotypeBufferEntry> entryBuffer;
		m_TargetEntryBuffers.push_back(entryBuffer);
		m_TargetEntryRowIndexes.push_back(0);
	}	
	m_BufferSize = 0;
}

void CHPFileBufferWriter::WriteGenotypeEntry(int target, CHPGenotypeEntry &entry)
{
	GenotypeBufferEntry bufferEntry;
	bufferEntry.call = entry.GetCall();
	bufferEntry.confidence = entry.GetConfidence();
	bufferEntry.RAS1 = entry.GetRAS1();
	bufferEntry.RAS2 = entry.GetRAS2();
	bufferEntry.aaCall = entry.GetAACall();
	bufferEntry.abCall = entry.GetABCall();
	bufferEntry.bbCall = entry.GetBBCall();
	bufferEntry.noCall = entry.GetNoCall();
	m_TargetEntryBuffers[target].push_back(bufferEntry);
	m_BufferSize += sizeof(GenotypeBufferEntry);
	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPFileBufferWriter::FlushBuffer()
{
	if (m_BufferSize > 0) {
		for (int target=0; target<m_CHPFileNames->size(); target++)
		{
			CalvinCHPFileUpdater updater;
			updater.OpenCHPFile((*m_CHPFileNames)[target].c_str());
			updater.UpdateGenotypeEntryBuffer(m_TargetEntryRowIndexes[target], m_TargetEntryBuffers[target]);
			updater.CloseCHPFile();
			m_TargetEntryRowIndexes[target] += m_TargetEntryBuffers[target].size();
			m_TargetEntryBuffers[target].clear();
		}
	}
	m_BufferSize = 0;
}
