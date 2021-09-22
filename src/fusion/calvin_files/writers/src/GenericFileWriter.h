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
#ifndef _GenericFileWriter_HEADER_
#define _GenericFileWriter_HEADER_

#include "calvin_files/data/src/FileHeader.h"
#include "calvin_files/writers/src/DataGroupHeaderWriter.h"
#include "calvin_files/writers/src/DataGroupWriter.h"
#include "calvin_files/writers/src/FileHeaderWriter.h"
#include "calvin_files/writers/src/FileOutput.h"
#include "calvin_files/writers/src/FileWriteException.h"
//


#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

class GenericFileWriter
{
private:

	std::ofstream os;
	FileHeader* fileHdr;
	DataGroupWriterVector writers;

public:

	GenericFileWriter(FileHeader* p, bool truncate=true);
	~GenericFileWriter();

	void WriteHeader();

	void GetDataGroupWriters(DataGroupWriterIt &begin, DataGroupWriterIt &end);

	DataGroupWriter& GetDataGroupWriter(int32_t index);

	int32_t GetDataGroupCnt();

	int32_t GetFilePos();

	void SeekFromCurrentPos(int32_t offset);

	void SeekFromBeginPos(int32_t offset);

	void SeekFromEndPos(int32_t offset);

	DataGroupWriter* CreateDataGroupWriter(DataGroupHeader& hdr);

	void Write(u_int32_t p);

	std::ofstream& GetFileOStream() { return os; }

private:

	void OpenFileOStream(const std::string &filename, bool truncate);

	void CreateWriters();
};

}

#endif // _GenericFileWriter_HEADER_
