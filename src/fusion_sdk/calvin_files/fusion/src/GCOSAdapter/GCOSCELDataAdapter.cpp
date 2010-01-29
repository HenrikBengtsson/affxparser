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


#include "calvin_files/fusion/src/GCOSAdapter/GCOSCELDataAdapter.h"
//
#include "calvin_files/utils/src/StringUtils.h"
//

using namespace affxcel;
using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_utilities;
using namespace affymetrix_calvin_parameter;

/*
 * Constructor
 */
GCOSCELDataAdapter::GCOSCELDataAdapter()
{
}

/*
 * Destructor
 */
GCOSCELDataAdapter::~GCOSCELDataAdapter()
{
	gcosCel.Clear();
}

/*
 * Can this object read the file
 */
bool GCOSCELDataAdapter::CanReadFile()
{
	bool canRead = gcosCel.ReadHeader();
	gcosCel.Close();
	return canRead;
}

/*
 */
void GCOSCELDataAdapter::SetFileName(const std::string& value)
{
	gcosCel.SetFileName(value.c_str());
}

/*
 */
std::string GCOSCELDataAdapter::GetFileName() const
{
	return gcosCel.GetFileName();
}

/*
 */
void GCOSCELDataAdapter::SetError(const wchar_t* value)
{
	std::string error = StringUtils::ConvertWCSToMBS(value);
	gcosCel.SetError(error.c_str());
}

/*
 */
std::wstring GCOSCELDataAdapter::GetError()
{
	return StringUtils::ConvertMBSToWCS(gcosCel.GetError());
}

/*
 */
std::wstring GCOSCELDataAdapter::GetHeaderKey(const wchar_t* key)
{
	std::string keyString = StringUtils::ConvertWCSToMBS(key);
	return StringUtils::ConvertMBSToWCS(gcosCel.GetHeaderKey(keyString.c_str()));
}


/*
 */
std::wstring GCOSCELDataAdapter::GetHeader()
{
	return StringUtils::ConvertMBSToWCS(gcosCel.GetHeaderString());
}

/*
 */
std::wstring GCOSCELDataAdapter::GetAlg()
{
	return StringUtils::ConvertMBSToWCS(gcosCel.GetAlg()); 
}

/*
 */
std::wstring GCOSCELDataAdapter::GetAlgVer()
{
	std::wstring emptyStr;

	// to be implemented if needed
	return emptyStr;
}

/*
 */
std::wstring GCOSCELDataAdapter::GetParams()
{
	return StringUtils::ConvertMBSToWCS(gcosCel.GetParams());
}

/*
 */
std::wstring GCOSCELDataAdapter::GetAlgorithmParameter(const wchar_t *tag)
{
	std::string tagString = StringUtils::ConvertWCSToMBS(tag);
	return StringUtils::ConvertMBSToWCS(gcosCel.GetAlgorithmParameter(tagString.c_str()));
}

/*
 */
std::wstring GCOSCELDataAdapter::GetAlgorithmParameterTag(int index)
{
	return StringUtils::ConvertMBSToWCS(gcosCel.GetAlgorithmParameterTag(index));
}

/*
 */
std::wstring GCOSCELDataAdapter::GetAlgorithmParameters()
{
	return StringUtils::ConvertMBSToWCS(gcosCel.GetAlgorithmParameters());
}

/*
 */
void GCOSCELDataAdapter::GetParameters(FusionTagValuePairTypeList& values)
{
	FusionTagValuePairType tvp;
	int count = gcosCel.GetNumberAlgorithmParameters();
	for (int i=0; i<count; ++i)
	{
		tvp.Tag = StringUtils::ConvertMBSToWCS(gcosCel.GetAlgorithmParameterTag(i));
		tvp.Value = StringUtils::ConvertMBSToWCS(gcosCel.GetAlgorithmParameter(gcosCel.GetAlgorithmParameterTag(i).c_str()));
		tvp.DetailedType().SetName(tvp.Tag);
		tvp.DetailedType().SetValueText(tvp.Value);
		values.push_back(tvp);
	}
}

/*
 */
std::wstring GCOSCELDataAdapter::GetChipType()
{
	return StringUtils::ConvertMBSToWCS(gcosCel.GetChipType());
}

/*
 * Get the grid coordinates.
 */
affymetrix_fusion_io::FGridCoords GCOSCELDataAdapter::GetGridCorners()
{
	affymetrix_fusion_io::FGridCoords grid;
	GridCoordinatesType gct = gcosCel.GetGridCorners();

	grid.upperleft.x = gct.upperleft.x;
	grid.upperleft.y = gct.upperleft.y;
	grid.upperright.x = gct.upperright.x;
	grid.upperright.y = gct.upperright.y;
	grid.lowerright.x = gct.lowerright.x;
	grid.lowerright.y = gct.lowerright.y;
	grid.lowerleft.x = gct.lowerleft.x;
	grid.lowerleft.y = gct.lowerleft.y;

	return grid;
}

// Index/position conversions
/*
 */
int GCOSCELDataAdapter::IndexToX(int index)
{
	return gcosCel.IndexToX(index);
}

/*
 */
int GCOSCELDataAdapter::IndexToY(int index)
{
	return gcosCel.IndexToY(index);
}

/*
 */
int GCOSCELDataAdapter::XYToIndex(int x, int y)
{
	return gcosCel.XYToIndex(x, y);
}


// Accessors for intensity information.
/*
 */
void GCOSCELDataAdapter::GetEntry(int index, FusionCELFileEntryType &entry)
{
	CELFileEntryType gcosEntry;
	gcosCel.GetEntry(index, gcosEntry);
	CopyGCOSEntryToCalvin(gcosEntry, entry);
}

/*
 */
void GCOSCELDataAdapter::GetEntry(int x, int y, FusionCELFileEntryType &entry)
{
	CELFileEntryType gcosEntry;
	gcosCel.GetEntry(x, y, gcosEntry);
	CopyGCOSEntryToCalvin(gcosEntry, entry);
}

/*
 */
float GCOSCELDataAdapter::GetIntensity(int index)
{
	return gcosCel.GetIntensity(index);
}
/*
 */
int GCOSCELDataAdapter::GetIntensities(int index,std::vector<float>& intensities)
{
	return gcosCel.GetIntensities(index,intensities);
}

/*
 */
float GCOSCELDataAdapter::GetIntensity(int x, int y)
{
	return gcosCel.GetIntensity(x, y);
}

/*
 */
float GCOSCELDataAdapter::GetStdv(int index)
{
	return gcosCel.GetStdv(index);
}

/*
 */
float GCOSCELDataAdapter::GetStdv(int x, int y)
{
	return gcosCel.GetStdv(x, y);
}

/*
 */
short GCOSCELDataAdapter::GetPixels(int index)
{
	return gcosCel.GetPixels(index);
}

/*
 */
short GCOSCELDataAdapter::GetPixels(int x, int y)
{
	return gcosCel.GetPixels(x, y);
}

// Accessors for the mask/outlier flags

/*
 */
bool GCOSCELDataAdapter::IsMasked(int x, int y)
{
	return gcosCel.IsMasked(x, y);
}

/*
 */
bool GCOSCELDataAdapter::IsMasked(int index)
{
	return gcosCel.IsMasked(index);
}
/*
 */
bool GCOSCELDataAdapter::IsOutlier(int x, int y)
{
	return gcosCel.IsOutlier(x, y);
}

/*
 */
bool GCOSCELDataAdapter::IsOutlier(int index)
{
	return gcosCel.IsOutlier(index);
}

// For reading a file.
/*
 */
void GCOSCELDataAdapter::Close()
{
	gcosCel.Close();
}

/*
 */
bool GCOSCELDataAdapter::ReadHeader()
{
	return gcosCel.ReadHeader();
}

/*
 */
bool GCOSCELDataAdapter::Read(bool bIncludeMaskAndOutlier)
{
	return gcosCel.Read(bIncludeMaskAndOutlier);
}


/*
 */
void GCOSCELDataAdapter::Clear()
{
	gcosCel.Clear();
}

/*
 */
//void GCOSCELDataAdapter::SetAlgorithmName(const wchar_t *str)
//{
//	std::string name = StringUtils::ConvertWCSToMBS(str);
//	gcosCel.SetAlgorithmName(name.c_str());
//}

/*
 */
//void GCOSCELDataAdapter::AddAlgorithmParameter(const wchar_t *tag, const wchar_t *value)
//{
//	std::string tagString = StringUtils::ConvertWCSToMBS(tag);
//	std::string valueString = StringUtils::ConvertWCSToMBS(value);
//	gcosCel.AddAlgorithmParameter(tagString.c_str(), valueString.c_str());
//}

/*
 */
//void GCOSCELDataAdapter::SetChipType(const wchar_t *str)
//{
//	std::string type = StringUtils::ConvertWCSToMBS(str);
//	gcosCel.SetChipType(type.c_str());
//}

/*
 */
void GCOSCELDataAdapter::CopyGCOSEntryToCalvin(const CELFileEntryType& source, FusionCELFileEntryType& target)
{
	target.Intensity = source.Intensity;
	target.Stdv = source.Stdv;
	target.Pixels = source.Pixels;
}

/*
 * Returns the list of parameters associated with a data set, empty for GCOS files
 */
ParameterNameValueTypeList GCOSCELDataAdapter::GetDataSetParameters(const std::wstring &setName)
{
	ParameterNameValueTypeList params;
	return params;
}
