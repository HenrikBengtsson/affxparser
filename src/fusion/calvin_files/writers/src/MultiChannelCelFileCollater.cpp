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

#include "calvin_files/writers/src/MultiChannelCelFileCollater.h"

using namespace affymetrix_calvin_io;

/*
 * Default constructor
 */
MultiChannelCelFileCollater::MultiChannelCelFileCollater()
{
	wavelength = NO_WAVELENGTH;
}

/*
 * Destructor
 */
MultiChannelCelFileCollater::~MultiChannelCelFileCollater() 
{

}

void MultiChannelCelFileCollater::OpenReader(const std::string& file, GenericData& data)
{
	data.Clear();
	GenericFileReader reader;
	reader.SetFilename(file);
	reader.Open(data);
	reader.Close();
}

void MultiChannelCelFileCollater::Collate(const StringVector& celFiles, const std::string& outFile)
{
	FileHeader outFileHdr;
	outFileHdr.SetFilename(outFile);
	CopyHeaders(celFiles, outFileHdr);
	GenericFileWriter writer(&outFileHdr);
	writer.WriteHeader();
	WriteDataGroups(celFiles, writer);
}

void MultiChannelCelFileCollater::CopyHeaders(const StringVector& celFiles, FileHeader& outFileHdr)
{
	//read the first cel file header
	GenericData gReader;
	OpenReader(celFiles[0], gReader);
	CopyFileHdr(gReader.Header(), outFileHdr);
	DataGroupHeader dstGrp;
	CopyDataGroupHdr(gReader.Header().GetDataGroup(0), dstGrp);
	outFileHdr.AddDataGroupHdr(dstGrp);
	for(u_int32_t i = 1; i < celFiles.size(); i++)
	{
		//read the remaining cel file headers
		OpenReader(celFiles[i], gReader);
		DataGroupHeader dstGrp;
		UpdateWavelength(gReader.Header().GetGenericDataHdr());
		CopyDataGroupHdr(gReader.Header().GetDataGroup(0), dstGrp);
		outFileHdr.AddDataGroupHdr(dstGrp);
	}
	gReader.Clear();
}

void MultiChannelCelFileCollater::UpdateWavelength(GenericDataHeader* hdr)
{
	ParameterNameValueType p;
	if(hdr->FindNameValParam(AFFY_FILTER_WAVELENGTH, p))
	{
		wavelength = p.GetValueText();
	}
	else
	{
		wavelength = NO_WAVELENGTH;
	}
}


void MultiChannelCelFileCollater::WriteDataGroups(const StringVector& celFiles, GenericFileWriter& writer)
{

	int dataGroupCnt = writer.GetDataGroupCnt();
	for(int i = 0; i < dataGroupCnt; i++)
	{
		GenericFileReader reader;
		GenericData data;
		reader.SetFilename(celFiles[i]);
		reader.Open(data);
		DataGroupWriter grpWriter = writer.GetDataGroupWriter(i);
                DataGroupReader dgReader = reader.GetDataGroupReader(0);
		WriteDataGroup(dgReader, data.Header().GetDataGroup(0), grpWriter, i >= (dataGroupCnt - 1));
	}
}

void MultiChannelCelFileCollater::WriteDataGroup(DataGroupReader& grpReader, DataGroupHeader& grpHdr, DataGroupWriter& grpWriter, bool lastGroup)
{
	grpWriter.WriteHeader();
	for(int i = 0; i < grpReader.GetDataSetCnt(); i++)
	{
          DataSetReader reader = grpReader.GetDataSetReader(i);
          WriteDataSet(reader, grpHdr.GetDataSet(i), grpWriter.GetDataSetWriter(i));
	}

	if (!lastGroup)
	{
		grpWriter.UpdateNextDataGroupPos();
	}
}

void MultiChannelCelFileCollater::WriteDataSet(DataSetReader& setReader, DataSetHeader& setHdr, DataSetWriter& setWriter)
{
	setWriter.WriteHeader();		
	int sz = setHdr.GetDataSize();
	char* buffer = new char[sz];
	int total = 0;
	while(total < sz)
	{
		int count = setReader.ReadBuffer(buffer, sz);
		setWriter.WriteBuffer(buffer, count);
		total += count;
	}
	setWriter.UpdateNextDataSetOffset();
}

void MultiChannelCelFileCollater::CopyFileHdr(FileHeader& src, FileHeader& dst)
{
	CopyParentHeaders(src.GetGenericDataHdr(), dst.GetGenericDataHdr());
	CopyGenericHeader(src.GetGenericDataHdr(), dst.GetGenericDataHdr());
}

void MultiChannelCelFileCollater::CopyParentHeaders(GenericDataHeader* src, GenericDataHeader* dst)
{
	GenDataHdrVectorIt begin, end;
	src->GetParentIterators(begin, end);
	while(begin != end)
	{
		dst->AddParent(*begin);
		begin++;
	}
}

void MultiChannelCelFileCollater::CopyGenericHeader(GenericDataHeader* src, GenericDataHeader* dst)
{
	dst->SetFileTypeId(MULTI_CHANNEL_FILE_ID);
	CopyGenericHeaderParams(src, dst);
}

void MultiChannelCelFileCollater::CopyGenericHeaderParams(GenericDataHeader* src, GenericDataHeader* dst)
{
	ParameterNameValueTypeIt begin, end;
	src->GetNameValIterators(begin, end);
	while(begin != end)
	{
		if(begin->GetName() == AFFY_FILTER_WAVELENGTH)
		{
			wavelength = begin->GetValueText();
		}
		else
		{
			dst->AddNameValParam(*begin);
		}
		begin++;
	}
}

void MultiChannelCelFileCollater::CopyDataGroupHdr(DataGroupHeader& src, DataGroupHeader& dst)
{
	dst.SetName(wavelength);
	DataSetHdrIt begin, end;
	src.GetDataSetIterators(begin, end);
	while(begin != end)
	{
		DataSetHeader dstSet;
		CopyDataSetHdr(*begin, dstSet);
		dst.AddDataSetHdr(dstSet);
		begin++;
	}
}

void MultiChannelCelFileCollater::CopyDataSetHdr(DataSetHeader& src, DataSetHeader& dst)
{
	dst.SetName(src.GetName());
	dst.SetRowCnt(src.GetRowCnt());
	for(int i = 0; i < src.GetColumnCnt(); i++)
	{
		dst.AddColumn(src.GetColumnInfo(i));
	}
	ParameterNameValueTypeIt begin, end;
	src.GetNameValIterators(begin, end);
	while(begin != end)
	{
		dst.AddNameValParam(*begin);
		begin++;
	}
}
