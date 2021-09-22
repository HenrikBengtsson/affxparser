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


#include "calvin_files/parsers/src/GenericFileReader.h"
//
#include "calvin_files/exception/src/DevelopmentException.h"
#include "calvin_files/parsers/src/DataGroupHeaderReader.h"
#include "calvin_files/parsers/src/FileHeaderReader.h"
#include "calvin_files/parsers/src/FileInput.h"
//
#include "util/Fs.h"
//
#include <sys/stat.h>
#include <sys/types.h>
//

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_exceptions;

/*
 * Initialize the class.
 */
GenericFileReader::GenericFileReader()
{
	gendata = 0;
}

/*
 * Clean up.
 */
GenericFileReader::~GenericFileReader()
{
	CloseFile();
}

/*
 *  Read the file header of the generic file.
 */
void GenericFileReader::ReadHeader(affymetrix_calvin_io::GenericData& data, ReadHeaderOption option)
{
	OpenFile();
	switch(option)
	{
	case ReadNoDataGroupHeader:
		ReadFileHeaderNoDataGroupHeader(data);
		break;
	case ReadMinDataGroupHeader:
		ReadFileHeaderMinDP(data);
		break;
	case ReadAllHeaders:	// fall through
	default:
		ReadFileHeader(data);
		break;
	}
	CloseFile();
}

/*
 * Open the file stream.
 */
void GenericFileReader::OpenFile()
{
  Fs::aptOpen(fileStream,fileName, std::ios::in | std::ios::binary);
  if (!fileStream) {
    affymetrix_calvin_exceptions::FileNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
    throw e;
  }
}

/*
 * Closes the file stream.
 */
void GenericFileReader::CloseFile()
{
	if (fileStream.is_open())
		fileStream.close();
	gendata = 0;
}

/*
 * Read the file header and minimize amount of information read from the DataSetHeader.
 */
void GenericFileReader::ReadFileHeaderMinDP(affymetrix_calvin_io::GenericData& data)
{
	// Set the file name
	data.Header().SetFilename(fileName);

	FileHeaderReader fhReader(fileStream, data.Header());
	fhReader.Read();

	// TODO: Remove existing DataGroupHdrs.
	DataGroupHeaderReader dchReader;
	dchReader.ReadAllMinimumInfo(fileStream, data.Header(), fhReader.GetDataGroupCnt());
}

/*
 * Reads the file header of the generic file and reads all the DataSetHeader information.
 */
void GenericFileReader::ReadFileHeader(affymetrix_calvin_io::GenericData& data)
{
	// Set the file name
	data.Header().SetFilename(fileName);

	FileHeaderReader fhReader(fileStream, data.Header());
	fhReader.Read();

	DataGroupHeaderReader dchReader;
	dchReader.ReadAll(fileStream, data.Header(), fhReader.GetDataGroupCnt());
}

/*
 * Reads the file header of the generic file but does not read any DataGroupHeaders or DataSetHeaders.
 */
void GenericFileReader::ReadFileHeaderNoDataGroupHeader(GenericData& data)
{
	// Set the file name
	data.Header().SetFilename(fileName);

	FileHeaderReader fhReader(fileStream, data.Header());
	fhReader.Read();
}

/*
 * Open the file for reading
 */
void GenericFileReader::Open(GenericData& data, OpenHint hint)
{
	if (hint == All)
	{
		OpenFile();
		ReadFileHeader(data);
		gendata = &data;
	}
	else
	{
		NotImplementedException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

/*
 * Gets the number of DataGroups in the file.
 */
u_int32_t GenericFileReader::GetDataGroupCnt() const
{
	if (gendata)
		return gendata->DataGroupCnt();
	return 0;
}

/*
 * Gets DataGroupReader by index.  OpenHint should be All or sequential.
 */
DataGroupReader GenericFileReader::GetDataGroupReader(int32_t index)
{
	if (gendata==0 || fileStream.is_open() == false)
	{
		DataGroupNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	DataGroupHeader* dch = gendata->FindDataGroupHeader(index);
	if (dch == 0)
	{
		DataGroupNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	return DataGroupReader(fileStream, *dch);
}

/*
 * Gets the DataGroupReader by name.  OpenHint must be All.
 */
DataGroupReader GenericFileReader::GetDataGroupReader(const std::wstring& name)
{
	if (gendata==0 || fileStream.is_open() == false)
	{
		DataGroupNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	DataGroupHeader* dch = gendata->FindDataGroupHeader(name);
	if (dch == 0)
	{
		DataGroupNotFoundException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	return DataGroupReader(fileStream, *dch);}

/*
 * Closes the file.
 */
void GenericFileReader::Close()
{
	CloseFile();
}
