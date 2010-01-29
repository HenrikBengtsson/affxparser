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
#ifndef _DataGroupHeaderWriter_HEADER_
#define _DataGroupHeaderWriter_HEADER_

#include "calvin_files/data/src/DataGroupHeader.h"
//
#include <fstream>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

class DataGroupHeaderWriter
{

private:

    u_int32_t dataSetPos;
    u_int32_t nextDataGroupPos;

public:

    DataGroupHeaderWriter();
    ~DataGroupHeaderWriter() {}

    void Write(std::ofstream &os, const DataGroupHeader &dc);

    void UpdateDataSetPos(std::ofstream &os, u_int32_t pos) const;

    void UpdateNextDataGroupPos(std::ofstream &os, u_int32_t pos) const;

private:

    void WriteDataSetPos(std::ofstream &os, u_int32_t pos);

    void WriteNextDataGroupPos(std::ofstream &os, u_int32_t pos);

    void WriteName(std::ofstream &os, const DataGroupHeader &dc) const;

    void WriteDataSetCnt(std::ofstream &os, const DataGroupHeader &dc) const;
};

}

#endif // _DataGroupHeaderWriter_HEADER_
