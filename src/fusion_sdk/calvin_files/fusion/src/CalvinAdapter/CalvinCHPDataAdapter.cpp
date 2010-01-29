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


#include "calvin_files/fusion/src/CalvinAdapter/CalvinCHPDataAdapter.h"
//
#include "calvin_files/utils/src/StringUtils.h"
//

using namespace affxchp;
using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_utilities;
using namespace affymetrix_calvin_io;

/*! Constructor */
CalvinCHPHeaderAdapter::CalvinCHPHeaderAdapter(CHPData* chp) : calvinChp(chp)
{
}

/*! Destructor */
CalvinCHPHeaderAdapter::~CalvinCHPHeaderAdapter()
{
}

/*! Gets the assay type
* @return The assay type (FusionExpression, FusionGenotyping, FusionResequencing, FusionUniversal, FusionUnknown)
*/
AssayType CalvinCHPHeaderAdapter::GetAssayType() const
{
	AssayType result = FusionUnknown;

	std::string tmp = calvinChp->GetAssayType();
	if(tmp == CHP_EXPRESSION_ASSAY_TYPE)
	{
		result = FusionExpression;
	}
	else if(tmp == CHP_GENOTYPING_ASSAY_TYPE)
	{
		result = FusionGenotyping;
	}
	else if(tmp == CHP_RESEQUENCING_ASSAY_TYPE)
	{
		result = FusionResequencing;
	}
	else if(tmp == CHP_UNIVERSAL_ASSAY_TYPE)
	{
		result = FusionUniversal;
	}
	return result;
}


/*! Gets the number of columns
* @return The number of columns
*/
int CalvinCHPHeaderAdapter::GetCols() const
{
	return calvinChp->GetCols();
}

/*! Gets the number of rows
* @return The number of rows
*/
int CalvinCHPHeaderAdapter::GetRows() const
{
	return calvinChp->GetRows();
}

/*! Gets the number of probesets
* @return The number of probesets
*/
int CalvinCHPHeaderAdapter::GetNumProbeSets() const
{
	return calvinChp->GetEntryCount();
}

/*! Get the chip (array) type
* @return The chip (array) type
*/
std::wstring CalvinCHPHeaderAdapter::GetChipType() const
{
	return calvinChp->GetArrayType();
}

/*! Gets the algorithm name
* @return The algorithm name
*/
std::wstring CalvinCHPHeaderAdapter::GetAlgName() const
{
	return calvinChp->GetAlgName(); 
}

/*! Gets the algorithm version
* @return The version
*/
std::wstring CalvinCHPHeaderAdapter::GetAlgVersion() const
{
	return calvinChp->GetAlgVersion(); 
}
/*! Gets the algorithm parameters
* @return The number of feature columns
*/
void CalvinCHPHeaderAdapter::GetAlgorithmParameters(FusionTagValuePairTypeList& values)
{
	ParameterNameValueTypeVector nvt=calvinChp->GetAlgParams();
	values = Convert(nvt);
}

u_int32_t CalvinCHPHeaderAdapter::GetAlgorithmParameterCount()
{
	return (u_int32_t)calvinChp->GetAlgParams().size();
}

u_int32_t CalvinCHPHeaderAdapter::GetSummaryParameterCount()
{
	return (u_int32_t)calvinChp->GetChipSums().size();
}

FusionTagValuePairTypeList CalvinCHPHeaderAdapter::Convert(ParameterNameValueTypeVector& nvt)
{
	FusionTagValuePairTypeList list;
	FusionTagValuePairType type;
	for(ParameterNameValueTypeVector::size_type i = 0; i < nvt.size(); ++i)
	{
		type.Tag = nvt[i].GetName();
		type.Value = nvt[i].ToString();
		type.DetailedType() = nvt[i];
		list.push_back(type);
	}
	return list;
}

/*! Gets the summary parameters */
void CalvinCHPHeaderAdapter::GetSummaryParameters(FusionTagValuePairTypeList& values)
{
	ParameterNameValueTypeVector nvt=calvinChp->GetChipSums();
	values = Convert(nvt);
}

/*! Gets the parent CEL file
* @return The parent CEL file
*/
std::wstring CalvinCHPHeaderAdapter::GetParentCellFile() const
{
	return calvinChp->GetParentCell(); 
}

/*! Gets the prog ID
* @return The prog ID
*/
std::wstring CalvinCHPHeaderAdapter::GetProgID() const
{
	return calvinChp->GetProgId(); 
}

/*! Gets a specific algorithm parameter given a name/tag
* @return The specific algorithm parameter given a name/tag
*/
std::wstring CalvinCHPHeaderAdapter::GetAlgorithmParameter(const wchar_t *tag)
{
	std::wstring p = tag;
	return calvinChp->GetAlgParam(p).ToString(); 
}

/*! Gets a specific summary parameter given a name/tag
* @return The specific summary parameter given a name/tag
*/
std::wstring CalvinCHPHeaderAdapter::GetSummaryParameter(const wchar_t *tag)
{
	std::wstring p = tag;
	return calvinChp->GetChipSum(p).ToString(); 
}

/*! Gets the magic number
* @return The magic number
*/
int CalvinCHPHeaderAdapter::GetMagic() const
{
	return calvinChp->GetMagic(); 
}

/*! Gets the version number
	* @return The version number
	*/
int CalvinCHPHeaderAdapter::GetVersion() const
{
	return calvinChp->GetVersion(); 
}

/*! Gets the background zone information
* @return The background zone information
*/
void CalvinCHPHeaderAdapter::GetBackgroundZoneInfo(BackgroundZoneInfo& info)
{
	int32_t count = calvinChp->GetBackgroundZoneCnt();
	info.number_zones = (int)count;
	info.smooth_factor = 0.0f;
	
	CHPBackgroundZoneVector zones;
	calvinChp->GetBackgroundZones(0,count,zones);

	CHPBackgroundZoneVector::iterator begin = zones.begin();
	CHPBackgroundZoneVector::iterator end = zones.end();

	// Get the smooth factor from the first zone.
	if (begin != end)
		info.smooth_factor = begin->GetSmoothFactor();

	for(;begin != end; ++begin)
	{
		BackgroundZoneType bzt;
		bzt.background = begin->GetBackground();
		bzt.centerx = begin->GetCenterX();
		bzt.centery = begin->GetCenterY();
		
		info.zones.push_back(bzt);
	}
}

/*! Gets the list of background zone positions and values
* @return The list of background zone positions and values
*/
void CalvinCHPHeaderAdapter::GetBackgroundZones(BackgroundZoneTypeList& zones)
{
	int32_t count = calvinChp->GetBackgroundZoneCnt();

	CHPBackgroundZoneVector vec;
	calvinChp->GetBackgroundZones(0,count,vec);

	CHPBackgroundZoneVectorIt begin = vec.begin(), end = vec.end();
	for(; begin != end; begin++)
	{
		BackgroundZoneType type;
		type.background = begin->GetBackground();
		type.centerx = begin->GetCenterX();
		type.centery = begin->GetCenterY();
		zones.push_back(type);
	}
}

/*! Gets the background value for a given center coordinate
* @return The background value for a given center coordinate
*/
void CalvinCHPHeaderAdapter::GetBackgroundZone(BackgroundZoneType& type, int x, int y)
{
			CHPBackgroundZone zone;
			calvinChp->GetBackgroundZone(x,zone);
			type.background = zone.GetBackground();
			type.centerx = zone.GetCenterX();
			type.centery = zone.GetCenterY();
}

//////////////////////////////
// data section
/*
 * Constructor
 */
CalvinCHPDataAdapter::CalvinCHPDataAdapter()
{
	header = new CalvinCHPHeaderAdapter(&calvinChp);
}

/*
 * Destructor
 */
CalvinCHPDataAdapter::~CalvinCHPDataAdapter()
{
	calvinChp.Clear();
	delete header;
}

/*! Accessors to header.
* @return The header data object
*/
IFusionCHPHeaderAdapter& CalvinCHPDataAdapter::GetHeader()
{
	return *header;
}

/*! Returns the expression probe set result
* @param index The index to the result object of interest.
* @param result The expression result.
* @return True if the expression result was found.
*/ 
bool CalvinCHPDataAdapter::GetExpressionResults(int index, FusionExpressionProbeSetResults& result)
{
	if(calvinChp.GetAssayType() == CHP_EXPRESSION_ASSAY_TYPE)
	{
		CHPExpressionEntry entry;

		// row is the same as index
		calvinChp.GetEntry(index, entry);
		result.SetDetectionPValue(entry.GetDetectionPValue());

		/*! The signal value */
		result.SetSignal(entry.GetSignal());

		/*! The number of probe pairs in the set */
		result.SetNumPairs(entry.GetNumPairs());

		/*! The number of probe pairs used to calculate the signal value */
		result.SetNumUsedPairs(entry.GetNumPairsUsed());

		/*! The detection call */
		result.SetDetection(entry.GetDetection());

		/*! Flag indicating that comparison results exist */
		result.SetHasCompResults(entry.GetHasComparisonData());

		/*! The change p-value */
		result.SetChangePValue(entry.GetChangePValue());

		/*! The signal log ratio */
		result.SetSignalLogRatio(entry.GetSigLogRatio());

		/*! The signal log ratio low value */
		result.SetSignalLogRatioLow(entry.GetSigLogRatioLo());

		/*! The signal log ratio high value */
		result.SetSignalLogRatioHigh(entry.GetSigLogRatioHi());

		/*! The number of probe pairs in common between the experiment and baseline data */
		result.SetNumCommonPairs(entry.GetCommonPairs());

		/*! The change call */
		result.SetChange(entry.GetChange());

		return true;
	}
	return false;
}

/*! Returns the genotyping probe set result
* @param index The index to the result object of interest.
* @param result The genotyping result.
* @return True if the genotyping result was found.
*/
bool CalvinCHPDataAdapter::GetGenotypingResults(int index, FusionGenotypeProbeSetResults& result)
{
	if(calvinChp.GetAssayType() == CHP_GENOTYPING_ASSAY_TYPE)
	{
		CHPGenotypeEntry entry;
		calvinChp.GetEntry(index, entry);
		result.SetAlleleCall(entry.GetCall());
		result.SetConfidence(entry.GetConfidence());
		result.SetRAS1(entry.GetRAS1());
		result.SetRAS2(entry.GetRAS2());
		result.SetPValueAA(entry.GetAACall());
		result.SetPValueAB(entry.GetABCall());
		result.SetPValueBB(entry.GetBBCall());
		result.SetPValueNoCall(entry.GetNoCall());
		return true;
	}
	return false;
}

/*! Returns the universal (tag array) probe set result
* @param index The index to the result object of interest.
* @param The universal result.
* @return True if the universal result was found.
*/
bool CalvinCHPDataAdapter::GetUniversalResults(int index, FusionUniversalProbeSetResults& result)
{
	if(calvinChp.GetAssayType() == CHP_UNIVERSAL_ASSAY_TYPE)
	{
		CHPUniversalEntry entry;
		calvinChp.GetEntry(index, entry);
		result.SetBackground(entry.GetBackground());
		return true;
	}
	return false;
}

bool CalvinCHPDataAdapter::GetResequencingResults(FusionResequencingResults& results)
{
	if(calvinChp.GetAssayType() == CHP_RESEQUENCING_ASSAY_TYPE)
	{
		CHPReseqEntry entry;
		int32_t sz = calvinChp.GetEntryCount();
		results.ResizeCalledBases(sz);
		results.ResizeScores(sz);
		for(int i = 0; i < sz; i++)
		{
			calvinChp.GetEntry(i, entry);
			results.SetCalledBase(i, entry.call);
			results.SetScore(i, entry.score);
		}

		sz = calvinChp.GetForceCnt();
		results.ResizeForceCalls(sz);
		CHPReseqForceCall f;
		for(int i = 0; i < sz; i++)
		{
			calvinChp.GetForceCall(i, f);
			FusionForceCallType fusionType(f.position, f.call, f.reason);
			results.SetForceCall(i, fusionType);
		}

		sz = calvinChp.GetOrigCnt();
		results.ResizeOrigCalls(sz);
		CHPReseqOrigCall b;
		for(int i = 0; i < sz; i++)
		{
			calvinChp.GetOrigCall(i, b);
			FusionBaseCallType fusionType(b.position, b.call);
			results.SetOrigCall(i, fusionType);
		}
		return true;
	}
	return false;
}

// Functions to read file.
bool CalvinCHPDataAdapter::Read()
{
	CHPFileReader reader;
	try
	{
		reader.Read(calvinChp);
	}
	catch(...)
	{
		return false;
	}
	return true;
}

/*! Reads the header of the CHP file
* @return True if successful
*/
bool CalvinCHPDataAdapter::ReadHeader()
{
	CHPFileReader reader;
	try
	{
		reader.Read(calvinChp);
	}
	catch(...)
	{
		return false;
	}
	return true;
}

/*! Sets the file name.
* @param name The full path to the CHP file
*/
void CalvinCHPDataAdapter::SetFileName(const std::string& value)
{
	calvinChp.SetFilename(value);
}

/*! Gets the file name.
	* @return The full path to the CHP file.
*/
std::string CalvinCHPDataAdapter::GetFileName() const
{
	return calvinChp.GetFilename();
}


/*! Deallocates any memory used by the class object */
void CalvinCHPDataAdapter::Clear()
{
	calvinChp.Clear();
}

/*
 * Can this object read the file
 */
bool CalvinCHPDataAdapter::CanReadFile()
{
	CHPFileReader reader;
	try
	{
		reader.Read(calvinChp);
	}
	catch(affymetrix_calvin_exceptions::InvalidFileTypeException&)
	{
		return false;
	}

	return true;
}

/*
 * Get the id of the file
 */
AffymetrixGuidType CalvinCHPDataAdapter::FileId()
{
	return calvinChp.GetFileHeader()->GetGenericDataHdr()->GetFileId();
}


/*
 * Returns the GenericData object associated with a Calvin file, NULL for GCOS files.
 */
GenericData *CalvinCHPDataAdapter::GetGenericData()
{
	return &calvinChp.GetGenericData();
}

/*
 * Get the probe set name
 */
std::string CalvinCHPDataAdapter::GetProbeSetName(int index)
{
    if (calvinChp.GetAssayType() == CHP_EXPRESSION_ASSAY_TYPE)
  	{
	    CHPExpressionEntry entry;
	    calvinChp.GetEntry(index, entry);
        return entry.GetProbeSetName();
    }
    else if (calvinChp.GetAssayType() == CHP_GENOTYPING_ASSAY_TYPE)
    {
        CHPGenotypeEntry entry;
	    calvinChp.GetEntry(index, entry);
        return entry.GetProbeSetName();
    }
    return "";
}
