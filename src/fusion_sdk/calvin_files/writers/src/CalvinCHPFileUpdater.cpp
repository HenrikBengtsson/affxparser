/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006 Affymetrix, Inc.
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

#include "CalvinCHPFileUpdater.h"
#include "FileOutput.h"
#include "util/Err.h"
#include "util/Util.h"

using namespace std;
using namespace affymetrix_calvin_io;

/*! The data set index. */
#define ENTRY_DATA_SET 0

/*! The data group index. */
#define ENTRY_DATA_GROUP 0

/*! The signal column. */
#define ENTRY_COLUMN 1

/*
 * Initialize any needed members.
 */
CalvinCHPFileUpdater::CalvinCHPFileUpdater() : DataSetUpdater()
{
	m_CHPFile = NULL;
}

/*
 * Clean up.
 */
CalvinCHPFileUpdater::~CalvinCHPFileUpdater()
{
	CloseCHPFile();
}

void CalvinCHPFileUpdater::OpenCHPFile(const char *fileName)
{
	CloseCHPFile();
	Initialize(fileName);
	m_CHPFile = new ofstream(fileName, ios::out | ios::in | ios::binary);
	if (!m_CHPFile) { Err::errAbort("Error: CalvinCHPFileUpdater::OpenCHPFile() - Unable to open CHP file for updating: " + ToStr(fileName)); }
}

void CalvinCHPFileUpdater::UpdateExpressionQuantification(int row, float quantification)
{
	SeekToPosition(*m_CHPFile, ENTRY_DATA_GROUP, ENTRY_DATA_SET, row, ENTRY_COLUMN);
	FileOutput::WriteFloat(*m_CHPFile, quantification);
}

void CalvinCHPFileUpdater::UpdateGenotypeEntry(int row, u_int8_t call, float confidence, float RAS1, float RAS2, float aaCall, float abCall, float bbCall, float noCall)
{
	SeekToPosition(*m_CHPFile, ENTRY_DATA_GROUP, ENTRY_DATA_SET, row, ENTRY_COLUMN);
	FileOutput::WriteUInt8(*m_CHPFile, call);
	FileOutput::WriteFloat(*m_CHPFile, confidence);
	FileOutput::WriteFloat(*m_CHPFile, RAS1);
	FileOutput::WriteFloat(*m_CHPFile, RAS2);
	FileOutput::WriteFloat(*m_CHPFile, aaCall);
	FileOutput::WriteFloat(*m_CHPFile, abCall);
	FileOutput::WriteFloat(*m_CHPFile, bbCall);
	FileOutput::WriteFloat(*m_CHPFile, noCall);
}

void CalvinCHPFileUpdater::CloseCHPFile()
{
	if (m_CHPFile != NULL)
	{
		if (m_CHPFile->is_open() == true) { m_CHPFile->close(); }
		delete m_CHPFile;
		m_CHPFile = NULL;
	}
}
