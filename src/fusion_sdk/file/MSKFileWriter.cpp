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

#include "file/MSKFileWriter.h"
//
#include <fstream>
#include <istream>
//

#ifdef _MSC_VER
#pragma warning(disable: 4996) // don't show deprecated warnings.
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
CMSKFileWriter::CMSKFileWriter()
{
}

/*
 * Clean up any used memory
 */
CMSKFileWriter::~CMSKFileWriter()
{
	Clear();
}

/*
 * Write the file contents. The probe indicies are one based in the file.
 */
bool CMSKFileWriter::Save()
{
	m_strError = "";

	// Check if the array type has been set.
	if (m_strArrayType.length() == 0)
	{
		m_strError = "The array type has not been set.";
		return false;
	}

	// Open the mask file.
	ofstream outstream(m_strFileName.c_str(), ios::out);
	if (!outstream)
	{
		m_strError = "Unable to create the mask file.";
		return false;
	}

	// The first line is the array type
	outstream << m_strArrayType << endl;

	// Save the comp section.
	int size = (int) m_Sets.size();
	if (size > 0)
	{
		outstream << endl << COMP_SECTION << endl;
		for (list<string>::const_iterator it=m_Sets.begin(); it!=m_Sets.end(); ++it)
		{
			outstream << (*it) << endl;
		}
	}

	// Save the call section.
	size = (int) m_Indicies.size();
	if (size > 0)
	{
		outstream << endl << CALL_SECTION << endl;
		for (ProbeSetIndiciesListConstIt it=m_Indicies.begin(); it!=m_Indicies.end(); ++it)
		{
			const ProbeSetIndiciesType &entry = *it;
			outstream << entry.probeSetName << "\t";
			int index=0;
			size = (int) entry.indicies.size();
			for (list<int>::const_iterator inIt=entry.indicies.begin(); inIt!=entry.indicies.end(); ++inIt)
			{
				outstream << (*inIt) + 1;
				if (index < size-1)
					outstream << ",";
				++index;
			}
			outstream << endl;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
