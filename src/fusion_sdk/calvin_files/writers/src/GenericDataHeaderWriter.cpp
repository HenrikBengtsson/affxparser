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


#include "calvin_files/writers/src/GenericDataHeaderWriter.h"
//
#include "calvin_files/writers/src/FileOutput.h"
//

using namespace affymetrix_calvin_io;

void GenericDataHeaderWriter::Write(std::ofstream &os, GenericDataHeader &g)
{
	WriteFileTypeId(os, g);
	WriteFileId(os, g);
	WriteFileCreationTime(os, g);
	WriteLocale(os, g);
	WriteNameValParamCnt(os, g);
	WriteNameValParams(os, g);
	WriteParentHdrCnt(os, g);
	WriteParentHdrs(os, g);
}

void GenericDataHeaderWriter::WriteFileTypeId(std::ofstream &os, const GenericDataHeader &g) const
{
	FileOutput::WriteString8(os, g.GetFileTypeId());
}

void GenericDataHeaderWriter::WriteFileId(std::ofstream &os, const GenericDataHeader &g) const
{
	FileOutput::WriteString8(os, g.GetFileId());
}

void GenericDataHeaderWriter::WriteFileCreationTime(std::ofstream &os, const GenericDataHeader &g) const
{
	FileOutput::WriteString16(os, g.GetFileCreationTime());
}

void GenericDataHeaderWriter::WriteLocale(std::ofstream &os, const GenericDataHeader &g) const
{
	FileOutput::WriteString16(os, g.GetLocale());
}

void GenericDataHeaderWriter::WriteNameValParamCnt(std::ofstream &os, const GenericDataHeader &g) const
{
	FileOutput::WriteInt32(os, g.GetNameValParamCnt());
}

void GenericDataHeaderWriter::WriteNameValParams(std::ofstream &os, GenericDataHeader &g)
{
	ParameterNameValueTypeIt begin;
	ParameterNameValueTypeIt end;
	g.GetNameValIterators(begin, end);
	while(begin != end)
	{
		FileOutput::WriteString16(os, begin->GetName());
		MIMEValue mv = begin->GetMIMEValue();
		u_int32_t sz;
		const void* ptr = mv.GetValue(sz);
		FileOutput::WriteBlob(os, ptr, sz);
		FileOutput::WriteString16(os, begin->GetMIMEType());
		begin++;
	}
}

void GenericDataHeaderWriter::WriteParentHdrCnt(std::ofstream &os, const GenericDataHeader &g) const
{
	FileOutput::WriteInt32(os, g.GetParentCnt());
}

void GenericDataHeaderWriter::WriteParentHdrs(std::ofstream &os, GenericDataHeader &g)
{
	GenDataHdrVectorIt begin;
	GenDataHdrVectorIt end;
	g.GetParentIterators(begin, end);
	while(begin != end)
	{
		Write(os, *begin);
		begin++;
	}
}
