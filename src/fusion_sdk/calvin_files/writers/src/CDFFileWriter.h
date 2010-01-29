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
#ifndef _CDFFileWriter_HEADER_
#define _CDFFileWriter_HEADER_

#include "calvin_files/data/src/CDFData.h"
#include "calvin_files/writers/src/CDFProbeSetWriter.h"
#include "calvin_files/writers/src/GenericFileWriter.h"
//
#include <fstream>
//

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

class CDFFileWriter
{

private:

	GenericFileWriter* writer;
	DataGroupWriter* dataGroupWriter;
	DataSetWriter* contentsWriter;
	DataSetHeader probeSetPlaneHdr;
	int32_t contentsPos;
	int32_t currentCubePos;

public:

	CDFFileWriter(CDFData &p);
	~CDFFileWriter();

	CDFProbeSetWriter* CreateProbeSetWriter(const std::wstring& xdaBlockName,
																					u_int8_t unitType,
																					u_int8_t direction,
																					u_int32_t atoms,
																					u_int32_t cells,
																					u_int32_t probeSetNumber,
																					u_int8_t cellsPerAtom);

	/*! Prepares a data group for writing
	 @param probeSetName Probe set name
	 @param dataSetCnt number of data sets to be written
	 */
	void OpenDataGroup(const std::wstring& probeSetName, int dataSetCnt);

	/*! Closes data group after writing */
	void CloseDataGroup();

private:

	void WriteHeaders();

	void WriteContentsEntry(const std::wstring& probeSetName);

};

}

#endif // _CDFFileWriter_HEADER_
