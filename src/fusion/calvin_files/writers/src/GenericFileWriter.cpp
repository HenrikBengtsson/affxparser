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
#include "calvin_files/writers/src/GenericFileWriter.h"
#include "calvin_files/utils/src/StringUtils.h"
//
#include "util/Fs.h"
//
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_utilities;

GenericFileWriter::GenericFileWriter(FileHeader* p, bool truncate)
{
    fileHdr = p;
    OpenFileOStream(fileHdr->GetFilename(), truncate);
    CreateWriters();
}

GenericFileWriter::~GenericFileWriter()
{
    writers.clear();
    os.flush();
    os.close();
}

int32_t GenericFileWriter::GetFilePos()
{
    return os.tellp();
}

void GenericFileWriter::SeekFromCurrentPos(int32_t offset)
{
    os.seekp(offset, std::ios::cur);
}

void GenericFileWriter::SeekFromBeginPos(int32_t offset)
{
    os.seekp(offset, std::ios::beg);
}

void GenericFileWriter::SeekFromEndPos(int32_t offset)
{
    os.seekp(offset, std::ios::end);
}

void GenericFileWriter::WriteHeader()
{
    FileHeaderWriter writer;
    writer.Write(os, *fileHdr);
    writer.UpdateDataGroupOffset(os, os.tellp());
}

DataGroupWriter& GenericFileWriter::GetDataGroupWriter(int32_t index)
{
    return writers[index];
}

int32_t GenericFileWriter::GetDataGroupCnt()
{
    return (int32_t)writers.size();
}

void GenericFileWriter::GetDataGroupWriters(DataGroupWriterIt &begin, DataGroupWriterIt &end)
{
    begin = writers.begin();
    end = writers.end();
}

void GenericFileWriter::OpenFileOStream(const std::string &file, bool truncate)
{
    if(truncate)
    {
      Fs::aptOpen(os, file, std::ios::out|std::ios::trunc|std::ios::binary);
    }
    else
    {
      Fs::aptOpen(os,file, std::ios::out|std::ios::binary|std::ios::in);
    }
    if (!os.is_open() && !os.good())
    {
        affymetrix_calvin_exceptions::CalvinException e(L"Could not open file \'" + StringUtils::ConvertMBSToWCS(file) + L"\'", -1);
        throw e;
    }
}

void GenericFileWriter::CreateWriters()
{
    int sz = fileHdr->GetDataGroupCnt();
    for(int i = 0; i < sz; i++)
    {
        DataGroupWriter* p = CreateDataGroupWriter(fileHdr->GetDataGroup(i));
        writers.push_back(*p);
        delete p;
    }
}

DataGroupWriter* GenericFileWriter::CreateDataGroupWriter(DataGroupHeader& hdr)
{
    return new DataGroupWriter(&os, &hdr);
}

void GenericFileWriter::Write(u_int32_t p)
{
    FileOutput::WriteUInt32(os, p);
}
