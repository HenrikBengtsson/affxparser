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

#ifndef _PSIFileData_HEADER_
#define _PSIFileData_HEADER_

/*! \file PSIFileData.h This file provides reading capaibilities for PSI files.
 */

//////////////////////////////////////////////////////////////////////

#include <cstring>
#include <fstream>
#include <string>
#include <vector>
//

//////////////////////////////////////////////////////////////////////

namespace affxpsi
{

/*! A type to hold the probe set information file entry */
typedef struct _ProbeSetInfo
{
	/*! The name of the probe set */
	std::string probeSetName;

	/*! The number of probe pairs in the set */
	int numberPairs;

} ProbeSetInfo;

/*! An STL vector of ProbeSetInfo objects */
typedef std::vector<ProbeSetInfo> ProbeSetInfoVector;

/*! Stores the contents of a PSI file.
 *
 * The PSI file the probe set names and the number of probe pairs in each set.
 */
class CPSIFileData
{
public:
	/*! Constructor */
	CPSIFileData();

	/*! Destructor */
	~CPSIFileData();

protected:
	/*! The name of the PSI file */
	std::string m_strFileName;

	/*! The probe set information */
	ProbeSetInfoVector m_ProbeSets;

public:
	/*! Sets the file name.
	 * @param name The name of the PSI file.
	 */
	void SetFileName(const char *name) { m_strFileName = name; }

	/*! Gets the file name.
	 * @return The file name.
	 */
	const char *GetFileName() const { return m_strFileName.c_str(); }

	/*! Gets the number of probe sets.
	 * @return The number of probe sets.
	 */
	int GetProbeSetCount() const { return (int) m_ProbeSets.size(); }

	/*! Gets the probe set name.
	 * @param index The index to the probe set vector.
	 * @return The probe set name.
	 */
	std::string GetProbeSetName(int index) const { return m_ProbeSets[index].probeSetName; }

	/*! Gets the number of probe pairs.
	 * @param index The index to the probe set vector.
	 * @return The number of probe pairs.
	 */
	int GetProbePairs(int index) const { return m_ProbeSets[index].numberPairs; }

	/*! Reads the contents of the file.
	 * @return True if successful
	 */
	bool Read();

	/*! Checks for the existance of a file.
	 * @return True if the file exists
	 */
	bool Exists();

	/*! Clears memory associated with the class */
	void Clear();
};

////////////////////////////////////////////////////////////////////

} // namespace

//////////////////////////////////////////////////////////////////////

#endif

