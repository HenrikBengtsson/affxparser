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


#ifndef _GenericDataHeaderWriter_HEADER_
#define _GenericDataHeaderWriter_HEADER_

#include "calvin_files/data/src/GenericDataHeader.h"
//
#include <fstream>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

class GenericDataHeaderWriter
{

public:

	GenericDataHeaderWriter() {}
	~GenericDataHeaderWriter() {}

	void Write(std::ofstream &os, GenericDataHeader &g);

private:

	void WriteFileTypeId(std::ofstream &os, const GenericDataHeader &g) const;

	void WriteFileId(std::ofstream &os, const GenericDataHeader &g) const;

	void WriteFileCreationTime(std::ofstream &os, const GenericDataHeader &g) const;

	void WriteLocale(std::ofstream &os, const GenericDataHeader &g) const;

	void WriteNameValParamCnt(std::ofstream &os, const GenericDataHeader &dc) const;

	void WriteNameValParams(std::ofstream &os, GenericDataHeader &g);

	void WriteParentHdrCnt(std::ofstream &os, const GenericDataHeader &dc) const;

	void WriteParentHdrs(std::ofstream &os, GenericDataHeader &g);
};

}

#endif // _GenericDataHeaderWriter_HEADER_
