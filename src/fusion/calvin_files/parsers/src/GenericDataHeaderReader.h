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


#ifndef _GenericDataHeaderReader_HEADER_
#define _GenericDataHeaderReader_HEADER_

/*! \file GenericDataHeaderReader.h This file provides methods to read a generic data file header.
 */

#include "calvin_files/data/src/GenericDataHeader.h"
//
#include <fstream>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // dont show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{
/*! This class reads the GenericDataHeader from a file. */
class GenericDataHeaderReader
{
public:
	/*! Constructor
	 *	@param fs Open stream positioned at the start of the first GenericDataHeader.
	 */
	GenericDataHeaderReader(std::ifstream& fs);

public:
	/*! Read the GenericDataHeader and all parent GenericDataHeaders from the input stream.
	 *  @param gdh Reference to the GenericDataHeader object to which to add the GenericDataHeader information.
	 */
	void Read(GenericDataHeader& gdh);
	/*! Read the GenericDataHeader and all parent GenericDataHeaders from the input stream.
	 *  @param gdh Reference to the GenericDataHeader object to which to add the GenericDataHeader information.
	 *  @param doUniqueAdds Hint to indicate to ensure no duplicates are added to the collection. If true, the 
	 * check is made and performance is N^2, if false, no check is made and is assuming you are just reading from a file.
	 */
	void Read(GenericDataHeader& gdh, bool doUniqueAdds);

protected:
	/*! A reference to the file stream. */
	std::ifstream& fileStream;
};
}
#endif // _GenericDataHeaderReader_HEADER_
