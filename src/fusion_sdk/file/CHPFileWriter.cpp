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

#include "file/CHPFileWriter.h"
//
#include "file/FileWriter.h"
//
#include <fstream>
#include <istream>
#include <sys/stat.h>
#include <sys/types.h>
//

#ifdef _INCLUDE_UNISTD_HEADER_
#include <unistd.h>
#endif

using namespace affxchp;
using namespace affxchpwriter;

//////////////////////////////////////////////////////////////////////

#define DELIMCHAR 0x14
#define MIN_CELLSTR 4
#define CHP_FILE_MAGIC_NUMBER 65
#define NON_RESEQ_CHP_FILE_VERSION_NUMBER 1
#define RESEQ_CHP_FILE_VERSION_NUMBER 2
#define EXPRESSION_ABSOLUTE_STAT_ANALYSIS 2
#define EXPRESSION_COMPARISON_STAT_ANALYSIS 3

//////////////////////////////////////////////////////////////////////

CCHPFileWriter::CCHPFileWriter() : CCHPFileData()
{
	m_ProbeSetIndex = 0;
}

//////////////////////////////////////////////////////////////////////

CCHPFileWriter::~CCHPFileWriter()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////

bool CCHPFileWriter::CreateNewFile()
{
	// Open the file.
	m_strError = "";
	m_NewChpFile.open(m_FileName.c_str(), std::ios::out | std::ios::binary);
	if (!m_NewChpFile)
	{
		m_strError = "Unable to open the file.";
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

void CCHPFileWriter::InitializeForWriting(affxcdf::CCDFFileData &cdf, bool allocateMemory)
{
	InitializeForWriting(
		cdf.GetHeader().GetRows(),
		cdf.GetHeader().GetCols(),
		cdf.GetHeader().GetNumProbeSets(),
		cdf.GetChipType().c_str(),
		cdf.GetProbeSetType(0),
		allocateMemory);
}

//////////////////////////////////////////////////////////////////////

void CCHPFileWriter::InitializeForWriting(int numRows, int numCols, int numProbeSets, const char *chipType, affxcdf::GeneChipProbeSetType probeSetType, bool allocateMemory)
{
	m_ProbeSetIndex = 0;

	// Set the header values.
	m_Header.SetCols(numCols);
	m_Header.SetRows(numRows);
	m_Header.SetNumProbeSets(numProbeSets);
	m_Header.SetChipType(chipType);

	switch (probeSetType)
	{
	case affxcdf::UnknownProbeSetType:
		m_Header.SetAssayType(CCHPFileHeader::Unknown);
		break;

	case affxcdf::ExpressionProbeSetType:
		m_Header.SetAssayType(CCHPFileHeader::Expression);
		break;

	case affxcdf::GenotypingProbeSetType:
		m_Header.SetAssayType(CCHPFileHeader::Genotyping);
		break;

	case affxcdf::ResequencingProbeSetType:
		m_Header.SetAssayType(CCHPFileHeader::Resequencing);
		break;

	case affxcdf::TagProbeSetType:
		m_Header.SetAssayType(CCHPFileHeader::Universal);
		break;

	default:
		m_Header.SetAssayType(CCHPFileHeader::Unknown);
		break;
	}

	// Don't allocate for resequencing.
	if (probeSetType == affxcdf::ResequencingProbeSetType)
		return;

	// Allocate memory for probe set results
	m_ProbeSetResults.clear();
	if (allocateMemory == true)
	{
		m_ProbeSetResults.resize(numProbeSets);
		CProbeSetResults *pResults;
		for (int iset=0; iset<numProbeSets; iset++)
		{
			switch (probeSetType)
			{
				case affxcdf::ExpressionProbeSetType:
					pResults = new CExpressionProbeSetResults;
					break;

				case affxcdf::GenotypingProbeSetType:
					pResults = new CGenotypeProbeSetResults;
					break;

				case affxcdf::TagProbeSetType:
					pResults = new CUniversalProbeSetResults;
					break;

				default:
					pResults = NULL;
					break;
			}
			m_ProbeSetResults[iset] = pResults;
		}
	}
}

//////////////////////////////////////////////////////////////////////

void CCHPFileWriter::SetParentCelFileName(const char *str)
{
	m_Header.SetParentCellFile(str);
}

//////////////////////////////////////////////////////////////////////

void CCHPFileWriter::SetProgID(const char *str)
{
	m_Header.SetProgID(str);
}

//////////////////////////////////////////////////////////////////////

void CCHPFileWriter::SetAlgorithmName(const char *str)
{
	std::string name = str;
	if (m_Header.GetAssayType() == CCHPFileHeader::Expression)
	{
		name = "Expression" + std::string(str);
	}

	else if (m_Header.GetAssayType() == CCHPFileHeader::Genotyping)
	{
		name = "Genotyping" + std::string(str);
	}

	m_Header.SetAlgName(name.c_str());
}

//////////////////////////////////////////////////////////////////////

void CCHPFileWriter::SetAlgorithmVersion(const char *str)
{
	m_Header.SetAlgVersion(str);
}

//////////////////////////////////////////////////////////////////////

void CCHPFileWriter::AddAlgorithmParameter(const char *tag, const char *value)
{
	TagValuePairType param;
	param.Tag = tag;
	param.Value = value;
	m_Header.AlgorithmParameters().push_back(param);
}

//////////////////////////////////////////////////////////////////////

void CCHPFileWriter::AddChipSummaryParameter(const char *tag, const char *value)
{
	TagValuePairType param;
	param.Tag = tag;
	param.Value = value;
	m_Header.SummaryParameters().push_back(param);
}


//////////////////////////////////////////////////////////////////////

void CCHPFileWriter::AddBackgroundInfo(int nZones, float smoothFactor)
{
	m_Header.GetBackgroundZoneInfo().number_zones = nZones;
	m_Header.GetBackgroundZoneInfo().smooth_factor = smoothFactor;
}

//////////////////////////////////////////////////////////////////////

void CCHPFileWriter::AddBackgroundZone(int x, int y, float value)
{
	BackgroundZoneType zone;
	zone.centerx = (float) x;
	zone.centery = (float) y;
	zone.background = value;
	m_Header.GetBackgroundZoneInfo().zones.push_back(zone);
}

//////////////////////////////////////////////////////////////////////

void CCHPFileWriter::SetExpressionEntry(int index, CExpressionProbeSetResults *pEntry)
{
	CExpressionProbeSetResults *pExpressionResult = GetExpressionResults(index);
	*pExpressionResult = *pEntry;
}

//////////////////////////////////////////////////////////////////////

void CCHPFileWriter::SetMappingEntry(int index, CGenotypeProbeSetResults *pEntry)
{
	CGenotypeProbeSetResults *pMappingResult = GetGenotypingResults(index);
	*pMappingResult = *pEntry;
}

//////////////////////////////////////////////////////////////////////

void CCHPFileWriter::SetUniversalEntry(int index, CUniversalProbeSetResults *pEntry)
{
	CUniversalProbeSetResults *pUniversalResult = GetUniversalResults(index);
	*pUniversalResult = *pEntry;
}

//////////////////////////////////////////////////////////////////////

void CCHPFileWriter::SetResequencingResults(CResequencingResults *pResults)
{
	m_ReseqResults = *pResults;
}

//////////////////////////////////////////////////////////////////////

bool CCHPFileWriter::SaveHeader()
	{
	// Only continue if genotyping or expression
	if (m_Header.GetAssayType() != CCHPFileHeader::Expression && 
		m_Header.GetAssayType() != CCHPFileHeader::Genotyping && 
		m_Header.GetAssayType() != CCHPFileHeader::Universal && 
		m_Header.GetAssayType() != CCHPFileHeader::Resequencing)
	{
		m_strError = "The software only supports writing expression, genotyping, universal or resequencing CHP files.";
		return false;
	}

	// Write the header
	int magic = CHP_FILE_MAGIC_NUMBER;
	WriteInt32_I(m_NewChpFile, magic);

	// All but resequencing is version 1
	int version = NON_RESEQ_CHP_FILE_VERSION_NUMBER;
	if (m_Header.GetAssayType() == CCHPFileHeader::Resequencing)
		version = RESEQ_CHP_FILE_VERSION_NUMBER;
	WriteInt32_I(m_NewChpFile, version);
	WriteUInt16_I(m_NewChpFile, m_Header.GetCols());
	WriteUInt16_I(m_NewChpFile, m_Header.GetRows());
	WriteInt32_I(m_NewChpFile, m_Header.GetNumProbeSets());
	WriteInt32_I(m_NewChpFile, 0); // no qc data extracted.
	WriteInt32_I(m_NewChpFile, m_Header.GetAssayType());
	WriteCString(m_NewChpFile, m_Header.GetProgID());
	WriteCString(m_NewChpFile, m_Header.GetParentCellFile());
	WriteCString(m_NewChpFile, m_Header.GetChipType());
	WriteCString(m_NewChpFile, m_Header.GetAlgName());
	WriteCString(m_NewChpFile, m_Header.GetAlgVersion());
	WriteInt32_I(m_NewChpFile, (int) m_Header.AlgorithmParameters().size());
	TagValuePairTypeList::iterator iter;
	for (iter=m_Header.AlgorithmParameters().begin(); iter!=m_Header.AlgorithmParameters().end(); ++iter)
	{
		WriteCString(m_NewChpFile, iter->Tag);
		WriteCString(m_NewChpFile, iter->Value);
	}
	WriteInt32_I(m_NewChpFile, (int) m_Header.SummaryParameters().size());
	for (iter=m_Header.SummaryParameters().begin(); iter!=m_Header.SummaryParameters().end(); ++iter)
	{
		WriteCString(m_NewChpFile, iter->Tag);
		WriteCString(m_NewChpFile, iter->Value);
	}

	// Write the zone info.
	WriteInt32_I(m_NewChpFile, m_Header.GetBackgroundZoneInfo().number_zones);
	WriteFloat_I(m_NewChpFile, m_Header.GetBackgroundZoneInfo().smooth_factor);
	BackgroundZoneTypeList::iterator start(m_Header.GetBackgroundZoneInfo().zones.begin());
	BackgroundZoneTypeList::iterator end(m_Header.GetBackgroundZoneInfo().zones.end());
	BackgroundZoneType zone;
	for (; start != end; ++start)
	{
		zone = (*start);
		WriteFloat_I(m_NewChpFile, start->centerx);
		WriteFloat_I(m_NewChpFile, start->centery);
		WriteFloat_I(m_NewChpFile, start->background);
	}

	return !m_NewChpFile.fail();
}

//////////////////////////////////////////////////////////////////////

bool CCHPFileWriter::SaveExpressionEntry(CExpressionProbeSetResults *pEntry)
{
	// Save the data size for the first probe set.
	if (m_ProbeSetIndex == 0)
	{
		int resultsSize = UCHAR_SIZE + FLOAT_SIZE + FLOAT_SIZE + USHORT_SIZE + USHORT_SIZE; 
		unsigned char analysisType = EXPRESSION_ABSOLUTE_STAT_ANALYSIS;
		if (pEntry->m_HasCompResults)
		{
			analysisType = EXPRESSION_COMPARISON_STAT_ANALYSIS;
			resultsSize += UCHAR_SIZE + FLOAT_SIZE + FLOAT_SIZE + FLOAT_SIZE + FLOAT_SIZE + USHORT_SIZE;
		}
		WriteUInt8(m_NewChpFile, analysisType);
		WriteInt32_I(m_NewChpFile, resultsSize);
	}

	// Write the absolute data.
	WriteUInt8(m_NewChpFile, pEntry->Detection);
	WriteFloat_I(m_NewChpFile, pEntry->DetectionPValue);
	WriteFloat_I(m_NewChpFile, pEntry->Signal);
	WriteUInt16_I(m_NewChpFile, pEntry->NumPairs);
	WriteUInt16_I(m_NewChpFile, pEntry->NumUsedPairs);

	// Write the comparison data
	if (pEntry->m_HasCompResults == true)
	{
		WriteUInt8(m_NewChpFile, pEntry->Change);
		WriteFloat_I(m_NewChpFile, pEntry->ChangePValue);
		WriteFloat_I(m_NewChpFile, pEntry->SignalLogRatio);
		WriteFloat_I(m_NewChpFile, pEntry->SignalLogRatioLow);
		WriteFloat_I(m_NewChpFile, pEntry->SignalLogRatioHigh);
		WriteUInt16_I(m_NewChpFile, pEntry->NumCommonPairs);
	}

	++m_ProbeSetIndex;

	return !m_NewChpFile.fail();
}

//////////////////////////////////////////////////////////////////////

bool CCHPFileWriter::SaveMappingEntry(affxchp::CGenotypeProbeSetResults *pEntry)
{
	// Save the data size for the first probe set.
	float fval;
	if (m_ProbeSetIndex == 0)
	{
		int resultsSize = UCHAR_SIZE + FLOAT_SIZE + FLOAT_SIZE + FLOAT_SIZE + FLOAT_SIZE + FLOAT_SIZE;
		WriteInt32_I(m_NewChpFile, resultsSize);
	}

	// Write probe set result.
	WriteUInt8(m_NewChpFile, pEntry->AlleleCall);
	WriteFloat_I(m_NewChpFile, pEntry->Confidence);

	fval = pEntry->pvalue_AA;
	if (fval == 0)
		fval = pEntry->RAS1;
	WriteFloat_I(m_NewChpFile, fval);

	fval = pEntry->pvalue_AB;
	if (fval == 0)
		fval = pEntry->RAS2;
	WriteFloat_I(m_NewChpFile, fval);

	WriteFloat_I(m_NewChpFile, pEntry->pvalue_BB);
	WriteFloat_I(m_NewChpFile, pEntry->pvalue_NoCall);

	++m_ProbeSetIndex;

	return !m_NewChpFile.fail();
}

//////////////////////////////////////////////////////////////////////

bool CCHPFileWriter::SaveUniversalEntry(affxchp::CUniversalProbeSetResults *pEntry)
{
	// Save the data size for the first probe set.
	if (m_ProbeSetIndex == 0)
	{
		int resultsSize = FLOAT_SIZE;
		WriteInt32_I(m_NewChpFile, resultsSize);
	}
	WriteFloat_I(m_NewChpFile, pEntry->GetBackground());

	++m_ProbeSetIndex;

	return !m_NewChpFile.fail();
}

//////////////////////////////////////////////////////////////////////

bool CCHPFileWriter::Save()
{
	if (SaveHeader() == false)
		return false;

	// Write the probe set data
	if (m_Header.GetAssayType() == CCHPFileHeader::Expression)
	{
		// Set the type of analysis
		for (int iset=0; iset<m_Header.GetNumProbeSets(); iset++)
		{
			CExpressionProbeSetResults * pResults = GetExpressionResults(iset);
			SaveExpressionEntry(pResults);
		}
	}
	else if (m_Header.GetAssayType() == CCHPFileHeader::Genotyping)
	{
		for (int iset=0; iset<m_Header.GetNumProbeSets(); iset++)
		{
			CGenotypeProbeSetResults * pResults = GetGenotypingResults(iset);
			SaveMappingEntry(pResults);
		}
	}
	else if (m_Header.GetAssayType() == CCHPFileHeader::Universal)
	{
		for (int iset=0; iset<m_Header.GetNumProbeSets(); iset++)
		{
			CUniversalProbeSetResults * pResults = GetUniversalResults(iset);
			SaveUniversalEntry(pResults);
		}
	}
	else if (m_Header.GetAssayType() == CCHPFileHeader::Resequencing)
	{
		int32_t dataSize=0;
		int index;

		// Write the data size
		dataSize =
			(
			(m_ReseqResults.GetCalledBasesSize() * (sizeof(char) + sizeof(float))) +
			(m_ReseqResults.GetForceCallsSize() * (sizeof(int) + sizeof(char) + sizeof(unsigned char))) +
			(m_ReseqResults.GetOrigCallsSize() * (sizeof(int) + sizeof(char)))
			);
		WriteInt32_I(m_NewChpFile, dataSize);

		// Write the base calls and scores.
		dataSize = m_ReseqResults.GetCalledBasesSize();
		WriteInt32_I(m_NewChpFile, dataSize);
		for (index=0; index<dataSize; index++)
		{
			WriteInt8(m_NewChpFile, m_ReseqResults.GetCalledBase(index));
		}
		for (index=0; index<dataSize; index++)
		{
			WriteFloat_I(m_NewChpFile, m_ReseqResults.GetScore(index));
		}

		// Write the force calls
		dataSize = m_ReseqResults.GetForceCallsSize();
		WriteInt32_I(m_NewChpFile, dataSize);
		ForceCallType forceCall;
		for (index=0; index<dataSize; index++)
		{
			forceCall = m_ReseqResults.GetForceCall(index);
			WriteInt32_I(m_NewChpFile, forceCall.position);
			WriteInt8(m_NewChpFile, forceCall.call);
			WriteUInt8(m_NewChpFile, forceCall.reason);
		}

		// Read the orig calls
		dataSize = m_ReseqResults.GetOrigCallsSize();
		WriteInt32_I(m_NewChpFile, dataSize);
		BaseCallType baseCall;
		for (index=0; index<dataSize; index++)
		{
			baseCall = m_ReseqResults.GetOrigCall(index);
			WriteInt32_I(m_NewChpFile, baseCall.position);
			WriteInt8(m_NewChpFile, baseCall.call);
		}
	}

	// Close the file and check the status.
	return Close();
}

//////////////////////////////////////////////////////////////////////

bool CCHPFileWriter::Close()
{
	if (m_NewChpFile.is_open() == true)
	{
		m_NewChpFile.close();
		return !m_NewChpFile.fail();
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
