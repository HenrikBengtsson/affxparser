////////////////////////////////////////////////////////////////
//
// Copyright (C) 2008 Affymetrix, Inc.
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

#ifndef _DataSetHeaderAppend_HEADER_
#define _DataSetHeaderAppend_HEADER_


#include "calvin_files/parameter/src/ParameterNameValueType.h"
//
#include <cstring>
#include <string>
//

namespace affymetrix_calvin_io
{

/*! Information for a data set. */
typedef struct _DataSetHeaderInformation
{
	/*! The parameters for the data set */
	affymetrix_calvin_parameter::ParameterNameValueTypeList params;

	/*! The data set name */
	std::wstring dataSetName;

} DataSetHeaderInformation;

/*! A class to provide functions to add a data set to an existing file. */
class DataSetHeaderAppend
{
private:
	/*! Add a data set header to the file
	 * @param file The name of the file.
	 * @param params The parameters for the data set.
	 * @param groupName The name of the data group.
	 * @param setName The name of the data set.
	 */
	//void AddDataSetHeader(const std::string &file, const affymetrix_calvin_parameter::ParameterNameValueTypeList &params, const std::wstring &groupName, const std::wstring &setName);

public:
	/*! Add a data set header to the file
	 * @param file The name of the file.
	 * @param groupName The name of the group for the data sets.
	 * @param params A list of data set name/parameters.
	 */
	void AddDataSetHeader(const std::string &file, const std::wstring &groupName, const std::list<DataSetHeaderInformation> &params);
};

}

#endif
