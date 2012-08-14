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

#ifdef _MSC_VER
#include "windows.h"
#endif

#include "calvin_files/parameter/src/ParameterNameValueType.h"
//
#include "calvin_files/utils/src/StringUtils.h"
//
#include <cstring>
#include <string.h>
#include <string>
//

#ifdef _MSC_VER
#pragma warning(disable: 4996) // don't show deprecated warnings.
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
using namespace affymetrix_calvin_utilities;

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
  : Name(name), Type(mimeType), Value(mimeValue, mimeValueSize)
{
}

/*
 * MIME constructor.  Useful when reading in from a file. No interpretation attempted.
 */
ParameterNameValueType::ParameterNameValueType(const std::wstring& name, const MIMEValue& mimeValue, const std::wstring& mimeType)
  : Name(name), Type(mimeType), Value(mimeValue)
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
		affymetrix_calvin_exceptions::ParameterMismatchException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

void ParameterNameValueType::SetValueFloat(float value)
{
	type_punned pun;
	pun.v_float=value;
	IntToValue(pun.v_int32, FloatMIMEType);
}

std::wstring ParameterNameValueType::GetValueText() const
{
	if (Type == TextMIMEType)
	{
		u_int32_t size = 0;
		const u_int16_t* v = (const u_int16_t*)Value.GetValue(size);
		u_int32_t len = size / sizeof(u_int16_t);
		wchar_t* value = new wchar_t[len+1];
		
		for (u_int32_t i = 0; i < len; ++i)
		{
			value[i] = ntohs(v[i]);
		}
		value[len] = 0;
		std::wstring result = value;
		delete [] value;
		return result;
	}
	else
	{
		affymetrix_calvin_exceptions::ParameterMismatchException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

void ParameterNameValueType::SetValueText(const std::wstring &value, int32_t reserve)
{
	Type = TextMIMEType;

	u_int32_t len = (u_int32_t) value.length();
	u_int32_t maxLn = len;
	if (reserve != -1 && (u_int32_t)reserve > len)
		maxLn = (u_int32_t)reserve;

	u_int16_t* buf = new u_int16_t[maxLn];
	for (u_int32_t i=0; i<len; ++i)
	{
		buf[i] = (u_int16_t) value[i];
		buf[i] = htons(buf[i]);
	}
	for(u_int32_t i = len; i < maxLn; ++i)
		buf[i] = 0;

	Value.SetValue(buf, maxLn*sizeof(u_int16_t));
	delete [] buf;
}

std::string ParameterNameValueType::GetValueAscii() const
{
	if (Type == AsciiMIMEType)
	{
		u_int32_t size = 0;
		const char* v = (const char*)Value.GetValue(size);
		char* value = new char[size+1];
		for (u_int32_t i=0; i<size; i++)
			value[i] = v[i];
		value[size] = 0;
		std::string result = value;
		delete [] value;
		return result;
	}
	else
	{
		affymetrix_calvin_exceptions::ParameterMismatchException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

void ParameterNameValueType::SetValueAscii(const std::string &value, int32_t reserve)
{
	Type = AsciiMIMEType;

	u_int32_t len = (u_int32_t) value.length();

	if (reserve == -1 || (u_int32_t)reserve <=  len)
	{
		Value.SetValue(value.c_str(), len);
	}
	else
	{
		char* buf = new char[reserve];
		memcpy(buf, value.c_str(), len);
		for(int32_t i = len; i < reserve; ++i)
			buf[i] = 0;

		Value.SetValue(buf, reserve);
		delete [] buf;
	}
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
		affymetrix_calvin_exceptions::ParameterMismatchException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

void ParameterNameValueType::IntToValue(u_int32_t value, const wchar_t* type)
{
	Type = type;

	char buf[NUMBER_BUFFER_LEN];
	memset(buf, 0, NUMBER_BUFFER_LEN);
	value = htonl(value);
	memcpy(buf, &value, sizeof(u_int32_t));
	Value.SetValue(buf, NUMBER_BUFFER_LEN);
}

std::wstring ParameterNameValueType::ToString() const
{
	const int32_t bufSize = 100;
	wchar_t buf[bufSize]; // a googol, room for to count more things than there are in the universe.
	std::wstring result;
	switch(GetParameterType())
	{
		case Int8Type:
			FormatString1(buf, bufSize, L"%d", GetValueInt8());
			result = buf;
			break;
		case Int16Type:
			FormatString1(buf, bufSize, L"%d", GetValueInt16());
			result = buf;
			break;
		case Int32Type:
			FormatString1(buf, bufSize, L"%d", GetValueInt32());
			result = buf;
			break;
		case UInt8Type:
			FormatString1(buf, bufSize, L"%u", GetValueUInt8());
			result = buf;
			break;
		case UInt16Type:
			FormatString1(buf, bufSize, L"%u", GetValueUInt16());
			result = buf;
			break;
		case UInt32Type:
			FormatString1(buf, bufSize, L"%u", GetValueUInt32());
			result = buf;
			break;
		case FloatType:
			FormatString1(buf, bufSize, L"%f", GetValueFloat());
			result = buf;
			break;
		case TextType:
			result = GetValueText();
			break;
		case AsciiType:
			result = StringUtils::ConvertMBSToWCS(GetValueAscii());
			break;
  default:
    // unhandled cases are ok
    break;
	}
	return result;
}

std::wstring ParameterNameValueDefaultRequiredType::DefaultToString()
{
	const int32_t bufSize = 100;
	wchar_t buf[bufSize]; // a googol, room for to count more things than there are in the universe.
	std::wstring result;
	switch(GetParameterType())
	{
		case Int8Type:
			FormatString1(buf, bufSize, L"%d", GetDefaultValueInt8());
			result = buf;
			break;
		case Int16Type:
			FormatString1(buf, bufSize, L"%d", GetDefaultValueInt16());
			result = buf;
			break;
		case Int32Type:
			FormatString1(buf, bufSize, L"%d", GetDefaultValueInt32());
			result = buf;
			break;
		case UInt8Type:
			FormatString1(buf, bufSize, L"%u", GetDefaultValueUInt8());
			result = buf;
			break;
		case UInt16Type:
			FormatString1(buf, bufSize, L"%u", GetDefaultValueUInt16());
			result = buf;
			break;
		case UInt32Type:
			FormatString1(buf, bufSize, L"%u", GetDefaultValueUInt32());
			result = buf;
			break;
		case FloatType:
			FormatString1(buf, bufSize, L"%f", GetDefaultValueFloat());
			result = buf;
			break;
		case TextType:
			result = GetDefaultValueText();
			break;
		case AsciiType:
			result = StringUtils::ConvertMBSToWCS(GetDefaultValueAscii());
			break;
  default:
    // Unhandled case is ok.
    break;
	}
	return result;
}

/*
 * Set the parameter type based on the type.
 */
void ParameterNameValueDefaultRequiredType::SetParameterType()
{
	ParameterNameValueType::ParameterType paramType = GetParameterType();
	switch (paramType)
	{
	case Int8Type:
	case UInt8Type:
	case Int16Type:
	case UInt16Type:
	case Int32Type:
	case UInt32Type:
		valueType = IntegerParameterType;
		break;

	case FloatType:
		valueType = FloatParameterType;
		break;
		
	case TextType:
	case AsciiType:
		valueType = TextParameterType;
		break;

	default:
		break;
	}
}

/*
 * MIME constructor.  Useful when reading in from a file. No interpretation attempted.
 */
ParameterNameValueDefaultRequiredType::ParameterNameValueDefaultRequiredType
(
	const std::wstring& name,
	const void* mimeValue,
	int32_t mimeValueSize,
	const std::wstring& mimeType,
	const void *defaultMimeValue,
	int32_t defaultMimeValueSize,
	bool req
)
{
	valueType = NoParameterType;
	Name = name;
	Value = MIMEValue(mimeValue, mimeValueSize);
	Type = mimeType;
	defaultValue = MIMEValue(defaultMimeValue, defaultMimeValueSize);
	hasDefault = true;
	required = req;
	SetParameterType();
}

/*
 * MIME constructor.  Useful when reading in from a file. No interpretation attempted.
 */
ParameterNameValueDefaultRequiredType::ParameterNameValueDefaultRequiredType
(
	const std::wstring& name,
	const MIMEValue& mimeValue,
	const std::wstring& mimeType,
	const MIMEValue &defaultMimeValue,
	bool req
)
{
	valueType = NoParameterType;
	Name = name;
	Value = mimeValue;
	Type = mimeType;
	hasDefault = true;
	defaultValue = defaultMimeValue;
	required = req;
	SetParameterType();
}

/*
 * Copy the data.
 */
ParameterNameValueDefaultRequiredType ParameterNameValueDefaultRequiredType::operator=(const ParameterNameValueDefaultRequiredType& param)
{
	Name = param.Name;
	Value = param.Value;
	Type = param.Type;
	required = param.required ;
	hasDefault = param.hasDefault;
	defaultValue = param.defaultValue;
	valueType = param.valueType;
	controlled = param.controlled;
	controlMultiValues = param.controlMultiValues;
	return *this;
}

int8_t ParameterNameValueDefaultRequiredType::GetDefaultValueInt8() const
{
	return (int8_t)DefaultValueToInt(Int8MIMEType);
}

void ParameterNameValueDefaultRequiredType::SetDefaultValueInt8(int8_t value)
{
	hasDefault = true;
	IntToDefaultValue(value, Int8MIMEType);
}

u_int8_t ParameterNameValueDefaultRequiredType::GetDefaultValueUInt8() const
{
	return (u_int8_t)DefaultValueToInt(UInt8MIMEType);
}

void ParameterNameValueDefaultRequiredType::SetDefaultValueUInt8(u_int8_t value)
{
	hasDefault = true;
	IntToDefaultValue(value, UInt8MIMEType);
}

int16_t ParameterNameValueDefaultRequiredType::GetDefaultValueInt16() const
{
	return (int16_t)DefaultValueToInt(Int16MIMEType);
}

void ParameterNameValueDefaultRequiredType::SetDefaultValueInt16(int16_t value)
{
	hasDefault = true;
	IntToDefaultValue(value, Int16MIMEType);
}

u_int16_t ParameterNameValueDefaultRequiredType::GetDefaultValueUInt16() const
{
	return (u_int16_t)DefaultValueToInt(UInt16MIMEType);
}

void ParameterNameValueDefaultRequiredType::SetDefaultValueUInt16(u_int16_t value)
{
	hasDefault = true;
	IntToDefaultValue(value, UInt16MIMEType);
}

int32_t ParameterNameValueDefaultRequiredType::GetDefaultValueInt32() const
{
	return (int32_t)DefaultValueToInt(Int32MIMEType);
}

void ParameterNameValueDefaultRequiredType::SetDefaultValueInt32(int32_t value)
{
	hasDefault = true;
	IntToDefaultValue(value, Int32MIMEType);
}

u_int32_t ParameterNameValueDefaultRequiredType::GetDefaultValueUInt32() const
{
	return (u_int32_t)DefaultValueToInt(UInt32MIMEType);
}

void ParameterNameValueDefaultRequiredType::SetDefaultValueUInt32(u_int32_t value)
{
	hasDefault = true;
	IntToDefaultValue(value, UInt32MIMEType);
}

float ParameterNameValueDefaultRequiredType::GetDefaultValueFloat() const
{
	if (Type == FloatMIMEType)
	{
		int32_t ival = DefaultValueToInt(FloatMIMEType);
		float value = 0;
		memcpy(&value, &ival, sizeof(int32_t));
		return value;
	}
	else
	{
		affymetrix_calvin_exceptions::ParameterMismatchException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

void ParameterNameValueDefaultRequiredType::SetDefaultValueFloat(float value)
{
	hasDefault = true;
	type_punned pun;
	pun.v_float=value;
	IntToDefaultValue(pun.v_int32, FloatMIMEType);
}

std::wstring ParameterNameValueDefaultRequiredType::GetDefaultValueText() const
{
	if (Type == TextMIMEType)
	{
		u_int32_t size = 0;
		const u_int16_t* v = (const u_int16_t*)defaultValue.GetValue(size);
		u_int32_t len = size / sizeof(u_int16_t);
		wchar_t* value = new wchar_t[len+1];
				
		for (u_int32_t i = 0; i < len; ++i)
		{
			value[i] = ntohs(v[i]);
		}
		value[len] = 0;
		std::wstring result = value;
		delete [] value;
		return result;
	}
	else
	{
		affymetrix_calvin_exceptions::ParameterMismatchException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

void ParameterNameValueDefaultRequiredType::SetDefaultValueText(const std::wstring &value, int32_t reserve)
{
	Type = TextMIMEType;
	hasDefault = true;
	u_int32_t len = (u_int32_t) value.length();
	u_int32_t maxLn = len;
	if (reserve != -1 && (u_int32_t)reserve > len)
		maxLn = (u_int32_t)reserve;

	u_int16_t* buf = new u_int16_t[maxLn];
	for (u_int32_t i=0; i<len; ++i)
	{
		buf[i] = (u_int16_t) value[i];
		buf[i] = htons(buf[i]);
	}
	for(u_int32_t i = len; i < maxLn; ++i)
		buf[i] = 0;
	defaultValue.SetValue(buf, maxLn*sizeof(u_int16_t));
	delete [] buf;
}

std::string ParameterNameValueDefaultRequiredType::GetDefaultValueAscii() const
{
	if (Type == AsciiMIMEType)
	{
		u_int32_t size = 0;
		const char* v = (const char*)defaultValue.GetValue(size);
		char* value = new char[size+1];
		for (u_int32_t i=0; i<size; i++)
			value[i] = v[i];
		value[size] = 0;
		std::string result = value;
		delete [] value;
		return result;
	}
	else
	{
		affymetrix_calvin_exceptions::ParameterMismatchException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

void ParameterNameValueDefaultRequiredType::SetDefaultValueAscii(const std::string &value, int32_t reserve)
{
	Type = AsciiMIMEType;
	hasDefault = true;
	u_int32_t len = (u_int32_t) value.length();
	if (reserve == -1 || (u_int32_t)reserve <=  len)
	{
		defaultValue.SetValue(value.c_str(), len);
	}
	else
	{
		char* buf = new char[reserve];
		memcpy(buf, value.c_str(), len);
		for(int32_t i = len; i < reserve; ++i)
			buf[i] = 0;

		defaultValue.SetValue(buf, reserve);
		delete [] buf;
	}
}

u_int32_t ParameterNameValueDefaultRequiredType::DefaultValueToInt(const wchar_t* mimeType) const
{
	if (Type == mimeType)
	{
		u_int32_t size = 0;
		const void* v = defaultValue.GetValue(size);
		u_int32_t value = 0;
		memcpy(&value, v, sizeof(u_int32_t));
		value = ntohl(value);
		return value;
	}
	else
	{
		affymetrix_calvin_exceptions::ParameterMismatchException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

void ParameterNameValueDefaultRequiredType::IntToDefaultValue(u_int32_t value, const wchar_t* type)
{
	Type = type;
	char buf[NUMBER_BUFFER_LEN];
	memset(buf, 0, NUMBER_BUFFER_LEN);
	value = htonl(value);
	memcpy(buf, &value, sizeof(u_int32_t));
	defaultValue.SetValue(buf, NUMBER_BUFFER_LEN);
}

/*
 * Convert the type to a string.
 */
std::wstring ParameterNameValueDefaultRequiredType::ParameterValueTypeToString(ParameterValueType value)
{
	switch (value)
	{
	case NoParameterType:
		return L"";

	case IntegerParameterType:
		return L"Int";
		
	case FloatParameterType:
		return L"Float";

	case TextParameterType:
		return L"String";

	case DateParameterType:
		return L"Date";

	case TimeParameterType:
		return L"Time";

	case DateTimeParameterType:
		return L"DateTime";

	case ControlSingleParameterType:
		return L"SingleControl";
		
	case ControlMultiParameterType:
		return L"MultiControl";

	default:
		return L"";
	}
}

/*
 * Convert the string to a type.
 */
ParameterNameValueDefaultRequiredType::ParameterValueType ParameterNameValueDefaultRequiredType::ParameterValueTypeFromString(const std::wstring &value)
{
	if (value == L"Int")
		return IntegerParameterType;
		
	else if (value == L"Float")
		return FloatParameterType;

	else if (value == L"String")
		return TextParameterType;

	else if (value == L"Date")
		return DateParameterType;

	else if (value == L"Time")
		return TimeParameterType;

	else if (value == L"DateTime")
		return DateTimeParameterType;

	else if (value == L"SingleControl")
		return ControlSingleParameterType;
		
	else if (value == L"MultiControl")
		return ControlMultiParameterType;

	else
		return NoParameterType;
}

