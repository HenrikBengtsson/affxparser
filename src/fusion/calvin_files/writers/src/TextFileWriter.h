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


#ifndef _TextFileWriter_HEADER_
#define _TextFileWriter_HEADER_

/*! \file TextFileWriter.h This file provides interfaces to store information in a text parameter file.
 */

#include "calvin_files/writers/src/FileWriteException.h"
//
#include <cstring>
#include <fstream>
#include <map>
#include <string>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // dont show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

/*! This class provides interfaces to store text parameter information. */
class TextFileWriter
{
public:
	/*! Constructor */
	TextFileWriter();

	/*! Destructor */
	~TextFileWriter();

protected:
	/*! The file stream. */
	std::ofstream fileStream;

	/*! Opens the file for writing.
	 *
	 * @param fileName The name of the file to open.
	 */
	void OpenFile(const std::string &fileName);

	/*! Writes the body of the file.
	 *
	 * @param textData The parameter name/value pairs.
	 */
	void WriteFileBody(std::map<std::string, std::string> &textData);

	/*! Closes the file */
	void CloseFile();


public:
	/*! Writes the parameters to the text file.
	 *
	 * @param fileName The name of the text file to write.
	 * @param textData The text file parameter name/value pairs.
	 * @exception affymetrix_calvin_exceptions::CFileCreateException The output file cannot be created.
	 * @exception affymetrix_calvin_exceptions::CFileWriteException The file was not written properly.
	 */
	void WriteFile(const std::string &fileName, std::map<std::string, std::string> &textData);
};

};

#endif // _TextFileWriter_HEADER_
