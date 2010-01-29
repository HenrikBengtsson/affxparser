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

#include "file/GRCFileWriter.h"
//
#include "file/FileWriter.h"
//
#include <fstream>
//

//////////////////////////////////////////////////////////////////////

using namespace affxgrc;
using namespace affx1lq;
using namespace std;
using namespace affymetrix_grid_control;

//////////////////////////////////////////////////////////////////////

/*! The probe sequence for a non-synthesized probe */
#define STR_NONSYNTH "!"

/*! The B2 probe sequences (25mer) */
#define STR_CTRLB2_25	"CAGCAGTTCTACGATGGCAAGTCCT"

/*! The B2 probe sequences (21mer) */
#define STR_CTRLB2_21	"GCAGTTCTACGATGGCAAGTC"

/*! The B2 probe sequences Extended */
#define STR_EXTENDB2_11 "CCGTTCAGCAGTACTACGATGGCAA"

/*! The B1 probe sequences (25mer) */
#define STR_CTRLB1_25	"AGGACTTGCCATCGTAGAACTGCTG"

/*! The B1 probe sequences (21mer) */
#define STR_CTRLB1_21	"GACTTGCCATCGTAGAACTGC"

/*! Qualifier for extended B2 */
#define	STR_EXTENDB2_QUALIFIER	"extend948"

/*! The probe length of the B2 extended probes. */
#define STR_EXTENDB2_PROBE_LENGTH 24

//////////////////////////////////////////////////////////////////////

/*
 * Initialize the class to null values
 */
CGRCFileWriter::CGRCFileWriter()
{
}

/*
 * Clean up any used memory
 */
CGRCFileWriter::~CGRCFileWriter()
{
}

/*
 * Extract the data from the 1LQ file.
 */
void CGRCFileWriter::PopulateData(const C1LQFileData &libData) {
	ParseLibData(libData);
}

/* 
 * Sets the B1 probe at the given index.
 */
void CGRCFileWriter::SetB1(int index, int x, int y)
{
	FeatureCoordinate coord;
	coord.x = x;
	coord.y = y;
	if (index < m_Grid.GetNumB1Probes())
		m_Grid.SetB1(index, coord);
}

/* 
 * Sets the B2 probe at the given index.
 */
void CGRCFileWriter::SetB2(int index, int x, int y)
{
	FeatureCoordinate coord;
	coord.x = x;
	coord.y = y;
	if (index < m_Grid.GetNumB2Probes())
		m_Grid.SetB2(index, coord);
}

/* 
 * Sets the NS probe at the given index.
 */
void CGRCFileWriter::SetNS(int index, int x, int y)
{
	FeatureCoordinate coord;
	coord.x = x;
	coord.y = y;
	if (index < m_Grid.GetNumNSProbes())
		m_Grid.SetNS(index, coord);
}

/*
 * Extract the data from the 1LQ file.
 * If shift mask design then 
 *	put the extended B2's in the B1 bin.
 */
void CGRCFileWriter::ParseLibData(const C1LQFileData &libData)
{
	m_Grid.Clear();

	m_Grid.SetColumns(libData.GetNumberColumns());
	m_Grid.SetRows(libData.GetNumberRows());

	int nNS=0;
	int nB1=0;
	int nB2=0;
	bool bShiftMask=false;
	const list<DataEntry> &entries = libData.GetEntries();


	// Determine if shift mask design.
	for (list<DataEntry>::const_iterator it = entries.begin(); bShiftMask == false && it != entries.end(); it++)
	{
		DataEntry entry = *it;
		if ( entry.probe == STR_EXTENDB2_11 ||
			(entry.qualifier == STR_EXTENDB2_QUALIFIER && entry.plength == STR_EXTENDB2_PROBE_LENGTH))
			bShiftMask = true;
	}


	// Count the number of probes in each bin.
	for (list<DataEntry>::const_iterator it = entries.begin(); it != entries.end(); it++)
	{
		DataEntry entry = *it;

		// Increment the counters for the non-synth probes.
		if(entry.probe == STR_NONSYNTH)
		{
			nNS++;
		}

		// Increment B2 if shift mask B2 probe.
		else if (bShiftMask == true && (entry.qualifier == STR_EXTENDB2_QUALIFIER && entry.plength == STR_EXTENDB2_PROBE_LENGTH))
		{
			nB2++;
		}

		// Increment B1 bin if shift mask ext B2 probe.
		else if (bShiftMask == true && entry.probe == STR_EXTENDB2_11)
		{
			nB1++;
		}

		// Increment B2 bin if B2 probe.
		else if (entry.probe == STR_CTRLB2_25 || entry.probe == STR_CTRLB2_21)
		{
			nB2++;
		}

		// Increment B1 bin if checkerboard design and B1 probe.
		else if (bShiftMask == false && (entry.probe == STR_CTRLB1_25 || entry.probe == STR_CTRLB1_21))
		{
			nB1++;
		}
	}

	// Allocate memory for the arrays
	m_Grid.ResizeB1(nB1);
	m_Grid.ResizeB2(nB2);
	m_Grid.ResizeNS(nNS);

	// Extract the coordinates from the 1LQ data.
	int indexB1=0;
	int indexB2=0;
	int indexns=0;
	FeatureCoordinate coord;
	for (list<DataEntry>::const_iterator it = entries.begin(); it != entries.end(); it++)
	{
		DataEntry entry = *it;

		// Store the coordinates.
		if(entry.probe == STR_NONSYNTH)
		{	
			coord.x = entry.x;
			coord.y = entry.y;
			m_Grid.SetNS(indexns, coord);
			++indexns;
		}
		else if (bShiftMask == true && (entry.qualifier == STR_EXTENDB2_QUALIFIER) && (entry.plength == STR_EXTENDB2_PROBE_LENGTH))
		{
			coord.x = entry.x;
			coord.y = entry.y;
			m_Grid.SetB2(indexB2, coord);
			++indexB2;
		}
		else if (bShiftMask == true && entry.probe == STR_EXTENDB2_11)
		{
			coord.x = entry.x;
			coord.y = entry.y;
			m_Grid.SetB1(indexB1, coord);
			++indexB1;
		}
		else if (entry.probe == STR_CTRLB2_25 || entry.probe == STR_CTRLB2_21)
		{
			coord.x = entry.x;
			coord.y = entry.y;
			m_Grid.SetB2(indexB2, coord);
			++indexB2;
		}
		else if (bShiftMask == false && (entry.probe == STR_CTRLB1_25 || entry.probe == STR_CTRLB1_21))
		{
			coord.x = entry.x;
			coord.y = entry.y;
			m_Grid.SetB1(indexB1, coord);
			++indexB1;
		}
	}
}

/*
 * Extract the data from the 1LQ file and write the GRC file
 */
bool CGRCFileWriter::Write(const C1LQFileData &libData, bool opposite /* = false */)
{
	ParseLibData(libData);
	return WriteFile(opposite);
}

/*
 * write the data to a GRC file
 */
bool CGRCFileWriter::Write(bool opposite /* = false */)
{
	return WriteFile(opposite);
}

/*
 * Write the GRC file
 * If not opposite, print B2 first, then B1
 * Otherwise, print B1 first, then B2
 */
bool CGRCFileWriter::WriteFile(bool opposite /* = false */)
{
	// Open the grc file.
	ofstream grc(m_strFileName.c_str(), ios::out | ios::binary);
	if (!grc)
	{
		return false;
	}

	// Write the magic/version
	uint8_t ver=GRC_VERSION;
	uint8_t magic=GRC_MAGIC;
	WriteUInt8(grc, magic);
	WriteUInt8(grc, ver);

	// Write the counts.
	WriteInt32_N(grc, m_Grid.GetColumns());
	WriteInt32_N(grc, m_Grid.GetRows());
	if (!opposite) {
		WriteInt32_N(grc, m_Grid.GetNumB2Probes());
		WriteInt32_N(grc, m_Grid.GetNumB1Probes());
	} else {
		WriteInt32_N(grc, m_Grid.GetNumB1Probes());
		WriteInt32_N(grc, m_Grid.GetNumB2Probes());
	}
	WriteInt32_N(grc, m_Grid.GetNumNSProbes());

	// Write the coordinates.
	uint16_t sval;
	int n;
	if (!opposite) {
		n=m_Grid.GetNumB2Probes();
		for (int i=0; i<n; i++)
		{
			sval = m_Grid.GetB2(i).x;
			WriteUInt16_N(grc, sval);
			sval = m_Grid.GetB2(i).y;
			WriteUInt16_N(grc, sval);
		}
		
		n=m_Grid.GetNumB1Probes();
		for (int i=0; i<n; i++)
		{
			sval = m_Grid.GetB1(i).x;
			WriteUInt16_N(grc, sval);
			sval = m_Grid.GetB1(i).y;
			WriteUInt16_N(grc, sval);
		}
	} else {		
		n=m_Grid.GetNumB1Probes();
		for (int i=0; i<n; i++)
		{
			sval = m_Grid.GetB1(i).x;
			WriteUInt16_N(grc, sval);
			sval = m_Grid.GetB1(i).y;
			WriteUInt16_N(grc, sval);
		}

		n=m_Grid.GetNumB2Probes();
		for (int i=0; i<n; i++)
		{
			sval = m_Grid.GetB2(i).x;
			WriteUInt16_N(grc, sval);
			sval = m_Grid.GetB2(i).y;
			WriteUInt16_N(grc, sval);
		}
	}

	n=m_Grid.GetNumNSProbes();
	for (int i=0; i<n; i++)
	{
		sval = m_Grid.GetNS(i).x;
		WriteUInt16_N(grc, sval);
		sval = m_Grid.GetNS(i).y;
		WriteUInt16_N(grc, sval);
	}

	// Close and return status.
	grc.close();
	if (grc.fail())
	{
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
