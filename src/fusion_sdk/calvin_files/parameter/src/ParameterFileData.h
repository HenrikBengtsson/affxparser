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


#ifndef _ParameterFileData_HEADER_
#define _ParameterFileData_HEADER_

/*! \file ParameterFileData.h This file provides definitions for data in a parameter file.
 */

#include <cstring>
#include <list>
#include <string>
//

namespace affymetrix_calvin_parameter
{

/*! The attributes in the ParameterFile element. */
typedef struct _ParameterFileAttributesType
{
    std::wstring company; /*! The company name. */
    std::wstring userName; /*! The user name. */
    std::wstring contentVersion; /*! The content version. */
} ParameterFileAttributesType;

/*! The attributes in the Implementation element. */
typedef struct _ImplementationAttributesType
{
    std::wstring name; /*! The name. */
	std::wstring version; /*! The version number. */
	std::wstring executableFileName; /*! The exe file name. */
	std::wstring description; /*! The description */
} ImplementationAttributesType;

/*! The attributes in the Parameter element. */
typedef struct _ParameterType
{
    std::wstring name; /*! The name. */
    std::wstring index; /*! The index. */
    std::wstring displayName; /*! The display name. */
    std::wstring category; /*! The category. */
    std::wstring isEditable; /*! The idEditable flag */
    std::wstring type; /*! The type */
    std::wstring currentValue; /*! The current value */
    std::wstring minValue; /*! The minimum value */
    std::wstring maxValue; /*! The maximum value */
    std::wstring defaultValue; /*! The default value */
    std::wstring precision; /*! The precision for floating point numbers */
    std::wstring maxLength; /*! The maximum length of a string value */
    std::wstring description; /*! The description */
} ParameterType;

/*! A list of parameters. */
typedef std::list<ParameterType> ParameterTypeList;

/*! Provides data storage for ParameterFile files. This version
    does not store all of the contents of a parameter file.
    Missing items includes the ParameterSet, MetaData and Control
    elements.
*/
class ParameterFileData
{
public:
    /*! Clears the members in the class. */
    void Clear();

    /*! Constructor. */
    ParameterFileData();

    /*! The parameter file attributes. */
    ParameterFileAttributesType &ParameterFileAttributes() { return parameterFileAttributes; }

    /*! The implementation attributes. */
    ImplementationAttributesType &ImplementationAttributes() { return implementationAttributes; }

    /*! The list of parameters. */
    ParameterTypeList &Parameters() { return parameters; }

private:
    /*! The parameter file attributes. */
    ParameterFileAttributesType parameterFileAttributes;

    /*! The implementation attributes. */
    ImplementationAttributesType implementationAttributes;

    /*! The list of parameters. */
    ParameterTypeList parameters;
};

}

#endif // _ParameterFileData_HEADER_
