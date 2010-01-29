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


#ifndef _TextFileReader_HEADER_
#define _TextFileReader_HEADER_

/*! \file TextFileReader.h This file provides interfaces to store information in a text parameter file.
 */

#include "calvin_files/parsers/src/FileException.h"
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
class TextFileReader
{
public:
	/*! Constructor */
	TextFileReader();

	/*! Destructor */
	~TextFileReader();

protected:
	/*! Reads the body of the file.
	 *
	 * @param fileStream The file stream.
	 * @param textData The parameter name/value pairs.
	 */
	void ReadFileBody(std::ifstream &fileStream, std::map<std::string, std::string> &textData);

public:
	/*! Reads the entire contents of the file.
	 *
	 * @param fileName The name of the text file to read.
	 * @param textData The text file parameter name/value pairs.
	 * @exception affymetrix_calvin_exceptions::CFileNotFoundException The file does not exist.
	 */
	void ReadFile(const std::string &fileName, std::map<std::string, std::string> &textData);

	/*! Reads the entire contents of the file.
	 *
	 * @param fileStream The file stream of the open text file.
	 * @param textData The text file parameter name/value pairs.
	 */
	void ReadFile(std::ifstream &fileStream, std::map<std::string, std::string> &textData);
};

};

#endif // _TextFileReader_HEADER_
