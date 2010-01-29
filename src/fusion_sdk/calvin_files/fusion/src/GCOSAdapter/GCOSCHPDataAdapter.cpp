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


#include "calvin_files/fusion/src/GCOSAdapter/GCOSCHPDataAdapter.h"
//
#include "calvin_files/utils/src/StringUtils.h"
//
#include "file/TagValuePair.h"
//

using namespace affxchp;
using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_utilities;


GCOSCHPHeaderAdapter::GCOSCHPHeaderAdapter(affxchp::CCHPFileData* chp): gcosChp(chp)
{

}

GCOSCHPHeaderAdapter::~GCOSCHPHeaderAdapter()
{
}


int GCOSCHPHeaderAdapter::GetCols() const
{
	return gcosChp->GetHeader().GetCols();
}

/*!  */
int GCOSCHPHeaderAdapter::GetRows() const
{
	return gcosChp->GetHeader().GetRows();
}

/*!  */
int GCOSCHPHeaderAdapter::GetNumProbeSets() const
{
	return gcosChp->GetHeader().GetNumProbeSets();
}

/*!  */
AssayType GCOSCHPHeaderAdapter::GetAssayType() const
{
	AssayType assayFusion = (AssayType)gcosChp->GetHeader().GetAssayType();
	return assayFusion;
}

/*!  */
std::wstring GCOSCHPHeaderAdapter::GetChipType() const
{
	return StringUtils::ConvertMBSToWCS(gcosChp->GetHeader().GetChipType());
}

/*!  */
std::wstring GCOSCHPHeaderAdapter::GetAlgName() const
{
	return StringUtils::ConvertMBSToWCS(gcosChp->GetHeader().GetAlgName()); 
}

/*!  */
std::wstring GCOSCHPHeaderAdapter::GetAlgVersion() const
{
	return StringUtils::ConvertMBSToWCS(gcosChp->GetHeader().GetAlgVersion()); 
}
/*! Gets the algorithm parameters
	* @return The number of feature columns
	*/
void GCOSCHPHeaderAdapter::GetAlgorithmParameters(FusionTagValuePairTypeList& values)
{
	ConvertGCOS(gcosChp->GetHeader().AlgorithmParameters(),values);
}

/*! Gets the algorithm parameter count
* @return The number of algorithm parameters
*/
u_int32_t GCOSCHPHeaderAdapter::GetAlgorithmParameterCount()
{
	TagValuePairTypeList &values = gcosChp->GetHeader().AlgorithmParameters();
	return (u_int32_t)values.size();
}

/*! Gets the summary parameter count
* @return The number of summary parameters
*/
u_int32_t GCOSCHPHeaderAdapter::GetSummaryParameterCount()
{
	TagValuePairTypeList &values = gcosChp->GetHeader().SummaryParameters();
	return (u_int32_t)values.size();
}

/*! Gets the summary parameters
	* @return The summary parameters
	*/
void GCOSCHPHeaderAdapter::GetSummaryParameters(FusionTagValuePairTypeList& values)
{
	ConvertGCOS(gcosChp->GetHeader().SummaryParameters(),values);
}

/*! Gets the parent CEL file
	* @return The parent CEL file
	*/
std::wstring GCOSCHPHeaderAdapter::GetParentCellFile() const
{
	return StringUtils::ConvertMBSToWCS(gcosChp->GetHeader().GetParentCellFile()); 
}

/*! Gets the prog ID
	* @return The prog ID
	*/
std::wstring GCOSCHPHeaderAdapter::GetProgID() const
{
	return StringUtils::ConvertMBSToWCS(gcosChp->GetHeader().GetProgID()); 
}
	/*! Gets a specific algorithm parameter given a name/tag
	* @return The specific algorithm parameter given a name/tag
	*/
std::wstring GCOSCHPHeaderAdapter::GetAlgorithmParameter(const wchar_t *tag)
{
	std::string hdr = StringUtils::ConvertWCSToMBS(tag);
	return StringUtils::ConvertMBSToWCS(gcosChp->GetHeader().GetAlgorithmParameter(hdr.c_str())); 
}

/*! Gets a specific summary parameter given a name/tag
	* @return The specific summary parameter given a name/tag
	*/
std::wstring GCOSCHPHeaderAdapter::GetSummaryParameter(const wchar_t *tag)
{
	std::string hdr = StringUtils::ConvertWCSToMBS(tag);
	return StringUtils::ConvertMBSToWCS(gcosChp->GetHeader().GetSummaryParameter(hdr.c_str())); 
}

/*! Gets the background zone information
	* @return The background zone information
	*/
void GCOSCHPHeaderAdapter::GetBackgroundZoneInfo(BackgroundZoneInfo& info)
{
	info = gcosChp->GetHeader().GetBackgroundZoneInfo(); 
}

/*! Gets the list of background zone positions and values
	* @return The list of background zone positions and values
	*/
void GCOSCHPHeaderAdapter::GetBackgroundZones(BackgroundZoneTypeList& zones)
{
	zones = gcosChp->GetHeader().GetBackgroundZones(); 
}

/*! Gets the background value for a given center coordinate
	* @return The background value for a given center coordinate
	*/
void GCOSCHPHeaderAdapter::GetBackgroundZone(BackgroundZoneType& type, int x, int y)
{
	type = gcosChp->GetHeader().GetBackgroundZone(x,y); 
}

/*! Gets the magic number
	* @return The magic number
	*/
int GCOSCHPHeaderAdapter::GetMagic() const
{
	return gcosChp->GetHeader().GetMagicNumber(); 
}

/*! Gets the version number
	* @return The version number
	*/
int GCOSCHPHeaderAdapter::GetVersion() const
{
	return gcosChp->GetHeader().GetVersionNumber(); 
}

//////////////////////////////
// data section
/*
 * Constructor
 */
GCOSCHPDataAdapter::GCOSCHPDataAdapter()
{
	header = new GCOSCHPHeaderAdapter(&gcosChp);
}

/*
 * Destructor
 */
GCOSCHPDataAdapter::~GCOSCHPDataAdapter()
{
	gcosChp.Clear();
	delete header;
}

/*! Accessors to header.
	* @return The header data object
	*/
IFusionCHPHeaderAdapter& GCOSCHPDataAdapter::GetHeader()
{
	return *header;
}

/*! Returns the expression probe set result
	* @param index The index to the result object of interest.
  * @param result The expression result.
  * @return True if the expression result was found.
	*/ 
bool GCOSCHPDataAdapter::GetExpressionResults(int index, FusionExpressionProbeSetResults& result)
{
	CExpressionProbeSetResults* ps = 0;
	ps = gcosChp.GetExpressionResults(index);
	if (ps)
	{
		result.SetDetectionPValue(ps->DetectionPValue);
		result.SetSignal(ps->Signal);
		result.SetNumPairs(ps->NumPairs);
		result.SetNumUsedPairs(ps->NumUsedPairs);
		result.SetDetection(ps->Detection);
		result.SetHasCompResults(ps->m_HasCompResults);
		result.SetChangePValue(ps->ChangePValue);
		result.SetSignalLogRatio(ps->SignalLogRatio);
		result.SetSignalLogRatioLow(ps->SignalLogRatioLow);
		result.SetSignalLogRatioHigh(ps->SignalLogRatioHigh);
		result.SetNumCommonPairs(ps->NumCommonPairs);
		result.SetChange(ps->Change);
		return true;
	}

	return false;
}


/*! Returns the genotyping probe set result
	* @param index The index to the result object of interest.
	 * @param result The genotyping result.
	 * @return True if the genotyping result was found.
	*/
bool GCOSCHPDataAdapter::GetGenotypingResults(int index, FusionGenotypeProbeSetResults& result)
{
	CGenotypeProbeSetResults* ps = 0;
	ps = gcosChp.GetGenotypingResults(index);
	if (ps)
	{
		result.SetAlleleCall(ps->AlleleCall);
		result.SetConfidence(ps->Confidence);
		result.SetRAS1(ps->RAS1);
		result.SetRAS2(ps->RAS2);
		result.SetPValueAA(ps->pvalue_AA);
		result.SetPValueAB(ps->pvalue_AB);
		result.SetPValueBB(ps->pvalue_BB);
		result.SetPValueNoCall(ps->pvalue_NoCall);
		return true;
	}

	return false;
}

/*! Returns the universal (tag array) probe set result
	* @param index The index to the result object of interest.
	* @param The universal result.
	* @return True if the universal result was found.
	*/
bool GCOSCHPDataAdapter::GetUniversalResults(int index, FusionUniversalProbeSetResults& result)
{
	CUniversalProbeSetResults* ps = 0;
	ps = gcosChp.GetUniversalResults(index);
	if (ps)
	{
		result.SetBackground(ps->GetBackground());
		return true;
	}
	return false;
}

bool GCOSCHPDataAdapter::GetResequencingResults(FusionResequencingResults& result)
{
	CResequencingResults* ps = 0;
	ps = gcosChp.GetResequencingResults();
	if (ps)
	{
		int32_t sz = ps->GetCalledBasesSize();
		for(int i = 0; i < sz; i++)
		{
			result.AddCalledBase(ps->GetCalledBase(i));
		}
		sz = ps->GetScoresSize();
		for(int i = 0; i < sz; i++)
		{
			result.AddScore(ps->GetScore(i));
		}
		sz = ps->GetForceCallsSize();
		for(int i = 0; i < sz; i++)
		{
			ForceCallType f = ps->GetForceCall(i);
			FusionForceCallType fusionType(f.position, f.call, f.reason);
			result.AddForceCall(fusionType);
		}
		sz = ps->GetOrigCallsSize();
		for(int i = 0; i < sz; i++)
		{
			BaseCallType b = ps->GetOrigCall(i);
			FusionBaseCallType fusionType(b.position, b.call);
			result.AddOrigCall(fusionType);
		}

		return true;
	}
	return false;
}

// Functions to read file.
bool GCOSCHPDataAdapter::Read()
{
	return gcosChp.Read();
}

/*! Reads the header of the CHP file
	* @return True if successful
	*/
bool GCOSCHPDataAdapter::ReadHeader()
{
	return gcosChp.ReadHeader();
}

/*! Sets the file name.
	* @param name The full path to the CHP file
	*/
void GCOSCHPDataAdapter::SetFileName(const std::string& value)
{
	gcosChp.SetFileName(value.c_str());
}

/*! Gets the file name.
	* @return The full path to the CHP file.
	*/
std::string GCOSCHPDataAdapter::GetFileName() const
{
	return gcosChp.GetFileName();
}

/*! Deallocates any memory used by the class object */
void GCOSCHPDataAdapter::Clear()
{
	gcosChp.Clear();
}

/*
 * Can this object read the file
 */
bool GCOSCHPDataAdapter::CanReadFile()
{
	bool canRead = gcosChp.ReadHeader();
	gcosChp.Clear();
	return canRead;
}


void affymetrix_fusion_io::ConvertFusion(FusionTagValuePairTypeList& fromList, TagValuePairTypeList& toList)
{
	FusionTagValuePairTypeList::iterator begin = fromList.begin();
	FusionTagValuePairTypeList::iterator end = fromList.end();

	for(; begin != end; begin++)
	{
		TagValuePairType type;
		type.Tag = StringUtils::ConvertWCSToMBS(begin->Tag);
		type.Value = StringUtils::ConvertWCSToMBS(begin->Value);
		toList.push_back(type);
	}
}
void affymetrix_fusion_io::ConvertGCOS(TagValuePairTypeList& fromList, FusionTagValuePairTypeList& toList)
{
	TagValuePairTypeList::iterator begin = fromList.begin();
	TagValuePairTypeList::iterator end = fromList.end();

	for(; begin != end; begin++)
	{
		FusionTagValuePairType type;
		type.Tag = StringUtils::ConvertMBSToWCS(begin->Tag);
		type.Value = StringUtils::ConvertMBSToWCS(begin->Value);
		toList.push_back(type);
	}
}
