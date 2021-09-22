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


#include "calvin_files/fusion/src/FusionCHPLegacyData.h"
//
#include "calvin_files/fusion/src/CalvinAdapter/CalvinCHPDataAdapter.h"
#include "calvin_files/fusion/src/FusionTagValuePairType.h"
#include "calvin_files/fusion/src/GCOSAdapter/GCOSCHPDataAdapter.h"
#include "calvin_files/parsers/src/FileException.h"
#include "calvin_files/utils/src/StringUtils.h"
//
#include <cassert>
#include <sys/stat.h>
#include <sys/types.h>
//

using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_exceptions;
using namespace affymetrix_calvin_utilities;

/*! Used to register the CHP reader. */
FusionCHPLegacyData::Reg FusionCHPLegacyData::reg;

/*! The class name. */
static AffymetrixGuidType ObjectName = "FusionCHPLegacyData";


/*! Constructor - register the legacy file type. */
namespace affymetrix_fusion_io
{
FusionCHPLegacyData::Reg ::Reg() : FusionCHPDataReg()
{
  std::list<std::string> ids;
  ids.push_back(CHP_EXPRESSION_ASSAY_TYPE);
  ids.push_back(CHP_RESEQUENCING_ASSAY_TYPE);
  ids.push_back(CHP_GENOTYPING_ASSAY_TYPE);
  ids.push_back(CHP_UNIVERSAL_ASSAY_TYPE);
  ids.push_back("");
  SetFileTypeIds(ids);
}
};

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

/*
 * Convert to the CHP pointer.
 */
FusionCHPLegacyData * FusionCHPLegacyData::FromBase(FusionCHPData *chip)
{
	if (chip != NULL && chip->GetObjectName() == ObjectName)
		return (FusionCHPLegacyData *)chip;
	return NULL;
}

AffymetrixGuidType FusionCHPLegacyData::GetObjectName()
{
	return ObjectName;
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

u_int32_t FusionCHPHeader::SummaryParameterCount()
{
	CheckAdapter();
	return adapter->GetHeader().GetSummaryParameterCount();
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

void FusionCHPHeader::CheckAdapter() const
{
	if (adapter == 0)
	{
		FileNotOpenException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

/////////////////////////////
//// Data

FusionCHPHeader& FusionCHPLegacyData::GetHeader() 
{ 
    CheckAdapter();
    return header; 
}

void FusionCHPLegacyData::CheckAdapter() const
{
	if (adapter == 0)
	{
		FileNotOpenException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

FusionCHPLegacyData::FusionCHPLegacyData()
{
	adapter = 0;
}

FusionCHPLegacyData::~FusionCHPLegacyData()
{
    DeleteAdapter();
}

AffymetrixGuidType FusionCHPLegacyData::FileId()
{
	CheckAdapter();
	return adapter->FileId();
}

GenericData *FusionCHPLegacyData::GetGenericData()
{
	CheckAdapter();
	return adapter->GetGenericData();
}

std::string FusionCHPLegacyData::GetProbeSetName(int index)
{
    CheckAdapter();
    return adapter->GetProbeSetName(index);
}

bool FusionCHPLegacyData::GetExpressionResults(int index, FusionExpressionProbeSetResults& result)
{
	CheckAdapter();
	return adapter->GetExpressionResults(index, result);
}	

bool FusionCHPLegacyData::GetGenotypingResults(int index, FusionGenotypeProbeSetResults& result)
{
	CheckAdapter();
	return adapter->GetGenotypingResults(index, result);
}

bool FusionCHPLegacyData::GetUniversalResults(int index, FusionUniversalProbeSetResults& result)
{
	CheckAdapter();
	return adapter->GetUniversalResults(index, result);
}

bool FusionCHPLegacyData::GetReseqResults(FusionResequencingResults &results)
{
	CheckAdapter();
	return adapter->GetResequencingResults(results);
}

bool FusionCHPLegacyData::Read()
{
	CreateAdapter();
    adapter->SetFileName(filename);
	return adapter->Read();
}

bool FusionCHPLegacyData::ReadHeader()
{
	CreateAdapter();
    adapter->SetFileName(filename);
	return adapter->ReadHeader();
}

void FusionCHPLegacyData::Clear()
{
	CheckAdapter();
    adapter->Clear();

}

void FusionCHPLegacyData::CreateAdapter()
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
					UnableToOpenFileException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
                    if(gcosAdapter!= NULL)
                        delete gcosAdapter;
                    gcosAdapter = 0;
					throw e;
				}
			}
			else
			{
                adapter = 0;
				UnableToOpenFileException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
                if(gcosAdapter!= NULL)
                    delete gcosAdapter;
                gcosAdapter = 0;
				throw e;
			}
		}
	}
}

void FusionCHPLegacyData::DeleteAdapter()
{
	if (adapter)
		delete adapter;
	adapter = 0;
	header.adapter = 0;
}
