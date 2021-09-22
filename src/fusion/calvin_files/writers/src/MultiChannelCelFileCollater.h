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
#ifndef _MultiChannelCelFileCollater_HDR_
#define _MultiChannelCelFileCollater_HDR_

#include <vector>
#include "calvin_files/portability/src/AffymetrixBaseTypes.h"
#include "calvin_files/utils/src/AffyStlCollectionTypes.h"
#include "calvin_files/data/src/FileHeader.h"
#include "calvin_files/data/src/GenericData.h"
#include "calvin_files/parameter/src/ParameterNameValueType.h"
#include "calvin_files/writers/src/GenericFileWriter.h"
#include "calvin_files/parsers/src/GenericFileReader.h"
#include "calvin_files/writers/src/CombinedDatConstants.h"

#ifdef _MSC_VER
#pragma warning(disable: 4290) // don't show warnings about throw keyword on function declarations.
#endif

namespace affymetrix_calvin_io
{

#define NO_WAVELENGTH L"wavelength unavailable"
#define MULTI_CHANNEL_FILE_ID "affymetrix-calvin-multi-intensity"

class MultiChannelCelFileCollater
{

private:

	std::wstring wavelength;

	void CopyHeaders(const std::vector<std::string>& celFiles, FileHeader& outFileHdr);

	void CopyDataSetHdr(DataSetHeader& src, DataSetHeader& dst);

	void CopyParentHeaders(GenericDataHeader* src, GenericDataHeader* dst);

	void CopyGenericHeader(GenericDataHeader* src, GenericDataHeader* dst);

	void CopyGenericHeaderParams(GenericDataHeader* src, GenericDataHeader* dst);

	void WriteDataGroups(const StringVector& celFiles, GenericFileWriter& writer);

	void UpdateWavelength(GenericDataHeader* hdr);

protected:

	void CopyDataGroupHdr(DataGroupHeader& src, DataGroupHeader& dst);

	void CopyFileHdr(FileHeader& src, FileHeader& dst);

	void OpenReader(const std::string& file, GenericData& data);

	void WriteDataGroup(DataGroupReader& gReader, DataGroupHeader& fileHdr, DataGroupWriter& writer, bool lastGroup);

	void WriteDataSet(DataSetReader& setReader, DataSetHeader& setHdr, DataSetWriter& setWriter);

public:

	MultiChannelCelFileCollater();

	~MultiChannelCelFileCollater();

	void Collate(const StringVector& celFiles, const std::string& outFile);
};

}

#endif // _MultiChannelCelFileCollater_HDR_
