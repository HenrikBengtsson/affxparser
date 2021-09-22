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


#ifndef _ParameterFileReader_HEADER_
#define _ParameterFileReader_HEADER_

/*! \file ParameterFileReader.h This file provides interfaces to read a parameter file.
 */

#include "calvin_files/parameter/src/ParameterFileData.h"
//
#include <cstring>
#include <fstream>
#include <string>
//

namespace affymetrix_calvin_io
{

/*! This class provides interfaces to read a parameter file. This version
    does not store all of the contents of a parameter file.
    Missing items includes the ParameterSet, MetaData and Control
    elements.
*/
class ParameterFileReader
{
public:
	/*! Constructor */
	ParameterFileReader();

	/*! Destructor */
	~ParameterFileReader();

public:
	/*! Reads the entire contents of the file.
	 *
	 * @param fileName The name of the parameter file to read.
	 * @param parameterData The parameter data from the input file.
	 */
	bool Read(const std::string &fileName, affymetrix_calvin_parameter::ParameterFileData &parameterFileData);
};

};

#endif // _ParameterFileReader_HEADER_
