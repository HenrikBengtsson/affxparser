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
#ifndef _CelFileWriter_HEADER_
#define _CelFileWriter_HEADER_

#include "calvin_files/data/src/CELData.h"
#include "calvin_files/writers/src/GenericFileWriter.h"
//
#include <fstream>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

class CelFileWriter
{

private:

	GenericFileWriter* writer;
	DataSetWriter* dataSetWriter;
	int32_t intensityPos;
	int32_t stdDevPos;
	int32_t pixelPos;
	int32_t outlierPos;
	int32_t maskPos;
	int32_t intensityLimitPos;
	int32_t stdDevLimitPos;
	int32_t pixelLimitPos;
	int32_t outlierLimitPos;
	int32_t maskLimitPos;

public:

	CelFileWriter(CelFileData &p);
	~CelFileWriter();

	void WriteIntensities(const FloatVector &v);

	void WriteStdDevs(const FloatVector &v);

	void WritePixels(const Int16Vector &v);

	void WriteOutlierCoords(const XYCoordVector &coords);

	void WriteMaskCoords(const XYCoordVector &coords);

private:

	void WriteHeaders();

	void SetFilePositions();

};

}

#endif // _CelFileWriter_HEADER_
