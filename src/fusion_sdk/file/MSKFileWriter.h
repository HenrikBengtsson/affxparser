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

#ifndef _MSKFileWriter_HEADER_
#define _MSKFileWriter_HEADER_

/*! \file MSKFileWriter.h This file provides writing capaibilities for MSK files.
 */

//////////////////////////////////////////////////////////////////////

#include "file/MSKFileData.h"
//

//////////////////////////////////////////////////////////////////////

namespace affxmsk
{

/*! Provides functions to write a MSK file.
 *
 * The mask file contains two sections. The first section stores a list of probe sets which
 * is used by the scaling and normalization functions of the MAS5 algorithm. The second section
 * defines the list of probe pairs to ignore in the MAS5 algorithm.
 *
 * The indicies are stored in the file one based. When read, they are converted to zero based.
 */
class CMSKFileWriter : public CMSKFileData
{
public:
	/*! Constructor */
	CMSKFileWriter();

	/*! Destructor */
	~CMSKFileWriter();

public:
	/*! Sets the array type.
	 * @param arrayType The probe array type.
	 */
	void SetArrayType(const char *arrayType) { m_strArrayType = arrayType; }

	/*! Saves the data to a MSK file.
	 *
	 * @return True if successful
	 */
	bool Save();

	/*! Adds a set of probe indicies to the list.
	 * @param indicies The probe indicies (the indicies should be one bases here).
	 */
	void AddProbeSetIndicies(const ProbeSetIndiciesType &indicies) { m_Indicies.push_back(indicies); }

	/*! Adds a probe set to the list.
	 * @param probeSet The probe set to add.
	 */
	void AddProbeSet(const char *probeSet) { m_Sets.push_back(probeSet); }
};

////////////////////////////////////////////////////////////////////

} // namespace

//////////////////////////////////////////////////////////////////////

#endif
