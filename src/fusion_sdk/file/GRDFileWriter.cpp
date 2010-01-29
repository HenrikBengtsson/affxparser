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

#include "file/GRDFileWriter.h"
//
#include "file/FileIO.h"
#include "file/FileWriter.h"
//
#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <istream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
//

using namespace affxgrd;

#ifdef _MSC_VER
#pragma warning(disable: 4996) // don't show deprecated warnings.
#endif


#define IDENTIFIER_SIZE 8
#define GRD_FILE_IDENTIFIER "\211GRD\r\n\032\n"
const int GRD_FILE_IDENTIFIER_LEN = 8;
#define FILE_VERSION 1.0f

//////////////////////////////////////////////////////////////////////

bool CGRDFileWriter::WriteHeader()
{
	if (WriteFixedHeader() == false)
		return false;
	if (WriteTagHeader() == false)
		return false;
	if (WriteOptimumCorners() == false)
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CGRDFileWriter::WriteFixedHeader()
{
	// Write the header.
	m_outstr.write(GRD_FILE_IDENTIFIER, GRD_FILE_IDENTIFIER_LEN);

	WriteFloat_N(m_outstr, m_HeaderData.GetVersion());

	// Write the dimensions of the array
	WriteUInt32_N(m_outstr, m_HeaderData.GetRows());
	WriteUInt32_N(m_outstr, m_HeaderData.GetCols());

	// Write the other members.
	WriteFloat_N(m_outstr, m_HeaderData.GetFeaturePitchX());
	WriteFloat_N(m_outstr, m_HeaderData.GetFeaturePitchY());
	WriteFloat_N(m_outstr, m_HeaderData.GetFeatureSetbackX());
	WriteFloat_N(m_outstr, m_HeaderData.GetFeatureSetbackY());

	// Check the status.
	if (m_outstr.fail())
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CGRDFileWriter::WriteTagHeader()
{
	// write the number of bytes in the section - placeholder
	std::streampos posStart = m_outstr.tellp();
	WriteUInt32_N(m_outstr, 0L);

	StrStrMap& props = m_HeaderData.GetParameters();

	// write the number of name-value pair strings.
	WriteUInt32_N(m_outstr, props.size());

	for( StrStrMap::iterator ii = props.begin(); ii != props.end(); ++ii )
	{
		WriteString_N(m_outstr, ii->first);
		WriteString_N(m_outstr, ii->second);
	}

	// go back and write the number of bytes
	std::streampos posCurr = m_outstr.tellp();
	m_outstr.seekp(posStart);
	WriteUInt32_N(m_outstr, posCurr - posStart);
	m_outstr.seekp(posCurr);

	// Check the status.
	if (m_outstr.fail())
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CGRDFileWriter::WriteOptimumCorners()
{
	const uint32_t nSubgridCoordSize = FLOAT_SIZE*8;

	// write the number of bytes in the section
	WriteUInt32_N(m_outstr, nSubgridCoordSize*m_HeaderData.GetNumSubgrids()+ULONG_SIZE*2);

	// write the number of subgrids.
	WriteUInt32_N(m_outstr, m_HeaderData.GetNumSubgrids());

	// write subgrids
	for (uint32_t i = 0; i < m_HeaderData.GetNumSubgrids(); ++i  )
	{
		FRECT r = m_HeaderData.GetOptSubgrid(i);

		WriteFloat_N(m_outstr, r.ul.fx);
		WriteFloat_N(m_outstr, r.ul.fy);
		WriteFloat_N(m_outstr, r.ur.fx);
		WriteFloat_N(m_outstr, r.ur.fy);
		WriteFloat_N(m_outstr, r.ll.fx);
		WriteFloat_N(m_outstr, r.ll.fy);
		WriteFloat_N(m_outstr, r.lr.fx);
		WriteFloat_N(m_outstr, r.lr.fy);
	}

	// Check the status.
	if (m_outstr.fail())
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CGRDFileWriter::CreateNewFile()
{
	// Create a new file.
	m_outstr.open(m_FileName.c_str(), std::ios::out | std::ios::binary);
	if (m_outstr)
	{
		return WriteHeader();
	}
	return false;
}

//////////////////////////////////////////////////////////////////////

bool CGRDFileWriter::WriteCenter(const FCOORD& center)
{
	// Write the center data.
	WriteFloat_N(m_outstr, center.fx);
	WriteFloat_N(m_outstr, center.fy);

	// Check the status.
	if (m_outstr.fail())
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CGRDFileWriter::CloseNewFile()
{
	// Close the file.
	m_outstr.close();

	// Check the status.
	if (m_outstr.fail())
		return false;
	return true;
}


