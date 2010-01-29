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


#include "file/CHPFileUpdater.h"
//
#include "file/FileWriter.h"
//
#include "util/Err.h"
#include "util/Util.h"
//

using namespace std;
using namespace affxchp;
using namespace affxchpwriter;

//////////////////////////////////////////////////////////////////////

#define EXPRESSION_ABSOLUTE_STAT_ANALYSIS 2

//////////////////////////////////////////////////////////////////////

/*
 * Initialize any needed members.
 */
CCHPFileUpdater::CCHPFileUpdater() : CCHPFileWriter()
{
	m_CHPFile = NULL;
}

/*
 * Clean up.
 */
CCHPFileUpdater::~CCHPFileUpdater()
{
	CloseCHPFile();
}

void CCHPFileUpdater::OpenCHPFile(const char *fileName)
{
	CloseCHPFile();
	m_CHPFile = new ofstream(fileName, ios::binary | ios::app);
	if (!m_CHPFile) { Err::errAbort("CCHPFileUpdater::OpenCHPFile() - Unable to open CHP file for updating: " + ToStr(fileName)); }
}

void CCHPFileUpdater::UpdateExpressionEntry(int row, unsigned char Detection, float DetectionPValue, float Signal, unsigned short NumPairs, unsigned short NumUsedPairs)
{
	// Save the data size for the first probe set.
	if (row == 0)
	{
		unsigned char analysisType = EXPRESSION_ABSOLUTE_STAT_ANALYSIS;
		int resultsSize = UCHAR_SIZE + FLOAT_SIZE + FLOAT_SIZE + USHORT_SIZE + USHORT_SIZE; 
		WriteUInt8(*m_CHPFile, analysisType);
		WriteInt32_I(*m_CHPFile, resultsSize);
	}

	// Write the absolute data.
	WriteUInt8(*m_CHPFile, Detection);
	WriteFloat_I(*m_CHPFile, DetectionPValue);
	WriteFloat_I(*m_CHPFile, Signal);
	WriteUInt16_I(*m_CHPFile, NumPairs);
	WriteUInt16_I(*m_CHPFile, NumUsedPairs);
}

void CCHPFileUpdater::UpdateGenotypeEntry(int row, uint8_t call, float confidence, float RAS1, float RAS2, float aaCall, float abCall, float bbCall, float noCall)
{
	// Save the data size for the first probe set.
	if (row == 0)
	{
		int resultsSize = UCHAR_SIZE + FLOAT_SIZE + FLOAT_SIZE + FLOAT_SIZE + FLOAT_SIZE + FLOAT_SIZE; 
		WriteInt32_I(*m_CHPFile, resultsSize);
	}

	// Write probe set result.
	WriteUInt8(*m_CHPFile, call);
	WriteFloat_I(*m_CHPFile, confidence);
	if (aaCall == 0) { WriteFloat_I(*m_CHPFile, RAS1); }
	else { WriteFloat_I(*m_CHPFile, aaCall); }
	if (abCall == 0) { WriteFloat_I(*m_CHPFile, RAS2); }
	else { WriteFloat_I(*m_CHPFile, abCall); }
	WriteFloat_I(*m_CHPFile, bbCall);
	WriteFloat_I(*m_CHPFile, noCall);
}

void CCHPFileUpdater::CloseCHPFile()
{
	if (m_CHPFile != NULL)
	{
		if (m_CHPFile->is_open() == true) { m_CHPFile->close(); }
		delete m_CHPFile;
		m_CHPFile = NULL;
	}
}
