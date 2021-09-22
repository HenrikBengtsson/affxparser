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

#ifndef _CELFILEWRITER_H_
#define _CELFILEWRITER_H_

/*! \file CELFileWriter.h This file provides CEL file writing capabilities.
 */

#include "file/CELFileData.h"
//

namespace affxcel
{

///////////////////////////////////////////////////////////////////////////////
///  affxcel::CCELFileWriter
///
///  \brief CEL file writer object
///
///  Include write support of the following file formats:
///  - text (version 3)
///  - xda binary (version 4)
///  - transcriptome binary (internal use only)
///  - compact binary (not supported by software other than Bruce)
///
///  All file formats include the same header information. The intensity data
///  for each cell are stored as follows:
///  - text format
///		- mean (data type: float - 4 bytes)
///		- stdev (data type: float - 4 bytes)
///		- pixels (data type: short - 2 bytes)
///  - xda binary format
///		- mean (data type: float - 4 bytes)
///		- stdev (data type: float - 4 bytes)
///		- pixels (data type: short - 2 bytes)
///  - trancriptome binary format
///		- mean (data type: unsigned short - 2 bytes)
///		- stdev (data type: unsigned short - 2 bytes)
///		- pixels (data type: unsigned char - 1 byte)
///  - compact binary format
///		- mean (data type: unsigned short - 2 bytes)
///////////////////////////////////////////////////////////////////////////////
class CCELFileWriter : public CCELFileData  
{
public:
	/*! Constructor */
	CCELFileWriter();

	/*! Destructor */
	~CCELFileWriter();

public:
	///////////////////////////////////////////////////////////////////////////////
	///  inline public  GetThisPtr
	///  \brief Retrieve the pointer of the current object instance
	///  @return affxcel::CCELFileWriter *	Pointer to current object instance
	///////////////////////////////////////////////////////////////////////////////
	affxcel::CCELFileWriter *GetThisPtr() { return this; }

	/*! Writes a version 3 ASCII text CEL file.
	 * @return True if successful
	 */
	bool WriteTextCel();

	/*! Writes an XDA binary CEL file.
	 * @return True if successful
	 */
	bool WriteXDABCel();

	/*! Writes a transcriptome binary CEL file.
	 * @return True if successful
	 */
	bool WriteTranscriptomeBCel();

	/*! Writes a compact binary CEL file.
	 * @return True if successful
	 */
	bool WriteCompactBCel();
};

//////////////////////////////////////////////////////////////////////

} // namespace

////////////////////////////////////////////////////////////////////
#endif // _CELFILEWRITER_H_
