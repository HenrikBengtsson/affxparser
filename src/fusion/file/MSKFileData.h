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

#ifndef _MSKFileData_HEADER_
#define _MSKFileData_HEADER_

/*! \file MSKFileData.h This file provides reading capaibilities for MSK files.
 */

//////////////////////////////////////////////////////////////////////

#include <cstring>
#include <fstream>
#include <list>
#include <string>
//

//////////////////////////////////////////////////////////////////////

namespace affxmsk
{

/*! A type to hold the indicies for a probe set */
typedef struct _ProbeSetIndiciesType
{
	/*! The name of the probe set */
	std::string probeSetName;

	/*! The list of indicies for the probe set */
    std::list<int> indicies;
} ProbeSetIndiciesType;

/*! An STL list of ProbeSetIndiciesType objects */
typedef std::list<ProbeSetIndiciesType> ProbeSetIndiciesList;

/*! An STL constant iterator for the ProbeSetIndiciesList object */
typedef ProbeSetIndiciesList::const_iterator ProbeSetIndiciesListConstIt;

/*! An STL list of probe set names */
typedef std::list<std::string> ProbeSetList;

/*! An STL constant iterator for the ProbeSetList object */
typedef ProbeSetList::const_iterator ProbeSetListConstIt;

////////////////////////////////////////////////////////////////////

/*! Stores the contents of a MSK file.
 *
 * The mask file contains two sections. The first section stores a list of probe sets which
 * is used by the scaling and normalization functions of the MAS5 algorithm. The second section
 * defines the list of probe pairs to ignore in the MAS5 algorithm.
 */
class CMSKFileData
{
public:
	/*! Constructor */
	CMSKFileData();

	/*! Destructor */
	~CMSKFileData();

protected:
	/*! The name of the MSK file */
	std::string m_strFileName;

	/*! An error string when a read error occurs */
	std::string m_strError;

	/*! The array type in the mask file */
	std::string m_strArrayType;

	/*! The list of probe set indicies */
	ProbeSetIndiciesList m_Indicies;

	/*! The List of probe sets */
	ProbeSetList m_Sets;

public:
	/*! Sets the file name.
	 *
	 * @param name The name of the MSK file.
	 */
	void SetFileName(const char *name) { m_strFileName = name; }

	/*! Gets the file name.
	 *
	 * @return The file name.
	 */
	const char *GetFileName() const { return m_strFileName.c_str(); }

	/*! Gets the array type.
	 *
	 * @return The probe array type in the mask file.
	 */
	const char *GetArrayType() const { return m_strArrayType.c_str(); }

	/*! Gets the number of probe set indicies in the mask file
	 *
	 * @return The number of probe set indicies in the mask file
	 */
	int GetProbeSetIndiciesListCount() const { return (int) m_Indicies.size(); }

	/*! Gets the number of probe sets in the mask file.
	 *
	 * @return The number of probe sets in the mask file.
	 */
	int GetProbeSetListCount() const { return (int) m_Sets.size(); }

	/*! The error string if a read error occurs
	 *
	 * @return The error string if an error occurs
	 */
	std::string GetError() const { return m_strError; }

	/*! Returns the probe set indicies iterators
	 *
	 * @param begin The head of the list.
	 * @param end The end of the list.
	 */
	void GetProbeSetIndiciesIterators(ProbeSetIndiciesListConstIt &begin, ProbeSetIndiciesListConstIt &end);

	/*! Returns the probe set iterators
	 *
	 * @param begin The head of the list.
	 * @param end The end of the list.
	 */
	void GetProbeSetIterators(ProbeSetListConstIt &begin, ProbeSetListConstIt &end);

	/*! Reads the contents of the file.
	 *
	 * @return True if successful
	 */
	bool Read();

	/*! Checks for the existance of a file.
	 *
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
