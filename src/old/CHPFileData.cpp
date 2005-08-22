/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2004 Affymetrix, Inc.
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

#include "CHPFileData.h"
#include "FileIO.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <istream>
#include <fstream>

#ifdef _INCLUDE_UNISTD_HEADER_
#include <unistd.h>
#endif

using namespace affxchp;

//////////////////////////////////////////////////////////////////////

#define DELIMCHAR 0x14
#define MIN_CELLSTR 4
#define CHP_FILE_MAGIC_NUMBER 65
#define CHP_FILE_VERSION_NUMBER 2
#define EXPRESSION_ABSOLUTE_STAT_ANALYSIS 2
#define EXPRESSION_COMPARISON_STAT_ANALYSIS 3

#ifdef WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

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
	BackgroundZoneType zone;
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
	m_FullPathName = "";
	m_DataPath = "";
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
	if (index < m_Header.GetNumProbeSets() || m_Header.GetAssayType() == CCHPFileHeader::Expression)
	{
		return (CExpressionProbeSetResults *) m_ProbeSetResults[index];
	}
	return NULL;
}
	
//////////////////////////////////////////////////////////////////////

CGenotypeProbeSetResults *CCHPFileData::GetGenotypingResults(int index)
{
	if (index < m_Header.GetNumProbeSets() || m_Header.GetAssayType() == CCHPFileHeader::Genotyping)
	{
		return (CGenotypeProbeSetResults *) m_ProbeSetResults[index];
	}
	return NULL;
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
		return "AB";
		break;

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
	// Format the full path name.
	m_FullPathName = m_DataPath;
	m_FullPathName += PATH_SEPARATOR;
	m_FullPathName += m_FileName;

	// Find the file stats.
	struct stat st;
	return (stat(m_FullPathName.c_str(), &st) == 0);
}

//////////////////////////////////////////////////////////////////////

bool CCHPFileData::Open(bool bReadHeaderOnly)
{
	bool retVal = false;

	// Format the full path name.
	m_FullPathName = m_DataPath;
	m_FullPathName += PATH_SEPARATOR;
	m_FullPathName += m_FileName;


	// Open the file.
	std::ifstream instr;
	bool bXDA = false;
	if (IsXDACompatibleFile())
		bXDA = true;
	instr.open(m_FullPathName.c_str(), std::ios::in | std::ios::binary);

	// Check if open
	if (!instr)
	{
		m_strError = "Unable to open the file.";
		return false;
	}


	// Read the header
	if (bXDA)
	{
		int magic;
		int version;
		int ival;
		unsigned short usval;
		char *sval=NULL;
		float fval;
		unsigned char ucval;


		// Read the magic number.
		READ_INT(instr, magic);
		m_Header.m_Magic = magic;

		// Check if new type.
		if (magic != CHP_FILE_MAGIC_NUMBER)
		{
			m_strError = "The file does not appear to be the correct format.";
			return false;
		}

		// Read the version
		READ_INT(instr, version);
		m_Header.m_Version = version;

		// Check for version 1 or 2
		if (version > CHP_FILE_VERSION_NUMBER)
		{
			m_strError = "Unable to read this version of the CHP file.";
			return false;
		}

		// Get the dimensions of the array
		READ_USHORT(instr, usval);
		m_Header.m_Cols=usval;
		READ_USHORT(instr, usval);
		m_Header.m_Rows=usval;

		// Number of probe sets.
		READ_INT(instr, ival);
		m_Header.m_NumProbeSets = ival;
		READ_INT(instr, ival); // no qc data extracted.

		// Assay type
		READ_INT(instr, ival);
		m_Header.m_AssayType = (CCHPFileHeader::GeneChipAssayType)ival;

		// Prog ID.
		READ_STRING(instr, sval);
		m_Header.m_ProgID = sval;
		delete[] sval;

		// Parent cell file.
		READ_STRING(instr, sval);
		m_Header.m_ParentCellFile = sval;
		delete[] sval;

		// Chip type
		READ_STRING(instr, sval);
		m_Header.m_ChipType = sval;
		delete[] sval;

		// Algorithm
		READ_STRING(instr, sval);
		m_Header.m_AlgorithmName = sval;
		delete[] sval;

		// Algorithm version
		READ_STRING(instr, sval);
		m_Header.m_AlgorithmVersion = sval;
		delete[] sval;

		// Algorithm parameters.
		int nParams=0;
		int iParam;
		READ_INT(instr, nParams);
		TagValuePairType param;
		for (iParam=0; iParam<nParams; iParam++)
		{
			READ_STRING(instr, sval);
			param.Tag = sval;
			delete[] sval;

			READ_STRING(instr, sval);
			param.Value = sval;
			delete[] sval;

			m_Header.m_AlgorithmParameters.push_back(param);
		}

		// Summary parameters
		READ_INT(instr, nParams);
		for (iParam=0; iParam<nParams; iParam++)
		{
			READ_STRING(instr, sval);
			param.Tag = sval;
			delete[] sval;

			READ_STRING(instr, sval);
			param.Value = sval;
			delete[] sval;

			m_Header.m_SummaryParameters.push_back(param);
		}


		// Background info		
		READ_INT(instr, nParams);
		m_Header.m_BackgroundZoneInfo.number_zones = nParams;
		READ_FLOAT(instr, fval);
		m_Header.m_BackgroundZoneInfo.smooth_factor = fval;

		// Background zones
		BackgroundZoneType zone;
		for (iParam=0; iParam<nParams; iParam++)
		{
			READ_FLOAT(instr, fval);
			zone.centerx = fval;
			READ_FLOAT(instr, fval);
			zone.centery = fval;
			READ_FLOAT(instr, fval);
			zone.background = fval;

			m_Header.m_BackgroundZoneInfo.zones.push_back(zone);
		}

		// Return if only reading the header.
		if (bReadHeaderOnly == true)
			return true;


		// Only continue if genotyping or expression
		if (m_Header.m_AssayType != CCHPFileHeader::Expression && m_Header.m_AssayType != CCHPFileHeader::Genotyping)
		{
			m_strError = "The software only supports reading expression or genotyping CHP files.";
			return false;
		}

		// Read the probe set data
		m_ProbeSetResults.resize(m_Header.m_NumProbeSets);
		if (m_Header.m_AssayType == CCHPFileHeader::Expression)
		{
			// Get the type of analysis
			READ_UCHAR(instr, ucval); // EXPRESSION_ABSOLUTE_STAT_ANALYSIS or EXPRESSION_COMPARISON_STAT_ANALYSIS
			READ_INT(instr, ival);

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
				READ_UCHAR(instr, ucval);
				pResults->Detection = ucval;

				READ_FLOAT(instr, fval);
				pResults->DetectionPValue = fval;
				
				READ_FLOAT(instr, fval);
				pResults->Signal = fval;
				
				READ_USHORT(instr, usval);
				pResults->NumPairs = usval;
				
				READ_USHORT(instr, usval);
				pResults->NumUsedPairs = usval;

				pResults->m_HasCompResults = false;


				// Read the comparison data
				if (analysisType == EXPRESSION_COMPARISON_STAT_ANALYSIS)
				{
					pResults->m_HasCompResults = true;

					READ_UCHAR(instr, ucval);
					pResults->Change = ucval;

					READ_FLOAT(instr, fval);
					pResults->ChangePValue = fval;

					READ_FLOAT(instr, fval);
					pResults->SignalLogRatio = fval;

					READ_FLOAT(instr, fval);
					pResults->SignalLogRatioLow = fval;

					READ_FLOAT(instr, fval);
					pResults->SignalLogRatioHigh = fval;

					READ_USHORT(instr, usval);
					pResults->NumCommonPairs = usval;
				}
			}
		}
		else if (m_Header.m_AssayType == CCHPFileHeader::Genotyping)
		{
			const int DM_ALG_RESULT_SIZE = 21;
			int dataSize=0;
			READ_INT(instr, dataSize);
			for (int iset=0; iset<m_Header.m_NumProbeSets; iset++)
			{
				CGenotypeProbeSetResults * pResults = new CGenotypeProbeSetResults;
				m_ProbeSetResults[iset] = pResults;

				// Read probe set result.
				READ_UCHAR(instr, ucval);
				pResults->AlleleCall = ucval;

				READ_FLOAT(instr, fval);
				pResults->Confidence = fval;

				READ_FLOAT(instr, fval);
				pResults->RAS1 = fval;
				pResults->pvalue_AA = fval;

				READ_FLOAT(instr, fval);
				pResults->RAS2 = fval;
				pResults->pvalue_AB = fval;

				if (dataSize == DM_ALG_RESULT_SIZE)
				{
					READ_FLOAT(instr, fval);
					pResults->pvalue_BB = fval;

					READ_FLOAT(instr, fval);
					pResults->pvalue_NoCall = fval;
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
		READ_FIXED_STRING(instr, vString, (int) strlen(AppName));
		if (strcmp(vString, AppName))
		{
			m_strError = "The file does not appear to be the correct format.";
			return false;
		}

		// Read the version number
		char cval;
		unsigned char ucval;
		unsigned short usval;
		int ival;
		float fval;
		char *sval;
		READ_INT(instr, ival);
		m_Header.m_Version = ival;
		if (ival < 12)
		{
			m_strError = "This version of the CHP file is not supported by the parser.";
			return false;
		}

	    // Read algorithm type string.
		READ_STRING(instr, sval);
		m_Header.m_AlgorithmName=sval;
		delete[] sval;

		READ_STRING(instr, sval);
		m_Header.m_AlgorithmVersion=sval;
		delete[] sval;

		// Read parameters.
		READ_STRING(instr, sval);
		std::string sSource = sval;
		m_Header.ParseString(m_Header.m_AlgorithmParameters, sSource, " ", "=");
		delete[] sval;

		// Read summary
		READ_STRING(instr, sval);
		std::string sSummary = sval;
		m_Header.ParseString(m_Header.m_SummaryParameters, sSummary, " ", "=");
		delete[] sval;

		READ_INT(instr, ival);
		m_Header.m_Rows = ival;

		READ_INT(instr, ival);
		m_Header.m_Cols = ival;

		READ_INT(instr, ival);
		m_Header.m_NumProbeSets = ival;

		// unused data.
		READ_INT(instr, ival);
		int maxvalue = ival;
		READ_INT(instr, ival);
		int j;
		for (j=0; j<m_Header.m_NumProbeSets; j++)
			READ_INT(instr, ival);
		for (j=0; j<maxvalue; j++)
			READ_INT(instr, ival);
		for (j=0; j<maxvalue; j++)
		{
			READ_INT(instr, ival);
			if (j == 0)
			{
				if (ival == 3)
					m_Header.m_AssayType = CCHPFileHeader::Expression;

				else if (ival == 2)
					m_Header.m_AssayType = CCHPFileHeader::Genotyping;

				else
					m_Header.m_AssayType = CCHPFileHeader::Unknown;
			}
		}
		for (j=0; j<m_Header.m_NumProbeSets; j++)
			READ_INT(instr, ival);

		// Get the chip type
		char ctype[256];
		READ_FIXED_STRING(instr, ctype, 256);
		m_Header.m_ChipType = ctype;

		// The parent CEL file.
		READ_FIXED_STRING(instr, ctype, 256);
		m_Header.m_ParentCellFile = ctype;

		// The prog ID.
		READ_STRING(instr, sval);
		m_Header.m_ProgID = sval;
		delete [] sval;


		// Return if only reading the header.
		if (bReadHeaderOnly == true)
			return true;


		// Only continue if genotyping or expression
		if (m_Header.m_AssayType != CCHPFileHeader::Expression && m_Header.m_AssayType != CCHPFileHeader::Genotyping)
		{
			m_strError = "The software only supports reading expression or genotyping CHP files.";
			return false;
		}

		// Read the probe set data
		m_ProbeSetResults.resize(m_Header.m_NumProbeSets);
		if (m_Header.m_AssayType == CCHPFileHeader::Expression)
		{
			// Read each probe set result.
			for (int iset=0; iset<m_Header.m_NumProbeSets; iset++)
			{
				CExpressionProbeSetResults * pResults = new CExpressionProbeSetResults;
				m_ProbeSetResults[iset] = pResults;

				READ_INT(instr, ival);
				pResults->NumPairs = ival;

				READ_INT(instr, ival);
				pResults->NumUsedPairs = ival;

				if (m_Header.m_Version <= 12) // unused
					READ_INT(instr, ival);

				READ_INT(instr, ival); // unused
				if (m_Header.m_Version == 12)
				{
					READ_INT(instr, ival); // unused
					READ_INT(instr, ival); // unused
					READ_INT(instr, ival); // unused
				}

				READ_FLOAT(instr, fval);
				pResults->DetectionPValue = fval;

				if (m_Header.m_Version == 12)
				{
					READ_FLOAT(instr, fval); // unused
				}

				READ_FLOAT(instr, fval);
				pResults->Signal = fval;

				READ_INT(instr, ival);
				pResults->Detection = ival;

				// unused
				int ip;
				for (ip=0; ip<pResults->NumPairs; ++ip)
				{
					READ_FLOAT(instr, fval);
					READ_INT(instr, ival);
					if (m_Header.m_Version == 12)
					{
						READ_INT(instr, ival);
						READ_INT(instr, ival);
						READ_FLOAT(instr, fval);
						READ_FLOAT(instr, fval);
						READ_INT(instr, ival);
						READ_CHAR(instr, cval);
						READ_CHAR(instr, cval);
					}
					else
					{
						READ_USHORT(instr, usval);
						READ_USHORT(instr, usval);
					}
					if (m_Header.m_Version == 12)
					{
						READ_INT(instr, ival);
						READ_INT(instr, ival);
						READ_FLOAT(instr, fval);
						READ_FLOAT(instr, fval);
						READ_INT(instr, ival);
						READ_CHAR(instr, cval);
						READ_CHAR(instr, cval);
					}
					else
					{
						READ_USHORT(instr, usval);
						READ_USHORT(instr, usval);
					}
				}

				READ_INT(instr, ival);
				pResults->m_HasCompResults = (ival == 1 ? true : false);

				if (pResults->m_HasCompResults == true)
				{
					READ_INT(instr, ival);
					pResults->NumCommonPairs = ival;

					if (m_Header.m_Version == 12)
					{
						READ_INT(instr, ival); // unused
						READ_INT(instr, ival); // unused
						READ_INT(instr, ival); // unused
					}

					READ_INT(instr, ival);
					pResults->Change = ival;

					READ_CHAR(instr, cval); // unused
					if (m_Header.m_Version == 12)
					{
						READ_CHAR(instr, cval); // unused
						READ_INT(instr, ival); // unused
						READ_INT(instr, ival); // unused
					}

					READ_INT(instr, ival);
					pResults->SignalLogRatioHigh = (float) ival / 1000;

					READ_INT(instr, ival); // unused

					if (m_Header.m_Version == 12)
						READ_INT(instr, ival); // unused

					READ_INT(instr, ival);
					pResults->SignalLogRatio = (float) ival / 1000;

					if (m_Header.m_Version == 12)
						READ_INT(instr, ival); // unused

					READ_INT(instr, ival);
					pResults->SignalLogRatioLow = (float) ival / 1000;

					if (m_Header.m_Version == 12)
					{
						READ_INT(instr, ival);
						pResults->ChangePValue = (float) ival / 1000;
					}
					else
					{
						READ_FLOAT(instr, fval);
						pResults->ChangePValue = fval;
					}
				}
			}
		}
		else if (m_Header.m_AssayType == CCHPFileHeader::Genotyping)
		{
			for (int iset=0; iset<m_Header.m_NumProbeSets; iset++)
			{
				CGenotypeProbeSetResults * pResults = new CGenotypeProbeSetResults;
				m_ProbeSetResults[iset] = pResults;

				// Unused data
				int ngroups=0;
				READ_INT(instr, ival);
				ngroups = ival;
				int ig;
				for (ig=0; ig<ngroups; ig++)
				{
					READ_INT(instr, ival);
					READ_STRING(instr, sval);
					delete[] sval;
					READ_CHAR(instr, cval);
					READ_INT(instr, ival);
					READ_INT(instr, ival);
					READ_INT(instr, ival);
				}
				READ_CHAR(instr, cval);
				if (cval == 1)
				{
					READ_INT(instr, ival);
					READ_STRING(instr, sval);
					delete[] sval;
					READ_STRING(instr, sval);
					delete[] sval;
					READ_STRING(instr, sval);
					delete[] sval;
					READ_INT(instr, ival);
					READ_INT(instr, ival);

					// The call
					READ_UCHAR(instr, ucval);
					pResults->AlleleCall = ucval;

					// The confidence
					if (m_Header.m_Version == 12)
					{
						READ_INT(instr, ival);
						pResults->Confidence = (float) ival / 1000;
					}
					else
					{
						READ_FLOAT(instr, fval);
						pResults->Confidence = fval;
					}

					// unused
					READ_FLOAT(instr, fval);
					READ_FLOAT(instr, fval);
					READ_FLOAT(instr, fval);

					// RAS 1 and 2
					READ_FLOAT(instr, fval);
					pResults->RAS1 = fval;
					READ_FLOAT(instr, fval);
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
				READ_STRING(instr, sval);
				delete[] sval;
				READ_STRING(instr, sval);
				delete[] sval;
				READ_INT(instr, ival);
				int np = ival;
				int ip;
				for (ip=0; ip<np; ++ip)
				{
					READ_INT(instr, ival);
					if (m_Header.m_Version == 12)
					{
						READ_INT(instr, ival);
						READ_INT(instr, ival);
						READ_INT(instr, ival);
						READ_INT(instr, ival);
						READ_INT(instr, ival);
						READ_CHAR(instr, cval);
						READ_CHAR(instr, cval);
					}
					else
					{
						READ_USHORT(instr, usval);
						READ_USHORT(instr, usval);
					}
					if (m_Header.m_Version == 12)
					{
						READ_INT(instr, ival);
						READ_INT(instr, ival);
						READ_INT(instr, ival);
						READ_INT(instr, ival);
						READ_INT(instr, ival);
						READ_CHAR(instr, cval);
						READ_CHAR(instr, cval);
					}
					else
					{
						READ_USHORT(instr, usval);
						READ_USHORT(instr, usval);
					}
				}
			}
		}
		retVal = true;
	}

	return retVal;		
}

//////////////////////////////////////////////////////////////////////

bool CCHPFileData::IsXDACompatibleFile()
{
	// Format the full path name.
	m_FullPathName = m_DataPath;
	m_FullPathName += PATH_SEPARATOR;
	m_FullPathName += m_FileName;

	// Open the file.
	std::ifstream instr(m_FullPathName.c_str(), std::ios::in | std::ios::binary);
	if (!instr)
		return false;

	// Read the magic number from the file.
	int magic=0;
	READ_INT(instr, magic);
	bool bXDAFile = (magic == CHP_FILE_MAGIC_NUMBER);
	instr.close();

	return bXDAFile;
}

//////////////////////////////////////////////////////////////////////

void CCHPFileData::SetFileName(const char *name)
{
	std::string file = name;
	int index = (int) file.rfind(PATH_SEPARATOR);
	if (index != -1)
	{
		m_FileName = file.c_str() + index + 1;
		m_DataPath = file;
		m_DataPath.resize(index);
	}
	else
		m_FileName = file;
}

//////////////////////////////////////////////////////////////////////
