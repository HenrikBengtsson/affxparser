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


#include "AffymetrixParameterConsts.h"
#include "GenericDataHeader.h"
#include <algorithm>

using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_parameter;

GenericDataHeader::GenericDataHeader() 
{
	locale = US_ENGLISH_LOCALE;
}

void GenericDataHeader::Clear()
{
	fileTypeId.clear();
	fileId.clear();
	fileCreationTime.clear();
	nameValParams.clear();
	GenericDataHdrs.clear();
}

void GenericDataHeader::SetFileTypeId(const std::string &p)
{
	fileTypeId = p;
}

std::string GenericDataHeader::GetFileTypeId() const
{
	return fileTypeId;
}

void GenericDataHeader::SetFileId(const affymetrix_calvin_utilities::AffymetrixGuidType &p)
{
	fileId = p;
}

affymetrix_calvin_utilities::AffymetrixGuidType GenericDataHeader::GetFileId() const
{
	return fileId;
}

void GenericDataHeader::SetFileCreationTime(const std::wstring &p)
{
	fileCreationTime = p;
}

std::wstring GenericDataHeader::GetFileCreationTime() const
{
	return fileCreationTime;
}

void GenericDataHeader::SetLocale(const std::wstring &p)
{
	locale = p;
}

std::wstring GenericDataHeader::GetLocale() const
{
	return locale;
}

void GenericDataHeader::AddNameValParam(const ParameterNameValueType &entry)
{
	ParameterNameValueTypeIt ii = FindNameValParam(entry);
	if (ii == nameValParams.end())
	{
		// Add
		nameValParams.push_back(entry);
	}
	else
	{
		// Update
		*ii = entry;
	}
}

ParameterNameValueType GenericDataHeader::GetNameValParam(int32_t index)
{
	return nameValParams[index];
}

int32_t GenericDataHeader::GetNameValParamCnt() const
{
	return (int32_t)nameValParams.size();
}

void GenericDataHeader::GetNameValIterators(ParameterNameValueTypeIt &begin, ParameterNameValueTypeIt &end)
{
	begin = nameValParams.begin();
	end = nameValParams.end();
}

void GenericDataHeader::AddParent(const GenericDataHeader &hdr)
{
	GenericDataHdrs.push_back(hdr);
}

void GenericDataHeader::GetParentIterators(GenDataHdrVectorIt &begin, GenDataHdrVectorIt &end)
{
	begin = GenericDataHdrs.begin();
	end = GenericDataHdrs.end();
}

int32_t GenericDataHeader::GetParentCnt() const
{
	return (int32_t)GenericDataHdrs.size();
}

GenericDataHeader GenericDataHeader::GetParent(int32_t index) const
{
	return GenericDataHdrs[index];
}

/*
 * Find an immediate parent GenericDataHeader based on file type id.  Does not search grand-parents or above.
 */
GenericDataHeader* GenericDataHeader::FindParent(const std::string& fileTypeId)
{
	GenericDataHeader* parentGDH = 0;
	GenDataHdrVectorIt begin, end;
	GetParentIterators(begin, end);
	for (GenDataHdrVectorIt ii = begin; ii != end; ++ii)
	{
		if (ii->GetFileTypeId() == fileTypeId)
		{
			parentGDH = &(*ii);
			break;
		}
	}
	return parentGDH;
}

bool GenericDataHeader::FindNameValParam(const std::wstring& name, ParameterNameValueType& result)
{
	ParameterNameValueType t;
	t.SetName(name);
	t.SetValueText(L"");
	ParameterNameValueTypeIt found = FindNameValParam(t);
	if (found != nameValParams.end())
	{
		result = *found;
		return true;
	}
	return false;
}

ParameterNameValueTypeIt GenericDataHeader::FindNameValParam(const ParameterNameValueType& p)
{
	ParameterNameValueTypeIt begin, end;
	begin = nameValParams.begin();
	end = nameValParams.end();

	ParameterNameValueTypeIt ii = std::find(begin, end, p);
	if (ii != end)
	{
		return ii;
	}
	else
		return end;
}

bool GenericDataHeader::GetNameValParamsBeginsWith(const std::wstring& beginsWith, ParameterNameValueTypeVector& p)
{
	p.clear();

	for (ParameterNameValueTypeIt ii = nameValParams.begin(); ii != nameValParams.end(); ++ii)
	{
		if (ii->GetName().find(beginsWith) == 0)
		{
			p.push_back(*ii);
		}
	}
	return (p.size() > 0);
}

