/////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Affymetrix, Inc.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License,
// or (at your option) any later version.
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
/////////////////////////////////////////////////////////////////

#include "FusionCHPData.h"
#include "FileException.h"
#include "CalvinCHPDataAdapter.h"
#include "GCOSCHPDataAdapter.h"
#include "FusionTagValuePairType.h"
#include "StringUtils.h"


#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_exceptions;
using namespace affymetrix_calvin_utilities;

/*
 */
FusionCHPHeader::FusionCHPHeader()
{
	adapter = 0;
}

/*
 */
FusionCHPHeader::~FusionCHPHeader()
{

}

bool FusionCHPData::Exists()
{
	assert(filename != "");

	// Find the file stats.
	struct stat st;
	return ((stat(filename.c_str(), &st) == 0)? true: false);
}

// Accessors for header information.
/*
 */
int FusionCHPHeader::GetVersion() const
{
	CheckAdapter();
	return adapter->GetHeader().GetVersion();
}

/*
 */
int FusionCHPHeader::GetCols() const
{
	CheckAdapter();
	return adapter->GetHeader().GetCols();
}

/*
 */
int FusionCHPHeader::GetRows() const
{
	CheckAdapter();
	return adapter->GetHeader().GetRows();
}


int FusionCHPHeader::GetNumProbeSets() const 
{
	CheckAdapter();
	return adapter->GetHeader().GetNumProbeSets(); 
}

AssayType FusionCHPHeader::GetAssayType() const
{
	CheckAdapter();
	return 	adapter->GetHeader().GetAssayType();
}

std::wstring FusionCHPHeader::GetChipType() const
{
	CheckAdapter();
	return adapter->GetHeader().GetChipType();
}

std::wstring FusionCHPHeader::GetAlgName() const 
{ 
	CheckAdapter();
	return adapter->GetHeader().GetAlgName();
}

std::wstring FusionCHPHeader::GetAlgVersion() const 
{ 
	CheckAdapter();
	return adapter->GetHeader().GetAlgVersion();
}


void FusionCHPHeader::AlgorithmParameters(FusionTagValuePairTypeList& valuesFusion)
{ 
	CheckAdapter();
	adapter->GetHeader().GetAlgorithmParameters(valuesFusion);
}
	
u_int32_t FusionCHPHeader::AlgorithmParameterCount()
{
	CheckAdapter();
	return adapter->GetHeader().GetAlgorithmParameterCount();
}

void FusionCHPHeader::SummaryParameters(FusionTagValuePairTypeList& p)
{ 
	CheckAdapter();
	adapter->GetHeader().GetSummaryParameters(p);
}

std::wstring FusionCHPHeader::GetParentCellFile() const
{
	CheckAdapter();
	return adapter->GetHeader().GetParentCellFile();
}

std::wstring FusionCHPHeader::GetProgID() const
{
	CheckAdapter();
	return adapter->GetHeader().GetProgID();
}

std::wstring FusionCHPHeader::GetAlgorithmParameter(const wchar_t* tag)
{
	CheckAdapter();
	return adapter->GetHeader().GetAlgorithmParameter(tag);

}

std::wstring FusionCHPHeader::GetSummaryParameter(const wchar_t* tag)
{
	CheckAdapter();
	return adapter->GetHeader().GetSummaryParameter(tag);
}

void FusionCHPHeader::GetBackgroundZoneInfo(BackgroundZoneInfo& info)
{ 
	CheckAdapter();
	adapter->GetHeader().GetBackgroundZoneInfo(info);
}

void FusionCHPHeader::GetBackgroundZones(BackgroundZoneTypeList& zones)
{ 
	CheckAdapter();
	adapter->GetHeader().GetBackgroundZones(zones);
}

void FusionCHPHeader::GetBackgroundZone(BackgroundZoneType& type, int x, int y)
{
	CheckAdapter();
	adapter->GetHeader().GetBackgroundZone(type,x,y);
}

int FusionCHPHeader::GetMagic() const
{ 
	CheckAdapter();
	return adapter->GetHeader().GetMagic();
}


void FusionCHPHeader::Clear()
{
	adapter = 0;
}


/*
 */
void FusionCHPHeader::CheckAdapter() const
{
	if (adapter == 0)
	{
		FileNotOpenException e;
		throw e;
	}
}




/////////////////////////////
//// Data

FusionCHPHeader& FusionCHPData::GetHeader() 
{ 
    CheckAdapter();
    return header; 
}

void FusionCHPData::CheckAdapter() const
{
	if (adapter == 0)
	{
		FileNotOpenException e;
		throw e;
	}
}

FusionCHPData::FusionCHPData()
{
	adapter = 0;
}

FusionCHPData::~FusionCHPData()
{
    DeleteAdapter();
}


bool FusionCHPData::GetExpressionResults(int index, FusionExpressionProbeSetResults& result)
{
	CheckAdapter();
	return adapter->GetExpressionResults(index, result);
}	

bool FusionCHPData::GetGenotypingResults(int index, FusionGenotypeProbeSetResults& result)
{
	CheckAdapter();
	return adapter->GetGenotypingResults(index, result);
}

bool FusionCHPData::GetUniversalResults(int index, FusionUniversalProbeSetResults& result)
{
	CheckAdapter();
	return adapter->GetUniversalResults(index, result);
}

bool FusionCHPData::Read()
{
	CreateAdapter();
    adapter->SetFileName(filename);
	return adapter->Read();
}

bool FusionCHPData::ReadHeader()
{
	CreateAdapter();
    adapter->SetFileName(filename);
	return adapter->ReadHeader();
}

void FusionCHPData::Clear()
{
	CheckAdapter();
    adapter->Clear();

}

void FusionCHPData::CreateAdapter()
{
	if (adapter)
		return;

	// Create a Calvin adapter
	IFusionCHPDataAdapter* calvinAdapter = new CalvinCHPDataAdapter();
	if (calvinAdapter)
	{
		calvinAdapter->SetFileName(filename);
		if (calvinAdapter->CanReadFile())
		{
			adapter = calvinAdapter;
			header.Clear();
			header.adapter = calvinAdapter;
		}
		else
		{
			delete calvinAdapter;
			IFusionCHPDataAdapter* gcosAdapter = new GCOSCHPDataAdapter();
			if (gcosAdapter)
			{
				gcosAdapter->SetFileName(filename);
				if (gcosAdapter->CanReadFile())
				{
					adapter = gcosAdapter;
					header.Clear();
					header.adapter = gcosAdapter;
				}
				else
				{
                    adapter = 0;
					UnableToOpenFileException e;
                    if(gcosAdapter!= NULL)
                        delete gcosAdapter;
                    gcosAdapter = 0;
					throw e;
				}
			}
			else
			{
                adapter = 0;
				UnableToOpenFileException e;
                if(gcosAdapter!= NULL)
                    delete gcosAdapter;
                gcosAdapter = 0;
				throw e;
			}
		}
	}
}

void FusionCHPData::DeleteAdapter()
{
	if (adapter)
		delete adapter;
	adapter = 0;
	header.adapter = 0;
}

void FusionCHPData::SetFileName(const char *str)
{
	filename = str;
	///TODO: Why is this local and why are we not calling the adapter->SetFileName?????
	///TODO: what happens if the caller sets the filename again???
}

/*
 */
std::string FusionCHPData::GetFileName() const
{
	std::string temp = filename;
	if(adapter != 0)
	{
		temp = adapter->GetFileName();
	}
	return temp;
}