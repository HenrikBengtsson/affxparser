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

#include "file/CHPFileData.h"
//
#include "file/FileIO.h"
//
#include "util/Fs.h"
//
#include <cstring>
#include <fstream>
#include <istream>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
//

#ifdef _INCLUDE_UNISTD_HEADER_
#include <unistd.h>
#endif

using namespace affxchp;

//////////////////////////////////////////////////////////////////////

/*! The CHP file magic number */
#define CHP_FILE_MAGIC_NUMBER 65

/*! The max CHP file version the parser can read */
#define CHP_FILE_VERSION_NUMBER 2

/*! Identifier to indicate absolute expression analysis results stored. */
#define EXPRESSION_ABSOLUTE_STAT_ANALYSIS 2

/*! Identifier to indicate comparison expression analysis results stored. */
#define EXPRESSION_COMPARISON_STAT_ANALYSIS 3

/*! Used to convert floating point values stored as ints in older CHP files. */
#define ROUNDFLOAT 1000

//////////////////////////////////////////////////////////////////////

void CCHPFileHeader::ParseString
(
	TagValuePairTypeList &tagList,
	std::string sSource, 
	std::string sDelimiter1, 
	std::string sDelimiter2
)
{
	std::string buf;
	TagValuePairType param;
	std::list<std::string> delList;
	std::list<std::string> delList2;

	// Skip delimiters at beginning and find first "non-delimiter" (1st delimiter)
	std::string::size_type lastPos1 = sSource.find_first_not_of(sDelimiter1, 0);
	std::string::size_type pos1     = sSource.find_first_of(sDelimiter1, lastPos1);

	// Loop to get tokens
    while (std::string::npos != pos1 || std::string::npos != lastPos1)
	{		
		// Get and store all parsed tokens
		buf = sSource.substr(lastPos1, pos1 - lastPos1);
		delList.push_back(buf);

		// Skip delimiters and find next "non-delimiter" (1st delimiter)
		lastPos1 = sSource.find_first_not_of(sDelimiter1, pos1);
		pos1 = sSource.find_first_of(sDelimiter1, lastPos1);
	}

	// Reverse through stored tokens
	std::string sTemp;
	std::list<std::string>::reverse_iterator rev(delList.end());
	std::list<std::string>::reverse_iterator rev_end(delList.begin());
	for (; rev != rev_end; ++rev)
	{
		buf = (*rev);

		// Make sure "buf" has 2nd delimiter, otherwise add to previous "buf"
		std::string::size_type posTemp = buf.find_first_of(sDelimiter2, 0);
		if (std::string::npos == posTemp)
		{
			// Need to add the 1st delimiter back in the front of the "buf"
			// Don't know how many delimiters to add, so let's just add one
			buf = sDelimiter1 + buf;
			sTemp = buf + sTemp;
		}
		else
		{
			// Finally store the valid "name=value" token
			buf += sTemp;
			delList2.push_back(buf);
			sTemp = "";
		}
	}

	// Now lets finally iterate over tokens to parse the "name" and "value"
	std::list<std::string>::iterator iter;
	for (iter=delList2.begin(); iter!=delList2.end(); ++iter)
	{
		buf = (*iter);

		// Skip delimiters at beginning and find first "non-delimiter" (2nd delimiter)
		std::string::size_type lastPos2 = buf.find_first_not_of(sDelimiter2, 0);
		std::string::size_type pos2     = buf.find_first_of(sDelimiter2, lastPos2);
		
		// Validate before assigning "Tag"
		if (std::string::npos != pos2 || std::string::npos != lastPos2)
		{
			param.Tag = buf.substr(lastPos2, pos2 - lastPos2);
			param.Value = "";

			// Skip delimiters and find next "non-delimiter" (2nd delimiter)
			lastPos2 = buf.find_first_not_of(sDelimiter2, pos2);
			pos2 = buf.find_first_of(sDelimiter2, lastPos2);

			// Validate before assigning "Value"
			if (std::string::npos != pos2 || std::string::npos != lastPos2)
				param.Value = buf.substr(lastPos2, pos2 - lastPos2);

			// Add to tag list
			tagList.push_back(param);
		}
	}
}

//////////////////////////////////////////////////////////////////////

std::string CCHPFileHeader::GetAlgorithmParameter(const char *tag)
{
	std::string value;
	TagValuePairTypeList::iterator iter;
	for (iter=m_AlgorithmParameters.begin(); iter!=m_AlgorithmParameters.end(); ++iter)
	{
		if (*iter == tag)
		{
			value = iter->Value;
			break;
		}
	}
	return value;
}

//////////////////////////////////////////////////////////////////////

std::string CCHPFileHeader::GetSummaryParameter(const char *tag)
{
	std::string value;
	TagValuePairTypeList::iterator iter;
	for (iter=m_SummaryParameters.begin(); iter!=m_SummaryParameters.end(); ++iter)
	{
		if (*iter == tag)
		{
			value = iter->Value;
			break;
		}
	}
	return value;
}

//////////////////////////////////////////////////////////////////////

BackgroundZoneType CCHPFileHeader::GetBackgroundZone(int x, int y)
{
	BackgroundZoneType zone = {0.0, 0.0, 0.0};

	BackgroundZoneTypeList::iterator iter;
	for (iter=m_BackgroundZoneInfo.zones.begin(); iter!=m_BackgroundZoneInfo.zones.end(); ++iter)
	{
		if (iter->centerx == x && iter->centery == y)
		{
			zone = (*iter);
			break;
		}
	}
	return zone;
}

//////////////////////////////////////////////////////////////////////

CCHPFileHeader::CCHPFileHeader()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////

void CCHPFileHeader::Clear()
{
	m_Magic=0;
	m_Version=0;
	m_Cols=0;
	m_Rows=0;
	m_NumProbeSets=0;
	m_AssayType=Unknown;
	m_ChipType = "";
	m_AlgorithmName = "";
	m_AlgorithmVersion = "";
	m_ParentCellFile = "";
	m_ProgID = "";
	m_AlgorithmParameters.erase(m_AlgorithmParameters.begin(), m_AlgorithmParameters.end());
	m_SummaryParameters.erase(m_SummaryParameters.begin(), m_SummaryParameters.end());
	m_BackgroundZoneInfo.number_zones = 0;
	m_BackgroundZoneInfo.smooth_factor = 0.0f;
	m_BackgroundZoneInfo.zones.erase(m_BackgroundZoneInfo.zones.begin(), m_BackgroundZoneInfo.zones.end());
}

//////////////////////////////////////////////////////////////////////

CCHPFileHeader::~CCHPFileHeader()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////

CCHPFileData::CCHPFileData()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////

CCHPFileData::~CCHPFileData()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////

void CCHPFileData::Clear()
{
	m_Header.Clear();
	m_FileName = "";
	m_strError = "";
	int n = (int) m_ProbeSetResults.size();
	for (int i=0; i<n; i++)
	{
		delete m_ProbeSetResults[i];
		m_ProbeSetResults[i]=NULL;
	}
	m_ProbeSetResults.erase(m_ProbeSetResults.begin(), m_ProbeSetResults.end());
}

//////////////////////////////////////////////////////////////////////

CExpressionProbeSetResults *CCHPFileData::GetExpressionResults(int index)
{
	if (index < m_Header.GetNumProbeSets() && m_Header.GetAssayType() == CCHPFileHeader::Expression)
	{
		return (CExpressionProbeSetResults *) m_ProbeSetResults[index];
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////

CGenotypeProbeSetResults *CCHPFileData::GetGenotypingResults(int index)
{
	if (index < m_Header.GetNumProbeSets() && m_Header.GetAssayType() == CCHPFileHeader::Genotyping)
	{
		return (CGenotypeProbeSetResults *) m_ProbeSetResults[index];
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////

CResequencingResults *CCHPFileData::GetResequencingResults()
{
	return (m_Header.GetAssayType() == CCHPFileHeader::Resequencing ? &m_ReseqResults : NULL);
}

//////////////////////////////////////////////////////////////////////

CUniversalProbeSetResults *CCHPFileData::GetUniversalResults(int index)
{
	if (index < m_Header.GetNumProbeSets() && m_Header.GetAssayType() == CCHPFileHeader::Universal)
	{
		return (CUniversalProbeSetResults *) m_ProbeSetResults[index];
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////

CUniversalProbeSetResults CUniversalProbeSetResults::operator = (CUniversalProbeSetResults &src)
{
	SetBackground(src.GetBackground());
	return *this;
}

//////////////////////////////////////////////////////////////////////

std::string CGenotypeProbeSetResults::GetAlleleCallString()
{
	switch (AlleleCall)
	{
	case ALLELE_A_CALL:
		return "A";
		break;

	case ALLELE_B_CALL:
		return "B";
		break;

	case ALLELE_AB_CALL:
		return "AB";break;

	default:
		return "No Call";
		break;
	}
	return "";
}

//////////////////////////////////////////////////////////////////////

CGenotypeProbeSetResults CGenotypeProbeSetResults::operator = (CGenotypeProbeSetResults &src)
{
	AlleleCall = src.AlleleCall;
	Confidence = src.Confidence;
	RAS1 = src.RAS1;
	RAS2 = src.RAS2;
	pvalue_AA = src.pvalue_AA;
	pvalue_AB = src.pvalue_AB;
	pvalue_BB = src.pvalue_BB;
	pvalue_NoCall = src.pvalue_NoCall;

	return *this;
}

//////////////////////////////////////////////////////////////////////

std::string CExpressionProbeSetResults::GetDetectionString()
{
	switch (Detection)
	{
	case ABS_PRESENT_CALL:
		return "P";
		break;

	case ABS_MARGINAL_CALL:
		return "M";
		break;

	case ABS_ABSENT_CALL:
		return "A";
		break;

	case ABS_NO_CALL:
		return "No Call";
		break;

	default:
		break;
	}
	return "";
}

//////////////////////////////////////////////////////////////////////

std::string CExpressionProbeSetResults::GetChangeString()
{
	switch (Change)
	{
	case COMP_INCREASE_CALL:
		return "I";
		break;

	case COMP_DECREASE_CALL:
		return "D";
		break;

	case COMP_MOD_INCREASE_CALL:
		return "MI";
		break;

	case COMP_MOD_DECREASE_CALL:
		return "MD";
		break;

	case COMP_NO_CHANGE_CALL:
		return "NC";
		break;

	case COMP_NO_CALL:
		return "No Call";
		break;

	default:
		break;
	}

	return "";
}

//////////////////////////////////////////////////////////////////////

CExpressionProbeSetResults CExpressionProbeSetResults::operator = (CExpressionProbeSetResults &src)
{
	DetectionPValue = src.DetectionPValue;
	Signal = src.Signal; 
	NumPairs = src.NumPairs;
	NumUsedPairs = src.NumUsedPairs;
	Detection = src.Detection;
	m_HasCompResults = src.m_HasCompResults;
	ChangePValue = src.ChangePValue; 
	SignalLogRatio = src.SignalLogRatio; 
	SignalLogRatioLow = src.SignalLogRatioLow; 
	SignalLogRatioHigh = src.SignalLogRatioHigh; 
	NumCommonPairs = src.NumCommonPairs;
	Change = src.Change;

	return *this;
}

//////////////////////////////////////////////////////////////////////

bool CCHPFileData::Read()
{
	// Open the file
	if (Open() == false)
	{
		Clear();
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CCHPFileData::ReadHeader()
{
	// Read the header, close if failed.
	if (Open(true) == false)
	{
		Clear();
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CCHPFileData::Exists()
{
	// Find the file stats.
	struct stat st;
	return (stat(m_FileName.c_str(), &st) == 0);
}

//////////////////////////////////////////////////////////////////////

bool CCHPFileData::Open(bool bReadHeaderOnly)
{
	bool retVal = false;

	// Open the file.
	std::ifstream instr;
	bool bXDA = false;
	if (IsXDACompatibleFile())
		bXDA = true;
	Fs::aptOpen(instr, m_FileName, std::ios::in | std::ios::binary);

	// Check if open
	if (!instr)
	{
		m_strError = "Unable to open the file.";
		return false;
	}


	// Read the header
	if (bXDA)
	{
		int32_t magic;
		int32_t version;
		int32_t ival;
		uint16_t usval;
		char *sval=NULL;
		float fval;
		uint8_t ucval;


		// Read the magic number.
		ReadInt32_I(instr, magic);
		m_Header.m_Magic = magic;

		// Check if new type.
		if (magic != CHP_FILE_MAGIC_NUMBER)
		{
			m_strError = "The file does not appear to be the correct format.";
			return false;
		}

		// Read the version
		ReadInt32_I(instr, version);
		m_Header.m_Version = version;

		// Check for version 1 or 2
		if (version > CHP_FILE_VERSION_NUMBER)
		{
			m_strError = "Unable to read this version of the CHP file.";
			return false;
		}

		// Get the dimensions of the array
		ReadUInt16_I(instr, usval);
		m_Header.m_Cols=usval;
		ReadUInt16_I(instr, usval);
		m_Header.m_Rows=usval;

		// Number of probe sets.
		ReadInt32_I(instr, ival);
		m_Header.m_NumProbeSets = ival;
		ReadInt32_I(instr, ival); // no qc data extracted.

		// Assay type
		ReadInt32_I(instr, ival);
		m_Header.m_AssayType = (CCHPFileHeader::GeneChipAssayType)ival;

		// Prog ID.
		ReadCString_I(instr, sval);
		m_Header.m_ProgID = sval;
		delete[] sval;

		// Parent cell file.
		ReadCString_I(instr, sval);
		m_Header.m_ParentCellFile = sval;
		delete[] sval;

		// Chip type
		ReadCString_I(instr, sval);
		m_Header.m_ChipType = sval;
		delete[] sval;

		// Algorithm
		ReadCString_I(instr, sval);
		m_Header.m_AlgorithmName = sval;
		delete[] sval;

		// Algorithm version
		ReadCString_I(instr, sval);
		m_Header.m_AlgorithmVersion = sval;
		delete[] sval;

		// Algorithm parameters.
		int32_t nParams=0;
		int32_t iParam;
		ReadInt32_I(instr, nParams);
		TagValuePairType param;
		for (iParam=0; iParam<nParams; iParam++)
		{
			ReadCString_I(instr, sval);
			param.Tag = sval;
			delete[] sval;

			ReadCString_I(instr, sval);
			param.Value = sval;
			delete[] sval;

			m_Header.m_AlgorithmParameters.push_back(param);
		}

		// Summary parameters
		ReadInt32_I(instr, nParams);
		for (iParam=0; iParam<nParams; iParam++)
		{
			ReadCString_I(instr, sval);
			param.Tag = sval;
			delete[] sval;

			ReadCString_I(instr, sval);
			param.Value = sval;
			delete[] sval;

			m_Header.m_SummaryParameters.push_back(param);
		}


		// Background info		
		ReadInt32_I(instr, nParams);
		m_Header.m_BackgroundZoneInfo.number_zones = nParams;
		ReadFloat_I(instr, fval);
		m_Header.m_BackgroundZoneInfo.smooth_factor = fval;

		// Background zones
		BackgroundZoneType zone;
		for (iParam=0; iParam<nParams; iParam++)
		{
			ReadFloat_I(instr, fval);
			zone.centerx = fval;
			ReadFloat_I(instr, fval);
			zone.centery = fval;
			ReadFloat_I(instr, fval);
			zone.background = fval;

			m_Header.m_BackgroundZoneInfo.zones.push_back(zone);
		}

		// Return if only reading the header.
		if (bReadHeaderOnly == true)
			return true;


		// Only continue if genotyping or expression
		if (m_Header.m_AssayType != CCHPFileHeader::Expression &&
			m_Header.m_AssayType != CCHPFileHeader::Genotyping &&
			m_Header.m_AssayType != CCHPFileHeader::Universal &&
			m_Header.m_AssayType != CCHPFileHeader::Resequencing)
		{
			m_strError = "The software only supports reading expression, genotyping, tag or resequencing CHP files.";
			return false;
		}

		// Read the probe set data
		if (m_Header.m_AssayType == CCHPFileHeader::Expression)
		{
			m_ProbeSetResults.resize(m_Header.m_NumProbeSets);
			// Get the type of analysis
			ReadUInt8(instr, ucval); // EXPRESSION_ABSOLUTE_STAT_ANALYSIS or EXPRESSION_COMPARISON_STAT_ANALYSIS
			ReadInt32_I(instr, ival);

			int analysisType=ucval;
			if (analysisType != EXPRESSION_ABSOLUTE_STAT_ANALYSIS && analysisType != EXPRESSION_COMPARISON_STAT_ANALYSIS)
			{
				m_strError = "The software only supports reading MAS 5 and above expression CHP files.";
				return false;
			}

			// Read each probe set result.
			for (int iset=0; iset<m_Header.m_NumProbeSets; iset++)
			{
				CExpressionProbeSetResults * pResults = new CExpressionProbeSetResults;
				m_ProbeSetResults[iset] = pResults;

				// Read the absolute data.
				ReadUInt8(instr, ucval);
				pResults->Detection = ucval;

				ReadFloat_I(instr, fval);
				pResults->DetectionPValue = fval;
				
				ReadFloat_I(instr, fval);
				pResults->Signal = fval;
				
				ReadUInt16_I(instr, usval);
				pResults->NumPairs = usval;
				
				ReadUInt16_I(instr, usval);
				pResults->NumUsedPairs = usval;

				pResults->m_HasCompResults = false;


				// Read the comparison data
				if (analysisType == EXPRESSION_COMPARISON_STAT_ANALYSIS)
				{
					pResults->m_HasCompResults = true;

					ReadUInt8(instr, ucval);
					pResults->Change = ucval;

					ReadFloat_I(instr, fval);
					pResults->ChangePValue = fval;

					ReadFloat_I(instr, fval);
					pResults->SignalLogRatio = fval;

					ReadFloat_I(instr, fval);
					pResults->SignalLogRatioLow = fval;

					ReadFloat_I(instr, fval);
					pResults->SignalLogRatioHigh = fval;

					ReadUInt16_I(instr, usval);
					pResults->NumCommonPairs = usval;
				}
			}
		}
		else if (m_Header.m_AssayType == CCHPFileHeader::Genotyping)
		{
			m_ProbeSetResults.resize(m_Header.m_NumProbeSets);
			const int DM_ALG_RESULT_SIZE = 21;
			int32_t dataSize=0;
			ReadInt32_I(instr, dataSize);
			for (int iset=0; iset<m_Header.m_NumProbeSets; iset++)
			{
				CGenotypeProbeSetResults * pResults = new CGenotypeProbeSetResults;
				m_ProbeSetResults[iset] = pResults;

				// Read probe set result.
				ReadUInt8(instr, ucval);
				pResults->AlleleCall = ucval;

				ReadFloat_I(instr, fval);
				pResults->Confidence = fval;

				ReadFloat_I(instr, fval);
				pResults->RAS1 = fval;
				pResults->pvalue_AA = fval;

				ReadFloat_I(instr, fval);
				pResults->RAS2 = fval;
				pResults->pvalue_AB = fval;

				if (dataSize == DM_ALG_RESULT_SIZE)
				{
					ReadFloat_I(instr, fval);
					pResults->pvalue_BB = fval;

					ReadFloat_I(instr, fval);
					pResults->pvalue_NoCall = fval;
				}
			}
		}
		else if (m_Header.m_AssayType == CCHPFileHeader::Universal)
		{
			m_ProbeSetResults.resize(m_Header.m_NumProbeSets);
			int32_t dataSize=0;
			float bg;
			ReadInt32_I(instr, dataSize);
			for (int iset=0; iset<m_Header.m_NumProbeSets; iset++)
			{
				CUniversalProbeSetResults * pResults = new CUniversalProbeSetResults;
				m_ProbeSetResults[iset] = pResults;

				// Read probe set result.
				ReadFloat_I(instr, bg);
				pResults->SetBackground(bg);
			}
		}

		else if (m_Header.m_AssayType == CCHPFileHeader::Resequencing)
		{
			int32_t dataSize=0;
			int8_t call;
			int32_t pos;
			uint8_t reason;
			float  score;
			int index;

			// Read the data size
			ReadInt32_I(instr, dataSize);

			// Read the base calls and scores.
			m_ReseqResults.Clear();
			ReadInt32_I(instr, dataSize);
			m_ReseqResults.ResizeCalledBases(dataSize);
			m_ReseqResults.ResizeScores(dataSize);
			for (index=0; index<dataSize; index++)
			{
				ReadInt8(instr, call);
				m_ReseqResults.SetCalledBase(index, (char)call);
			}
			for (index=0; index<dataSize; index++)
			{
				ReadFloat_I(instr, score);
				m_ReseqResults.SetScore(index, score);
			}

			// Read the force and original calls.
			if (m_Header.m_Version >= 2)
			{
				// Read the force calls
				ReadInt32_I(instr, dataSize);
				m_ReseqResults.ResizeForceCalls(dataSize);
				ForceCallType forceCall;
				for (index=0; index<dataSize; index++)
				{
					ReadInt32_I(instr, pos);
					forceCall.position = pos;
					ReadInt8(instr, call);
					forceCall.call = call;
					ReadUInt8(instr, reason);
					forceCall.reason = reason;
					m_ReseqResults.SetForceCall(index, forceCall);
				}

				// Read the orig calls
				ReadInt32_I(instr, dataSize);
				m_ReseqResults.ResizeOrigCalls(dataSize);
				BaseCallType baseCall;
				for (index=0; index<dataSize; index++)
				{
					ReadInt32_I(instr, pos);
					baseCall.position = pos;
					ReadInt8(instr, call);
					baseCall.call = call;
					m_ReseqResults.SetOrigCall(index, baseCall);
				}
			}
		}

		retVal = true;
	}
	else
	{
		// Read the string that defines the CHP file (older format).
		char vString[256]="";
		char AppName[256]="GeneChip Sequence File";
		ReadFixedCString(instr, vString, (int) strlen(AppName));
		if (strcmp(vString, AppName))
		{
			m_strError = "The file does not appear to be the correct format.";
			return false;
		}

		// Read the version number
		int8_t cval;
		uint8_t ucval;
		uint16_t usval;
		int32_t ival;
		float fval;
		char *sval;
		ReadInt32_I(instr, ival);
		m_Header.m_Version = ival;

		// Read algorithm type string.
		ReadCString_I(instr, sval);
		m_Header.m_AlgorithmName=sval;
		delete[] sval;

		ReadCString_I(instr, sval);
		m_Header.m_AlgorithmVersion=sval;
		delete[] sval;

		// Read parameters.
		ReadCString_I(instr, sval);
		std::string sSource = sval;
		m_Header.ParseString(m_Header.m_AlgorithmParameters, sSource, " ", "=");
		delete[] sval;

		// Read summary
		ReadCString_I(instr, sval);
		std::string sSummary = sval;
		m_Header.ParseString(m_Header.m_SummaryParameters, sSummary, " ", "=");
		delete[] sval;

		// rows
		ReadInt32_I(instr, ival);
		m_Header.m_Rows = ival;

		// cols
		ReadInt32_I(instr, ival);
		m_Header.m_Cols = ival;

		// #probe sets
		ReadInt32_I(instr, ival);
		m_Header.m_NumProbeSets = ival;

		// the maximum probe set number in the array
		ReadInt32_I(instr, ival);
		int maxvalue = ival;

		// #qc probe sets
		ReadInt32_I(instr, ival);

		// probe set numbers
		int j;
		for (j=0; j<m_Header.m_NumProbeSets; j++)
			ReadInt32_I(instr, ival);

		// #probe pairs in each probe set.
		for (j=0; j<maxvalue; j++)
			ReadInt32_I(instr, ival);
		
		// type of probe set
		for (j=0; j<maxvalue; j++)
		{
			ReadInt32_I(instr, ival);
			if (j == 0)
			{
				switch (ival)
				{
				case 1:
					m_Header.m_AssayType = CCHPFileHeader::Resequencing;
					break;

				case 2:
					m_Header.m_AssayType = CCHPFileHeader::Genotyping;
					break;

				case 3:
					m_Header.m_AssayType = CCHPFileHeader::Expression;
					break;

				case 7:
					m_Header.m_AssayType = CCHPFileHeader::Universal;
					break;

				default:
					m_Header.m_AssayType = CCHPFileHeader::Unknown;
					break;
				}
			}
		}

		// This must be a resequencing design if there are no probe sets.
		if (m_Header.m_NumProbeSets == 0)
			m_Header.m_AssayType = CCHPFileHeader::Resequencing;

		// Check for valid versions.
		if ((m_Header.m_Version < 12 && (m_Header.m_AssayType == CCHPFileHeader::Expression || m_Header.m_AssayType == CCHPFileHeader::Genotyping )) ||
			(m_Header.m_Version < 10 && m_Header.m_AssayType == CCHPFileHeader::Universal))
		{
			m_strError = "This version of the CHP file is not supported by the parser.";
			return false;
		}


		// # probe probes per element (probe pair, probe quartet, etc).
		short *tagCells=NULL;
		if (m_Header.m_AssayType == CCHPFileHeader::Universal && bReadHeaderOnly == false)
			tagCells = new short[m_Header.m_NumProbeSets];
		for (j=0; j<m_Header.m_NumProbeSets; j++)
		{
			ReadInt32_I(instr, ival);
			if (tagCells)
				tagCells[j] = (short)ival;
		}

		// Get the chip type
		char ctype[256];
		ReadFixedCString(instr, ctype, 256);
		m_Header.m_ChipType = ctype;

		// The parent CEL file.
		ReadFixedCString(instr, ctype, 256);
		m_Header.m_ParentCellFile = ctype;

		// The prog ID.
		ReadCString_I(instr, sval);
		m_Header.m_ProgID = sval;
		delete [] sval;


		// Return if only reading the header.
		if (bReadHeaderOnly == true)
			return true;


		// Only continue if genotyping or expression
		if (m_Header.m_AssayType != CCHPFileHeader::Expression &&
			m_Header.m_AssayType != CCHPFileHeader::Genotyping &&
			m_Header.m_AssayType != CCHPFileHeader::Universal &&
			m_Header.m_AssayType != CCHPFileHeader::Resequencing)
		{
			m_strError = "The software only supports reading expression, genotyping, tag or resequencing CHP files.";
			return false;
		}

		// Read the probe set data
		if (m_Header.m_AssayType == CCHPFileHeader::Expression)
		{
			// Read each probe set result.
			m_ProbeSetResults.resize(m_Header.m_NumProbeSets);
			for (int iset=0; iset<m_Header.m_NumProbeSets; iset++)
			{
				CExpressionProbeSetResults * pResults = new CExpressionProbeSetResults;
				m_ProbeSetResults[iset] = pResults;

				ReadInt32_I(instr, ival);
				pResults->NumPairs = ival;

				ReadInt32_I(instr, ival);
				pResults->NumUsedPairs = ival;

				if (m_Header.m_Version <= 12) // unused
					ReadInt32_I(instr, ival);

				ReadInt32_I(instr, ival); // unused
				if (m_Header.m_Version == 12)
				{
					ReadInt32_I(instr, ival); // unused
					ReadInt32_I(instr, ival); // unused
					ReadInt32_I(instr, ival); // unused
				}

				ReadFloat_I(instr, fval);
				pResults->DetectionPValue = fval;

				if (m_Header.m_Version == 12)
				{
					ReadFloat_I(instr, fval); // unused
				}

				ReadFloat_I(instr, fval);
				pResults->Signal = fval;

				ReadInt32_I(instr, ival);
				pResults->Detection = ival;

				// unused
				int ip;
				for (ip=0; ip<pResults->NumPairs; ++ip)
				{
					ReadFloat_I(instr, fval);
					ReadInt32_I(instr, ival);
					if (m_Header.m_Version == 12)
					{
						ReadInt32_I(instr, ival);
						ReadInt32_I(instr, ival);
						ReadFloat_I(instr, fval);
						ReadFloat_I(instr, fval);
						ReadInt32_I(instr, ival);
						ReadInt8(instr, cval);
						ReadInt8(instr, cval);
					}
					else
					{
						ReadUInt16_I(instr, usval);
						ReadUInt16_I(instr, usval);
					}
					if (m_Header.m_Version == 12)
					{
						ReadInt32_I(instr, ival);
						ReadInt32_I(instr, ival);
						ReadFloat_I(instr, fval);
						ReadFloat_I(instr, fval);
						ReadInt32_I(instr, ival);
						ReadInt8(instr, cval);
						ReadInt8(instr, cval);
					}
					else
					{
						ReadUInt16_I(instr, usval);
						ReadUInt16_I(instr, usval);
					}
				}

				ReadInt32_I(instr, ival);
				pResults->m_HasCompResults = (ival == 1 ? true : false);

				if (pResults->m_HasCompResults == true)
				{
					ReadInt32_I(instr, ival);
					pResults->NumCommonPairs = ival;

					if (m_Header.m_Version == 12)
					{
						ReadInt32_I(instr, ival); // unused
						ReadInt32_I(instr, ival); // unused
						ReadInt32_I(instr, ival); // unused
					}

					ReadInt32_I(instr, ival);
					pResults->Change = ival;

					ReadInt8(instr, cval); // unused
					if (m_Header.m_Version == 12)
					{
						ReadInt8(instr, cval); // unused
						ReadInt32_I(instr, ival); // unused
						ReadInt32_I(instr, ival); // unused
					}

					ReadInt32_I(instr, ival);
					pResults->SignalLogRatioHigh = (float) ival / ROUNDFLOAT;

					ReadInt32_I(instr, ival); // unused

					if (m_Header.m_Version == 12)
						ReadInt32_I(instr, ival); // unused

					ReadInt32_I(instr, ival);
					pResults->SignalLogRatio = (float) ival / ROUNDFLOAT;

					if (m_Header.m_Version == 12)
						ReadInt32_I(instr, ival); // unused

					ReadInt32_I(instr, ival);
					pResults->SignalLogRatioLow = (float) ival / ROUNDFLOAT;

					if (m_Header.m_Version == 12)
					{
						ReadInt32_I(instr, ival);
						pResults->ChangePValue = (float) ival / ROUNDFLOAT;
					}
					else
					{
						ReadFloat_I(instr, fval);
						pResults->ChangePValue = fval;
					}
				}
			}
		}
		else if (m_Header.m_AssayType == CCHPFileHeader::Genotyping)
		{
			m_ProbeSetResults.resize(m_Header.m_NumProbeSets);
			for (int iset=0; iset<m_Header.m_NumProbeSets; iset++)
			{
				CGenotypeProbeSetResults * pResults = new CGenotypeProbeSetResults;
				m_ProbeSetResults[iset] = pResults;

				// Unused data
				int ngroups=0;
				ReadInt32_I(instr, ival);
				ngroups = ival;
				int ig;
				for (ig=0; ig<ngroups; ig++)
				{
					ReadInt32_I(instr, ival);
					ReadCString_I(instr, sval);
					delete[] sval;
					ReadInt8(instr, cval);
					ReadInt32_I(instr, ival);
					ReadInt32_I(instr, ival);
					ReadInt32_I(instr, ival);
				}
				ReadInt8(instr, cval);
				if (cval == 1)
				{
					ReadInt32_I(instr, ival);
					ReadCString_I(instr, sval);
					delete[] sval;
					ReadCString_I(instr, sval);
					delete[] sval;
					ReadCString_I(instr, sval);
					delete[] sval;
					ReadInt32_I(instr, ival);
					ReadInt32_I(instr, ival);

					// The call
					ReadUInt8(instr, ucval);
					pResults->AlleleCall = ucval;

					// The confidence
					if (m_Header.m_Version == 12)
					{
						ReadInt32_I(instr, ival);
						pResults->Confidence = (float) ival / ROUNDFLOAT;
					}
					else
					{
						ReadFloat_I(instr, fval);
						pResults->Confidence = fval;
					}

					// unused
					ReadFloat_I(instr, fval);
					ReadFloat_I(instr, fval);
					ReadFloat_I(instr, fval);

					// RAS 1 and 2
					ReadFloat_I(instr, fval);
					pResults->RAS1 = fval;
					ReadFloat_I(instr, fval);
					pResults->RAS2 = fval;
				}
				else
				{
					pResults->Confidence = 0.0f;
					pResults->RAS1 = 0.0f;
					pResults->RAS2 = 0.0f;
					pResults->AlleleCall = ALLELE_NO_CALL;
				}


				// 100K results are not stored in this version.
				pResults->pvalue_AA = 0.0f;
				pResults->pvalue_AB = 0.0f;
				pResults->pvalue_BB = 0.0f;
				pResults->pvalue_NoCall = 0.0f;

				// unused
				ReadCString_I(instr, sval);
				delete[] sval;
				ReadCString_I(instr, sval);
				delete[] sval;
				ReadInt32_I(instr, ival);
				int np = ival;
				int ip;
				for (ip=0; ip<np; ++ip)
				{
					ReadInt32_I(instr, ival);
					if (m_Header.m_Version == 12)
					{
						ReadInt32_I(instr, ival);
						ReadInt32_I(instr, ival);
						ReadInt32_I(instr, ival);
						ReadInt32_I(instr, ival);
						ReadInt32_I(instr, ival);
						ReadInt8(instr, cval);
						ReadInt8(instr, cval);
					}
					else
					{
						ReadUInt16_I(instr, usval);
						ReadUInt16_I(instr, usval);
					}
					if (m_Header.m_Version == 12)
					{
						ReadInt32_I(instr, ival);
						ReadInt32_I(instr, ival);
						ReadInt32_I(instr, ival);
						ReadInt32_I(instr, ival);
						ReadInt32_I(instr, ival);
						ReadInt8(instr, cval);
						ReadInt8(instr, cval);
					}
					else
					{
						ReadUInt16_I(instr, usval);
						ReadUInt16_I(instr, usval);
					}
				}
			}
		}
		else if (m_Header.m_AssayType == CCHPFileHeader::Universal)
		{
			m_ProbeSetResults.resize(m_Header.m_NumProbeSets);
			int32_t unused_32;
			uint16_t unused_u16;
			int8_t unused_8;
			int32_t natoms;
			int32_t ibg;
			float bg;

			// Read each probe set result.
			for (int iset=0; iset<m_Header.m_NumProbeSets; iset++)
			{
				CUniversalProbeSetResults * pResults = new CUniversalProbeSetResults;
				m_ProbeSetResults[iset] = pResults;

				// unused (wildtype) length(int), string(len)
				int ibase;
				ReadInt32_I(instr, unused_32);
				for (ibase=0; ibase<unused_32; ibase++)
				{
					ReadInt8(instr, unused_8);
				}
				// unused (called) length(int), string(len)
				ReadInt32_I(instr, unused_32);
				for (ibase=0; ibase<unused_32; ibase++)
				{
					ReadInt8(instr, unused_8);
				}

				// unused (#atoms(int))
				ReadInt32_I(instr, natoms);
				for (int iatom=0; iatom<natoms; iatom++)
				{
					ReadInt32_I(instr, ibg);
					if (iatom == 0)
					{
						bg = (float) ibg / ROUNDFLOAT;
						pResults->SetBackground(bg);
					}

					// Ignore the probe level data.
					for (int icell=0; icell<tagCells[iset]; icell++)
					{
						if (m_Header.m_Version <= 12)
						{
							ReadInt32_I(instr, unused_32); // unused X coordinate
							ReadInt32_I(instr, unused_32); // unused Y coordinate
						}
						else
						{
							ReadUInt16_I(instr, unused_u16); // unused X coordinate
							ReadUInt16_I(instr, unused_u16); // unused Y coordinate
						}
						if (m_Header.m_Version <= 12)
						{
							ReadInt32_I(instr, unused_32); // unused intensity
							ReadInt32_I(instr, unused_32); // unused stdv
							ReadInt32_I(instr, unused_32); // unused pixel count
							if (m_Header.m_Version >= 8)
							{
								ReadInt8(instr, unused_8); // unused mask
								ReadInt8(instr, unused_8); // unused outlier
							}
						}
					}
				}
			}
		}
		else if (m_Header.m_AssayType == CCHPFileHeader::Resequencing)
		{
			int32_t dataSize=0;
			int32_t i32;
			int8_t i8;
			int16_t i16;
			float  score;
			int index;
			std::string str;

			// Read the base calls.
			m_ReseqResults.Clear();
			ReadInt32_I(instr, dataSize);
			m_ReseqResults.ResizeCalledBases(dataSize);
			m_ReseqResults.ResizeScores(dataSize);
			for (index=0; index<dataSize; index++)
			{
				ReadInt8(instr, i8);
				m_ReseqResults.SetCalledBase(index, (char)i8);
			}

			// Ignore the edit section - there were none with the non-XDA file.
			for (index=0; index<dataSize; index++)
			{
				ReadInt8(instr, i8);
			}

			// Ignore the confidence section - there were none with the non-XDA file.
			for (index=0; index<dataSize; index++)
			{
				ReadInt8(instr, i8);
			}

			// Ignore the unit index section - there were none with the non-XDA file.
			for (index=0; index<dataSize; index++)
			{
				ReadInt16_I(instr, i16);
			}

			ReadInt32_I(instr, i32);
			if (i32>0)
			{
				ReadFixedString(instr, str, i32);
				ReadInt32_I(instr, i32);
				if (i32>0)
					ReadFixedString(instr, str, i32);
			}

			for (index=0; index<dataSize; index++)
			{
				ReadFloat_I(instr, score);
				m_ReseqResults.SetScore(index, score);
			}
		}
		retVal = true;
		delete[] tagCells;
	}

	return retVal;		
}

//////////////////////////////////////////////////////////////////////

bool CCHPFileData::IsXDACompatibleFile()
{
	// Open the file.
	std::ifstream instr;
	Fs::aptOpen(instr, m_FileName.c_str(), std::ios::in | std::ios::binary);
	if (!instr)
		return false;

	// Read the magic number from the file.
	int32_t magic=0;
	ReadInt32_I(instr, magic);
	bool bXDAFile = (magic == CHP_FILE_MAGIC_NUMBER);
	instr.close();

	return bXDAFile;
}

//////////////////////////////////////////////////////////////////////

bool CCHPFileData::IsMas5File()
{
	// Open the file.
	std::ifstream instr;
	Fs::aptOpen(instr, m_FileName.c_str(), std::ios::in | std::ios::binary);
	if (!instr) return false;

	// Read the string that defines the CHP file (older format).
	char vString[256]="";
	char AppName[256]="GeneChip Sequence File";
	ReadFixedCString(instr, vString, (int) strlen(AppName));
	if (strcmp(vString, AppName))
	{
		return false;
	}

	// Read the version number
	int32_t ival;
	ReadInt32_I(instr, ival);
	if(ival < 12)
	{
		return false;
	}
	instr.close();

	return true;
}

