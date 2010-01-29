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

#ifndef _EXPFileData_HEADER_
#define _EXPFileData_HEADER_

/*! \file EXPFileData.h This file provides reading capaibilities for EXP files.
 */

//////////////////////////////////////////////////////////////////////

#include "file/TagValuePair.h"
//
#include <cstring>
#include <fstream>
#include <list>
#include <string>
//

//////////////////////////////////////////////////////////////////////

namespace affxexp
{

/*! Stores the contents of a EXP file. */
class CEXPFileData
{
public:
	/*! Constructor */
	CEXPFileData();

	/*! Destructor */
	~CEXPFileData();

protected:
	/*! The name of the EXP file */
	std::string m_strFileName;

	/*! The array type in the EXP file */
	std::string m_strArrayType;

	/*! A list of scan parameters. */
	TagValuePairTypeList m_Scan;

	/*! A list of hyb parameters. */
	TagValuePairTypeList m_Hyb;

	/*! A list of sample parameters. */
	TagValuePairTypeList m_Sample;

public:
	/*! Sets the file name.
	 *
	 * @param name The name of the EXP file.
	 */
	void SetFileName(const char *name) { m_strFileName = name; }

	/*! Gets the file name.
	 *
	 * @return The file name.
	 */
	std::string GetFileName() const { return m_strFileName; }

	/*! Gets the array type.
	 *
	 * @return The probe array type in the mask file.
	 */
	std::string GetArrayType() const { return m_strArrayType; }

	/*! Sets the array type.
	 *
	 * @param arrayType The probe array type.
	 */
	void SetArrayType(const char *arrayType) { m_strArrayType = arrayType; }

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

	/*! Gets the list of scan parameters.
	 * @return The list of scan parameters.
	 */
	TagValuePairTypeList &GetScanParameters() { return m_Scan; }

	/*! Gets the list of hyb parameters.
	 * @return The list of hyb parameters.
	 */
	TagValuePairTypeList &GetHybParameters() { return m_Hyb; }

	/*! Gets the list of sample parameters.
	 * @return The list of sample parameters.
	 */
	TagValuePairTypeList &GetSampleParameters() { return m_Sample; }
};

} // namespace

//////////////////////////////////////////////////////////////////////

#endif
