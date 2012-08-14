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


#include "calvin_files/parsers/src/GridControlFileReader.h"
//
#include "calvin_files/parameter/src/Parameter.h"
#include "calvin_files/parsers/src/GenericFileReader.h"
//
#include "file/GridControlData.h"
//

using namespace affymetrix_calvin_io;
using namespace affymetrix_grid_control;
using namespace affymetrix_calvin_parameter;

/*
 * Constructor
 */
GridControlFileReader::GridControlFileReader()
{
}

/*
 * Destructor
 */
GridControlFileReader::~GridControlFileReader()
{
}

/*
 * Read the file contents into the data object.
 */
void GridControlFileReader::Read(const std::string &fileName, GridControlData& data)
{
	// Clear the old data and read the file.
	data.Clear();
	GenericFileReader reader;
	GenericData gdata;
	reader.SetFilename(fileName);
	reader.ReadHeader(gdata);

	// Check the file identifier
	if (gdata.FileIdentifier() != GRD_FILE_TYPE_IDENTIFIER)
	{
		affymetrix_calvin_exceptions::InvalidFileTypeException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}

	// Get the header parameters.
	ParameterNameValueType param;
	gdata.Header().GetGenericDataHdr()->FindNameValParam(GRD_ROWS_PARAMETER_NAME, param);
	data.SetRows(param.GetValueInt32());
	gdata.Header().GetGenericDataHdr()->FindNameValParam(GRD_COLUMNS_PARAMETER_NAME, param);
	data.SetColumns(param.GetValueInt32());

	// Get the XY coordinates from the dataSets.
	FeatureCoordinate coord;
	u_int16_t x;
	u_int16_t y;
	int nRows;
	DataSet *dataSet;

	// First the B1 probes.
	dataSet = gdata.DataSet(GRD_FILE_COORDINATE_GROUP_NAME, GRD_FILE_B1_SET_NAME);
	if (dataSet->Open() == false)
	{
		affymetrix_calvin_exceptions::DataSetNotOpenException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
	nRows = dataSet->Rows();
	data.ResizeB1(nRows);
	for (int iRow=0; iRow<nRows; iRow++)
	{
		dataSet->GetData(iRow, 0, x);
		dataSet->GetData(iRow, 1, y);
		coord.x = x;
		coord.y = y;
		data.SetB1(iRow, coord);
	}
	dataSet->Close();
	dataSet->Delete();

	// Next the B2 probes.
	dataSet = gdata.DataSet(GRD_FILE_COORDINATE_GROUP_NAME, GRD_FILE_B2_SET_NAME);
	if (dataSet->Open() == false)
	{
		affymetrix_calvin_exceptions::DataSetNotOpenException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
	nRows = dataSet->Rows();
	data.ResizeB2(nRows);
	for (int iRow=0; iRow<nRows; iRow++)
	{
		dataSet->GetData(iRow, 0, x);
		dataSet->GetData(iRow, 1, y);
		coord.x = x;
		coord.y = y;
		data.SetB2(iRow, coord);
	}
	dataSet->Close();
	dataSet->Delete();

	// Last the NS probes.
	dataSet = gdata.DataSet(GRD_FILE_COORDINATE_GROUP_NAME, GRD_FILE_NS_SET_NAME);
	if (dataSet->Open() == false)
	{
		affymetrix_calvin_exceptions::DataSetNotOpenException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
	nRows = dataSet->Rows();
	data.ResizeNS(nRows);
	for (int iRow=0; iRow<nRows; iRow++)
	{
		dataSet->GetData(iRow, 0, x);
		dataSet->GetData(iRow, 1, y);
		coord.x = x;
		coord.y = y;
		data.SetNS(iRow, coord);
	}
	dataSet->Close();
	dataSet->Delete();
}
