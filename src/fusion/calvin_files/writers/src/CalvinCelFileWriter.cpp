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
#include "calvin_files/writers/src/CalvinCelFileWriter.h"
//
#include "calvin_files/data/src/CELData.h"
//

using namespace affymetrix_calvin_io;

CelFileWriter::CelFileWriter(CelFileData &p) 
{
	p.SetVersion(CurrentCelFileVersion);
	writer = new GenericFileWriter(p.GetFileHeader());
	intensityPos = 0;
	stdDevPos = 0;
	pixelPos = 0;
	outlierPos = 0;
	maskPos = 0;
	WriteHeaders();
}

CelFileWriter::~CelFileWriter() 
{
	delete writer;
}

void CelFileWriter::WriteHeaders()
{
	writer->WriteHeader();
	DataGroupWriter* dataGroupWriter = &writer->GetDataGroupWriter(0);
	dataGroupWriter->WriteHeader();

	DataSetWriterIt begin;
	DataSetWriterIt end;
	dataGroupWriter->GetDataSetWriters(begin, end);
	while(begin != end)
	{
		dataSetWriter = &(*begin);
		begin->WriteHeader();
		SetFilePositions();
		begin++;
	}
}

void CelFileWriter::WriteIntensities(const FloatVector& v)
{
	writer->SeekFromBeginPos(intensityPos);
	FloatVectorConstIt begin = v.begin();
	FloatVectorConstIt end = v.end();
	while(begin != end)
	{
		dataSetWriter->Write(*begin);
		begin++;
	}
	intensityPos = writer->GetFilePos();
}

void CelFileWriter::WriteStdDevs(const FloatVector& v)
{
	writer->SeekFromBeginPos(stdDevPos);
	FloatVectorConstIt begin = v.begin();
	FloatVectorConstIt end = v.end();
	while(begin != end)
	{
		dataSetWriter->Write(*begin);
		begin++;
	}
	stdDevPos = writer->GetFilePos();
}

void CelFileWriter::WritePixels(const Int16Vector &v)
{
	writer->SeekFromBeginPos(pixelPos);
	Int16VectorConstIt begin = v.begin();
	Int16VectorConstIt end = v.end();
	while(begin != end)
	{
		dataSetWriter->Write(*begin);
		begin++;
	}
	pixelPos = writer->GetFilePos();
}

void CelFileWriter::WriteOutlierCoords(const XYCoordVector &v)
{
	writer->SeekFromBeginPos(outlierPos);
	XYCoordConstIt begin = v.begin();
	XYCoordConstIt end = v.end();
	while(begin != end)
	{
		dataSetWriter->Write(begin->xCoord);
		dataSetWriter->Write(begin->yCoord);
		begin++;
	}
	outlierPos = writer->GetFilePos();
}

void CelFileWriter::WriteMaskCoords(const XYCoordVector &v)
{
	writer->SeekFromBeginPos(maskPos);
	XYCoordConstIt begin = v.begin();
	XYCoordConstIt end = v.end();
	while(begin != end)
	{
		dataSetWriter->Write(begin->xCoord);
		dataSetWriter->Write(begin->yCoord);
		begin++;
	}
	maskPos = writer->GetFilePos();
}

void CelFileWriter::SetFilePositions()
{
	const std::wstring &name = dataSetWriter->GetDataSetName();
	int32_t dataSetSz = dataSetWriter->GetDataSetSize();
	
	if(name == CelIntensityLabel)
	{
		intensityPos = writer->GetFilePos();
		intensityLimitPos = intensityPos + dataSetSz;
	}
	else if(name == CelStdDevLabel)
	{
		stdDevPos = writer->GetFilePos();
		stdDevLimitPos = stdDevPos + dataSetSz;
	}
	else if(name == CelPixelLabel)
	{
		pixelPos = writer->GetFilePos();
		pixelLimitPos = pixelPos + dataSetSz;
	}
	else if(name == CelOutlierLabel)
	{
		outlierPos = writer->GetFilePos();
		outlierLimitPos = outlierPos + dataSetSz;
	}
	else
	{
		maskPos = writer->GetFilePos();
		maskLimitPos = maskPos + dataSetSz;
	}

	writer->SeekFromCurrentPos(dataSetSz + 1);
	dataSetWriter->UpdateNextDataSetOffset();
}
