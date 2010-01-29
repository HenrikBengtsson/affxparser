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
#ifndef _DataSetHeaderWriter_HEADER_
#define _DataSetHeaderWriter_HEADER_

#include "calvin_files/data/src/DataSetHeader.h"
//
#include <fstream>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

class DataSetHeaderWriter
{

private:

	u_int32_t dataPos;
	u_int32_t nextDataSetPos;

public:

	DataSetHeaderWriter() { dataPos = 0; nextDataSetPos = 0; }
	~DataSetHeaderWriter() {}

	void Write(std::ofstream &os, DataSetHeader &dc);

	void UpdateDataOffset(std::ofstream &os, u_int32_t pos) const;

	void UpdateNextDataSetOffset(std::ofstream &os, u_int32_t pos) const;

private:

	void WriteDataOffset(std::ofstream &os, u_int32_t pos);

	void WriteNextDataSetOffset(std::ofstream &os, u_int32_t pos);

	void WriteName(std::ofstream &os, const DataSetHeader &dc) const;

	void WriteNameValCnt(std::ofstream &os, const DataSetHeader &dc) const;

	void WriteNameValParams(std::ofstream &os, DataSetHeader &dc);

	void WriteColumnTypes(std::ofstream &os, const DataSetHeader &dc) const;

	void WriteColumnCnt(std::ofstream &os, const DataSetHeader &dc) const;

	void WriteRowCnt(std::ofstream &os, const DataSetHeader &dc) const;
};

};

#endif // _DataSetHeaderWriter_HEADER_
