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
 * @file   CalvinCHPQuantificationFileBufferWriter.cpp
 * @author David Le
 * @date   Mon May 15 12:09:42 2006
 * 
 * @brief  Class for writing quantifications to a buffer before writing to CHP files.
 */

#include "calvin_files/writers/src/CalvinCHPQuantificationFileBufferWriter.h"
//
#include "calvin_files/writers/src/CalvinCHPFileUpdater.h"
//

using namespace affymetrix_calvin_io;

CHPQuantificationFileBufferWriter::CHPQuantificationFileBufferWriter()
{
	m_BufferSize = 0;
	m_MaxBufferSize = MAX_BUFFER_SIZE;
}

CHPQuantificationFileBufferWriter::~CHPQuantificationFileBufferWriter()
{
	FlushBuffer();
	Cleanup();
}

void CHPQuantificationFileBufferWriter::Cleanup()
{
	for (int target=0; target<m_TargetQuantificationBuffers.size(); target++) 
	{ 
		m_TargetQuantificationBuffers[target].erase(m_TargetQuantificationBuffers[target].begin(), m_TargetQuantificationBuffers[target].end());
	}
	m_TargetQuantificationBuffers.erase(m_TargetQuantificationBuffers.begin(), m_TargetQuantificationBuffers.end());
}

void CHPQuantificationFileBufferWriter::Initialize(std::vector<std::string> *CHPFileNames)
{
	m_CHPFileNames = CHPFileNames;
	Cleanup();
	for (int i=0; i<m_CHPFileNames->size(); i++)
	{
		std::vector<float> quantificationBuffer;
		m_TargetQuantificationBuffers.push_back(quantificationBuffer);
		m_TargetQuantificationRowIndexes.push_back(0);
	}
	m_BufferSize = 0;
}

void CHPQuantificationFileBufferWriter::WriteQuantificationEntry(int target, float quantification)
{
	m_TargetQuantificationBuffers[target].push_back(quantification);
	m_BufferSize += sizeof(float);
	if (m_BufferSize > m_MaxBufferSize)
	{
		FlushBuffer();
	}
}

void CHPQuantificationFileBufferWriter::FlushBuffer()
{
	if(m_BufferSize > 0) {
		for (int target=0; target<m_CHPFileNames->size(); target++)
		{
			CalvinCHPFileUpdater updater;
			updater.OpenCHPFile((*m_CHPFileNames)[target].c_str());
			updater.UpdateExpressionQuantificationBuffer(m_TargetQuantificationRowIndexes[target], m_TargetQuantificationBuffers[target]);
			updater.CloseCHPFile();
			m_TargetQuantificationRowIndexes[target] += m_TargetQuantificationBuffers[target].size();
			m_TargetQuantificationBuffers[target].clear();
		}
	}
	m_BufferSize = 0;
}
