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

//
#include "file/EXPFileData.h"
//
#include "file/FileIO.h"
//
#include <cstring>
#include <fstream>
#include <istream>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
//

#ifdef _MSC_VER
#pragma warning(disable: 4996) // don't show deprecated warnings.
#endif

using namespace affxexp;
using namespace std;

/*! The first line of an EXP file */
#define EXP_HEADER_LINE_1 "Affymetrix GeneChip Experiment Information"

/*! The second line of an EXP file */
#define EXP_HEADER_LINE_2 "Version"

/*! The sample information section name. */
#define SAMPLE_SECTION_NAME "[Sample Info]"

/*! The scanner section name. */
#define SCANNER_SECTION_NAME "[Scanner]"

/*! The fluidics section name. */
#define FLUIDICS_SECTION_NAME "[Fluidics]"

/*! The array type tag name. */
#define ARRAY_TYPE_TAG "Chip Type"

/*! The protocol tag name. */
#define PROTOCOL_TAG "Protocol"

/*! The station tag name. */
#define STATION_TAG "Station"



/* 
 * Trim white space from left and right
 */
void TrimWhiteSpaces(std::string& s)
{
	string::size_type n;

	n = s.find_first_not_of( ' ' );
	if( n != std::string::npos )
		s = s.substr( n );

	n = s.find_last_not_of( ' ' );
	if( n == std::string::npos )
		s.clear();
	else
		s = s.substr( 0, n+1 );
}


/*
 * Initialize the class to null values
 */
CEXPFileData::CEXPFileData()
{
}

/*
 * Clean up any used memory
 */
CEXPFileData::~CEXPFileData()
{
	Clear();
}

/*
 * Deallocate any memory for all the class members and initialize them back to
 * null or zero values.
 */
void CEXPFileData::Clear()
{
	m_strArrayType = "";
	m_Scan.clear();
	m_Hyb.clear();
	m_Sample.clear();
}

/*
 * Check for the file existance
 */
bool CEXPFileData::Exists()
{
	// Find the file stats.
	struct stat st;
	return (stat(m_strFileName.c_str(), &st) == 0);
}

/*
 * Read the file contents. The first two lines are the header followed by
 * [Sample Info], [Fluidics] and [Scanner] sections.
 */
bool CEXPFileData::Read()
{
	// Clear any existing data.
	Clear();

	// Open the file.
	ifstream instream(m_strFileName.c_str(), ios::in);
	if (!instream)
	{
		Clear();
		return false;
	}

	// The first two lines are the header.
	const int LINELENGTH = 1024;
	char str[LINELENGTH];
    ReadNextLine(instream, str, LINELENGTH);
	if (strncmp(str, EXP_HEADER_LINE_1, strlen(EXP_HEADER_LINE_1)) != 0)
	{
		Clear();
		return false;
	}
	ReadNextLine(instream, str, LINELENGTH);
	if (strncmp(str, EXP_HEADER_LINE_2, strlen(EXP_HEADER_LINE_2)) != 0)
	{
		Clear();
		return false;
	}

	// The possible sections.
	typedef enum { NO_SECTION, SAMPLE_SECTION, FLUIDICS_SECTION, SCANNER_SECTION } CurrentSectionType;
	CurrentSectionType currentSection = NO_SECTION;

	// The remaining are the sample, fluidics and scanner sections
	bool captureAll = false;
	while (!instream.eof())
	{
        ReadNextLine(instream, str, LINELENGTH);

		// Skip blank lines.
		if (strlen(str) == 0)
			continue;

		// Check for the start of each section.
		if (strncmp(str, SAMPLE_SECTION_NAME, strlen(SAMPLE_SECTION_NAME)) == 0)
		{
			currentSection = SAMPLE_SECTION;
			continue;
		}

		else if (strncmp(str, FLUIDICS_SECTION_NAME, strlen(FLUIDICS_SECTION_NAME)) == 0)
		{
			currentSection = FLUIDICS_SECTION;
			continue;
		}

		else if (strncmp(str, SCANNER_SECTION_NAME, strlen(SCANNER_SECTION_NAME)) == 0)
		{
			currentSection = SCANNER_SECTION;
			continue;
		}

		// Parse the line into a name/value pair.
		TagValuePairType param;
		char *paramStr = strtok(str, "\t");
		param.Tag = paramStr;
		paramStr = strtok(NULL, "\t");
		if (paramStr != NULL)
			param.Value = paramStr;
		else
			param.Value = "";
		TrimWhiteSpaces(param.Value);


		// Take everything between the "Protocol" and "Station" tags. These
		// may include error messages in the tag name with blank values.
		if (currentSection == FLUIDICS_SECTION && param.Tag == PROTOCOL_TAG)
			captureAll = true;
		else if (currentSection == FLUIDICS_SECTION && param.Tag == STATION_TAG)
			captureAll = false;

		// If the value is blank then skip it.
		if (param.Value == "" && captureAll == false)
			continue;

		// Double check if the "Protocol" tag is blank and skip it.
		if (param.Tag == PROTOCOL_TAG && param.Value == "")
			continue;

		// Check for the special array type line
		if (currentSection == SAMPLE_SECTION && param.Tag == ARRAY_TYPE_TAG)
		{
			m_strArrayType = param.Value;
		}

		// Store the parameter into the sample section.
		else if (currentSection == SAMPLE_SECTION)
		{
			m_Sample.push_back(param);
		}

		// Store the parameter into the fluidics section.
		else if (currentSection == FLUIDICS_SECTION)
		{
			m_Hyb.push_back(param);
		}

		// Store the parameter into the scanner section.
		else if (currentSection == SCANNER_SECTION)
		{
			m_Scan.push_back(param);
		}
	}

	// Close the file.
	instream.close();

	return true;
}

//////////////////////////////////////////////////////////////////////
