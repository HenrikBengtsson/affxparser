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
#ifndef _DATFileUpdater_HEADER_
#define _DATFileUpdater_HEADER_

#include "calvin_files/data/src/DATData.h"
#include "calvin_files/parsers/src/DATFileReader.h"
#include "calvin_files/writers/src/FileHeaderUpdater.h"
#include "calvin_files/writers/src/GenericFileWriter.h"
//
#include <fstream>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

class DATFileUpdater
{

private:

	DATData* updateData;
	DATData* currentData;
	GenericFileWriter* writer;
	DataSetWriter* dataSetWriter;

public:

	DATFileUpdater(DATData &p);
	~DATFileUpdater();

	void Update();

private:

	void WriteGridData(DataSetHeader& gridHdr, u_int32_t offset) const;

	void WriteSubGridData(DataSetHeader& SubGridHdr, u_int32_t offset) const;

	u_int32_t GetGridDataSetOffset(FileHeader fh) const;

	u_int32_t GetSubGridDataSetOffset(FileHeader fh) const;

	DataSetHeader CreateGridDataSetHeader() const;

	DataSetHeader CreateSubGridDataSetHeader() const;

	void UpdateDataSetCnt(FileHeader fh, u_int32_t cnt) const;

	void ReadCurrentData() const;

	void UpdateFileHeader();

	void AddGridDataSetHeaderParameters(DataSetHeader& hdr) const;
};

}

#endif // _DATFileUpdater_HEADER_
