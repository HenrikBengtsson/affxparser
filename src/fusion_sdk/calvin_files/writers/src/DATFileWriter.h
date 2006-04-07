#ifndef _DATFileWriter_HEADER_
#define _DATFileWriter_HEADER_

#include <fstream>
#include "DATData.h"
#include "GenericFileWriter.h"

#ifdef WIN32
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
