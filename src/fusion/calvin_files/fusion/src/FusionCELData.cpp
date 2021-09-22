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


#include "calvin_files/fusion/src/FusionCELData.h"
//
#include "calvin_files/fusion/src/CalvinAdapter/CalvinCELDataAdapter.h"
#include "calvin_files/fusion/src/GCOSAdapter/GCOSCELDataAdapter.h"
#include "calvin_files/parsers/src/FileException.h"
#include "calvin_files/utils/src/FileUtils.h"
//
#include "util/Fs.h"
//
#include <cassert>
#include <sys/stat.h>
#include <sys/types.h>
//

using namespace affymetrix_fusion_io;
using namespace affymetrix_calvin_io;
using namespace affymetrix_calvin_exceptions;
using namespace affymetrix_calvin_utilities;
using namespace affymetrix_calvin_parameter;

/*
 * Default constructor
 */
FusionCELData::FusionCELData()
{
	adapter = 0;
	parameterListRead = false;
}

/*
 * Destructor
 */
FusionCELData::~FusionCELData()
{
	Clear();
	DeleteAdapter();
}

/*
 * Set the file name.
 */
void FusionCELData::SetFileName(const char *str)
{
	filename = str;
}

/*
 * Get the file name.
 */
std::string FusionCELData::GetFileName() const
{
	return filename;
}

/*
 * Get the file id.
 */
AffymetrixGuidType FusionCELData::GetFileId() const
{
	CheckAdapter();
	return adapter->GetFileId();
}

/*
 * Get the generic data object.
 */
GenericData *FusionCELData::GetGenericData()
{
	CheckAdapter();
	return adapter->GetGenericData();
}

/*! Maps X/Y coordinates to CEL file index.
 * @param x The x coordinate.
 * @param y The y coordinate.
 * @param r The number of rows.
 * @param c The number of columns.
 * @return The index to the intensity arrays.
 */
int FusionCELData::XYToIndex(int x, int y, int r, int c)
{
  return ((y*c) + x);
}

/*
 * Returns the list of parameters associated with a data set, empty for GCOS files
 */
ParameterNameValueTypeList FusionCELData::GetDataSetParameters(const std::wstring &setName)
{
	CheckAdapter();
	return adapter->GetDataSetParameters(setName);
}

/*
 * Set the error string.
 */
void FusionCELData::SetError(const wchar_t* value)
{
	CheckAdapter();
	adapter->SetError(value);
}

/*
 * Retrieve error string.
 */
std::wstring FusionCELData::GetError()
{
	CheckAdapter();
	return adapter->GetError();
}

std::wstring FusionCELData::GetHeaderKey(const wchar_t* key)
{
	CheckAdapter();
	return adapter->GetHeaderKey(key);
}


/*
 * Retrieve CEL file format version number.
 */
int FusionCELData::GetVersion()
{
	CheckAdapter();
	return adapter->GetVersion();
}

/*
 * Retrieve number of columns in array.
 */
int FusionCELData::GetCols()
{
	CheckAdapter();
	return adapter->GetCols();
}

/*
 * Retrieve number of rows in array.
 */
int FusionCELData::GetRows()
{
	CheckAdapter();
	return adapter->GetRows();
}

/*
 * Retrieve number of cells in array.
 */
int FusionCELData::GetNumCells()
{
	CheckAdapter();
	return adapter->GetNumCells();
}

/*
 * Retrieve header in a single string
 */
std::wstring FusionCELData::GetHeader()
{
	CheckAdapter();
	return adapter->GetHeader();
}

/*
 * Retrieve algorithm name.
 */
std::wstring FusionCELData::GetAlg()
{
	CheckAdapter();
	return adapter->GetAlg();
}

/*
 * Retrieve algorithm version.
 */
std::wstring FusionCELData::GetAlgVer()
{
	CheckAdapter();
	return adapter->GetAlgVer();
}

/*
 * Retrieve algorithm parameters.
 */
std::wstring FusionCELData::GetParams()
{
	CheckAdapter();
	return adapter->GetParams();
}

/*
 * Retrieve algorithm parameter of specified tag
 */
std::wstring FusionCELData::GetAlgorithmParameter(const wchar_t *tag)
{
	CheckAdapter();
	return adapter->GetAlgorithmParameter(tag);
}

/*
 * Retrieves the algorithm parameter name (tag) for a given index position.
 */
std::wstring FusionCELData::GetAlgorithmParameterTag(int index)
{
	CheckAdapter();
	return adapter->GetAlgorithmParameterTag(index);
}

/*
 * Retrieves the number of algorithm parameters.
 */
int FusionCELData::GetNumberAlgorithmParameters()
{
	CheckAdapter();
	return adapter->GetNumberAlgorithmParameters();
}

/*
 * Retrieve algorithm parameters
 */
std::wstring FusionCELData::GetAlgorithmParameters()
{
	CheckAdapter();
	return adapter->GetAlgorithmParameters();
}

/*
 * Retreive the algorithm parameters in a list
 */
FusionTagValuePairTypeList &FusionCELData::GetParameters()
{
	CheckAdapter();
	FillParameterList();
	return parameterList;
}

/*
 * Get the DatHeader string.
 */
std::wstring FusionCELData::GetDatHeader()
{
	CheckAdapter();
	return adapter->GetDatHeader();
}

/*
 * Retrieve chip type.
 */
std::wstring FusionCELData::GetChipType()
{
	CheckAdapter();
	return adapter->GetChipType();
}

/*
 * Get the library package
 */
std::wstring FusionCELData::GetLibraryPackageName()
{
   	CheckAdapter();
    return adapter->GetLibraryPackageName();
}

/*
 * Get the master file
 */
std::wstring FusionCELData::GetMasterFileName()
{
   	CheckAdapter();
    return adapter->GetMasterFileName();
}

/*
 * Retrieve cell margin.
 */
int FusionCELData::GetCellMargin()
{
	CheckAdapter();
	return adapter->GetCellMargin();
}

/*
 * Retrieve number of outliers.
 */
unsigned int FusionCELData::GetNumOutliers()
{
	CheckAdapter();
	return adapter->GetNumOutliers();
}

/*
 *	Retrieve number of masked cells.
 */
unsigned int FusionCELData::GetNumMasked()
{
	CheckAdapter();
	return adapter->GetNumMasked();
}

/*
 * Get the grid coordinates.
 */
affymetrix_fusion_io::FGridCoords FusionCELData::GetGridCorners()
{
	CheckAdapter();
	return adapter->GetGridCorners();
}

// Index/position conversions
/*
 * Get x coordinates from index.
 */
int FusionCELData::IndexToX(int index)
{
	CheckAdapter();
	return adapter->IndexToX(index);
}

/*
 * Get y coordinates from index.
 */
int FusionCELData::IndexToY(int index)
{
	CheckAdapter();
	return adapter->IndexToY(index);
}

/*
 * Convert x, y coordinates to index.
 */
int FusionCELData::XYToIndex(int x, int y)
{
	CheckAdapter();
	return adapter->XYToIndex(x, y);
}

// Accessors for intensity information.
/*
 * Retrieve a CEL file entry.
 */
void FusionCELData::GetEntry(int index, FusionCELFileEntryType &entry)
{
	CheckAdapter();
	return adapter->GetEntry(index, entry);
}

/*
 * Retrieve a CEL file entry.
 */
void FusionCELData::GetEntry(int x, int y, FusionCELFileEntryType &entry)
{
	CheckAdapter();
	return adapter->GetEntry(x, y, entry);
}

/*
 * Retrieve a CEL file intensity.
 */
float FusionCELData::GetIntensity(int index)
{
	CheckAdapter();
	return adapter->GetIntensity(index);
}

/*
 * Retrieve vector of CEL file intensities.
 */
int FusionCELData::GetIntensities(int index,std::vector<float>& intensities)
{
	CheckAdapter();
	return adapter->GetIntensities(index,intensities);
}

/*
 * Retrieve a CEL file intensity.
 */
float FusionCELData::GetIntensity(int x, int y)
{
	CheckAdapter();
	return adapter->GetIntensity(x, y);
}

/*
 * Retrieve a CEL file stdv value.
 */
float FusionCELData::GetStdv(int index)
{
	CheckAdapter();
	return adapter->GetStdv(index);
}

/*
 * Retrieve a CEL file stdv value.
 */
float FusionCELData::GetStdv(int x, int y)
{
	CheckAdapter();
	return adapter->GetStdv(x, y);
}

/*
 * Retrieve a CEL file pixel count.
 */
short FusionCELData::GetPixels(int index)
{
	CheckAdapter();
	return adapter->GetPixels(index);
}

/*
 * Retrieve a CEL file pixel count.
 */
short FusionCELData::GetPixels(int x, int y)
{
	CheckAdapter();
	return adapter->GetPixels(x, y);
}

// Accessors for the mask/outlier flags
/*
 * Retrieve a CEL file mask flag.
 */
bool FusionCELData::IsMasked(int x, int y)
{
	CheckAdapter();
	return adapter->IsMasked(x, y);
}

/*
 * Retrieve a CEL file mask flag.
 */
bool FusionCELData::IsMasked(int index)
{
	CheckAdapter();
	return adapter->IsMasked(index);
}

/*
 * Retrieve a CEL file outlier flag.
 */
bool FusionCELData::IsOutlier(int x, int y)
{
	CheckAdapter();
	return adapter->IsOutlier(x, y);
}

/*
 * Retrieve a CEL file outlier flag.
 */
bool FusionCELData::IsOutlier(int index)
{
	CheckAdapter();
	return adapter->IsOutlier(index);
}

// For reading a file.

/*
 * Closes the file.
 */
void FusionCELData::Close()
{
    if (adapter != NULL)
        adapter->Close();
}

/*
 * Returns the file size.
 */
unsigned int FusionCELData::GetFileSize()
{
  return Fs::fileSize(filename);
}

/*
 * Check if the file exists.
 */
bool FusionCELData::Exists()
{
  
	assert(filename != "");
        return FileUtils::Exists(filename.c_str());
}

/*
 * Reads the header of the CEL file.
 */
bool FusionCELData::ReadHeader()
{
    if (Exists() == false)
        return false;
    try
    {
        CreateAdapter();
        return adapter->ReadHeader();
    }
    catch (...)
    {
        return false;
    }
}

/*
 * Reads the CEL file.
 */
bool FusionCELData::Read(bool bIncludeMaskAndOutliers)
{
    if (Exists() == false)
        return false;
    try
    {
    	CreateAdapter();
	    return adapter->Read(bIncludeMaskAndOutliers);
    }
    catch (...)
    {
        return false;
    }
}

/*
 * Determine CEL file format and call appropriate function to read file using the specified file name.
 */
bool FusionCELData::ReadEx(const char *filename_, int state)
{
	filename = filename_;
    if (Exists() == false)
        return false;
    try
    {
        CreateAdapter();
	    return adapter->ReadEx(filename_, state);
    }
    catch (...)
    {
        return false;
    }
}

/*
 * Returns the reading state.
 */
int FusionCELData::GetReadState()
{
	CheckAdapter();
	return adapter->GetReadState();
}

/*
 * Clears the members.
 */
void FusionCELData::Clear()
{
	if (adapter)
		adapter->Clear();
	parameterListRead = false;
	parameterList.clear();
}

/*
 */
void FusionCELData::CreateAdapter()
{
	DeleteAdapter();

	// Create a Calvin adapter
	IFusionCELDataAdapter* calvinAdapter = new CalvinCELDataAdapter;
	if (calvinAdapter)
	{
		calvinAdapter->SetFileName(filename);
		if (calvinAdapter->CanReadFile())
		{
			adapter = calvinAdapter;
		}
		else
		{
			delete calvinAdapter;
			IFusionCELDataAdapter* gcosAdapter = new GCOSCELDataAdapter;
			if (gcosAdapter)
			{
				gcosAdapter->SetFileName(filename);
				if (gcosAdapter->CanReadFile())
				{
					adapter = gcosAdapter;
				}
				else
				{
					UnableToOpenFileException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
					throw e;
				}
			}
			else
			{
				UnableToOpenFileException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
				throw e;
			}
		}
	}
}

/*
 */
void FusionCELData::DeleteAdapter()
{
	if (adapter)
		delete adapter;
	adapter = 0;
}

/*
 */
void FusionCELData::CheckAdapter() const
{
	if (adapter == 0)
	{
		FileNotOpenException e(L"Calvin",L"Default Description, Please Update!",affymetrix_calvin_utilities::DateTime::GetCurrentDateTime().ToString(),std::string(__FILE__),(u_int16_t)__LINE__,0);
		throw e;
	}
}

/*
 */
void FusionCELData::FillParameterList()
{
	if (parameterListRead == false)
	{
		adapter->GetParameters(parameterList);
		parameterListRead = true;
	}
}

/* Sets the active data group for a multi-group CEL file. Default is the first group. */
void FusionCELData::SetActiveDataGroup(const std::wstring &groupName)
{
	CheckAdapter();
	adapter->SetActiveDataGroup(groupName);
}

/* Is this a multi-color CEL file? */
bool FusionCELData::IsMultiColor()
{
	CheckAdapter();
	return adapter->IsMultiColor();
}


/* Returns a list of the channel (ie data group) names */
WStringVector FusionCELData::GetChannels()
{
	CheckAdapter();
	return adapter->GetChannels();
}
