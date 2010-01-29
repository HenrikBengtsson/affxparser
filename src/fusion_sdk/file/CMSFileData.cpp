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

#include "file/CMSFileData.h"
//
#include "file/IniFile.h"
//
#include <cassert>
#include <cstring>
#include <ctime>
#include <fstream>
#include <istream>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
//

using namespace std;
using namespace affxcms;

//////////////////////////////////////////////////////////////////////

CCMSFileHeader::CCMSFileHeader() : m_nVersion(CMS_FILE_VERSION)
{
}

/////////////////////////////////////////////////////////////////

CCMSFileData::CCMSFileData()
{
}

/////////////////////////////////////////////////////////////////

CCMSFileData::~CCMSFileData()
{
}

/////////////////////////////////////////////////////////////////

bool CCMSFileData::Read()
{
	// Check if file exists
	if (Exists() == false)
		return false;

	// Open the file
	if (Open() == false)
	{
		Close();
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////

bool CCMSFileData::ReadHeader()
{
	// Check if file exists
	if (Exists() == false)
		return false;

	// Open the file
	if (Open(true) == false)
	{
		Close();
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////

bool CCMSFileData::Open(bool bReadHeaderOnly)
{
	// First close the file.
	Close();

	return ReadFile(bReadHeaderOnly);
}

/////////////////////////////////////////////////////////////////

void CCMSFileData::Close()
{
	m_arrayList.clear();
	m_snpList.clear();
}

/////////////////////////////////////////////////////////////////

bool CCMSFileData::ReadFile(bool bReadHeaderOnly)
{
	try
	{
		ostringstream strstrm;
		int iCT=0;
		string strSection, strKey, strValue, strUnit, strChipType;
		int nValue=0;
		int nUnit=0;
		CIniFile iniFile;

		// Get file version
		strValue = iniFile.GetValue(CMS_HEADER_KEY_VERSION, CMS_SECTION_HEADER, m_strFileName);
		nValue = atoi(strValue.c_str());
		m_Header.SetVersion(nValue);

		// Get assay
		strValue = iniFile.GetValue(CMS_HEADER_KEY_ASSAY, CMS_SECTION_HEADER, m_strFileName);
		m_Header.SetAssay(strValue.c_str());

		// Get array type count
		strValue = iniFile.GetValue(CMS_HEADER_KEY_ARRAYCOUNT, CMS_SECTION_HEADER, m_strFileName);
		nValue = atoi(strValue.c_str());
		m_Header.SetArrayCount(nValue);

		// Get common SNP count
		strValue = iniFile.GetValue(CMS_HEADER_KEY_SNPCOUNT, CMS_SECTION_HEADER, m_strFileName);
		nValue = atoi(strValue.c_str());
		m_Header.SetSNPCount(nValue);

		// Stop if just reading the header.
		if (bReadHeaderOnly)
			return true;

		// Read data section list of array types
		strValue = iniFile.GetValue(CMS_DATA_TYPES, CMS_SECTION_DATA, m_strFileName);

		// Parse array types
		string::size_type lastPos = strValue.find_first_not_of(CMS_DATA_DEL, 0);
		string::size_type pos     = strValue.find_first_of(CMS_DATA_DEL, lastPos);
		while (std::string::npos != pos || std::string::npos != lastPos)
		{		
			// Get and store all parsed tokens
			strChipType = strValue.substr(lastPos, pos - lastPos);
			m_arrayList.insert(map<int,string>::value_type(iCT++, strChipType));

			// Skip delimiters and find next "non-delimiter" (1st delimiter)
			lastPos = strValue.find_first_not_of(CMS_DATA_DEL, pos);
			pos = strValue.find_first_of(CMS_DATA_DEL, lastPos);
		}

		// Loop to get common SNPs		
		for (int iSNP=0; iSNP<m_Header.GetSNPCount(); ++iSNP)
		{		
			SNPTypeInfo snp;

			// Read next SNP line
			strstrm.str("");
			strstrm << CMS_DATA_SNP << iSNP+1;
			strValue = iniFile.GetValue(strstrm.str().c_str(), CMS_SECTION_DATA, m_strFileName);

			// Store SNP ID
			snp.snpID = strstrm.str();

			// Parse SNP IDs
			iCT=0;
			std::string::size_type lastPos = strValue.find_first_not_of(CMS_DATA_DEL, 0);
			std::string::size_type pos     = strValue.find_first_of(CMS_DATA_DEL, lastPos);
			while (std::string::npos != pos || std::string::npos != lastPos)
			{		
				// Get nex snpID
				strUnit = strValue.substr(lastPos, pos - lastPos);
				nUnit = atoi(strUnit.c_str());

				// Store chiptype and snpID into map
				if (iCT < (int)m_arrayList.size())
				{
					strChipType = m_arrayList[iCT++];
					snp.chiptype_to_unit.insert(map<string,int>::value_type(strChipType, nUnit));
				}

				// Skip delimiters and find next "non-delimiter" (1st delimiter)
				lastPos = strValue.find_first_not_of(CMS_DATA_DEL, pos);
				pos = strValue.find_first_of(CMS_DATA_DEL, lastPos);
			}

			// Insert common SNP structure
			m_snpList.push_back(snp);
		}
	}
	catch(...)
	{
		m_strError = "Unspecified read error.";
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////

bool CCMSFileData::Exists()
{
	// Find the file stats.
	struct stat st;
	if (stat(m_strFileName.c_str(), &st) == 0)
	{
		return true;
	}
	else
	{
		m_strError = "CMS file " + m_strFileName + " doesn't exist";
		return false;
	}
}
