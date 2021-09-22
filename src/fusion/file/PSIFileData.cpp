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
#include "file/PSIFileData.h"
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

using namespace affxpsi;
using namespace std;

//////////////////////////////////////////////////////////////////////

/*
 * Initialize the class to null values
 */
CPSIFileData::CPSIFileData()
{
}

//////////////////////////////////////////////////////////////////////

/*
 * Clean up any used memory
 */
CPSIFileData::~CPSIFileData()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////

/*
 * Deallocate any memory for all the class members.
 */
void CPSIFileData::Clear()
{
	m_ProbeSets.clear();
}

//////////////////////////////////////////////////////////////////////

/*
 * Check for the file existance
 */
bool CPSIFileData::Exists()
{
	// Find the file stats.
	struct stat st;
	return (stat(m_strFileName.c_str(), &st) == 0);
}

//////////////////////////////////////////////////////////////////////

/*
 * Read the file contents.
 */
bool CPSIFileData::Read()
{
	// Clear any existing data.
	Clear();

	// Open the mask file.
	ifstream instream(m_strFileName.c_str(), ios::in);
	if (!instream)
	{
		Clear();
		return false;
	}

	// The first line is the number of probe sets
	int nSets=0;
	const int LINELENGTH = 128;
	char str[LINELENGTH];
	instream.getline(str, LINELENGTH);
	if (sscanf(str, "#Probe Sets: %d", &nSets) == 0)
	{
		Clear();
		return false;
	}

	// The remaining lines contain the probe set names.
	m_ProbeSets.resize(nSets);
	int num;
	int iSet=0;
	char name[LINELENGTH];
	while (instream.getline(str, LINELENGTH))
	{
		// Skip blank lines.
		if (strlen(str) == 0)
			continue;

		// Parse and store the line.
		sscanf(str, "%*s %s %d", name, &num);
		m_ProbeSets[iSet].probeSetName = name;
		m_ProbeSets[iSet].numberPairs = num;
		++iSet;

	}
	return true;
}

//////////////////////////////////////////////////////////////////////

