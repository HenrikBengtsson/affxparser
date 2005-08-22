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

#include "ParameterNameValueType.h"
#include "StringUtils.h"

#ifdef WIN32
#pragma warning(disable: 4996) // don't show deprecated warnings.
#include <winsock2.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#endif

/*! Custom MIME types */
const wchar_t Int8MIMEType[] = L"text/x-calvin-integer-8";
const wchar_t UInt8MIMEType[] = L"text/x-calvin-unsigned-integer-8";
const wchar_t Int16MIMEType[] = L"text/x-calvin-integer-16";
const wchar_t UInt16MIMEType[] = L"text/x-calvin-unsigned-integer-16";
const wchar_t Int32MIMEType[] = L"text/x-calvin-integer-32";
const wchar_t UInt32MIMEType[] = L"text/x-calvin-unsigned-integer-32";
const wchar_t FloatMIMEType[] = L"text/x-calvin-float";
const wchar_t TextMIMEType[] = L"text/plain";
const wchar_t AsciiMIMEType[] = L"text/ascii";

const u_int32_t NUMBER_BUFFER_LEN = 16;

/*! Type table.  Order matches the ParameterType enum */
static const wchar_t* TypeTable[] = {
	Int8MIMEType,
	UInt8MIMEType,
	Int16MIMEType,
	UInt16MIMEType,
	Int32MIMEType,
	UInt32MIMEType,
	FloatMIMEType,
	TextMIMEType,
	AsciiMIMEType
};

using namespace affymetrix_calvin_parameter;

MIMEValue::MIMEValue()
{
	arr = 0;
	arrSize = 0;
}

MIMEValue::MIMEValue(const void* value, u_int32_t size)
{
	arr = new char[size];
	memcpy(arr, value, size);
	arrSize = size;
}

MIMEValue::MIMEValue(const MIMEValue& source)
{
	arr = 0;
	*this = source;
}

MIMEValue::~MIMEValue()
{
	delete [] arr;
}

MIMEValue& MIMEValue::operator=(const MIMEValue& source)
{
	if (&source != this)
	{
		delete [] arr;
		arr = new char[source.arrSize];
		memcpy(arr, source.arr, source.arrSize);
		arrSize = source.arrSize;
	}
	return *this;
}

bool MIMEValue::operator==(const MIMEValue& lhs)
{
	if (lhs.arrSize != arrSize)
		return false;
	return (memcmp(lhs.arr, arr, arrSize) == 0);
}

bool MIMEValue::operator!=(const MIMEValue& lhs)
{
	return !(*this==lhs);
}

void MIMEValue::SetValue(const void* value, u_int32_t size)
{
	delete [] arr;
	arr = new char[size];
	memset(arr, 0, size);
	memcpy(arr, value, size);
	arrSize = size;
}

const void* MIMEValue::GetValue(u_int32_t& size) const
{
	size = arrSize;
	return arr;
}

/*
 * MIME constructor.  Useful when reading in from a file. No interpretation attempted.
 */
ParameterNameValueType::ParameterNameValueType(const std::wstring& name, const void* mimeValue, int32_t mimeValueSize, const std::wstring& mimeType)
: Name(name), Value(mimeValue, mimeValueSize), Type(mimeType)
{
}

/*
 * MIME constructor.  Useful when reading in from a file. No interpretation attempted.
 */
ParameterNameValueType::ParameterNameValueType(const std::wstring& name, const MIMEValue& mimeValue, const std::wstring& mimeType)
: Name(name), Value(mimeValue), Type(mimeType)
{
}

/*
 * Get the parameter type
 */
ParameterNameValueType::ParameterType ParameterNameValueType::GetParameterType() const
{
	for (int i=0; i<UnknownType; ++i)
	{
		if (Type == TypeTable[i])
			return (ParameterType)i;
	}
	return UnknownType;
}

/*
 * Get the parameter type
 */

int8_t ParameterNameValueType::GetValueInt8() const
{
	return (int8_t)ValueToInt(Int8MIMEType);
}

void ParameterNameValueType::SetValueInt8(int8_t value)
{
	IntToValue(value, Int8MIMEType);
}

u_int8_t ParameterNameValueType::GetValueUInt8() const
{
	return (u_int8_t)ValueToInt(UInt8MIMEType);
}

void ParameterNameValueType::SetValueUInt8(u_int8_t value)
{
	IntToValue(value, UInt8MIMEType);
}

int16_t ParameterNameValueType::GetValueInt16() const
{
	return (int16_t)ValueToInt(Int16MIMEType);
}

void ParameterNameValueType::SetValueInt16(int16_t value)
{
	IntToValue(value, Int16MIMEType);
}

u_int16_t ParameterNameValueType::GetValueUInt16() const
{
	return (u_int16_t)ValueToInt(UInt16MIMEType);
}

void ParameterNameValueType::SetValueUInt16(u_int16_t value)
{
	IntToValue(value, UInt16MIMEType);
}

int32_t ParameterNameValueType::GetValueInt32() const
{
	return (int32_t)ValueToInt(Int32MIMEType);
}

void ParameterNameValueType::SetValueInt32(int32_t value)
{
	IntToValue(value, Int32MIMEType);
}

u_int32_t ParameterNameValueType::GetValueUInt32() const
{
	return (u_int32_t)ValueToInt(UInt32MIMEType);
}

void ParameterNameValueType::SetValueUInt32(u_int32_t value)
{
	IntToValue(value, UInt32MIMEType);
}

float ParameterNameValueType::GetValueFloat() const
{
	if (Type == FloatMIMEType)
	{
		int32_t ival = ValueToInt(FloatMIMEType);
		float value = 0;
		memcpy(&value, &ival, sizeof(int32_t));
		return value;
	}
	else
	{
		affymetrix_calvin_exceptions::ParameterMismatchException e;
		throw e;
	}
}

void ParameterNameValueType::SetValueFloat(float value)
{
	IntToValue(*(int32_t *)&value, FloatMIMEType);
}

std::wstring ParameterNameValueType::GetValueText() const
{
	if (Type == TextMIMEType)
	{
		u_int32_t size = 0;
		const u_int16_t* v = (const u_int16_t*)Value.GetValue(size);
		u_int32_t len = size / sizeof(u_int16_t);
		std::wstring value;
		value.resize(len);
		
		for (u_int32_t i = 0; i < len; ++i)
		{
			value[i] = ntohs(v[i]);
		}
		return value;
	}
	else
	{
		affymetrix_calvin_exceptions::ParameterMismatchException e;
		throw e;
	}
}

void ParameterNameValueType::SetValueText(const std::wstring &value)
{
	Type = TextMIMEType;

	u_int32_t len = (u_int32_t) value.length();
	u_int16_t* buf = new u_int16_t[len];
	for (u_int32_t i=0; i<len; ++i)
	{
		buf[i] = (u_int16_t) value[i];
		buf[i] = htons(buf[i]);
	}
	Value.SetValue(buf, len*sizeof(u_int16_t));
	delete [] buf;
}

std::string ParameterNameValueType::GetValueAscii() const
{
	if (Type == AsciiMIMEType)
	{
		u_int32_t size = 0;
		const char* v = (const char*)Value.GetValue(size);
		std::string value;
		value.resize(size);
		for (u_int32_t i=0; i<size; i++)
			value[i] = v[i];
		return value;
	}
	else
	{
		affymetrix_calvin_exceptions::ParameterMismatchException e;
		throw e;
	}
}

void ParameterNameValueType::SetValueAscii(const std::string &value)
{
	Type = AsciiMIMEType;
	const char *buf = value.c_str();
	u_int32_t len = (u_int32_t) value.length();
	Value.SetValue(buf, len);
}

u_int32_t ParameterNameValueType::ValueToInt(const wchar_t* mimeType) const
{
	if (Type == mimeType)
	{
		u_int32_t size = 0;
		const void* v = Value.GetValue(size);
		u_int32_t value = 0;
		memcpy(&value, v, sizeof(u_int32_t));
		value = ntohl(value);
		return value;
	}
	else
	{
		affymetrix_calvin_exceptions::ParameterMismatchException e;
		throw e;
	}
}

void ParameterNameValueType::IntToValue(u_int32_t value, const wchar_t* type)
{
	Type = type;
/*
	wchar_t buf[NUMBER_LEN+1];
	swprintf(buf, NUMBER_LEN, L"%29u", value);
	Value = buf;
*/
	char buf[NUMBER_BUFFER_LEN];
	memset(buf, 0, NUMBER_BUFFER_LEN);	// for aesthetics
	value = htonl(value);
	memcpy(buf, &value, sizeof(u_int32_t));
	Value.SetValue(buf, NUMBER_BUFFER_LEN);
}

std::wstring ParameterNameValueType::ToString()
{
	const int32_t bufSize = 100;
	wchar_t buf[bufSize]; // a googol, room for to count more things than there are in the universe.
	std::wstring result;
	switch(GetParameterType())
	{
		case Int8Type:
			swprintf(buf, bufSize, L"%d", GetValueInt8());
			result = buf;
			break;
		case Int16Type:
			swprintf(buf, bufSize, L"%d", GetValueInt16());
			result = buf;
			break;
		case Int32Type:
			swprintf(buf, bufSize, L"%d", GetValueInt32());
			result = buf;
			break;
		case UInt8Type:
			swprintf(buf, bufSize, L"%u", GetValueUInt8());
			result = buf;
			break;
		case UInt16Type:
			swprintf(buf, bufSize, L"%u", GetValueUInt16());
			result = buf;
			break;
		case UInt32Type:
			swprintf(buf, bufSize, L"%u", GetValueUInt32());
			result = buf;
			break;
		case FloatType:
			swprintf(buf, bufSize, L"%f", GetValueFloat());
			result = buf;
			break;
		case TextType:
			result = GetValueText();
			break;
		case AsciiType:
			result = StringUtils::ConvertMBSToWCS(GetValueAscii());
			break;
	}
	return result;
}
