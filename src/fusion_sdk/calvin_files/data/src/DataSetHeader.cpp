/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License,
// or (at your option) any later version.
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
/////////////////////////////////////////////////////////////////

#include "DataSetHeader.h"
#include "FileHeader.h"
#include <algorithm>

using namespace affymetrix_calvin_io;

DataSetHeader::DataSetHeader() 
{
	Clear();
}

DataSetHeader::~DataSetHeader() 
{ 
	Clear(); 
}

void DataSetHeader::Clear()
{
	rowCount = 0;
	name.clear();
	nameValParams.clear();
	columnTypes.clear();
	headerStartFilePos = 0;
	dataStartFilePos = 0;
}

int32_t DataSetHeader::GetDataSize() const
{
	return GetRowSize() * rowCount;
}

int32_t DataSetHeader::GetRowSize() const
{
	u_int32_t result = 0;
	int32_t sz = GetColumnCnt();
	for(int i = 0; i < sz; i++)
	{
		result += GetColumnType(i).GetSize();
	}
	return result;
}

void DataSetHeader::SetName(const std::wstring &p)
{
	name = p;
}

std::wstring DataSetHeader::GetName() const
{
	return name;
}

int32_t DataSetHeader::GetNameValParamCnt() const
{
	return (int32_t)nameValParams.size();
}

void DataSetHeader::AddNameValParam(const ParameterNameValueType &p)
{
	nameValParams.push_back(p);
}

void DataSetHeader::GetNameValIterators(ParameterNameValueTypeConstIt &begin, ParameterNameValueTypeConstIt &end) const
{
	begin = nameValParams.begin();
	end = nameValParams.end();
}

void DataSetHeader::AddColumnType(const ColumnType& colType)
{
columnTypes.push_back(colType);
}

void DataSetHeader::AddIntColumnType()
{
	columnTypes.push_back(IntColumnType());
}

void DataSetHeader::AddUIntColumnType()
{
	columnTypes.push_back(UIntColumnType());
}

void DataSetHeader::AddShortColumnType()
{
	columnTypes.push_back(ShortColumnType());
}

void DataSetHeader::AddUShortColumnType()
{
	columnTypes.push_back(UShortColumnType());
}

void DataSetHeader::AddByteColumnType()
{
	columnTypes.push_back(ByteColumnType());
}

void DataSetHeader::AddUByteColumnType()
{
	columnTypes.push_back(UByteColumnType());
}

void DataSetHeader::AddFloatColumnType()
{
	columnTypes.push_back(FloatColumnType());
}

void DataSetHeader::AddAsciiColumnType(int32_t len)
{
	columnTypes.push_back(ASCIIColumnType(len));
}

void DataSetHeader::AddUnicodeColumnType(int32_t len)
{
	columnTypes.push_back(UnicodeColumnType(len));
}

ColumnType DataSetHeader::GetColumnType(int32_t index) const
{  
	return columnTypes[index];
}

int32_t DataSetHeader::GetRowCnt() const
{
	return rowCount;
}

void DataSetHeader::SetRowCnt(int32_t p)
{
	rowCount = p;
}

int32_t DataSetHeader::GetColumnCnt() const
{
	return (int32_t)columnTypes.size();
}

bool DataSetHeader::FindNameValParam(const std::wstring& name, ParameterNameValueType& result) const
{
	ParameterNameValueType t;
	t.SetName(name);
	t.SetValueText(L"");
	ParameterNameValueTypeConstIt found = FindNameValParam(t);
	if (found != nameValParams.end())
	{
		result = *found;
		return true;
	}
	return false;
}

ParameterNameValueTypeConstIt DataSetHeader::FindNameValParam(const ParameterNameValueType& p) const
{
	ParameterNameValueTypeConstIt begin, end;
	begin = nameValParams.begin();
	end = nameValParams.end();

	ParameterNameValueTypeConstIt ii = std::find(begin, end, p);
	if (ii != end)
	{
		return ii;
	}
	else
		return end;
}