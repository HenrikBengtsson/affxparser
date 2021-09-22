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
#ifndef _GenericFileUpdater_HEADER_
#define _GenericFileUpdater_HEADER_

#include "calvin_files/data/src/FileHeader.h"
#include "calvin_files/parsers/src/GenericFileReader.h"
#include "calvin_files/writers/src/GenericFileWriter.h"
//
#include <fstream>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

class GenericFileUpdater
{

private:

	GenericFileWriter* writer;
	FileHeader fileHdr;
	u_int32_t lastDataGrpOffset;
	u_int32_t lastDataGrpDataSetCnt;

public:

	GenericFileUpdater(const std::string& path);

	~GenericFileUpdater();

	DataSetWriter* GetUpdateDataSetWriter(DataSetHeader& dataSetHdr);

	void IncrementDataSetCnt();

private:

	void AppendDataSet(DataSetHeader& hdr, Uint8Vector& data, u_int32_t offset) const;
	
	bool FindLastDataSetHeader(FileHeader& fileHdr, DataSetHeader& dataSetHdr) const;

	FileHeader ReadFileHeader(const std::string& path) const;

	u_int32_t FindLastDataGroupOffset();

	u_int32_t GetLastDataGroupDataSetCnt();
};

}

#endif // _GenericFileUpdater_HEADER_
