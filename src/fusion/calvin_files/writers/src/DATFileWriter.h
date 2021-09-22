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
#ifndef _DATFileWriter_HEADER_
#define _DATFileWriter_HEADER_

#include "calvin_files/data/src/DATData.h"
#include "calvin_files/writers/src/GenericFileWriter.h"
//
#include <fstream>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

class DATFileWriter
{

private:

	GenericFileWriter* writer;
	DataSetWriter* dataSetWriter;
	int32_t pixelPos;
	int32_t statsPos;
	int32_t pixelLimitPos;
	int32_t statsLimitPos;

public:

	DATFileWriter(DATData &p);
	~DATFileWriter();

	virtual void WriteStats(const Uint16Vector &v);

	virtual void WritePixels(const Uint16Vector &v);

private:

	void WriteHeaders();

	void SetFilePositions();
};

}

#endif // _DATFileWriter_HEADER_
