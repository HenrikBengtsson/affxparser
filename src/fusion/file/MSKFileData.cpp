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
#include "file/MSKFileData.h"
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

#ifdef _INCLUDE_UNISTD_HEADER_
#include <unistd.h>
#endif

//////////////////////////////////////////////////////////////////////

using namespace affxmsk;
using namespace std;

//////////////////////////////////////////////////////////////////////

/*! The call section in the mask file contains probes to ignore from ana analysis */
#define CALL_SECTION "[Call]"

/*! The comp section in the mask file contains probe sets to use for scaling or normalization */
#define COMP_SECTION "[Comp]"

//////////////////////////////////////////////////////////////////////

/*
 * Initialize the class to null values
 */
CMSKFileData::CMSKFileData()
{
}

//////////////////////////////////////////////////////////////////////

/*
 * Clean up any used memory
 */
CMSKFileData::~CMSKFileData()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////

/*
 * Deallocate any memory for all the class members and initialize them back to
 * null or zero values.
 */
void CMSKFileData::Clear()
{
	m_strArrayType = "";
	m_Indicies.clear();
	m_Sets.clear();
}

//////////////////////////////////////////////////////////////////////

/*
 * Check for the file existance
 */
bool CMSKFileData::Exists()
{
	// Find the file stats.
	struct stat st;
	return (stat(m_strFileName.c_str(), &st) == 0);
}

//////////////////////////////////////////////////////////////////////

/*
 * Read the file contents.
 */
bool CMSKFileData::Read()
{
	// Clear any existing data.
	Clear();

	// Open the mask file.
	ifstream instream(m_strFileName.c_str(), ios::in);
	if (!instream)
	{
		Clear();
		m_strError = "Unable to open the mask file.";
		return false;
	}

	// The first line is the array type
	const int LINELENGTH = 1024;
	char str[LINELENGTH];
	instream.getline(str, LINELENGTH);
	m_strArrayType = str;


	// Search for the call or comp sections.
	bool bCallSectionFound = false;
	bool bCompSectionFound = false;
	while (instream.getline(str, LINELENGTH))
	{
		// Skip blank lines.
		if (strlen(str) == 0)
			continue;

		// If the call section is found then continue with the while loop.
		// Set the flags to start processing lines as from the call section
		// and stop processing lines as from the comp section.
		if (strncmp(str, CALL_SECTION, strlen(CALL_SECTION)) == 0)
		{
			bCallSectionFound = true;
			bCompSectionFound = false;
			continue;
		}

		// If the comp section is found then continue with the while loop.
		// Set the flags to start processing lines as from the comp section
		// and stop processing lines as from the call section.
		if (strncmp(str, COMP_SECTION, strlen(COMP_SECTION)) == 0)
		{
			bCallSectionFound = false;
			bCompSectionFound = true;
			continue;
		}

		// Parse the call section line.
		if (bCallSectionFound)
		{
			// Get the probe set name.
			char *token = strtok(str, "\t");
			ProbeSetIndiciesType indicies;
			indicies.probeSetName = token;

			// Get the mask values.
			char *maskValues = strtok(NULL, "\t");

			// Loop over all the mask pairs (start-stop).
			char *maskEntry;
			while ((maskEntry = strtok(maskValues, ",")) != NULL)
			{
				// Extract the first and last index from the token.
				int first=0;
				int last=0;
				if (sscanf(maskEntry, "%d-%d", &first, &last) == 1)
					last = first;
				maskValues = NULL;

				// The indicies are one based in the mask file. Convert them to zero based
				// and store them in the list.
				for (int i=first-1; i<last; i++)
				{
					indicies.indicies.push_back(i);
				}
			}
			m_Indicies.push_back(indicies);
		}

		else if (bCompSectionFound)
		{
			m_Sets.push_back(strtok(str, "\t"));
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

void CMSKFileData::GetProbeSetIndiciesIterators(ProbeSetIndiciesListConstIt &begin, ProbeSetIndiciesListConstIt &end)
{
	begin = m_Indicies.begin();
	end = m_Indicies.end();
}

//////////////////////////////////////////////////////////////////////

void CMSKFileData::GetProbeSetIterators(ProbeSetListConstIt &begin, ProbeSetListConstIt &end)
{
	begin = m_Sets.begin();
	end = m_Sets.end();
}

//////////////////////////////////////////////////////////////////////
