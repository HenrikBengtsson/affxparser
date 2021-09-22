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


#ifndef _GenericFileReader_HEADER_
#define _GenericFileReader_HEADER_

/*! \file GenericFileReader.h This file provides methods to read a generic data file.
 */

#include "calvin_files/data/src/GenericData.h"
#include "calvin_files/parsers/src/DataGroupReader.h"
#include "calvin_files/parsers/src/FileException.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
#include "calvin_files/utils/src/AffymetrixGuid.h"
//
#include <cstring>
#include <fstream>
#include <string>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // dont show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{
	/*! This class reads a generic data file. It is the top-level generic data file reader.*/
class GenericFileReader
{
public:
	/*! Constructor */
	GenericFileReader();

	/*! Destructor */
	~GenericFileReader();

	/*! Hint used when opening a file */
	enum OpenHint { All, Sequential, None };

	/*! Indicates how much header information to read */
	enum ReadHeaderOption { ReadAllHeaders, ReadMinDataGroupHeader, ReadNoDataGroupHeader };

public:
	/*! Gets the name of the input file.
	 *
	 * @return The name of the input file.
	 */
	const std::string GetFilename() const { return fileName; }

	/*! Sets the name of the input file.
	 *
	 * @param name The name of the input file.
	 */
	void SetFilename(const std::string &name) { fileName = name; }

	/*! Read the file header of the generic file.
	 *
	 * @param data A reference to a GenericData object that will receive header information from the file.
	 * @param option Indicates how much DataGroupHeader and DataSetHeader information to read.
	 * @exception affymetrix_calvin_exceptions::FileNotFoundException The file does not exist.
	 * @exception affymetrix_calvin_exceptions::InvalidVersionException The file version does not match.
	 * @exception affymetrix_calvin_exceptions::InvalidFileTypeException The file is not of the right type.
	 */
	void ReadHeader(GenericData& data, ReadHeaderOption option=ReadAllHeaders);

	/*! Open the file for reading
	 *	@param data A reference to a GenericData object that will receive header information from the file.  Amount of info depends on the hint.
	 *	@param hint A hint on how to open the file.
	 *	@exception affymetrix_calvin_exceptions::FileNotFoundException The file does not exist.
	 *	@exception affymetrix_calvin_exceptions::InvalidVersionException The file version does not match.
	 *	@exception affymetrix_calvin_exceptions::InvalidFileTypeException The file is not of the right type.
	 */
	void Open(GenericData& data, OpenHint hint=All);

	/*! Gets the number of DataGroups in the file.
	 *	@return The number of DataGroups in the file.
	 */
	u_int32_t GetDataGroupCnt() const;

	/*! Gets DataGroupReader by index.  OpenHint should be All or sequential.
	 *	@param index Data dataGroup index.  If opened for sequential access the index should be an increment of one from call to call.
	 *	@return A DataGroupReader to use to read the data.
	 *	@exception DataGroupNotFoundException
	 */
	DataGroupReader GetDataGroupReader(int32_t index);

	/*! Gets the DataGroupReader by name.  OpenHint must be All.
	 *	@param name Data dataGroup name.
	 *	@return A DataGroupReader to use to read the data.
	 *	@exception DataGroupNotFoundException
	 */
	DataGroupReader GetDataGroupReader(const std::wstring& name);

	/*! Gets the DataGroupReader based on the file position of the DataGroup.
	 *	@param index Data dataGroup file position.
	 *	@return A DataGroupReader to use to read the data.
	 */
//	DataGroupReader GetDataGroupReader(u_int32_t filePosition);

	/*! Closes the file.
	 */
	void Close();

protected:
	/*! Opens the file for reading */
	void OpenFile();

	/*! Read the file header and minimize amount of information read from the DataSetHeaders.
	 *  It does not attempt to read the complete DataSetHeader.  That is deferred
	 *  until accessed by the DataSet object.
	 *  @param data Reference to the GenericData object to fill.
	 */
	void ReadFileHeaderMinDP(GenericData& data);

	/*! Reads the file header of the generic file and reads all the DataSetHeader information.
	 *  @param data Reference to the GenericData object to fill.
	 */
	void ReadFileHeader(GenericData& data);

	/*! Reads the file header of the generic file but does not read any DataGroupHeaders or DataSetHeaders.
	 *  @param data Reference to the GenericData object to fill.
	 */
	void ReadFileHeaderNoDataGroupHeader(GenericData& data);

	/*! Closes the file */
	void CloseFile();

protected:
	/*! The name of the input file. */
	std::string fileName;

	/*! The file stream. */
	std::ifstream fileStream;

	/*! A pointer to the GenericData object */
	GenericData* gendata;
};

}

#endif // _GenericFileReader_HEADER_
