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


#include "calvin_files/writers/src/TextFileWriter.h"
//
#include "util/Fs.h"
//

using namespace affymetrix_calvin_io;

/*
 * Initialize the class.
 */
TextFileWriter::TextFileWriter()
{
}

/*
 * Clean up.
 */
TextFileWriter::~TextFileWriter()
{
}

/*
 * Write the entire file, the header and body.
 */
void TextFileWriter::WriteFile(const std::string &fileName, std::map<std::string, std::string> &textData)
{
	OpenFile(fileName);
	WriteFileBody(textData);
	CloseFile();
}

/*
 * Open the file stream.
 */
void TextFileWriter::OpenFile(const std::string &fileName)
{
  Fs::aptOpen(fileStream, fileName, std::ios::out);
  if (!fileStream.is_open() && !fileStream.good())
	{
		affymetrix_calvin_exceptions::FileCreateException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

/*
 * Closes the file stream.
 */
void TextFileWriter::CloseFile()
{
	fileStream.close();
	if (fileStream.fail())
	{
		affymetrix_calvin_exceptions::FileWriteException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

/*
 * The fhe parameters to the file in the format name=value.
 */
void TextFileWriter::WriteFileBody(std::map<std::string, std::string> &textData)
{
	std::map<std::string, std::string>::iterator it;
	for (it=textData.begin(); it!=textData.end(); it++)
	{
		fileStream << (*it).first << "=" << (*it).second << std::endl;
	}
}

