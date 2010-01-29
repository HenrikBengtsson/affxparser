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
#ifndef _FileHeaderWriter_HEADER_
#define _FileHeaderWriter_HEADER_

#include "calvin_files/data/src/FileHeader.h"
//
#include <fstream>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

class FileHeaderWriter
{
private:

	u_int32_t dataSetOffsetPos;

public:

	FileHeaderWriter() { dataSetOffsetPos = 0; }
	~FileHeaderWriter() {}

	void Write(std::ofstream &os, FileHeader &p);

	void UpdateDataGroupOffset(std::ofstream &os, u_int32_t offset) const;

private:

	void WriteMagicNumber(std::ofstream &os, FileHeader &p);

	void WriteVersion(std::ofstream &os, FileHeader &p);

	void WriteDataGroupCnt(std::ofstream &os, FileHeader &p);

	void WriteDataGroupOffset(std::ofstream &os, u_int32_t offset);

	void WriteGenericDataHdr(std::ofstream &os, FileHeader &f);
};

}

#endif // _FileHeaderWriter_HEADER_
