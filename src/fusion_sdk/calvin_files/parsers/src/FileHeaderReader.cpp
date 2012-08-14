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


#include "calvin_files/parsers/src/FileHeaderReader.h"
//
#include "calvin_files/parsers/src/FileInput.h"
#include "calvin_files/parsers/src/GenericDataHeaderReader.h"
//
#include <sys/stat.h>
#include <sys/types.h>
//

using namespace affymetrix_calvin_io;

/*! The expected magic number */
#define DATA_FILE_MAGIC_NUMBER 59

/*! The expected version number */
#define DATA_FILE_VERSION_NUMBER 1

/*
 * Constructor
 */
FileHeaderReader::FileHeaderReader( std::ifstream& fs, FileHeader &fh)
	: fileStream(fs), header(fh)
{
	dataGroupCnt = 0;
	firstDataGroupFilePos = 0;
}

/*
 * Read the FileHeader from the file.
 */
void FileHeaderReader::Read()
{
	ReadMagicNumber();
	ReadVersion();
	ReadDataGroupCnt();
	ReadFirstDataGroupFilePos();
	ReadGenericDataHdr();
}

/*
 * Reads the file magic number from the file.
 */
void FileHeaderReader::ReadMagicNumber()
{
	// Read magic number
	u_int8_t fileMagicNumber = FileInput::ReadInt8(fileStream);
	if (fileMagicNumber != DATA_FILE_MAGIC_NUMBER)
	{
		affymetrix_calvin_exceptions::InvalidFileTypeException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

/*
 * Reads the generic file version number from the file.
 */
void FileHeaderReader::ReadVersion()
{
	// Read generic data file version
	u_int8_t fileVersionNumber = FileInput::ReadInt8(fileStream);
	if (fileVersionNumber != DATA_FILE_VERSION_NUMBER)
	{
		affymetrix_calvin_exceptions::InvalidVersionException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

/*
 * Reads the DataGroup count from the file.
 */
void FileHeaderReader::ReadDataGroupCnt()
{
	dataGroupCnt = FileInput::ReadUInt32(fileStream);
	header.SetNumDataGroups(dataGroupCnt);
}

/*
 * Reads the file position of the first DataGroup.
 */
void FileHeaderReader::ReadFirstDataGroupFilePos()
{
	firstDataGroupFilePos = FileInput::ReadUInt32(fileStream);
	header.SetFirstDataGroupFilePos(firstDataGroupFilePos);
}

/*
 * Reads the GenericDataHeader from the file.
 */
void FileHeaderReader::ReadGenericDataHdr()
{
	// Read all the GenericDataHeader
	GenericDataHeader gdh;
	GenericDataHeaderReader gdhReader(fileStream);
	gdhReader.Read(gdh);

	// Set the GenericDataHeader in the FileHeader
	header.SetGenericDataHdr(gdh);
}

