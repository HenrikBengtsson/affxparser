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
 * @file   CalvinCHPFileBufferWriter.cpp
 * @author David Le
 * @date   Mon May 15 12:09:42 2006
 * 
 * @brief  Class for writing signals to a buffer before writing to CHP files.
 */

#include "CHPFileUpdater.h"
#include "CalvinCHPFileUpdater.h"
#include "CalvinCHPFileBufferWriter.h"

using namespace affxchpwriter;
using namespace affymetrix_calvin_io;

CHPFileBufferWriter::CHPFileBufferWriter()
{
	m_BufferSize = 0;
	m_MaxBufferSize = MAX_BUFFER_SIZE;
	m_XDACHPFormat = false;
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

void CHPFileBufferWriter::Initialize(std::vector<std::string> *CHPFileNames, bool XDACHPFormat)
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
	m_XDACHPFormat = XDACHPFormat;
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
		if (m_XDACHPFormat)
		{
			FlushBufferXDA();
		}
		else
		{
			FlushBufferCC();
		}
	}
}

void CHPFileBufferWriter::FlushBufferCC()
{
	if (m_BufferSize > 0) {
		for (int target=0; target<m_CHPFileNames->size(); target++)
		{
			CalvinCHPFileUpdater updater;
			updater.OpenCHPFile((*m_CHPFileNames)[target].c_str());
			for (int i=0; i<m_TargetEntryBuffers[target].size(); i++)
			{
				GenotypeBufferEntry bufferEntry = m_TargetEntryBuffers[target][i];
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
			m_TargetEntryRowIndexes[target] += m_TargetEntryBuffers[target].size();
			m_TargetEntryBuffers[target].erase(m_TargetEntryBuffers[target].begin(), m_TargetEntryBuffers[target].end());
		}
	}
	m_BufferSize = 0;
}

void CHPFileBufferWriter::FlushBufferXDA()
{
	if (m_BufferSize > 0) {
		for (int target=0; target<m_CHPFileNames->size(); target++)
		{
			CCHPFileUpdater updater;
			updater.OpenCHPFile((*m_CHPFileNames)[target].c_str());
			for (int i=0; i<m_TargetEntryBuffers[target].size(); i++)
			{
				GenotypeBufferEntry bufferEntry = m_TargetEntryBuffers[target][i];
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
			m_TargetEntryRowIndexes[target] += m_TargetEntryBuffers[target].size();
			m_TargetEntryBuffers[target].erase(m_TargetEntryBuffers[target].begin(), m_TargetEntryBuffers[target].end());
		}
	}
	m_BufferSize = 0;
}
