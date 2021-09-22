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


#include "calvin_files/parsers/src/DATFileReader.h"
//
#include "calvin_files/parsers/src/GenericFileReader.h"
//

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_utilities;
using namespace affymetrix_calvin_exceptions;

/*
 * Constructor
 */
DATFileReader::DATFileReader()
{
}

/*
 * Destructor
 */
DATFileReader::~DATFileReader()
{
}

/*
 */
void DATFileReader::Read(DATData& data)
{
	data.Clear();
	GenericFileReader reader;
	if (fileName.empty())
	{
		fileName = data.GetFilename();
	}
	reader.SetFilename(fileName);
	reader.Open(data.GetGenericData());
	DataGroupReader dcReader = reader.GetDataGroupReader(0);
	ReadGlobalGrid(data, dcReader);
	ReadSubgrids(data, dcReader);
	reader.Close();
}

/*
 * Read the global grid
 */
void DATFileReader::ReadGlobalGrid(DATData& data, DataGroupReader& dcReader)
{
	try
	{
		FRegion rgn;
		DataSetReader dpReader = dcReader.GetDataSetReader(DAT_GLOBAL_GRID);
		int32_t cols = dpReader.GetDataSetHeader().GetColumnCnt();

		u_int32_t gridStatus = 0;
		if (cols > 0)
		{
			dpReader.Read(gridStatus);
		}

		// coordinates are in floats, assume there is only one row
		FPoint value;
		for (int32_t i = 1; i < cols; i+=2)
		{
			dpReader.Read(value.x);
			dpReader.Read(value.y);
			rgn.pts.push_back(value);
		}

		data.SetGlobalGrid(gridStatus, rgn);

		ReadGridAlignmentAlgorithmParameters(data, dpReader.GetDataSetHeader());
	}
	catch(DataSetNotFoundException& e)
	{	// ignore
	}
}

/*
 * Read subgrids
 */
void DATFileReader::ReadSubgrids(DATData& data, DataGroupReader& dcReader)
{
	try
	{
		FRegion rgn;
		DataSetReader dpReader = dcReader.GetDataSetReader(DAT_SUBGRID);
		int32_t rows = dpReader.GetDataSetHeader().GetRowCnt();
		int32_t cols = dpReader.GetDataSetHeader().GetColumnCnt();

		// coordinates are in floats, assume there is only one row
		FPoint value;
		for (int32_t n = 0; n < rows; n++)
		{
			u_int32_t gridStatus = 0;
			if (cols > 0)
			{
				dpReader.Read(gridStatus);
			}

			for (int32_t i = 1; i < cols; i+=2)
			{
				dpReader.Read(value.x);
				dpReader.Read(value.y);
				rgn.pts.push_back(value);
			}
			data.AddSubgrid(gridStatus, rgn);
			rgn.Clear();
		}
	}
	catch(DataSetNotFoundException& e)
	{	// ignore
	}
}

/*
 * Read the grid alignment algorithm parameters.
 */
void DATFileReader::ReadGridAlignmentAlgorithmParameters(DATData& data, const DataSetHeader& dsh)
{
	data.ClearGridAlignmentAlgorithmParameters();

	ParameterNameValueTypeConstIt begin, end;
	dsh.GetNameValIterators(begin, end);

	for (ParameterNameValueTypeConstIt ii = begin; ii != end; ++ii)
	{
		data.AddGridAlignmentAlgorithmParameter(*ii);
	}
}
