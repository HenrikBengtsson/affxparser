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


#include "calvin_files/data/src/GenericDataHeader.h"
//
#include "calvin_files/parameter/src/AffymetrixParameterConsts.h"
//
#include <algorithm>
//

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
	AddNameValParam(entry, true);
}

/**
This override of the original method of the same name adds the parameter bool doUniqueAdds.

This parameter is a hint flag to determine whether to bypass the FindParameter call within. FindParameter
is used to ensure duplicates do not get entered into the collection. In most cases, the calling application is not updating or 
adding parameter to the collection, but is just reading the header parameters. Through testing it has been found
that bypassing the FindParameter call made, will reduce reads of the header greatly. 

If an application is updating or adding parameters to the collection and require no duplicates to be added, then do not use this
override method, but if you are just reading header parameters, then this will save time reading headers.
*/
void GenericDataHeader::AddNameValParam(const ParameterNameValueType &entry, bool doUniqueAdds)
{
	if(doUniqueAdds == true)
	{
		ParameterNameValueTypeIt ii = FindNameValParam(entry);
		if((ii == nameValParams.end()) == false)
		{
			*ii = entry;
		}
		else
		{
			paramNameIdxMap[entry.GetName()] =  nameValParams.size();
			nameValParams.push_back(entry);
		}
	}
	else
	{
		paramNameIdxMap[entry.GetName()] =  nameValParams.size();
		nameValParams.push_back(entry);
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
  std::map<std::wstring, int>::const_iterator idx = paramNameIdxMap.find(p.GetName());
  if (idx == paramNameIdxMap.end()) {
    return nameValParams.end();
  }
  return nameValParams.begin() + idx->second;

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

