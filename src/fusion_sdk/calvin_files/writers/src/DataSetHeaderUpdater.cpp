////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006 Affymetrix, Inc.
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


#include "calvin_files/writers/src/DataSetHeaderUpdater.h"
//
#include "calvin_files/writers/src/FileOutput.h"
//

using namespace std;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;

#define WCHAR_T_SIZE 2

/*
 * Constructor
 */

DataSetHeaderUpdater::DataSetHeaderUpdater(std::ofstream& fileStream, DataSetHeader &hdr)
{
	os = &fileStream;
	setHdr = &hdr;

	filePos = hdr.GetHeaderStartFilePos();
	filePos += sizeof(u_int32_t); // data position
	filePos += sizeof(u_int32_t); // next data set position
	filePos += sizeof(int32_t); // name length
	filePos += (u_int32_t)(WCHAR_T_SIZE*hdr.GetName().length()); // data set name
	filePos += sizeof(u_int32_t); // #parameters
}

/*
* Destructor
*/
DataSetHeaderUpdater::~DataSetHeaderUpdater()
{
}

/*
* Update the parameter list.
*/
bool DataSetHeaderUpdater::UpdateParameter(ParameterNameValueType &nvt)
{
	u_int32_t pos = filePos;
	ParameterNameValueType param;
	ParameterNameValueTypeConstIt b;
	ParameterNameValueTypeConstIt e;
	setHdr->GetNameValIterators(b, e);
	while (b != e)
	{
		param = *b;

		pos += sizeof(int32_t) + (int32_t)param.GetName().length()*WCHAR_T_SIZE;

		// Check that the types match and the values are the same size
		MIMEValue paramMIME = param.GetMIMEValue();
		MIMEValue nvtMINE = nvt.GetMIMEValue();
		if (nvt.GetName() == param.GetName() &&
			nvt.GetParameterType() == param.GetParameterType() &&
			nvtMINE.Size() == paramMIME.Size())
		{
			os->seekp(pos, std::ios::beg);
			u_int32_t sz;
			const void* ptr = nvtMINE.GetValue(sz);
			FileOutput::WriteBlob(*os, ptr, sz);
			return true;
		}

		pos += sizeof(int32_t) + paramMIME.Size();
		pos += sizeof(int32_t) + (int32_t)param.GetMIMEType().size()*WCHAR_T_SIZE;
		++b;
	}
	return false;
}

void DataSetHeaderUpdater::SeekNextDataSetPosition()
{
	u_int32_t pos = setHdr->GetHeaderStartFilePos();
	pos += sizeof(u_int32_t); // data position
	os->seekp(pos, std::ios::beg);
}

void DataSetHeaderUpdater::UpdateNextDataSetPosition(u_int32_t position)
{
	u_int32_t pos = os->tellp();
	SeekNextDataSetPosition();
	FileOutput::WriteUInt32(*os, position);
	os->seekp(pos, std::ios::beg);
}
