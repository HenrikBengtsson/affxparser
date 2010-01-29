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
#include "file/BEDFileData.h"
//
#include <cstring>
#include <fstream>
#include <string.h>
#include <string>
//

#ifdef _MSC_VER
#define snprintf _snprintf
#pragma warning(disable: 4996)
#endif

using namespace std;
using namespace affxbed;

/*! The tab character. */
#define TAB "\t"

/*
 * Clears any used memory.
 */
void IntervalGroup::Clear()
{
	parameters.clear();
	intervals.clear();
}

/*
 * Initialize the class.
 */
BEDFileData::BEDFileData()
{
}

/*
 * Clears any used memory.
 */
BEDFileData::~BEDFileData()
{
	Clear();
}

/*
 * Clears any used memory.
 */
void BEDFileData::Clear()
{
	browser="";
	track="";
	intervalGroups.clear();
}

/*
 * Format the browser string.
 */
void BEDFileData::FormatBrowser(const char *seq, int start, int stop)
{
	char line[256];
	snprintf(line,sizeof(line), "%s %s:%d-%d", BROWSER_LINE_START, seq, start, stop);
	browser=line;
}

/*
 * Format the track string.
 */
void BEDFileData::FormatTrack(const char *name, const char *desc)
{
	char line[256];
	if (desc && strlen(desc) > 0)
		snprintf(line,sizeof(line), "%s\"%s\" description=\"%s\"", TRACK_LINE_START, name, desc);
	else
		snprintf(line,sizeof(line), "%s\"%s\"", TRACK_LINE_START, name);
	track=line;
}


/*
 * Read the contents of the BED file into the BED object.
 */
bool BEDFileData::Read()
{
	Clear();

	// Open the file and read the data.
	ifstream instr(fileName.c_str(), ios::in);
	if (!instr)
		return false;

	// Read the remaining lines.
	int n;
	const int MAXLINELEN = 1024;
	char line[MAXLINELEN+1];
	char seq[MAXLINELEN/4+1];
	char name[MAXLINELEN/4+1];
	IntervalEntry interval;
	IntervalGroup group;
	IntervalGroup *pGroup=NULL;
	TagValuePairType param;
	bool firstParameter=true;

	while (instr.getline(line, MAXLINELEN))
	{
		// The browser line.
		if (strlen(line) > strlen(BROWSER_LINE_START) && strncmp(line, BROWSER_LINE_START, strlen(BROWSER_LINE_START)) == 0)
			Browser() = line;

		// The track line
		else if (strlen(line) > strlen(TRACK_LINE_START) && strncmp(line, TRACK_LINE_START, strlen(TRACK_LINE_START)) == 0)
			Track() = line;

		// Comment lines. These contain parameters.
		else if (line[0] == '#')
		{
			// If this is the first detected parameter then create a new group object.
			if (firstParameter == true)
			{
				firstParameter = false;
				group.Clear();
				intervalGroups.push_back(group);
				pGroup = &(*intervalGroups.rbegin());
			}
			char *token = strtok(line+1, TAB);
			param.Tag = token;
			token = strtok(NULL, TAB);
			if (token)
			{
				param.Value = token;
				pGroup->parameters.push_back(param);
			}
		}

		// This line contains an interval.
		else
		{
			// No longer looking at parameters. Reset the flag.
			firstParameter = true;

			// Create a new group. This is done here if the BED file has no parameters.
			if (intervalGroups.size() == 0)
			{
				group.Clear();
				intervalGroups.push_back(group);
			}

			// Get the last group and add the interval to it.
			pGroup = &(*intervalGroups.rbegin());
			n = sscanf(line, "%s %d %d %s %f %c", seq, &interval.start, &interval.stop, name, &interval.overlap, &interval.strand);
			if (n == 3)
			{
				strcpy(name, "");
				interval.overlap = 0;
				interval.strand = ' ';
			}
			interval.seq = seq;
			interval.probeSetName = name;
			pGroup->intervals.push_back(interval);
		}
	}
	instr.close();
	return true;
}
