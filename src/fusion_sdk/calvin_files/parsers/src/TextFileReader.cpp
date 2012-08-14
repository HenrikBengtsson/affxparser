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

//
#include "calvin_files/parsers/src/TextFileReader.h"
//
#include "util/Fs.h"
//
#include <cstring>
#include <string.h>
#include <string>
//

using namespace affymetrix_calvin_io;

/*
 * Initialize the class.
 */
TextFileReader::TextFileReader()
{
}

/*
 * Clean up.
 */
TextFileReader::~TextFileReader()
{
}

/*
 * Read the entire file, the header and body.
 */
void TextFileReader::ReadFile(const std::string &fileName, std::map<std::string, std::string> &textData)
{
	textData.clear();
	std::ifstream fileStream;
        Fs::aptOpen(fileStream, fileName);
	if (!fileStream)
	{
		affymetrix_calvin_exceptions::FileNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
	ReadFileBody(fileStream, textData);
	fileStream.close();
}

/*
 * Read the contents of the file using the input file stream.
 */
void TextFileReader::ReadFile(std::ifstream &fileStream, std::map<std::string, std::string> &textData)
{
	ReadFileBody(fileStream, textData);
}

/*
 * Read the file body. Each line of the file should contain
 * either a comment, blank line or name=value parameter.
 * Comment lines start with a # sign.
 */
void TextFileReader::ReadFileBody(std::ifstream &fileStream, std::map<std::string, std::string> &textData)
{
	const int MAX_LINE_LENGTH = 1024;
	char line[MAX_LINE_LENGTH];
	std::string name;
	std::string value;

	// Read each line and store only the name/value parameters.
	while (fileStream.getline(line, MAX_LINE_LENGTH))
	{
		if (line[0] == '#')
			continue;

		while (strlen(line) > 0 && iscntrl(line[strlen(line)-1]))
		{
			line[strlen(line)-1] = 0;
		}

		char *index = strchr(line, '=');
		if (index)
		{
			value = (index+1);
			line[ strlen(line) - strlen(index)] = 0;
			name = line;
			textData[name] = value;
		}
	}
}
