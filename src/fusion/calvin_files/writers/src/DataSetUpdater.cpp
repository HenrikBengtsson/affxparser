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


#include "calvin_files/writers/src/DataSetUpdater.h"
//
#include "calvin_files/data/src/GenericData.h"
#include "calvin_files/parsers/src/GenericFileReader.h"
#include "calvin_files/writers/src/FileOutput.h"
//

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_exceptions;
using namespace std;

/*
 * Initialize any needed members.
 */
DataSetUpdater::DataSetUpdater()
{
}

/*
 * Clean up.
 */
DataSetUpdater::~DataSetUpdater()
{
}

/*
 * Read the data file and cache the data headers.
 */
void DataSetUpdater::Initialize(const char *file)
{
	fileName = file;
	GenericData data;
	GenericFileReader reader;
	reader.SetFilename(fileName);
	reader.Open(data);

	int ng = data.Header().GetDataGroupCnt();
	positions.resize(ng);
	rowsizes.resize(ng);
	colsizes.resize(ng);
    dataSetNames.resize(ng);
	for (int ig=0; ig<ng; ig++)
	{
		DataGroupHeader &dgHdr = data.Header().GetDataGroup(ig);
		int ns = dgHdr.GetDataSetCnt();
		positions[ig].resize(ns);
		rowsizes[ig].resize(ns);
		colsizes[ig].resize(ns);
        dataSetNames[ig].resize(ns);
		for (int is=0; is<ns; is++)
		{
			DataSetHeader &dsHdr = dgHdr.GetDataSet(is);
			positions[ig][is] = dsHdr.GetDataStartFilePos();
			rowsizes[ig][is] = dsHdr.GetRowSize();
            dataSetNames[ig][is] = dsHdr.GetName();

			int nc = dsHdr.GetColumnCnt();
			colsizes[ig][is].resize(nc);
			for (int ic=0; ic<nc; ic++)
				colsizes[ig][is][ic] = dsHdr.GetColumnInfo(ic).GetSize();
		}
	}
	reader.Close();
}

void DataSetUpdater::SeekToPosition(ofstream &os, int groupIndex, int setIndex, int row, int col)
{
	int pos = positions[groupIndex][setIndex] + (row*rowsizes[groupIndex][setIndex]);
	for (int i=0; i<col; i++)
	{
		pos += colsizes[groupIndex][setIndex][i];
	}
	os.seekp(pos, std::ios::beg);
}

/*
 * Seek to the position and update the value.
 */
void DataSetUpdater::Update(int groupIndex, int setIndex, int row, int col, const string &value)
{
	ofstream os(fileName.c_str(), ios::out | ios::binary | ios::in);
	SeekToPosition(os, groupIndex, setIndex, row, col);
	FileOutput::WriteString8(os, value);
	os.close();
}

/*
 * Seek to the position and update the value.
 */
void DataSetUpdater::Update(int groupIndex, int setIndex, int row, int col, const wstring &value)
{
	ofstream os(fileName.c_str(), ios::out | ios::binary | ios::in);
	SeekToPosition(os, groupIndex, setIndex, row, col);
	FileOutput::WriteString16(os, value);
	os.close();
}

/*
 * Seek to the position and update the value.
 */
void DataSetUpdater::Update(int groupIndex, int setIndex, int row, int col, int8_t value)
{
	ofstream os(fileName.c_str(), ios::out | ios::binary | ios::in);
	SeekToPosition(os, groupIndex, setIndex, row, col);
	FileOutput::WriteInt8(os, value);
	os.close();
}

/*
 * Seek to the position and update the value.
 */
void DataSetUpdater::Update(int groupIndex, int setIndex, int row, int col, int16_t value)
{
	ofstream os(fileName.c_str(), ios::out | ios::binary | ios::in);
	SeekToPosition(os, groupIndex, setIndex, row, col);
	FileOutput::WriteInt16(os, value);
	os.close();
}

/*
 * Seek to the position and update the value.
 */
void DataSetUpdater::Update(int groupIndex, int setIndex, int row, int col, int32_t value)
{
	ofstream os(fileName.c_str(), ios::out | ios::binary | ios::in);
	SeekToPosition(os, groupIndex, setIndex, row, col);
	FileOutput::WriteInt32(os, value);
	os.close();
}

/*
 * Seek to the position and update the value.
 */
void DataSetUpdater::Update(int groupIndex, int setIndex, int row, int col, u_int8_t value)
{
	ofstream os(fileName.c_str(), ios::out | ios::binary | ios::in);
	SeekToPosition(os, groupIndex, setIndex, row, col);
	FileOutput::WriteUInt8(os, value);
	os.close();
}

/*
 * Seek to the position and update the value.
 */
void DataSetUpdater::Update(int groupIndex, int setIndex, int row, int col, u_int16_t value)
{
	ofstream os(fileName.c_str(), ios::out | ios::binary | ios::in);
	SeekToPosition(os, groupIndex, setIndex, row, col);
	FileOutput::WriteUInt16(os, value);
	os.close();
}

/*
 * Seek to the position and update the value.
 */
void DataSetUpdater::Update(int groupIndex, int setIndex, int row, int col, u_int32_t value)
{
	ofstream os(fileName.c_str(), ios::out | ios::binary | ios::in);
	SeekToPosition(os, groupIndex, setIndex, row, col);
	FileOutput::WriteUInt32(os, value);
	os.close();
}

/*
 * Seek to the position and update the value.
 */
void DataSetUpdater::Update(int groupIndex, int setIndex, int row, int col, float value)
{
	ofstream os(fileName.c_str(), ios::out | ios::binary | ios::in);
	SeekToPosition(os, groupIndex, setIndex, row, col);
	FileOutput::WriteFloat(os, value);
	os.close();
}
