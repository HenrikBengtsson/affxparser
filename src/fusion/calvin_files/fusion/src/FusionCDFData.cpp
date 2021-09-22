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


#include "calvin_files/fusion/src/FusionCDFData.h"
//
#include "calvin_files/fusion/src/FusionCDFQCProbeSetNames.h"
#include "calvin_files/parsers/src/CDFFileReader.h"
#include "calvin_files/parsers/src/GenericFileReader.h"
#include "calvin_files/utils/src/FileUtils.h"
#include "calvin_files/utils/src/StringUtils.h"
//
#include <cassert>
//

using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_utilities;
using namespace affymetrix_calvin_parameter;

////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Get CDF Format Version.
 */
int FusionCDFFileHeader::GetFormatVersion() const
{
	if (gcosHeader)
		return gcosHeader->GetFormatVersion();
	else if (calvinData)
		return calvinData->GetFormatVersion();
	else
		return 0;
}

/*
 * Get the CDF GUID.
 */
std::string FusionCDFFileHeader::GetGUID() const
{
	if (gcosHeader)
		return gcosHeader->GetGUID();
	else
		return std::string("");
}

/*
 * Get the integrity md5.
 */
std::string FusionCDFFileHeader::GetIntegrityMd5() const
{
	if (gcosHeader)
		return gcosHeader->GetIntegrityMd5();
	else
		return std::string("");
}

/*
 * Get the number of feature columns in the array.
 */
int FusionCDFFileHeader::GetCols() const
{
	if (gcosHeader)
		return gcosHeader->GetCols();
	else if (calvinData)
		return calvinData->GetArrayCols();
	else
		return 0;
}

/*
 * Get the number of feature rows in the array.
 */
int FusionCDFFileHeader::GetRows() const
{
	if (gcosHeader)
		return gcosHeader->GetRows();
	else if (calvinData)
		return calvinData->GetArrayRows();
	else
		return 0;
}

/*
 * Get the number of probe sets.
 */
int FusionCDFFileHeader::GetNumProbeSets() const
{
	if (gcosHeader)
		return gcosHeader->GetNumProbeSets();
	else if (calvinData && calvinData->GetGenericData().Header().GetGenericDataHdr()->GetFileTypeId() != AFFY_CNTRL_PS)
		return calvinData->GetProbeSetCnt();
	else
		return 0;
}

/*
 * Get the number of QC probe sets.
 */
int FusionCDFFileHeader::GetNumQCProbeSets() const
{
	if (gcosHeader)
		return gcosHeader->GetNumQCProbeSets();
	else if (calvinData && calvinData->GetGenericData().Header().GetGenericDataHdr()->GetFileTypeId() == AFFY_CNTRL_PS)
		return calvinData->GetProbeSetCnt();
	else
		return 0;
}

/*
 * Get the reference sequence (for resequencing arrays only).
 */
std::string &FusionCDFFileHeader::GetReference()
{
	return ref;
}

/*
 * Initializes the class based on GCOS information.
 */
void FusionCDFFileHeader::Initialize(affxcdf::CCDFFileData *data)
{
	gcosHeader = &data->GetHeader();
	calvinData = NULL;
	ref = gcosHeader->GetReference();
}

/*
 * Initializes the class based on Calvin information.
 */
void FusionCDFFileHeader::Initialize(affymetrix_calvin_io::CDFData *data)
{
	gcosHeader = NULL;
	calvinData = data;
	ref = calvinData->GetRefSequence();
}

/*
 * Constructor
 */
FusionCDFFileHeader::FusionCDFFileHeader()
{
	gcosHeader = NULL;
	calvinData = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Initialize the class members.
 */
FusionCDFQCProbeInformation::FusionCDFQCProbeInformation()
{
	calvinProbe = NULL;
	gcosProbe = NULL;
}

/*
 * Deallocate memory.
 */
FusionCDFQCProbeInformation::~FusionCDFQCProbeInformation()
{
	Clear();
}

/*
 * Initialize the class members.
 */
void FusionCDFQCProbeInformation::Initialize(int index, affxcdf::CCDFQCProbeSetInformation *gcosSet)
{
	Clear();
	gcosProbe = new affxcdf::CCDFQCProbeInformation;
	gcosSet->GetProbeInformation(index, *gcosProbe);
}

/*
 * Initialize the class members.
 */
void FusionCDFQCProbeInformation::Initialize(int index, CDFQCProbeSetInformation *calvinSet)
{
	Clear();
	calvinProbe = new CDFQCProbeInformation;
	calvinSet->GetProbeInformation(index, *calvinProbe);
}

/*
 * Clears the members.
 */
void FusionCDFQCProbeInformation::Clear()
{
	delete calvinProbe;
	calvinProbe = NULL;
	delete gcosProbe;
	gcosProbe = NULL;
}

/*! Gets the X cooridnate of the probe.
 */
int FusionCDFQCProbeInformation::GetX() const
{
	if (gcosProbe)
		return gcosProbe->GetX();
	else if (calvinProbe)
		return calvinProbe->GetX();
	else
		return 0;
}

/*! Gets the Y cooridnate of the probe.
 */
int FusionCDFQCProbeInformation::GetY() const
{
	if (gcosProbe)
		return gcosProbe->GetY();
	else if (calvinProbe)
		return calvinProbe->GetY();
	else
		return 0;
}

/*! Gets the probe length.
 */
int FusionCDFQCProbeInformation::GetPLen() const
{
	if (gcosProbe)
		return gcosProbe->GetPLen();
	else if (calvinProbe)
		return calvinProbe->GetPLen();
	else
		return 0;
}

/*! Gets the flag indicating if the probe is a perfect match probe.
 */
bool FusionCDFQCProbeInformation::IsPerfectMatchProbe() const
{
	if (gcosProbe)
		return gcosProbe->IsPerfectMatchProbe();
	else if (calvinProbe)
		return calvinProbe->IsPerfectMatchProbe();
	else
		return false;
}

/*! Gets a flag indicating if the probe is used for background calculations (blank feature).
 */
bool FusionCDFQCProbeInformation::IsBackgroundProbe() const
{
	if (gcosProbe)
		return gcosProbe->IsBackgroundProbe();
	else if (calvinProbe)
		return calvinProbe->IsBackgroundProbe();
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Get the probe set type.
 */
affxcdf::GeneChipQCProbeSetType FusionCDFQCProbeSetInformation::GetQCProbeSetType() const
{
	if (gcosSet)
		return gcosSet->GetQCProbeSetType();
	else if (calvinSet)
	{
		affxcdf::GeneChipQCProbeSetType qcType=affxcdf::UnknownQCProbeSetType;
		const std::wstring &qc = calvinSet->GetQCProbeSetType();
		for (int i=(int)affxcdf::UnknownQCProbeSetType; i<=(int)affxcdf::SpatialNormalizationPositiveQCProbeSetType; i++)
		{
			if (qc == FusionCDFQCProbeSetNames::GetStaticCDFQCProbeSetName((affxcdf::GeneChipQCProbeSetType)i))
			{
				qcType = (affxcdf::GeneChipQCProbeSetType)i;
				break;
			}
		}
		return qcType;
	}
	else
		return affxcdf::UnknownQCProbeSetType;
}

/*
 * Get the number of probes in the set.
 */
int FusionCDFQCProbeSetInformation::GetNumCells() const
{
	if (gcosSet)
		return gcosSet->GetNumCells();
	else if (calvinSet)
		return calvinSet->GetNumCells();
	else
		return 0;
}

/*
 * Get the information about a single probe in the set.
 */
void FusionCDFQCProbeSetInformation::GetProbeInformation(int index, FusionCDFQCProbeInformation & info)
{
	if (gcosSet)
		info.Initialize(index, gcosSet);
	else if (calvinSet)
		info.Initialize(index, calvinSet);
	else
		info.Clear();
}

/*
 * Initialize the class.
 */
FusionCDFQCProbeSetInformation::FusionCDFQCProbeSetInformation()
{
	gcosSet = NULL;
	calvinSet = NULL;
}

/*
 * Deallocate any used memory.
 */
FusionCDFQCProbeSetInformation::~FusionCDFQCProbeSetInformation()
{
	Clear();
}

/*
 * Create a GCOS QC probe set object and retrieve it from the CDF object.
 */
void FusionCDFQCProbeSetInformation::Initialize(int index, affxcdf::CCDFFileData *cdf)
{
	Clear();
	gcosSet = new affxcdf::CCDFQCProbeSetInformation;
	cdf->GetQCProbeSetInformation(index, *gcosSet);
}

/*
 * Create a GCOS QC probe set object and retrieve it from the CDF object.
 */
void FusionCDFQCProbeSetInformation::Initialize(affxcdf::GeneChipQCProbeSetType qcType, affxcdf::CCDFFileData *cdf)
{
	Clear();
	gcosSet = new affxcdf::CCDFQCProbeSetInformation;
	cdf->GetQCProbeSetInformation(qcType, *gcosSet);
}

/*
 * Create a Calvin QC probe set object and retrieve it from the CDF object.
 */
void FusionCDFQCProbeSetInformation::Initialize(int index, CDFData *cdf)
{
	Clear();
	calvinSet = new CDFQCProbeSetInformation;
	cdf->GetQCProbeSetInformation(index, *calvinSet);
}

/*
 * Create a Calvin QC probe set object and retrieve it from the CDF object.
 */
void FusionCDFQCProbeSetInformation::Initialize(affxcdf::GeneChipQCProbeSetType qcType, CDFData *cdf)
{
	Clear();
	calvinSet = new CDFQCProbeSetInformation;
	cdf->GetQCProbeSetInformation(qcType, *calvinSet);
}

/*
 * Deallocate any used memory.
 */
void FusionCDFQCProbeSetInformation::Clear()
{
	delete calvinSet;
	calvinSet = NULL;
	delete gcosSet;
	gcosSet = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Return the list index.
 */
int FusionCDFProbeInformation::GetListIndex() const
{
	if (gcosProbe)
		return gcosProbe->GetListIndex();
	else if (calvinProbe)
		return calvinProbe->GetListIndex();
	else
		return 0;
}

/*
 * Return the expos value.
 */
int FusionCDFProbeInformation::GetExpos() const
{
	if (gcosProbe)
		return gcosProbe->GetExpos();
	else if (calvinProbe)
		return calvinProbe->GetExpos();
	else
		return 0;
}

/*
 * Return the X coordinate.
 */
int FusionCDFProbeInformation::GetX() const
{
	if (gcosProbe)
		return gcosProbe->GetX();
	else if (calvinProbe)
		return calvinProbe->GetX();
	else
		return 0;
}

/*
 * Return the Y coordinate.
 */
int FusionCDFProbeInformation::GetY() const
{
	if (gcosProbe)
		return gcosProbe->GetY();
	else if (calvinProbe)
		return calvinProbe->GetY();
	else
		return 0;
}

/*
 * Return the probes base at the interrogation position.
 */
char FusionCDFProbeInformation::GetPBase() const
{
	if (gcosProbe)
		return gcosProbe->GetPBase();
	else if (calvinProbe)
		return (char) calvinProbe->GetPBase();
	else
		//return NULL;
    return 0;
}

/*
 * Return the targets base at the interrogation position.
 */
char FusionCDFProbeInformation::GetTBase() const
{
	if (gcosProbe)
		return gcosProbe->GetTBase();
	else if (calvinProbe)
		return (char) calvinProbe->GetTBase();
	else
		//return NULL;
    return 0;
}

/*
 * Return the probe length.
 */
unsigned short FusionCDFProbeInformation::GetProbeLength() const
{
	if (gcosProbe)
		return gcosProbe->GetProbeLength();
	else if (calvinProbe)
		return calvinProbe->GetProbeLength();
	else
		return 0;
}

/*
 * Return the probe grouping.
 */
unsigned short FusionCDFProbeInformation::GetProbeGrouping() const
{
	if (gcosProbe)
		return gcosProbe->GetProbeGrouping();
	else if (calvinProbe)
		return calvinProbe->GetProbeGrouping();
	else
		return 0;
}

/*
 * Initialize the class members.
 */
FusionCDFProbeInformation::FusionCDFProbeInformation()
{
	gcosProbe = NULL;
	calvinProbe = NULL;
}

/*
 * Deallocate any memory.
 */
FusionCDFProbeInformation::~FusionCDFProbeInformation()
{
	Clear();
}

/*
 * Initialize the class members.
 */
void FusionCDFProbeInformation::Initialize(int index, affxcdf::CCDFProbeGroupInformation *gcosGroup)
{
	Clear();
	gcosProbe = new affxcdf::CCDFProbeInformation;
	gcosGroup->GetCell(index, *gcosProbe);
}

/*
 * Initialize the class members.
 */
void FusionCDFProbeInformation::Initialize(int index, affymetrix_calvin_io::CDFProbeGroupInformation *calvinGroup)
{
	Clear();
	calvinProbe = new CDFProbeInformation;
	calvinGroup->GetCell(index, *calvinProbe);
}

/*
 * Clears the members.
 */
void FusionCDFProbeInformation::Clear()
{
	delete calvinProbe;
	calvinProbe = NULL;
	delete gcosProbe;
	gcosProbe = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Get the groups direction.
 */
affxcdf::DirectionType FusionCDFProbeGroupInformation::GetDirection() const
{
	if (gcosGroup)
		return gcosGroup->GetDirection();
	else if (calvinGroup)
	{
		DirectionType dir = calvinGroup->GetDirection();
		switch (dir)
		{
		case ProbeNoDirection:
			return affxcdf::NoDirection;
			break;

		case ProbeSenseDirection:
			return affxcdf::SenseDirection;
			break;

		case ProbeAntiSenseDirection:
			return affxcdf::AntiSenseDirection;
			break;

		case ProbeEitherDirection:
			return affxcdf::EitherDirection;
			break;

		default:
			return affxcdf::NoDirection;
			break;
		}
	}
	else
		return affxcdf::NoDirection;
}

/*
 * Get the number of lists (atoms) in the group.
 */
int FusionCDFProbeGroupInformation::GetNumLists() const
{
	if (gcosGroup)
		return gcosGroup->GetNumLists();
	else if (calvinGroup)
		return calvinGroup->GetNumLists();
	else
		return 0;
}

/*
 * Get the number of probes in the group.
 */
int FusionCDFProbeGroupInformation::GetNumCells() const
{
	if (gcosGroup)
		return gcosGroup->GetNumCells();
	else if (calvinGroup)
		return calvinGroup->GetNumCells();
	else
		return 0;
}

/*
 * Get the number of probes per list.
 */
int FusionCDFProbeGroupInformation::GetNumCellsPerList() const
{
	if (gcosGroup)
		return gcosGroup->GetNumCellsPerList();
	else if (calvinGroup)
		return calvinGroup->GetNumCellsPerList();
	else
		return 0;
}

/*
 * Get the start list index value.
 */
int FusionCDFProbeGroupInformation::GetStart() const
{
	if (gcosGroup)
		return gcosGroup->GetStart();
	else if (calvinGroup)
	{
		CDFProbeInformation probeInfo;
		calvinGroup->GetCell(0, probeInfo);
		return probeInfo.GetListIndex();
	}
	else
		return 0;
}

/*
 * Get the stop list index value.
 */
int FusionCDFProbeGroupInformation::GetStop() const
{
	if (gcosGroup)
		return gcosGroup->GetStop();
	else if (calvinGroup)
	{
		CDFProbeInformation probeInfo;
		calvinGroup->GetCell(calvinGroup->GetNumCells()-1, probeInfo);
		return probeInfo.GetListIndex();
	}
	else
		return 0;
}

/*
 * Get the group name.
 */
std::string FusionCDFProbeGroupInformation::GetName() const
{
	if (gcosGroup)
		return gcosGroup->GetName();
	else if (calvinGroup)
		return StringUtils::ConvertWCSToMBS(calvinGroup->GetName());
	else
		return "";
}

/*
 * Get the wobble situation.
 */
unsigned short FusionCDFProbeGroupInformation::GetWobbleSituation() const
{
	if (gcosGroup)
		return gcosGroup->GetWobbleSituation();
	else if (calvinGroup)
		return calvinGroup->GetWobbleSituation();
	else
		return 0;
}

/*
 * Get the allele code.
 */
unsigned short FusionCDFProbeGroupInformation::GetAlleleCode() const
{
	if (gcosGroup)
		return gcosGroup->GetAlleleCode();
	else if (calvinGroup)
		return calvinGroup->GetAlleleCode();
	else
		return 0;
}

/*
 * Get the channel.
 */
unsigned char FusionCDFProbeGroupInformation::GetChannel() const
{
	if (gcosGroup)
		return gcosGroup->GetChannel();
	else if (calvinGroup)
		return calvinGroup->GetChannel();
	else
		return 0;
}

/*
 * Get the probe replication type.
 */
affxcdf::ReplicationType FusionCDFProbeGroupInformation::GetRepType() const
{
	if (gcosGroup)
		return gcosGroup->GetRepType();
	else if (calvinGroup)
	{
		ReplicationType rep = calvinGroup->GetRepType();
		switch (rep)
		{
		case UnknownProbeRepType:
			return affxcdf::UnknownRepType;
			break;

		case DifferentProbeRepType:
			return affxcdf::DifferentRepType;
			break;

		case MixedProbeRepType:
			return affxcdf::MixedRepType;
			break;

		case IdenticalProbeRepType:
			return affxcdf::IdenticalRepType;
			break;

		default:
			return affxcdf::UnknownRepType;
			break;
		}
	}
	else
		return affxcdf::UnknownRepType;
}

/*
 * Retrieve the probe object given the index.
 */
void FusionCDFProbeGroupInformation::GetCell(int cell_index, FusionCDFProbeInformation & info)
{
	if (gcosGroup)
		info.Initialize(cell_index, gcosGroup);
	else if (calvinGroup)
		info.Initialize(cell_index, calvinGroup);
	else
		info.Clear();
}

/*
 * Initialize the numbers to NULL values.
 */
FusionCDFProbeGroupInformation::FusionCDFProbeGroupInformation()
{
	gcosGroup = NULL;
	calvinGroup = NULL;
}

/*
 * Deallocate any used memory.
 */
FusionCDFProbeGroupInformation::~FusionCDFProbeGroupInformation()
{
	Clear();
}

/*
 * Deallocate any used memory.
 */
void FusionCDFProbeGroupInformation::Clear()
{
	delete calvinGroup;
	calvinGroup = NULL;
	delete gcosGroup;
	gcosGroup = NULL;
}

/*
 * Get the group information.
 */
void FusionCDFProbeGroupInformation::Initialize(int index, affxcdf::CCDFProbeSetInformation *gcosSet)
{
	Clear();
	gcosGroup = new affxcdf::CCDFProbeGroupInformation;
	gcosSet->GetGroupInformation(index, *gcosGroup);
}

/*
 * Get the group information.
 */
void FusionCDFProbeGroupInformation::Initialize(int index, CDFProbeSetInformation *calvinSet)
{
	Clear();
	calvinGroup = new CDFProbeGroupInformation;
	calvinSet->GetGroupInformation(index, *calvinGroup);
}

////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Initialize the variables to NULL values.
 */
FusionCDFProbeSetInformation::FusionCDFProbeSetInformation()
{
	gcosSet = NULL;
	calvinSet = NULL;
}

/*
 * Deallocate member variables.
 */
FusionCDFProbeSetInformation::~FusionCDFProbeSetInformation()
{
	Clear();
}

/*
 * Deallocate member variables.
 */
void FusionCDFProbeSetInformation::Clear()
{
	delete gcosSet;
	gcosSet = NULL;
	delete calvinSet;
	calvinSet = NULL;
}

/*
 * Initializes the class for GCOS file reading.
 */
void FusionCDFProbeSetInformation::Initialize(int index, affxcdf::CCDFFileData *cdf)
{
	Clear();
	gcosSet = new affxcdf::CCDFProbeSetInformation;
	cdf->GetProbeSetInformation(index, *gcosSet);
}

/*
 * Initializes the class for Calvin file reading.
 */
void FusionCDFProbeSetInformation::Initialize(int index, CDFData *cdf)
{
	Clear();
	calvinSet = new CDFProbeSetInformation;
	cdf->GetProbeSetInformation(index, *calvinSet);
}

/*
 * Gets the probe set type.
 */
affxcdf::GeneChipProbeSetType FusionCDFProbeSetInformation::GetProbeSetType()
{
	if (gcosSet)
		return gcosSet->GetProbeSetType();
	else  if (calvinSet)
	{
		CDFDataTypeIds psType = calvinSet->GetProbeSetType();
		switch (psType)
		{
		case Expression:
			return affxcdf::ExpressionProbeSetType;
			break;
			
		case Genotyping:
			return affxcdf::GenotypingProbeSetType;
			break;

		case Tag:
			return affxcdf::TagProbeSetType;
			break;

		case Resequencing:
			return affxcdf::ResequencingProbeSetType;
			break;

        case CopyNumber:
            return affxcdf::CopyNumberProbeSetType;
            break;

        case GenotypeControl:
            return affxcdf::GenotypeControlProbeSetType;
            break;

        case ExpressionControl:
            return affxcdf::ExpressionControlProbeSetType;
            break;

        case Marker:
			return affxcdf::MarkerProbeSetType;
            break;

        case MultichannelMarker:
			return affxcdf::MultichannelMarkerProbeSetType;
            break;

		default:
			return affxcdf::UnknownProbeSetType;
			break;
		}
	}
	else
		return affxcdf::UnknownProbeSetType;
}

/*
 * Get the probe sets direction.
 */
affxcdf::DirectionType FusionCDFProbeSetInformation::GetDirection() const
{
	if (gcosSet)
		return gcosSet->GetDirection();
	else if (calvinSet)
	{
		DirectionType dir = calvinSet->GetDirection();
		switch (dir)
		{
		case ProbeNoDirection:
			return affxcdf::NoDirection;
			break;

		case ProbeSenseDirection:
			return affxcdf::SenseDirection;
			break;

		case ProbeAntiSenseDirection:
			return affxcdf::AntiSenseDirection;
			break;

		case ProbeEitherDirection:
            return affxcdf::EitherDirection;
			break;

		default:
			return affxcdf::NoDirection;
			break;
		}
	}
	else
		return affxcdf::NoDirection;
}

/*
 * Get the number of lists (atoms) in the group.
 */
int FusionCDFProbeSetInformation::GetNumLists() const
{
	if (gcosSet)
		return gcosSet->GetNumLists();
	else if (calvinSet)
		return calvinSet->GetNumLists();
	else
		return 0;
}

/*
 * Get the number of groups in the set.
 */
int FusionCDFProbeSetInformation::GetNumGroups() const
{
	if (gcosSet)
		return gcosSet->GetNumGroups();
	else if (calvinSet)
		return calvinSet->GetNumGroups();
	else
		return 0;
}

/*
 * Get the number of probes in the set.
 */
int FusionCDFProbeSetInformation::GetNumCells() const
{
	if (gcosSet)
		return gcosSet->GetNumCells();
	else if (calvinSet)
		return calvinSet->GetNumCells();
	else
		return 0;
}

/*
 * Get the number of probes per list.
 */
int FusionCDFProbeSetInformation::GetNumCellsPerList() const
{
	if (gcosSet)
		return gcosSet->GetNumCellsPerList();
	else if (calvinSet)
		return calvinSet->GetNumCellsPerList();
	else
		return 0;
}

/*
 * Get the probe set number.
 */
int FusionCDFProbeSetInformation::GetProbeSetNumber() const
{
	if (gcosSet)
		return gcosSet->GetProbeSetNumber();
	else if (calvinSet)
		return calvinSet->GetProbeSetNumber();
	else
		return 0;
}

/*
 * Get the group object.
 */
void FusionCDFProbeSetInformation::GetGroupInformation(int index, FusionCDFProbeGroupInformation & info)
{
	if (gcosSet)
		info.Initialize(index, gcosSet);
	else if (calvinSet)
		info.Initialize(index, calvinSet);
	else
		info.Clear();
}


////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Check the type and create the appropriate parser.
 */
void FusionCDFData::CreateObject()
{
	Close();
	if (FusionCDFData::IsCalvinCompatibleFile(fileName.c_str()) == false)
	{
		gcosData = new affxcdf::CCDFFileData;
	}
	else
	{
		calvinData = new affymetrix_calvin_io::CDFData;
	}
}

/*
 * Constructor
 */
FusionCDFData::FusionCDFData()
{
	gcosData = NULL;
	calvinData = NULL;
}

/*
 * Destructor
 */
FusionCDFData::~FusionCDFData()
{
	Close();
}

/*
 * Set the name of the file.
 */
void FusionCDFData::SetFileName(const char *name)
{
	fileName = name;
}

/*
 * Get the name of the file.
 */
std::string FusionCDFData::GetFileName() const
{
	return fileName;
}

/*
 * Get the header object.
 */
FusionCDFFileHeader &FusionCDFData::GetHeader()
{
	if (gcosData)
	{
		header.Initialize(gcosData);
	}
	else if (calvinData)
	{
		header.Initialize(calvinData);
	}
	return header;
}

/*
 * Get the GUID of the CDF file.
 * This only applies to CDF XDA format version >= 4.
 */
std::string FusionCDFData::GetGUID()
{
	if (!gcosData && !calvinData)
		CreateObject();
	if (gcosData)
	{
		gcosData->SetFileName(fileName.c_str());
		return gcosData->GetGUID();
	}
	else
		return std::string("");
}

/*
 * Get the integrity md5 of the CDF file.
 */
std::string FusionCDFData::GetIntegrityMd5()
{
	if (!gcosData && !calvinData)
		CreateObject();
	if (gcosData)
	{
		gcosData->SetFileName(fileName.c_str());
		return gcosData->GetIntegrityMd5();
	}
	else
		return std::string("");
}

/*
 * Get the error string.
 */
std::string FusionCDFData::GetError() const
{
	if (gcosData)
		return gcosData->GetError();
	else
		return "";
}

/*
 * Get the name of a probe set.
 */
std::string FusionCDFData::GetProbeSetName(int index) const
{
	if (gcosData)
		return gcosData->GetProbeSetName(index);
	else if (calvinData)
		return StringUtils::ConvertWCSToMBS(calvinData->GetProbeSetName(index));
	else
		return std::string("");
}

/*
 * Get the chip type (probe array type) of the CDF file.
 * This is the name of the file without extension for CDF XDA format version < 4.
 * For CDF XDA format version >= 4, this is array name without version.
 */
std::string FusionCDFData::GetChipType()
{
	if (!gcosData && !calvinData)
		CreateObject();
	if (gcosData)
	{
		gcosData->SetFileName(fileName.c_str());
		return gcosData->GetChipType();
	}
	else
	{
		int index = (int) fileName.rfind('\\');
		if (index == -1)
			index = (int) fileName.rfind('/');
		std::string chiptype = fileName.c_str() + index + 1;
		chiptype.resize(chiptype.length()-4);
		return chiptype;
	}
}

/*
 * Get the chip type (probe array type) of the CDF file.
 * This is the name of the file without extension for CDF XDA format version < 4. We
 * also include all substrings create by removing
 * characters to the right of each '.'
 * For CDF XDA format version >= 4, this is retrieved from the file header.
 */
std::vector<std::string> FusionCDFData::GetChipTypes()
{
	if (!gcosData && !calvinData)
		CreateObject();
	if (gcosData)
	{
		gcosData->SetFileName(fileName.c_str());
		return gcosData->GetChipTypes();
	}
	else
	{
		std::vector<std::string> chiptypes;
		std::string chiptype;

		int index = (int) fileName.rfind('\\');
		if (index == -1)
			index = (int) fileName.rfind('/');
		chiptype = fileName.c_str() + index + 1;
		chiptype.resize(chiptype.length()-4);

		// The full file name (minus .cdf extension) is the default (1st) 
		// chip type. This matches what GetChipType() returns.
		// ie: foo.bar.v1.r2.cdf -> foo.bar.v1.r2
		chiptypes.push_back(chiptype);

		//We then add all substrings starting at zero and ending at '.'
		// ie: foo.bar.v1.r2.cdf -> foo.bar.v1, foo.bar, foo
		std::string::size_type pos = chiptype.rfind(".",chiptype.size()-1);
		while (pos != std::string::npos){
			if(pos>0)
				chiptypes.push_back(chiptype.substr(0,pos));
			pos = chiptype.rfind(".",pos-1);
		}

		//ie: foo.bar.v1.r2, foo.bar.v1, foo.bar, foo
		return chiptypes;
	}
}

/*
 * Read the entire file.
 */
bool FusionCDFData::Read()
{
	CreateObject();
	if (gcosData)
	{
		gcosData->SetFileName(fileName.c_str());
		return gcosData->Read();
	}
	else
	{
		CDFFileReader reader;
		reader.SetFilename(fileName);
		try
		{
			reader.Read(*calvinData);
		}
		catch(...)
		{
			return false;
		}
		return true;
	}
}

/*
 * Read the header of the file only. Read a calvin file in its entirety - this really
 * reads the header until the data groups are opened.
 */
bool FusionCDFData::ReadHeader()
{
	CreateObject();
	if (gcosData)
	{
		gcosData->SetFileName(fileName.c_str());
		return gcosData->ReadHeader();
	}
	else
	{
		CDFFileReader reader;
		reader.SetFilename(fileName);
		try
		{
			reader.Read(*calvinData);
		}
		catch(...)
		{
			return false;
		}
		return true;
	}
}

/*
 * Check if the file exists.
 */
bool FusionCDFData::Exists()
{
	return FileUtils::Exists(fileName.c_str());
}

/*! Deallocates memory and closes any file handles. */
void FusionCDFData::Close()
{
	if (gcosData)
	{
		gcosData->Close();
		delete gcosData;
		gcosData = NULL;
	}
	if (calvinData)
	{
		delete calvinData;
		calvinData = NULL;
	}
}

/*
 * Determines if a CDF file is of the XDA (binary) format.
 */
bool FusionCDFData::IsXDACompatibleFile(const char *fileName)
{
	affxcdf::CCDFFileData cdf;
	cdf.SetFileName(fileName);
	return cdf.IsXDACompatibleFile();
}

/*
 * Determines if a CDF file is of the Calvin format.
 */
bool FusionCDFData::IsCalvinCompatibleFile(const char *fileName)
{
	GenericData data;
	GenericFileReader reader;
	reader.SetFilename(fileName);
	try
	{
		reader.ReadHeader(data, GenericFileReader::ReadNoDataGroupHeader);
		return true;
	}
	catch (affymetrix_calvin_exceptions::CalvinException)
	{
	}
	return false;
}

/*
 * Get the probe set type for non-qc probe sets.
 */
affxcdf::GeneChipProbeSetType FusionCDFData::GetProbeSetType(int index) const
{
	if (gcosData)
	{
		return gcosData->GetProbeSetType(index);
	}
	else if (calvinData && calvinData->GetGenericData().Header().GetGenericDataHdr()->GetFileTypeId() != AFFY_CNTRL_PS)
	{
		std::string dataTypeId = calvinData->GetDataTypeId();
		if (dataTypeId == AFFY_EXPR_PS)
			return affxcdf::ExpressionProbeSetType;

		else if (dataTypeId == AFFY_GENO_PS)
			return affxcdf::GenotypingProbeSetType;

		else if (dataTypeId == AFFY_RESEQ_PS)
			return affxcdf::ResequencingProbeSetType;

		else if (dataTypeId == AFFY_TAG_PS)
			return affxcdf::TagProbeSetType;

		else
			return affxcdf::UnknownProbeSetType;
	}
	else
		return affxcdf::UnknownProbeSetType;
}

/*
 * Get the probe set information.
 */
void FusionCDFData::GetProbeSetInformation(int index, FusionCDFProbeSetInformation & info)
{
	if (gcosData)
	{
		info.Initialize(index, gcosData);		
	}
	else if (calvinData && calvinData->GetGenericData().Header().GetGenericDataHdr()->GetFileTypeId() != AFFY_CNTRL_PS)
	{
		info.Initialize(index, calvinData);
	}
	else
		info.Clear();
}

/*
 * Get the QC probe set information by index.
 */
void FusionCDFData::GetQCProbeSetInformation(int index, FusionCDFQCProbeSetInformation & info)
{
	if (gcosData)
	{
		info.Initialize(index, gcosData);
	}
	else if (calvinData && calvinData->GetGenericData().Header().GetGenericDataHdr()->GetFileTypeId() == AFFY_CNTRL_PS)
	{
		info.Initialize(index, calvinData);
	}
	else
		info.Clear();
}

/*
 * Get the QC probe set information by type.
 */
void FusionCDFData::GetQCProbeSetInformation(affxcdf::GeneChipQCProbeSetType qcType, FusionCDFQCProbeSetInformation & info)
{
	if (gcosData)
	{
		info.Initialize(qcType, gcosData);
	}
	else if (calvinData && calvinData->GetGenericData().Header().GetGenericDataHdr()->GetFileTypeId() == AFFY_CNTRL_PS)
	{
		info.Initialize(qcType, calvinData);
	}
	else
		info.Clear();
}
