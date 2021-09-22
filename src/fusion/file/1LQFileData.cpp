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

#include "file/1LQFileData.h"
//
#include "file/FileIO.h"
//
#include <cstring>
#include <iostream>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
//

#ifdef _MSC_VER
#pragma warning(disable: 4996) // don't show deprecated warnings.
#endif

using namespace affx1lq;
using namespace std;

/*! The maximum line length in a 1LQ file */
#define MAXLINELENGTH 1024

/*
 * Initialize the members.
 */
C1LQFileData::C1LQFileData() :
	  m_NumberRows(0)
	, m_NumberColumns(0)
{
}

/*
 * Free all memory used when reading the 1LQ file.
 */
C1LQFileData::~C1LQFileData()
{
	Clear();
}

/*
 * Free all memory used when reading the 1LQ file.
 */
void C1LQFileData::Clear()
{
	m_NumberRows=0;
	m_NumberColumns=0;
	m_Entries.clear();
}

/*
 * Check if the file exists.
 */
bool C1LQFileData::Exists()
{
	// Find the file stats.
	struct stat st;
	return (stat(m_FileName.c_str(), &st) == 0);
}

/*
 * Open the file and read the header and data sections. If the file
 * fails to open then return a false.
 */
bool C1LQFileData::Read()
{
	// Clear memory first.
	Clear();

	// Open the file.
	ifstream instr;
	instr.open(m_FileName.c_str(), ios::in | ios::binary);

	// Check if open
	if (!instr)
	{
		return false;
	}

	// Read the header
	if (Read(instr) == false)
	{
		Clear();
		return false;
	}

	return true;
}


/*
 * Read the remaining lines in the file (skipping the column header line).
 * These are all data lines.
 * Parse each line and add it to the data list.
 * If the number of columns is not what is expected then return a false.
 * Ignore any lines that begin with a #.
 */
bool C1LQFileData::Read(std::ifstream &instr)
{
	const int maxSize = 128;
	char line[MAXLINELENGTH];
	char qual[maxSize];
	char feat[maxSize];
	char probe[maxSize];
	DataEntry entry;
	const char *format = "%d %d %s %d %s %s %d %d %d %c %c %c %c %d %d %d";
	int max_X = 0;
	int max_Y = 0;

	// Since the newer 1lq file does not contain the following lines,
	// do not use these values as the indicator of NumberOfRows or NumberOfColumns.
	// NumberOfRows should be the maximum value of Y+ 1
	// NumberOfColumns should be the maximum value of X + 1
	// Thus, skip the following lines:
	//   (optional) COLS/ROWS=448	448	Dummy
	//   (optional) Dummy Line.
	//   (column heading) X	Y	SEQUENCE	DESTYPE	FEATURE	QUALIFIER

	ReadNextLine(instr, line, MAXLINELENGTH);
   	if (strncmp("COLS/ROWS", line, 9) == 0) {
		ReadNextLine(instr, line, MAXLINELENGTH);
		// skip the Dummy Line.
		if (strncmp("Dummy", line, 5) == 0) {
			ReadNextLine(instr, line, MAXLINELENGTH);
		}
	}

	while (1)
	{
		line[0] = 0;
		ReadNextLine(instr, line, MAXLINELENGTH);
		if (strlen(line) == 0)
			break;
		if (line[0] == '#')
			continue;
		if (sscanf(line, format,
					&entry.x,
					&entry.y,
					probe,
					&entry.destype,
					feat,
					qual,
					&entry.expos,
					&entry.plength,
					&entry.position,
					&entry.cbase,
					&entry.pbase,
					&entry.tbase,
					&entry.ipbase,
					&entry.unit,
					&entry.block,
					&entry.atom
			) != NUM_1LQ_COLUMNS)
		{
			Clear();
			return false;
		}
		entry.probe = probe;
		entry.qualifier = qual;
		entry.feature = feat;
		m_Entries.push_back(entry);

		if (entry.x > max_X)
			max_X = entry.x;
		if (entry.y > max_Y)
			max_Y = entry.y;
	}

	// NumberOfColumns is max of X + 1
	// NumberOfRows is max of Y + 1
	m_NumberColumns = max_X + 1;
	m_NumberRows = max_Y +1;

	return true;
}

/*! Rotate the data one time. */
void C1LQFileData::Rotate() {
	int x;

	for(list<DataEntry>::iterator it=m_Entries.begin(); it!=m_Entries.end(); ++it)
	{	
		//   (x, y)     ---rotation-->   (x', y')
		//   (0, 0)                      (0, 331)
		//   (1, 0)                      (0, 330)
		//   (330, 331)                  (331, 1)
		//   (331, 331)                  (331, 0)
		//   m_NumberColumns = 332

		// Algorithm for rotation:
		x = (*it).x;
		(*it).x = (*it).y;
		(*it).y = m_NumberColumns - x - 1;
	} 

	// Number of Columns becomes number of Rows and vice versa
	x = m_NumberColumns;
	m_NumberColumns = m_NumberRows;
	m_NumberRows = x;


}


//////////////////////////////////////////////////////////////////////
