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


#include "calvin_files/writers/src/MMDATFileWriter.h"
//
#include "calvin_files/data/src/DATData.h"
//


/* General Description of the file writing process
 * The MMDATFileWriter extends the DATFileWriter by adding memory-mapped view
 * to the pixel DataSet data.
 * The DATFileWriter constructor writes the FileHeader, GenericDataHeader, DataGroupHeader
 * and DataSetHeader using fstream.  The header positions are computed and written in the proper
 * place at construction.
 * The Open method opens a memory mapped view on the pixel DataSet data section.  Data can be
 * written to the file by changing the data in the memory mapped view.  The view can also 
 * be moved over the pixel data to access all the data.  The view size is restricted to 
 * 200 MB at a time.
 * Closing the file will also cause the pixel data to be written to the file in network byte-order.
 * Warning: Using the same DATData object to re-open the file with the MMDATFileWriter would make it
 * possible to access the pixel data that is not in host byte-order.  You do not want to do this.
 * Once the MMDATFileWriter is closed, the file should be opened with the DATFileReader to access
 * the data.  Re-opening a file with the MMDATFileWriter to overwrite an existing file is OK.
 */

using namespace affymetrix_calvin_io;

/*
 * Constructor
 */
MMDATFileWriter::MMDATFileWriter(DATData &p) : DATFileWriter(p)
{
	pixelWriter = 0;
	filename = p.GetFilename();
	dataSetHdr = p.GetFileHeader()->GetDataGroup(0).FindDataSetHeader(DAT_PIXEL);
	rows = p.GetRows();
	cols = p.GetCols();
}

/*
 * Destructor
 */
MMDATFileWriter::~MMDATFileWriter() 
{
	Close();
}

/*
 * Opens the writer.
 */
bool MMDATFileWriter::Open()
{
	if (pixelWriter == 0)
	{
		if (dataSetHdr != 0)
		{
			pixelWriter = new MMDataSetDataWriter(*dataSetHdr, filename);
		}
		else
			return false;
	}

	if (pixelWriter)
		return pixelWriter->Open();
	else
		return false;
}

/*
 * Closes the writer.
 */
bool MMDATFileWriter::Close()
{
	if (pixelWriter)
	{
		HTONPixelDataSet();
		bool ret = pixelWriter->Close();
		delete pixelWriter;
		pixelWriter = 0;
		return ret;
	}
	else
		return true;
}

/*
 * Maps the pixel data for access starting at firstRow and continuing for rowsToMap.
 */
void MMDATFileWriter::MapPixelData(int32_t firstRow, int32_t rowsToMap)
{
	if (pixelWriter)
	{
		pixelWriter->MapData(firstRow*cols, rowsToMap*cols);
	}
}

/*
 * Get a pointer to the pixel data that has been mapped for access.
 */
u_int16_t* MMDATFileWriter::GetMappedPixelDataPtr()
{
	if (pixelWriter)
	{
		return (u_int16_t*)pixelWriter->GetMappedDataPtr();
	}
	return 0;
}

/*
 * Get the first DAT image row that has been mapped.
 */
int32_t MMDATFileWriter::GetFirstPixelRowMapped()
{
	if (pixelWriter)
	{
		return pixelWriter->GetFirstRowMapped()/cols;
	}
	return 0;
}

/*
 * Get the number of DAT image rows that have been mapped.
 */
int32_t MMDATFileWriter::GetPixelRowsMapped()
{
	if (pixelWriter)
	{
		return pixelWriter->GetRowsMapped()/cols;
	}
	return 0;
}

/*
 * Get the suggested maximum number of DAT image rows to map.
 */
int32_t MMDATFileWriter::GetMaxPixelRowsToMap()
{
	if (pixelWriter)
	{
		return pixelWriter->GetMaxRowsToMap()/cols;
	}
	return 0;
}

/*
 * Writes the pixel DataSet data in network byte order.
 */
void MMDATFileWriter::HTONPixelDataSet()
{
	if (pixelWriter)
	{
		pixelWriter->Close();
		if (pixelWriter->Open())
		{
			bool stop = false;
			do
			{
				int32_t firstPixel = pixelWriter->GetFirstRowMapped();
				int32_t numPixelsMapped = pixelWriter->GetRowsMapped();
				u_int16_t* pixel = (u_int16_t*)pixelWriter->GetMappedDataPtr();
				for (int32_t i = 0; i < numPixelsMapped; ++i, ++pixel)
				{
					*pixel = htons(*pixel);
				}

				if (firstPixel+numPixelsMapped >= rows*cols)
					stop = true;
				else
					pixelWriter->MapData(firstPixel+numPixelsMapped, pixelWriter->GetMaxRowsToMap());
			} while (stop == false);
		}
	}
}
