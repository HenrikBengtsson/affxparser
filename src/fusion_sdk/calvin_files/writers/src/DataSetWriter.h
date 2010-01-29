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
#ifndef _DataSetWriter_HEADER_
#define _DataSetWriter_HEADER_

#include "calvin_files/data/src/DataSetHeader.h"
#include "calvin_files/writers/src/DataSetHeaderWriter.h"
#include "calvin_files/writers/src/FileOutput.h"
//
#include <fstream>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

class DataSetWriter
{

public:

	DataSetWriter(std::ofstream* s, DataSetHeader* d);
	~DataSetWriter() { os->flush(); };

private:

	DataSetHeader* dataSetHdr;
	DataSetHeaderWriter hdrWriter;
	std::ofstream* os;
	int32_t columnCnt;
	int32_t columnIdx;
	int32_t rowIdx;
	int32_t rowCnt;

public:

	void WriteHeader();

	void UpdateNextDataSetOffset() const;

	void UpdateNextDataSetOffset(u_int32_t pos) const;

	void Write(int8_t p);

	void Write(u_int8_t p);

	void Write(int16_t p);

	void Write(u_int16_t p);

	void Write(int32_t p);

	void Write(u_int32_t p);

	void Write(float p);

	void Write(const std::string &p, int32_t maxLn);

	void Write(const std::wstring &p, int32_t maxLn);

	void WriteBuffer(char* psBuffer, int32_t iLength);

	std::wstring GetDataSetName() const;

	int32_t GetDataSetSize() const;

private:

};

/*! vector of DataSetHeaders */
typedef std::vector<DataSetWriter> DataSetWriterVector;
/*! constant iterator of DataSetHeaders */
typedef std::vector<DataSetWriter>::iterator DataSetWriterIt;

}

#endif // _DataSetWriter_HEADER_
