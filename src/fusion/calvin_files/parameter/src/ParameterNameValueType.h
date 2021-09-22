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


#ifndef _ParameterNameValueType_HEADER_
#define _ParameterNameValueType_HEADER_

/*! \file ParameterNameValueType.h This file provides definitions of parameter name-value-type.
 */

#include "calvin_files/parameter/src/ParameterException.h"
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
//
#include <cstring>
#include <list>
#include <string>
#include <vector>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_parameter
{

/*! A class to hold a MIME value 
 *  Is responsible for managing the memory for the mime value
 */
class MIMEValue
{
public:
	/*! Constructs a MIME value - default constructor */
	MIMEValue();

	/*! Constructs a MIME value
	 *	@param value A pointer to array containing the MIME encoded value
	 *	@param size The size of the array
	 */
	MIMEValue(const void* value, u_int32_t size);

	/*! Copy constructor
	 *	@param source MIMEValue object to copy
	 */
	MIMEValue(const MIMEValue& source);

	/*! Destructor */
	~MIMEValue();

	/*! Assignment operator
	 * @param source The source.
	 * @return The copied object.
	 */
	MIMEValue& operator=(const MIMEValue& source);

	/*! Operator equals
	 * @param lhs The left hand side to compare.
	 * @return True if the same.
	 */
	bool operator==(const MIMEValue& lhs);

	/*! Operator not equals.
	 * @param lhs The let hand side to compare.
	 * @return True if not equal.
	 */
	bool operator!=(const MIMEValue& lhs);

	/*! Set the controlled value
	 *	@param value A pointer to array containing the MIME encoded value
	 *	@param size The size of the array
	 */
	void SetValue(const void* value, u_int32_t size);

	/*! Get the controlled value
	 *	@param size Gets filled with the size of the constrolled array
	 *	@ return Const pointer to the MIME value array. (for writing)*/
	const void* GetValue(u_int32_t& size) const;

	/*! Gets the size of the array */
	u_int32_t Size() const { return arrSize; }

private:
	/*! The buffer to hold the data. */
	char* arr;

	/*! The size of the buffer. */
	u_int32_t arrSize;
};

/*! A class to hold a parameter name/value/type.
 *	This class will convert several built-in types between the MIME string and their native types.
 */
class ParameterNameValueType
{
public:
	/*! Default constructor */
	ParameterNameValueType() {}

	/*! MIME constructor.  Useful when reading in from a file 
	 *	@param name Parameter name
	 *	@param mimeValue MIME encoded value in a buffer
	 *	@param mimeValueSize The size in bytes of the MIME encoded value.
	 *	@param mimeType 
	 */
	ParameterNameValueType(const std::wstring& name, const void* mimeValue, int32_t mimeValueSize, const std::wstring& mimeType);

	/*! MIME constructor.  Useful when reading in from a file 
	 *	@param name Parameter name
	 *	@param mimeValue MIME encoded value
	 *	@param mimeType 
	 */
	ParameterNameValueType(const std::wstring& name, const MIMEValue& mimeValue, const std::wstring& mimeType);

	// Default copy constructor is fine

	/*! A copy operator.
	 *	@param param The parameter to copy.
	 */
	ParameterNameValueType operator=(const ParameterNameValueType& param) { Name = param.Name; Value = param.Value; Type = param.Type; return *this; }

	/*! An equality operator. Compares the parameter name.
	 *	@param param The parameter to compare.
	 *	@return True if the parameter names are the same.
	 */
	bool operator==(const ParameterNameValueType& param) const { if (param.Name == Name) return true; return false; }
	
	/*! An inequality operator.  Compares the parameter name.
	 *	@param param The parameter to compare.
	 *	@return True if the parameter names are different.
	 */
	bool operator!=(const ParameterNameValueType& param) const { if (param.Name == Name) return false; return true; }

	/*! An equality operator. Compares the parameter name.
	 *	@param name A parameter name to compare.
	 *	@return True if the parameter names are the same.
	 */
	bool operator==(const std::wstring &name) const { if (Name == name) return true; return false; }
	
	/*! An inequality operator.  Compares the parameter name.
	 *	@param name A parameter name to compare.
	 *	@return True if the parameter names are diffenent.
	 */
	bool operator!=(const std::wstring &name) const { if (Name == name) return false; return true; }

	/*! Less than comparison operator.  Compares the parameter name.
	 *	@param param The parameter to compare.
	 *	@return True if the target parameter name is lexically less than param.
	*/
	bool operator<(const ParameterNameValueType &param) const { return (Name < param.Name); }
	/*! Greater than comparison operator.  Compares the parameter name.
	 *	@param param The parameter to compare.
	 *	@return True if the target parameter name is lexically greter than param.
	*/
	bool operator>(const ParameterNameValueType &param) const { return (Name > param.Name); }

	/*! Get the name of the parameter
	 *	@ Name of the parameter
	 */
	std::wstring GetName() const { return Name; }

	/*! Set the name of the parameter.
	 *	@param value Name of the parameter.
	 */
	void SetName(const std::wstring& value) { Name = value; }
	
	/*! Enumerant of the built-in parameter types. */
	enum ParameterType { 
			  Int8Type,			/*! an 8 bit integer. */
			  UInt8Type, 		/*! an 8 bit unsigned integer. */
			  Int16Type,		/*! a 16 bit integer. */
			  UInt16Type,		/*! a 16 bit unsigned integer. */
			  Int32Type,		/*! a 32 bit integer. */
			  UInt32Type,		/*! a 32 bit unsigned integer. */
			  FloatType,		/*! a 32 bit floating point. */
			  TextType,			/*! a 16 bit character. */
			  AsciiType,		/*! an 8 bit character. */
			  UnknownType		/*! an 8 bit integer. */
	};
			  
	/*! Get the parameter type
	 *	@return The parameter type of the object.*/
	ParameterType GetParameterType() const;
	
	/*! Gets value as a int8_t
	 *	@return Value of the parameter as an int8_t
	 *	@exception affymetrix_calvin_exceptions::ParameterMismatchException Parameter is not an int8_t
	 */
	int8_t GetValueInt8() const;

	/*! Sets the value as an int8_t
	 *	@param value
	 */
	void SetValueInt8(int8_t value);

	/*! Gets value as a u_int8_t
	 *	@return Value of the parameter as an u_int8_t
	 *	@exception affymetrix_calvin_exceptions::ParameterMismatchException Parameter is not a u_int8_t
	 */
	u_int8_t GetValueUInt8() const;

	/*! Sets the value as a u_int8_t
	 *	@param value
	 */
	void SetValueUInt8(u_int8_t value);

	/*! Gets value as a int16_t
	 *	@return Value of the parameter as an int16_t
	 *	@exception affymetrix_calvin_exceptions::ParameterMismatchException Parameter is not an int16_t
	 */
	int16_t GetValueInt16() const;

	/*! Sets the value as an int16_t
	 *	@param value
	 */
	void SetValueInt16(int16_t value);

	/*! Gets value as a u_int16_t
	 *	@return Value of the parameter as an u_int16_t
	 *	@exception affymetrix_calvin_exceptions::ParameterMismatchException Parameter is not a u_int16_t
	 */
	u_int16_t GetValueUInt16() const;

	/*! Sets the value as a u_int16_t
	 *	@param value
	 */
	void SetValueUInt16(u_int16_t value);

	/*! Gets value as a int32_t
	 *	@return Value of the parameter as an int32_t
	 *	@exception affymetrix_calvin_exceptions::ParameterMismatchException Parameter is not an int32_t
	 */
	int32_t GetValueInt32() const;

	/*! Sets the value as an int32_t
	 *	@param value
	 */
	void SetValueInt32(int32_t value);

	/*! Gets value as a u_int32_t
	 *	@return Value of the parameter as an u_int32_t
	 *	@exception affymetrix_calvin_exceptions::ParameterMismatchException Parameter is not a u_int32_t
	 */
	u_int32_t GetValueUInt32() const;

	/*! Sets the value as a u_int32_t
	 *	@param value
	 */
	void SetValueUInt32(u_int32_t value);

	/*! Gets value as a float
	 *	@return Value of the parameter as an float
	 *	@exception affymetrix_calvin_exceptions::ParameterMismatchException Parameter is not a float
	 */
	float GetValueFloat() const;

	/*! Sets the value as a float
	 *	@param value
	 */
	void SetValueFloat(float value);

	/*! Gets value as a wstring
	 *	@return Value of the parameter as an wstring
	 *	@exception affymetrix_calvin_exceptions::ParameterMismatchException Parameter is not a text type
	 */
	std::wstring GetValueText() const;

	/*! Sets the value as a text type.
	 *	@param value String representation of the value.
	 *	@param reserve The total number of characters to reserve for the value. -1 indicates not to reserve any extra space.
	 */
	void SetValueText(const std::wstring &value, int32_t reserve = -1);

	/*! Gets value as a string
	 *	@return Value of the parameter as an string
	 *	@exception affymetrix_calvin_exceptions::ParameterMismatchException Parameter is not a text type
	 */
	std::string GetValueAscii() const;

	/*! Sets the value as a text type.
	 *	@param value String representation of the value.
	 *	@param reserve The total number of characters to reserve for the value. -1 indicates not to reserve any extra space.
	 */
	void SetValueAscii(const std::string &value, int32_t reserve = -1);
	
	// Raw MIME methods
	/*! Returns the mime type without interpretation.
	 *	@return String of the MIME type
	 */
	std::wstring GetMIMEType() const { return Type; }

	/*! Sets the mime type without attempting to interpret it.
	 *	@param value
	 */
	void SetMIMEType(const std::wstring& value) { Type = value; }

	/*! Returns the mime value without interpretation.
	 *	@return MIME encoded string.
	 */
	MIMEValue GetMIMEValue() const { return Value; }

	/*! Sets the mime value without attempting to interpret it.
	 *	@param value MIME encoded string.
	 */
	void SetMIMEValue(const MIMEValue& value) { Value = value; }

	/*! Converts the value of known types to a string
	 *	@return A string representation of the value
	 */
	std::wstring ToString() const;

protected:
	/*! Converts a value to an integer.
	 * @param mimeType The string representation.
	 * @return The integer representation.
	 */
	u_int32_t ValueToInt(const wchar_t* mimeType) const;

	/*! Converts an integer to a value.
	 * @param value The integer representation.
	 * @param type The string representation.
	 */
	void IntToValue(u_int32_t value, const wchar_t* type);

protected:
	/*! The name of the parameter */
	std::wstring Name;

	/*! The MIME type of the parameter */
	std::wstring Type;

	/*! The MIME value of the parameter */
	MIMEValue Value;
};

/*! An STL vector or parameter name value types. */
typedef std::vector<ParameterNameValueType> ParameterNameValueTypeVector;

/*! An STL list or parameter name value types. */
typedef std::list<ParameterNameValueType> ParameterNameValueTypeList;

/*! An STL constant iterator of ParameterNameValueType */
typedef std::vector<ParameterNameValueType>::const_iterator ParameterNameValueTypeConstIt;

/*! An STL iterator of ParameterNameValueType */
typedef std::vector<ParameterNameValueType>::iterator ParameterNameValueTypeIt;

/*! A class to hold a name/value/type/default/required attributes. */
class ParameterNameValueDefaultRequiredType : public ParameterNameValueType
{
public:
	/*! The type of value. */
	enum ParameterValueType
	{
		NoParameterType,		/*! Not set. */
		IntegerParameterType,	/*! Integer stored in the MIME value. */
		FloatParameterType,		/*! Floating point stored in the MIME value. */
		TextParameterType,		/*! Text stored in the MIME value. */
		DateParameterType,		/*! Date stored in the MIME value as text. */
		TimeParameterType,		/*! Time stored in the MIME value as text. */
		DateTimeParameterType,	/*! DateTime stored in the MIME value as text. */
		ControlSingleParameterType,	/*! Controlled single selection stored in the MIME value as text. */
		ControlMultiParameterType	/*! Controlled multi selection stored in the controlMulti list. */
	};

	/*! Converts the type to a string.
	 * @param value The type.
	 * @return The string representation.
	 */
	static std::wstring ParameterValueTypeToString(ParameterValueType value);

	/*! Converts the string to a type.
	 * @param value The string representation.
	 * @return The type.
	 */
	static ParameterValueType ParameterValueTypeFromString(const std::wstring &value);

protected:
	/*! The MIME value of the default parameter */
	MIMEValue defaultValue;

	/*! A flag to indicate if a default exist. */
	bool hasDefault;

	/*! A flag to indicate if the parameter is required. */
	bool required;

	/*! A list of parameter values for controlled vocabulary. */
	std::list<std::wstring> controlled;

	/*! A list of multi-selected controlled values. */
	std::list<std::wstring> controlMultiValues;

	/*! The type of value stored. */
	ParameterValueType valueType;

	/*! Converts a value to an integer.
	 * @param mimeType The string representation.
	 * @return The integer representation.
	 */
	u_int32_t DefaultValueToInt(const wchar_t* mimeType) const;

	/*! Converts an integer to a value.
	 * @param value The integer representation.
	 * @param type The string representation.
	 */
	void IntToDefaultValue(u_int32_t value, const wchar_t* type);

	/*! Sets the parameter type. */
	void SetParameterType();

public:
	
	/*! A list of multi-selected controlled values.
	 * @return The multi-selected controlled values.
	 */
	std::list<std::wstring> &ControlMultiValues() { return controlMultiValues; }

	/*! The type of value stored. */
	ParameterValueType &ValueType() { return valueType; }

	/*! A list of parameter values for controlled vocabulary.
	 * @return The list of parameter values for controlled vocabulary.
	 */
	std::list<std::wstring> &ControlledVocabulary() { return controlled; }

	/*! Returns the mime value without interpretation.
	 *	@return MIME encoded string.
	 */
	MIMEValue &DefaultMIMEValue() { return defaultValue; }

	/*! Gets the required flag.
	 * @return The required flag.
	 */
	bool &RequiredFlag() { return required; }

	/*! A flag to indicate if a default exist.
	 * @return The flag indicating if a default value exists.
	 */
	bool &HasDefault() { return hasDefault; }

	/*! Default constructor */
	ParameterNameValueDefaultRequiredType() { required = false; hasDefault = false; valueType = NoParameterType; }

	/*! MIME constructor.  Useful when reading in from a file 
	 *	@param name Parameter name
	 *	@param mimeValue MIME encoded value in a buffer
	 *	@param mimeValueSize The size in bytes of the MIME encoded value.
	 *	@param mimeType The MIME type.
	 *  @param defaultMimeValue The default MIME encoded value in a buffer.
	 *  @param defaultMimeValueSize The size in bytes of the default MIME encoded value.
	 *  @param req Flag to indicate if the parameter is required.
	 */
	ParameterNameValueDefaultRequiredType(const std::wstring& name, const void* mimeValue, int32_t mimeValueSize, const std::wstring& mimeType, const void *defaultMimeValue, int32_t defaultMimeValueSize, bool req);

	/*! MIME constructor.  Useful when reading in from a file 
	 *	@param name Parameter name
	 *	@param mimeValue MIME encoded value
	 *	@param mimeType The MIME type.
	 *  @param defaultMimeValue The default MIME encoded value in a buffer.
	 *  @param req Flag to indicate if the parameter is required.
	 */
	ParameterNameValueDefaultRequiredType(const std::wstring& name, const MIMEValue& mimeValue, const std::wstring& mimeType, const MIMEValue &defaultMimeValue, bool req);

	/*! A copy operator.
	 *	@param param The parameter to copy.
	 */
	ParameterNameValueDefaultRequiredType operator=(const ParameterNameValueDefaultRequiredType& param);

	/*! Converts default value to a string
	 *	@return A string representation of the default value
	 */
	std::wstring DefaultToString();

	/*! Gets default value as a int8_t
	 *	@return Value of the parameter as an int8_t
	 *	@exception affymetrix_calvin_exceptions::ParameterMismatchException Parameter is not an int8_t
	 */
	int8_t GetDefaultValueInt8() const;

	/*! Sets the default value as an int8_t
	 *	@param value The value
	 */
	void SetDefaultValueInt8(int8_t value);

	/*! Ges the default value as a u_int8_t
	 *	@return Value of the parameter as an u_int8_t
	 *	@exception affymetrix_calvin_exceptions::ParameterMismatchException Parameter is not a u_int8_t
	 */
	u_int8_t GetDefaultValueUInt8() const;

	/*! Sets the default value as a u_int8_t
	 *	@param value The value
	 */
	void SetDefaultValueUInt8(u_int8_t value);

	/*! Ges the default value as a int16_t
	 *	@return Value of the parameter as an int16_t
	 *	@exception affymetrix_calvin_exceptions::ParameterMismatchException Parameter is not an int16_t
	 */
	int16_t GetDefaultValueInt16() const;

	/*! Sets the default value as an int16_t
	 *	@param value The value
	 */
	void SetDefaultValueInt16(int16_t value);

	/*! Ges the default value as a u_int16_t
	 *	@return Value of the parameter as an u_int16_t
	 *	@exception affymetrix_calvin_exceptions::ParameterMismatchException Parameter is not a u_int16_t
	 */
	u_int16_t GetDefaultValueUInt16() const;

	/*! Sets the default value as a u_int16_t
	 *	@param value The value
	 */
	void SetDefaultValueUInt16(u_int16_t value);

	/*! Ges the default value as a int32_t
	 *	@return Value of the parameter as an int32_t
	 *	@exception affymetrix_calvin_exceptions::ParameterMismatchException Parameter is not an int32_t
	 */
	int32_t GetDefaultValueInt32() const;

	/*! Sets the default value as an int32_t
	 *	@param value The value
	 */
	void SetDefaultValueInt32(int32_t value);

	/*! Ges the default value as a u_int32_t
	 *	@return Value of the parameter as an u_int32_t
	 *	@exception affymetrix_calvin_exceptions::ParameterMismatchException Parameter is not a u_int32_t
	 */
	u_int32_t GetDefaultValueUInt32() const;

	/*! Sets the default value as a u_int32_t
	 *	@param value The value
	 */
	void SetDefaultValueUInt32(u_int32_t value);

	/*! Ges the default value as a float
	 *	@return Value of the parameter as an float
	 *	@exception affymetrix_calvin_exceptions::ParameterMismatchException Parameter is not a float
	 */
	float GetDefaultValueFloat() const;

	/*! Sets the default value as a float
	 *	@param value The value
	 */
	void SetDefaultValueFloat(float value);

	/*! Ges the default value as a wstring
	 *	@return Value of the parameter as an wstring
	 *	@exception affymetrix_calvin_exceptions::ParameterMismatchException Parameter is not a text type
	 */
	std::wstring GetDefaultValueText() const;

	/*! Sets the default value as a text type.
	 *	@param value String representation of the default value.
	 *	@param reserve The total number of characters to reserve for the value. -1 indicates not to reserve any extra space.
	 */
	void SetDefaultValueText(const std::wstring &value, int32_t reserve=-1);

	/*! Ges the default value as a string
	 *	@return Value of the parameter as an string
	 *	@exception affymetrix_calvin_exceptions::ParameterMismatchException Parameter is not a text type
	 */
	std::string GetDefaultValueAscii() const;

	/*! Sets the default value as a text type.
	 *	@param value String representation of the default value.
	 *	@param reserve The total number of characters to reserve for the value. -1 indicates not to reserve any extra space.
	 */
	void SetDefaultValueAscii(const std::string &value, int32_t reserve=-1);
};

/*! An STL vector or parameter name value types. */
typedef std::vector<ParameterNameValueDefaultRequiredType> ParameterNameValueDefaultRequiredTypeVector;

/*! An STL list or parameter name value types. */
typedef std::list<ParameterNameValueDefaultRequiredType> ParameterNameValueDefaultRequiredTypeList;

/*! An STL constant iterator of ParameterNameValueDefaultRequiredType */
typedef std::vector<ParameterNameValueDefaultRequiredType>::const_iterator ParameterNameValueDefaultRequiredTypeConstIt;

/*! An STL iterator of ParameterNameValueDefaultRequiredType */
typedef std::vector<ParameterNameValueDefaultRequiredType>::iterator ParameterNameValueDefaultRequiredTypeIt;

}
#endif	//_ParameterNameValueType_HEADER_
