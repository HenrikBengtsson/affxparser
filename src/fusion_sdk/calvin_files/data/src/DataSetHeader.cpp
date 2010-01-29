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


#include "calvin_files/data/src/DataSetHeader.h"
//
#include "calvin_files/data/src/FileHeader.h"
//
#include <algorithm>
//

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
	ClearNameValueParameters();
	columnTypes.clear();
	headerStartFilePos = 0;
	dataStartFilePos = 0;
	nextSetFilePos = 0;
}

void DataSetHeader::ClearNameValueParameters()
{
	nameValParams.clear();
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
		result += GetColumnInfo(i).GetSize();
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

void DataSetHeader::GetNameValIterators(ParameterNameValueTypeIt &begin, ParameterNameValueTypeIt &end)
{
	begin = nameValParams.begin();
	end = nameValParams.end();
}

void DataSetHeader::AddColumn(const ColumnInfo& colInfo)
{
columnTypes.push_back(colInfo);
}

void DataSetHeader::AddIntColumn(const std::wstring& name)
{
	columnTypes.push_back(IntColumn(name));
}

void DataSetHeader::AddUIntColumn(const std::wstring& name)
{
	columnTypes.push_back(UIntColumn(name));
}

void DataSetHeader::AddShortColumn(const std::wstring& name)
{
	columnTypes.push_back(ShortColumn(name));
}

void DataSetHeader::AddUShortColumn(const std::wstring& name)
{
	columnTypes.push_back(UShortColumn(name));
}

void DataSetHeader::AddByteColumn(const std::wstring& name)
{
	columnTypes.push_back(ByteColumn(name));
}

void DataSetHeader::AddUByteColumn(const std::wstring& name)
{
	columnTypes.push_back(UByteColumn(name));
}

void DataSetHeader::AddFloatColumn(const std::wstring& name)
{
	columnTypes.push_back(FloatColumn(name));
}

void DataSetHeader::AddAsciiColumn(const std::wstring& name, int32_t len)
{
	columnTypes.push_back(ASCIIColumn(name, len));
}

void DataSetHeader::AddUnicodeColumn(const std::wstring& name, int32_t len)
{
	columnTypes.push_back(UnicodeColumn(name, len));
}

ColumnInfo DataSetHeader::GetColumnInfo(int32_t index) const
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
