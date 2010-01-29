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

#if !defined(AFX_BARFILEWRITER_H__BDEC0B48_58F6_480C_A988_05355DCF0BA4__INCLUDED_)
#define AFX_BARFILEWRITER_H__BDEC0B48_58F6_480C_A988_05355DCF0BA4__INCLUDED_

/*! \file BARFileWriter.h This file provides BAR file writing capabilities.
 */

//////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(disable: 4786) // identifier was truncated in the debug information
#endif

//////////////////////////////////////////////////////////////////////

#include "file/BARFileData.h"
//


//////////////////////////////////////////////////////////////////////

namespace affxbarwriter
{

////////////////////////////////////////////////////////////////////

/*! This class provides writing capabilities for BAR files. */
class CBARFileWriter : public affxbar::CBARFileData
{
public:
	/*! Constructor */
	CBARFileWriter();

	/*! Destructor */
	~CBARFileWriter();

protected:
	/*! The output file stream. */
	std::ofstream m_NewBarFile;

public:
	/*! Creates a new file. 
	 * @return True if successful
	 */
	bool CreateNewFile();

	/*! Saves the data to the BAR file.
	 * @return True if successful
	 */
	bool Save();
};

////////////////////////////////////////////////////////////////////

} // namespace

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_BARFILEWRITER_H__BDEC0B48_58F6_480C_A988_05355DCF0BA4__INCLUDED_)
