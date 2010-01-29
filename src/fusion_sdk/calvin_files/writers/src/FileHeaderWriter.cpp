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
#include "calvin_files/writers/src/FileHeaderWriter.h"
//
#include "calvin_files/utils/src/AffymetrixGuid.h"
#include "calvin_files/writers/src/FileOutput.h"
#include "calvin_files/writers/src/GenericDataHeaderWriter.h"
//

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_utilities;

void FileHeaderWriter::Write(std::ofstream &os, FileHeader &g)
{
    WriteMagicNumber(os, g);
    WriteVersion(os, g);
    WriteDataGroupCnt(os, g);
    WriteDataGroupOffset(os, 0);
    WriteGenericDataHdr(os, g);
}

void FileHeaderWriter::WriteMagicNumber(std::ofstream &os, FileHeader &g)
{
	FileOutput::WriteInt8(os, g.GetMagicNumber());
}

void FileHeaderWriter::WriteVersion(std::ofstream &os, FileHeader &g)
{
	FileOutput::WriteInt8(os, g.GetVersion());
}

void FileHeaderWriter::WriteDataGroupCnt(std::ofstream &os, FileHeader &g)
{
	FileOutput::WriteInt32(os, g.GetDataGroupCnt());
}

void FileHeaderWriter::WriteDataGroupOffset(std::ofstream &os, u_int32_t offset)
{
    dataSetOffsetPos = os.tellp();
    FileOutput::WriteUInt32(os, offset);
}

void FileHeaderWriter::UpdateDataGroupOffset(std::ofstream &os, u_int32_t offset) const
{
    if(dataSetOffsetPos > 0) 
    {
        os.seekp(dataSetOffsetPos, std::ios::beg);
        FileOutput::WriteUInt32(os, offset);
        os.seekp(offset, std::ios::beg);
    }
}

void FileHeaderWriter::WriteGenericDataHdr(std::ofstream &os, FileHeader &g)
{
    GenericDataHeaderWriter writer;

		// Check if a file ID has been assign, if not assign one.
		if (g.GetGenericDataHdr()->GetFileId().length() == 0)
			g.GetGenericDataHdr()->SetFileId(AffymetrixGuid::GenerateNewGuid());

    writer.Write(os, *(g.GetGenericDataHdr()));
}
