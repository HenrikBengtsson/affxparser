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


#include "calvin_files/writers/src/GridControlFileWriter.h"
//
#include "calvin_files/data/src/FileHeader.h"
#include "calvin_files/parameter/src/Parameter.h"
#include "calvin_files/parsers/src/GridControlFileReader.h"
#include "calvin_files/writers/src/GenericFileWriter.h"
//

using namespace affymetrix_calvin_io;
using namespace std;
using namespace affymetrix_calvin_parameter;
using namespace affymetrix_grid_control;

/*
 * Initialize the members.
 */
GridControlFileWriter::GridControlFileWriter()
{
}

/*
 * Clean up any used memory.
 */
GridControlFileWriter::~GridControlFileWriter()
{
}

/*
 * Write the grid data to the GRC file.
 */
bool GridControlFileWriter::Write(const string &fileName, GridControlData &data)
{
	try
	{
		// Create a header object.
		FileHeader header;
		header.SetFilename(fileName);

		// Set the file type.
		GenericDataHeader *genericHeader = header.GetGenericDataHdr();
		genericHeader->SetFileId(GRD_FILE_TYPE_IDENTIFIER);
		genericHeader->SetFileTypeId(GRD_FILE_TYPE_IDENTIFIER);

		// Add the parameters
		ParameterNameValueType param;
		param.SetName(GRD_ROWS_PARAMETER_NAME);
		param.SetValueInt32(data.GetRows());
		genericHeader->AddNameValParam(param);
		param.SetName(GRD_COLUMNS_PARAMETER_NAME);
		param.SetValueInt32(data.GetColumns());
		genericHeader->AddNameValParam(param);

		// Cube objects
		DataGroupHeader dataGroupHeader;
		DataSetHeader dataSetHeader;

		// Create the data dataGroup.
		dataGroupHeader.SetName(GRD_FILE_COORDINATE_GROUP_NAME);

		// Create the B1 dataSet
		dataSetHeader.SetName(GRD_FILE_B1_SET_NAME);
		dataSetHeader.AddUShortColumn(L"");	// TBD: add column name
		dataSetHeader.AddUShortColumn(L"");
		dataSetHeader.SetRowCnt(data.GetNumB1Probes());
		dataGroupHeader.AddDataSetHdr(dataSetHeader);
		dataSetHeader.Clear();

		// Create the B2 dataSet
		dataSetHeader.SetName(GRD_FILE_B2_SET_NAME);
		dataSetHeader.AddUShortColumn(L"");
		dataSetHeader.AddUShortColumn(L"");
		dataSetHeader.SetRowCnt(data.GetNumB2Probes());
		dataGroupHeader.AddDataSetHdr(dataSetHeader);
		dataSetHeader.Clear();

		// Create the NS dataSet
		dataSetHeader.SetName(GRD_FILE_NS_SET_NAME);
		dataSetHeader.AddUShortColumn(L"");
		dataSetHeader.AddUShortColumn(L"");
		dataSetHeader.SetRowCnt(data.GetNumNSProbes());
		dataGroupHeader.AddDataSetHdr(dataSetHeader);
		dataSetHeader.Clear();

		// Add the dataGroup.
		header.AddDataGroupHdr(dataGroupHeader);

		// Write the file.
		GenericFileWriter writer(&header);

		// First the header.
		writer.WriteHeader();

		// Next the dataGroup header
		DataGroupWriter &dataGroupWriter = writer.GetDataGroupWriter(0);
		dataGroupWriter.WriteHeader();

		// Next the B1 dataSet
		DataSetWriter &dataSetWriter = dataGroupWriter.GetDataSetWriter(0);
		dataSetWriter.WriteHeader();
		int nProbes = data.GetNumB1Probes();
		u_int16_t coordVal;
		for (int i=0; i<nProbes; i++)
		{
			coordVal = data.GetB1(i).x;
			dataSetWriter.Write(coordVal);
			coordVal = data.GetB1(i).y;
			dataSetWriter.Write(coordVal);
		}
		dataSetWriter.UpdateNextDataSetOffset();


		// Next the B2 dataSet
		dataSetWriter = dataGroupWriter.GetDataSetWriter(1);
		dataSetWriter.WriteHeader();
		nProbes = data.GetNumB2Probes();
		for (int i=0; i<nProbes; i++)
		{
			coordVal = data.GetB2(i).x;
			dataSetWriter.Write(coordVal);
			coordVal = data.GetB2(i).y;
			dataSetWriter.Write(coordVal);
		}
		dataSetWriter.UpdateNextDataSetOffset();

		// Lastly the NS dataSet.
		dataSetWriter = dataGroupWriter.GetDataSetWriter(2);
		dataSetWriter.WriteHeader();
		nProbes = data.GetNumNSProbes();
		for (int i=0; i<nProbes; i++)
		{
			coordVal = data.GetNS(i).x;
			dataSetWriter.Write(coordVal);
			coordVal = data.GetNS(i).y;
			dataSetWriter.Write(coordVal);
		}
		dataSetWriter.UpdateNextDataSetOffset();

		// Close the dataGroup.
		dataGroupWriter.Close();
	}
	catch (...)
	{
		return false;
	}

	return true;
}
